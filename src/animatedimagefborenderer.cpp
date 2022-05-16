#include "animatedimagefborenderer.h"

#include <QDir>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLPixelTransferOptions>

AnimatedImageFBORenderer::AnimatedImageFBORenderer()
{

}

AnimatedImageFBORenderer::~AnimatedImageFBORenderer()
{
    if (texture) {
        texture->destroy();
        delete texture;
    }
    if (m_vbo) {
        if (m_vbo->isCreated()) {
            m_vbo->destroy();
        }
        delete m_vbo;
    }

    if (m_vao) {
        if (m_vao->isCreated()) {
            m_vao->destroy();
        }
        m_vao->deleteLater();
    }
}

void AnimatedImageFBORenderer::createTexture(const QStringList &pngFiles)
{
    if (!texture) {
        qDebug() << "Creating texture...";
        texture = new QOpenGLTexture(QOpenGLTexture::Target3D);
        texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
        texture->setWrapMode(QOpenGLTexture::Repeat);
        texture->setMipLevels(1);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
    }

    if (texture->isStorageAllocated()) {
        texture->destroy();
    }

    if (pngFiles.isEmpty()) {
        qDebug() << "No files provided, texture empty";
        return;
    }

    QImage first;
    first.load(pngFiles.first());
    if (!first.isNull()) {
        qDebug() << "Image size" << first.size();
        texture->setSize(first.size().width(), first.size().height(), pngFiles.size());
    } else {
        qWarning() << "Failed to load first image" << pngFiles.first();
        return;
    }
    texture->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
    QImage glImage = first.convertToFormat(QImage::Format_RGBA8888_Premultiplied);
    QOpenGLPixelTransferOptions opts;
    opts.setAlignment(1);

    texture->setData(0, 0, 0, texture->width(), texture->height(), 1, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage.constBits(), &opts);
    for (int i = 1; i < pngFiles.size(); ++i) {
        QImage img(pngFiles.at(i));
        if (img.isNull()) {
            qWarning() << "Failed to load image" << pngFiles.at(i) << "layer" << i << "remains uninitialized";
            continue;
        } else if (img.size() != first.size()) {
            qWarning() << "Image" << pngFiles.at(i) << "size differs from the texture size. Make sure that all images are equal in pixel size. Skipping frame" << i;
        }
        glImage = img.convertToFormat(QImage::Format_RGBA8888_Premultiplied);
        texture->setData(0, 0, i, texture->width(), texture->height(), 1, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage.constBits(), &opts);
    }
    qDebug() << "Finished loading" << pngFiles.size() << "images to 3D texture";
}

void AnimatedImageFBORenderer::initialize()
{
    initializeOpenGLFunctions();
    float vertices[] = {
        // positions            // texture coordinates
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f, // top left
        1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // bottom right
        1.0f,  1.0f, 0.0f,      1.0f, 1.0f, // top right
    };

    bool vertexOk = program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral("shaders/shader.vert"));
    bool fragmentOk = program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral("shaders/shader.frag"));
    qDebug() << "Vertex shader ok" << vertexOk << "fragment ok" << fragmentOk << "Linking shader program" << program.link();

    aPos_location = program.attributeLocation("aPos");
    aTexCoord_location = program.attributeLocation("aTexCoord");
    tPos_location = program.uniformLocation("tPos");

    m_vao = new QOpenGLVertexArrayObject();
    m_vao->create();
    m_vao->bind();

    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();
    m_vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo->allocate(vertices, sizeof(vertices));

    program.setAttributeBuffer(aPos_location, GL_FLOAT, 0 * sizeof(float), 3, 5 * sizeof(float));
    program.enableAttributeArray(aPos_location);

    program.setAttributeBuffer(aTexCoord_location, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
    program.enableAttributeArray(aTexCoord_location);

    m_vbo->release();
    m_vao->release();
}

void AnimatedImageFBORenderer::render()
{
    texture->bind();
    program.bind();
    program.setUniformValue(tPos_location, m_t);
    m_vao->bind();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // render a rectangle
    m_vao->release();
    program.release();
}

float AnimatedImageFBORenderer::t() const
{
    return m_t;
}

void AnimatedImageFBORenderer::setT(float newT)
{
    m_t = newT;
}

int AnimatedImageFBORenderer::textureWidth() const
{
    if (!texture) {
        return 0;
    }
    return texture->width();
}

int AnimatedImageFBORenderer::textureHeight() const
{
    if (!texture) {
        return 0;
    }
    return texture->height();
}
