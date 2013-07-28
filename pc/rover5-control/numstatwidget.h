#ifndef NUMSTATWIDGET_H
#define NUMSTATWIDGET_H

#include <QWidget>

class QLabel;

class CNumStatWidget : public QWidget
{
    QList<QLabel *> statWidgetList;

public:
    CNumStatWidget(const QString &l, int cols, QWidget *parent = 0);

    void set(int col, int val);
    void setF(int col, float val);
};

#endif // NUMSTATWIDGET_H
