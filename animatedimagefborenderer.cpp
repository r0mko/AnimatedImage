#include "animatedimagefborenderer.h"

#include <QDir>
#include <QOpenGLPixelTransferOptions>

AnimatedImageFBORenderer::AnimatedImageFBORenderer()
{

}

void AnimatedImageFBORenderer::createTexture(const QStringList &pngFiles)
{
    if (!texture) {
        qDebug() << "Creating texture...";
        texture = new QOpenGLTexture(QOpenGLTexture::Target3D);
        texture->setFormat(QOpenGLTexture::RGBA8_SNorm);
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

    program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral("shaders/shader.vert"));
    program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral("shaders/shader.frag"));
    program.link();
    m_t_index = program.uniformLocation("tPos");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr); // set pointer to 1st vertex shader uniform, aPos
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof (float))); // 2nd vertex shader uniform, aTexCoord

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    createTexture(QStringList());
}

void AnimatedImageFBORenderer::render()
{
    texture->bind();
    program.bind();
    program.setUniformValue(m_t_index, m_t);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // render a rectangle
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
