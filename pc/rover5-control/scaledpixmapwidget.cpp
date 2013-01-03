#include "scaledpixmapwidget.h"

#include <QPainter>
#include <QResizeEvent>

// Code based on http://wiki.forum.nokia.com/index.php/CS001347_-_Scaling_QPixmap_image

void CScaledPixmapWidget::paintEvent(QPaintEvent *)
{
    if (pixmap.isNull())
        return;

    QPainter painter(this);
    QPoint centerPoint;

    QMatrix m;
    m.rotate(rotateAngle);
    QPixmap pm = pixmap.transformed(m).scaled(widgetSize, Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation);

    centerPoint.setX((widgetSize.width()-pm.width()) / 2);
    centerPoint.setY((widgetSize.height()-pm.height()) / 2);

    painter.drawPixmap(centerPoint, pm);
}

void CScaledPixmapWidget::resizeEvent(QResizeEvent *event)
{
    widgetSize = event->size();
    QWidget::resizeEvent(event);
}

void CScaledPixmapWidget::setPixmap(const QPixmap &p)
{
    pixmap = p;
    update();
}

void CScaledPixmapWidget::setRotation(qreal r)
{
    rotateAngle = r;
    update();
}
