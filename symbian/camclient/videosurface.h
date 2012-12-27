#ifndef VIDEOSURFACE_H
#define VIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QImage>
#include <QVideoFrame>

class CVideoSurface : public QAbstractVideoSurface
{
    QVideoFrame currentVideoFrame;
    QImage::Format imageFormat;

public:
    explicit CVideoSurface(QObject *parent = 0)
        : QAbstractVideoSurface(parent), imageFormat(QImage::Format_Invalid) { }

    virtual bool start(const QVideoSurfaceFormat &format);
    virtual bool present(const QVideoFrame &frame);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
                QAbstractVideoBuffer::HandleType ht = QAbstractVideoBuffer::NoHandle) const;

    QImage getFrame(void);
};

#endif // VIDEOSURFACE_H
