#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

QFrame *createFrameGroupWidget(const QString &title, bool sunken)
{
    QFrame *ret = new QFrame;
    ret->setFrameStyle(QFrame::StyledPanel | ((sunken) ? QFrame::Sunken : QFrame::Raised));
    ret->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout *vbox = new QVBoxLayout(ret);
    vbox->setSpacing(0);
    vbox->setSpacing(0);

    QLabel *label = new QLabel(title);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    label->setAlignment(Qt::AlignCenter);
    vbox->addWidget(label, 0, Qt::AlignTop);

    return ret;
}
