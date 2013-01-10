#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>

QFrame *createFrameGroupWidget(const QString &title, bool sunken)
{
    QFrame *ret = new QFrame;
    ret->setFrameStyle(QFrame::StyledPanel | ((sunken) ? QFrame::Sunken : QFrame::Raised));
    ret->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *vbox = new QVBoxLayout(ret);
    vbox->setSpacing(0);
    vbox->setSpacing(0);

    QLabel *label = new QLabel(title);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    label->setAlignment(Qt::AlignCenter);
    vbox->addWidget(label/*, 0, Qt::AlignTop*/);

    return ret;
}

// Based on http://www.developer.nokia.com/Community/Wiki/Image_editing_techniques_and_algorithms_using_Qt#Brightness
void changeBrightness(QImage &img, int delta)
{
    const int imgw = img.width(), imgh = img.height();
    for(int x=0; x<imgw; ++x)
    {
        for(int y=0; y<imgh; ++y)
        {
            const QColor c(img.pixel(x, y));
            const int r = qBound(0, c.red() + delta, 255);
            const int g = qBound(0, c.green() + delta, 255);
            const int b = qBound(0, c.blue() + delta, 255);
            img.setPixel(x, y, qRgb(r, g, b));
        }
    }
}
