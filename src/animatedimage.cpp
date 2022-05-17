/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "animatedimage.h"
#include "animatedimagefborenderer.h"

#include <QOpenGLFramebufferObject>
#include <QThread>
#include <QDir>
#include <QQuickItemGrabResult>
#include <QtQuick/QQuickWindow>
#include "animatedimage.h"

class AnimatedImageRenderer : public QQuickFramebufferObject::Renderer
{
public:
    AnimatedImageRenderer()
    {
        renderer.initialize();
    }

    void render() override {
        renderer.render();
        update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::NoAttachment);
        format.setSamples(1);
        return new QOpenGLFramebufferObject(size, format);
    }

    AnimatedImageFBORenderer renderer;

protected:
    void synchronize(QQuickFramebufferObject *item) override;
};

AnimatedImage::AnimatedImage(QQuickItem *parentItem)
    : QQuickFramebufferObject(parentItem)
{
    setTextureFollowsItemSize(false);
    setImplicitSize(1, 1); // to force create renderer
}

QQuickFramebufferObject::Renderer *AnimatedImage::createRenderer() const
{
    return new AnimatedImageRenderer();
}

void AnimatedImageRenderer::synchronize(QQuickFramebufferObject *item)
{
    AnimatedImage* aitem = static_cast<AnimatedImage*>(item);

    if (aitem->buildMode.state > AnimatedImage::BuildingMode::Inactive) {
        if (aitem->buildMode.state == AnimatedImage::BuildingMode::Requested) {
            renderer.beginTextureUpload(aitem->buildMode.width, aitem->buildMode.height, aitem->buildMode.depth);
            aitem->setImplicitHeight(renderer.textureHeight());
            aitem->setImplicitWidth(renderer.textureWidth());
            aitem->buildMode.state = AnimatedImage::BuildingMode::Synchronized;
        }
        int i = 0;
        if (aitem->buildMode.pendingImages.size() == aitem->buildMode.depth) {
            for (const QImage &img : aitem->buildMode.pendingImages) {
                renderer.uploadFrame(i++, img);
            }
            aitem->buildMode.finishUpload();
        }
    } else if (aitem->isTextureDirty()) {
        renderer.createTexture(aitem->m_imageList);
        aitem->setImplicitHeight(renderer.textureHeight());
        aitem->setImplicitWidth(renderer.textureWidth());
        aitem->resetTextureDirty();
    }
    renderer.blur = aitem->blur();
    renderer.t1 = aitem->t1();
    renderer.t2 = aitem->t2();
    update();
}

const QString &AnimatedImage::fileNamePattern() const
{
    return m_fileNamePattern;
}

void AnimatedImage::setFileNamePattern(const QString &newFileNamePattern)
{
    if (m_fileNamePattern == newFileNamePattern)
        return;
    m_fileNamePattern = newFileNamePattern;
    m_textureDirty = true;
    rebuildImageList();
    buildMode.state = BuildingMode::Inactive;
    buildMode.pendingImages.clear();
    emit fileNamePatternChanged();
    update();
}

void AnimatedImage::beginCreateAnimation(int width, int height, int frameCount)
{
    buildMode.begin(width, height, frameCount);
    update();
}

void AnimatedImage::appendFrame(QVariant val)
{
    switch (buildMode.state) {
    case AnimatedImage::BuildingMode::Inactive:
        qWarning() << "Creating is not started. Before adding frames begin creating by calling beginCreateAnimation";
        return;
    case AnimatedImage::BuildingMode::Requested:
        qWarning() << "Not initialized yet. Please repeat later.";
        return;
    case AnimatedImage::BuildingMode::Synchronized:
        buildMode.state = BuildingMode::Uploading;
    case AnimatedImage::BuildingMode::Uploading: {
        QMutexLocker lck(&buildMode.mutex);
        QQuickItemGrabResult *result = qvariant_cast<QQuickItemGrabResult*>(val);
        buildMode.pendingImages.append(result->image());
        break;
    }
        break;
    case AnimatedImage::BuildingMode::UploadCompleted:
        qWarning() << "Too many frames. Uploading of" << buildMode.depth << "images is completed";
        return;
    }
}

void AnimatedImage::reset()
{
    m_textureDirty = true;
    rebuildImageList();
    buildMode.state = BuildingMode::Inactive;
    update();
}


void AnimatedImage::rebuildImageList()
{
    m_imageList.clear();
    QStringList patternDirComponents = m_fileNamePattern.split("/");
    QString fileFilter = patternDirComponents.takeLast();
    bool isRelative = !m_fileNamePattern.startsWith("/");
    QDir filePath = isRelative ? QDir::current() : QDir::root();
    for (const QString &dirComp : qAsConst(patternDirComponents)) {
        if (!filePath.cd(dirComp)) {
            qWarning() << "Failed to change directory to" << dirComp << "from" << filePath.canonicalPath();
            return;
        }
    }
    filePath.setSorting(QDir::Name); // TODO: make sorting user-configurable
    filePath.setNameFilters(QStringList() << fileFilter);
    for (const auto &it : filePath.entryInfoList(QDir::NoFilter, QDir::Name)) {
        m_imageList << it.filePath();
    }
}

void AnimatedImage::BuildingMode::begin(int width, int height, int frameCount)
{
    state = Requested;
    this->width = width;
    this->height = height;
    this->depth = frameCount;
}

void AnimatedImage::BuildingMode::finishUpload()
{
    pendingImages.clear();
    state = UploadCompleted;
}

qreal AnimatedImage::t1() const
{
    return m_t1;
}

void AnimatedImage::setT1(qreal newT1)
{
    if (qFuzzyCompare(m_t1, newT1))
        return;
    m_t1 = newT1;
    emit t1Changed();
    update();
}

qreal AnimatedImage::t2() const
{
    return m_t2;
}

void AnimatedImage::setT2(qreal newT2)
{
    if (qFuzzyCompare(m_t2, newT2))
        return;
    m_t2 = newT2;
    emit t2Changed();
    update();
}

bool AnimatedImage::blur() const
{
    return m_blur;
}

void AnimatedImage::setBlur(bool newBlur)
{
    if (m_blur == newBlur)
        return;
    m_blur = newBlur;
    emit blurChanged();
    update();
}
