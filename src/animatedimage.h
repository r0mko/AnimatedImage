#pragma once

#include <QtQuick/QQuickFramebufferObject>

#include <QImage>
#include <QMutex>

class LogoRenderer;
class QSGTextureProvider;

class AnimatedImage : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(AnimatedImage)

    Q_PROPERTY(qreal t1 READ t1 WRITE setT1 NOTIFY t1Changed)
    Q_PROPERTY(qreal t2 READ t2 WRITE setT2 NOTIFY t2Changed)
    Q_PROPERTY(QString fileNamePattern READ fileNamePattern WRITE setFileNamePattern NOTIFY fileNamePatternChanged)
    Q_PROPERTY(bool blur READ blur WRITE setBlur NOTIFY blurChanged)

public:
    AnimatedImage(QQuickItem *parentItem = nullptr);
    Renderer *createRenderer() const;

    const QString &fileNamePattern() const;
    void setFileNamePattern(const QString &newFileNamePattern);
    Q_INVOKABLE void beginCreateAnimation(int width, int height, int frameCount);
    Q_INVOKABLE void appendFrame(QVariant val);
    Q_INVOKABLE void reset();

    qreal t1() const;
    void setT1(qreal newT1);

    qreal t2() const;
    void setT2(qreal newT2);

    bool blur() const;
    void setBlur(bool newBlur);

signals:
    void tChanged();
    void fileNamePatternChanged();
    void t1Changed();
    void t2Changed();

    void blurChanged();

private:
    friend class AnimatedImageRenderer;
    void rebuildImageList();
    void onImageReady();
    QString m_fileNamePattern;
    QStringList m_imageList;
    bool m_textureDirty = true;

    inline bool isTextureDirty() const { return m_textureDirty; }
    inline void resetTextureDirty() { m_textureDirty = false; }


    struct BuildingMode {
        enum State {
            Inactive,
            Requested,
            Synchronized,
            Uploading,
            UploadCompleted
        };

        State state = Inactive;
        QList<QImage> pendingImages;
        int width, height, depth;
        QMutex mutex;
        void begin(int width, int height, int frameCount);
        void finishUpload();
    };
    BuildingMode buildMode;
    qreal m_t1;
    qreal m_t2;
    bool m_blur = true;
};

