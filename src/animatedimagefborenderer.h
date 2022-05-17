#pragma once

#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <qopenglshaderprogram.h>
#include <qopenglfunctions_4_1_core.h>
#include <qopenglfunctions.h>

class QOpenGLBuffer;
class QSGTexture;
class QOpenGLTexture;
class QOpenGLVertexArrayObject;

class AnimatedImageFBORenderer : protected QOpenGLFunctions
{

public:
    AnimatedImageFBORenderer();
    ~AnimatedImageFBORenderer();
    void createTexture(const QStringList &pngFiles);
    void createTexture(int width, int height, int depth);
    void initTexture();
    void initialize();
    void render();

    int textureWidth() const;
    int textureHeight() const;

    void beginTextureUpload(int width, int height, int depth);
    void uploadFrame(int frame, QImage image);
    void endTextureUpload();
    float t1 = 0.5;
    float t2 = 0.5;
    bool blur;

private:

    QOpenGLShaderProgram program;
    QOpenGLTexture *texture = nullptr;

    QOpenGLBuffer *m_vbo = nullptr;
    QOpenGLVertexArrayObject *m_vao = nullptr;

    struct {
        int vCoord;
        int uv;
        int t1;
        int t2;
        int depth;
        int blur;
    } pos;

};
