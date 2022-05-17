#include "animatedimagefborenderer.h"

#include <QDir>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QSGTexture>
#include <QOpenGLPixelTransferOptions>
#include <QOpenGLFramebufferObject>

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
        texture = new QOpenGLTexture(QOpenGLTexture::Target3D);
        initTexture();
    }

    if (texture->isStorageAllocated()) {
        texture->destroy();
        initTexture();
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
}

void AnimatedImageFBORenderer::createTexture(int width, int height, int depth)
{
    if (!texture) {
        texture = new QOpenGLTexture(QOpenGLTexture::Target3D);
        initTexture();
    }

    if (texture->isStorageAllocated()) {
        qDebug() << "Destroying texture";
        texture->destroy();
        initTexture();
    }
    texture->setSize(width, height, depth);
    texture->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
}

void AnimatedImageFBORenderer::initTexture()
{
    texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    texture->setWrapMode(QOpenGLTexture::Repeat);
    texture->setMipLevels(1);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
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

    program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, QLatin1String("shaders/shader.vert"));
    program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, QLatin1String("shaders/shader.frag"));
    program.link();

    pos.vCoord = program.attributeLocation("vCoord");
    pos.uv = program.attributeLocation("uv");
    pos.t1 = program.uniformLocation("t1");
    pos.t2 = program.uniformLocation("t2");
    pos.depth = program.uniformLocation("depth");
    pos.blur = program.uniformLocation("blur");

    m_vao = new QOpenGLVertexArrayObject();
    m_vao->create();
    m_vao->bind();

    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();
    m_vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo->allocate(vertices, sizeof(vertices));

    program.setAttributeBuffer(pos.vCoord, GL_FLOAT, 0 * sizeof(float), 3, 5 * sizeof(float));
    program.enableAttributeArray(pos.vCoord);

    program.setAttributeBuffer(pos.uv, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
    program.enableAttributeArray(pos.uv);

    m_vbo->release();
    m_vao->release();
}

void AnimatedImageFBORenderer::render()
{
    texture->bind();
    program.bind();
    program.setUniformValue(pos.t1, t1);
    program.setUniformValue(pos.t2, t2);
    program.setUniformValue(pos.depth, texture->depth());
    program.setUniformValue(pos.blur, blur);
    m_vao->bind();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // render a rectangle
    m_vao->release();
    program.release();
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

void AnimatedImageFBORenderer::beginTextureUpload(int width, int height, int depth)
{
    createTexture(width, height, depth);
}

void AnimatedImageFBORenderer::uploadFrame(int frame, QImage image)
{
    Q_ASSERT_X(texture, Q_FUNC_INFO, "texture not created");
    QImage glImage = image.convertToFormat(QImage::Format_RGBA8888_Premultiplied);
    QOpenGLPixelTransferOptions opts;
    opts.setAlignment(1);
    texture->setData(0, 0, frame, texture->width(), texture->height(), 1, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage.constBits(), &opts);
}
