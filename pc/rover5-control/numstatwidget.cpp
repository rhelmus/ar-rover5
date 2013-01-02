#include "numstatwidget.h"

#include <QHBoxLayout>
#include <QLabel>

CNumStatWidget::CNumStatWidget(const QString &l, int cols, QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *hbox = new QHBoxLayout(this);

    QLabel *label = new QLabel(l);
//    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hbox->addWidget(label/*, 0, Qt::AlignLeft*/);

    /*QWidget *vw = new QWidget;
    vw->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hbox->addWidget(vw, 0, Qt::AlignRight);
    QHBoxLayout *shbox = new QHBoxLayout(vw);*/

    for (int c=0; c<cols; ++c)
    {
        QLabel *vl = new QLabel("0");
        vl->setAlignment(Qt::AlignCenter);
        vl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        vl->setMinimumWidth(50);

        QFont font(vl->font());
        font.setPointSize(8);
        vl->setFont(font);

        QPalette pal = vl->palette();
        pal.setColor(foregroundRole(), Qt::yellow);
        pal.setColor(backgroundRole(), Qt::blue);
        vl->setPalette(pal);
        vl->setAutoFillBackground(true);

        statWidgetList << vl;
        hbox->addWidget(vl);
    }
}

void CNumStatWidget::set(int col, int val)
{
    statWidgetList[col]->setText(QString::number(val));
}
