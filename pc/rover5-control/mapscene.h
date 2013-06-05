#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QGraphicsScene>

class CMapScene : public QGraphicsScene
{
    Q_OBJECT

    QGraphicsPathItem *pathItem;

protected:
    void drawBackground(QPainter *painter, const QRectF &rect);

public:
    explicit CMapScene(QObject *parent = 0);

    void addPosition(qreal x, qreal y);
};

#endif // MAPSCENE_H
