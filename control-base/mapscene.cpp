#include "mapscene.h"

#include <QGraphicsPathItem>
#include <QPainter>
#include <QPainterPath>

namespace
{

enum
{
    CM_SIZE = 2,
    CELL_SIZE_CM = 10,
    CELL_SIZE_PX = CM_SIZE * CELL_SIZE_CM
};

int getCellFromPx(qreal px)
{
    if (px > 0.0)
        return px / CELL_SIZE_PX;
    return px / CELL_SIZE_PX - 0.5;
}

qreal getPxFromCellStart(int cell)
{
    return cell * CELL_SIZE_PX;
}

}

CMapScene::CMapScene(QObject *parent) : QGraphicsScene(parent)
{
    setSceneRect(-CELL_SIZE_PX * 5, -CELL_SIZE_PX * 5, CELL_SIZE_PX * 10, CELL_SIZE_PX * 10);

    pathItem = addPath(QPainterPath(), QPen(Qt::red, 2.5));
}

void CMapScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::blue, 0.15));

    const float startx = sceneRect().left(), endx = sceneRect().right();
    const float starty = sceneRect().top(), endy = sceneRect().bottom();

    for (float x=startx; x<=endx; x+=CELL_SIZE_PX)
        painter->drawLine(x, starty, x, endy);

    for (float y=starty; y<=endy; y+=CELL_SIZE_PX)
        painter->drawLine(startx, y, endx, y);
}

void CMapScene::addPosition(qreal x, qreal y)
{
    const qreal pxx = CM_SIZE * x, pxy = CM_SIZE * -y; // invert Y
    QPainterPath ppath(pathItem->path());

    ppath.lineTo(pxx, pxy);
    pathItem->setPath(ppath);

    QRectF r(sceneRect());
    if (!r.contains(pxx, pxy))
    {
        if (r.left() > pxx)
            r.setLeft(getPxFromCellStart(getCellFromPx(pxx)));
        if (r.right() < pxx)
            r.setRight(getPxFromCellStart(getCellFromPx(pxx) + 1));
        if (r.top() > pxy)
            r.setTop(getPxFromCellStart(getCellFromPx(pxy)));
        if (r.bottom() < pxy)
            r.setBottom(getPxFromCellStart(getCellFromPx(pxy) + 1));

        setSceneRect(r);
    }
}
