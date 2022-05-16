#pragma once

#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <qopenglshaderprogram.h>
#include <qopenglfunctions_4_1_core.h>
#include <qopenglfunctions.h>
#include <QOpenGLTexture>

class AnimatedImageFBORenderer : protected QOpenGLFunctions
{

public:
    AnimatedImageFBORenderer();
    void createTexture(const QStringList &pngFiles);
    void initialize();
    void render();
    float t() const;
    void setT(float newT);

    int textureWidth() const;
    int textureHeight() const;
private:
    QOpenGLShaderProgram program;
    QOpenGLTexture *texture = nullptr;

    uint32_t vbo;
    uint32_t vao;

    float m_t = 0.5;
    int m_t_index;
};
