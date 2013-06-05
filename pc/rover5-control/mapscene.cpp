#include "mapscene.h"

#include <QGraphicsPathItem>
#include <QPainter>
#include <QPainterPath>

CMapScene::CMapScene(QObject *parent) : QGraphicsScene(parent)
{
    setSceneRect(-500.0, -500.0, 1000.0, 1000.0);

    pathItem = addPath(QPainterPath(), QPen(Qt::red, 0.5));
}

void CMapScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::blue, 0.15));

    const float startx = sceneRect().left(), endx = sceneRect().right();
    const float starty = sceneRect().top(), endy = sceneRect().bottom();
    const float gridsize = 25.0;

    for (float x=startx; x<=endx; x+=gridsize)
        painter->drawLine(x, starty, x, endy);

    for (float y=starty; y<=endy; y+=gridsize)
        painter->drawLine(startx, y, endx, y);
}

void CMapScene::addPosition(qreal x, qreal y)
{
    QPainterPath ppath(pathItem->path());
    ppath.lineTo(x, y);
    pathItem->setPath(ppath);
}
