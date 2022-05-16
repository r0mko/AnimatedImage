#pragma once

#include <QtQuick/QQuickFramebufferObject>
class LogoRenderer;

class AnimatedImage : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(AnimatedImage)

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QString fileNamePattern READ fileNamePattern WRITE setFileNamePattern NOTIFY fileNamePatternChanged)

//    Q_PROPERTY(int frameCount READ frameCount NOTIFY frameCountChanged)
//    Q_PROPERTY(qreal playbackFramerate READ playbackFramerate WRITE setPlaybackFramerate NOTIFY playbackFramerateChanged)

public:
    AnimatedImage(QQuickItem *parentItem = nullptr);
    Renderer *createRenderer() const;
    qreal t() const;
    void setT(qreal newT);

    const QString &fileNamePattern() const;
    void setFileNamePattern(const QString &newFileNamePattern);

signals:
    void tChanged();
    void fileNamePatternChanged();

private:
    friend class AnimatedImageRenderer;
    void rebuildImageList();
    qreal m_t;
    QString m_fileNamePattern;
    QStringList m_imageList;
    inline bool isTextureDirty() const { return m_textureDirty; }
    inline void resetTextureDirty() { m_textureDirty = false; }
    bool m_textureDirty = true;
};

