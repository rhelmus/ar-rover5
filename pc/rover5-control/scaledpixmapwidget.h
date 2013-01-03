#ifndef SCALEDPIXMAPWIDGET_H
#define SCALEDPIXMAPWIDGET_H

#include <QWidget>

class CScaledPixmapWidget : public QWidget
{
    QPixmap pixmap;
    QSize widgetSize;
    qreal rotateAngle;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *event);

public:
    explicit CScaledPixmapWidget(QWidget *parent = 0)
        : QWidget(parent), rotateAngle(0.0) { }

    void setPixmap(const QPixmap &p);
    void setRotation(qreal r);
};

#endif // SCALEDPIXMAPWIDGET_H
