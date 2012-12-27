#include "camclient.h"
#include "videosurface.h"

#include <QVideoSurfaceFormat>

// Code based on camera example from Qt mobility:
// http://doc.qt.digia.com/qt/demos-mobile-qcamera.html
bool CVideoSurface::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format imgformat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    if ((imgformat != QImage::Format_Invalid) && !size.isEmpty())
    {
        imageFormat = imgformat;
        setNativeResolution(QSize(480, 360)); // UNDONE: make configurable
        QAbstractVideoSurface::start(format);
        return true;
    }

    return false;
}

bool CVideoSurface::present(const QVideoFrame &frame)
{
    currentVideoFrame = frame;

    if ((surfaceFormat().pixelFormat() != frame.pixelFormat()) ||
        (surfaceFormat().frameSize() != frame.size()))
    {
        stop();
        return false;
    }
    else
    {
        // UNDONE
        //m_target->updateVideo();
        CCamClient::getInstance()->updateVideo();
        return true;
    }
}

QImage CVideoSurface::getFrame()
{
    if (currentVideoFrame.map(QAbstractVideoBuffer::ReadOnly))
    {
        QImage image(currentVideoFrame.bits(), currentVideoFrame.width(),
                    currentVideoFrame.height(), currentVideoFrame.bytesPerLine(),
                    imageFormat);

        currentVideoFrame.unmap();

        return image;
    }

    return QImage();
}


QList<QVideoFrame::PixelFormat> CVideoSurface::supportedPixelFormats(
    QAbstractVideoBuffer::HandleType ht) const
{
    if (ht == QAbstractVideoBuffer::NoHandle)
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    else
        return QList<QVideoFrame::PixelFormat>();
}
