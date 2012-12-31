#include "drivewidget.h"

#include <QDebug>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QStyle>

CDriveWidget::CDriveWidget(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::StrongFocus);
    QGridLayout *grid = new QGridLayout(this);


    driveButtons[BUTTON_FWD] =
            createDriveButton(style()->standardIcon(QStyle::SP_ArrowUp));
    grid->addWidget(driveButtons[BUTTON_FWD], 0, 1);

    driveButtons[BUTTON_BWD] =
            createDriveButton(style()->standardIcon(QStyle::SP_ArrowDown));
    grid->addWidget(driveButtons[BUTTON_BWD], 1, 1);

    driveButtons[BUTTON_LEFT] =
            createDriveButton(style()->standardIcon(QStyle::SP_ArrowLeft));
    grid->addWidget(driveButtons[BUTTON_LEFT], 1, 0);

    driveButtons[BUTTON_RIGHT] =
            createDriveButton(style()->standardIcon(QStyle::SP_ArrowRight));
    grid->addWidget(driveButtons[BUTTON_RIGHT], 1, 2);
}

QPushButton *CDriveWidget::createDriveButton(const QIcon &icon)
{
    QPushButton *ret = new QPushButton;
    ret->setIcon(icon);
    ret->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    return ret;
}

bool CDriveWidget::canPressKey(int key) const
{
    if ((key == Qt::Key_Up) && (driveButtons[BUTTON_BWD]->isDown()))
        return false;
    if ((key == Qt::Key_Down) && (driveButtons[BUTTON_FWD]->isDown()))
        return false;
    if ((key == Qt::Key_Left) && (driveButtons[BUTTON_RIGHT]->isDown()))
        return false;
    if ((key == Qt::Key_Right) && (driveButtons[BUTTON_LEFT]->isDown()))
        return false;

    return true;
}

void CDriveWidget::updateDriveDir()
{
    DriveFlags dir = DRIVE_NONE;

    if (driveButtons[BUTTON_FWD]->isDown())
        dir |= DRIVE_FWD;
    if (driveButtons[BUTTON_BWD]->isDown())
        dir |= DRIVE_BWD;
    if (driveButtons[BUTTON_LEFT]->isDown())
        dir |= DRIVE_LEFT;
    if (driveButtons[BUTTON_RIGHT]->isDown())
        dir |= DRIVE_RIGHT;

    emit directionChanged(dir);
}

void CDriveWidget::keyPressEvent(QKeyEvent *event)
{
    if (!canPressKey(event->key()))
        return;

    if (event->key() == Qt::Key_Up)
        driveButtons[BUTTON_FWD]->setDown(true);
    else if (event->key() == Qt::Key_Down)
        driveButtons[BUTTON_BWD]->setDown(true);
    else if (event->key() == Qt::Key_Left)
        driveButtons[BUTTON_LEFT]->setDown(true);
    else if (event->key() == Qt::Key_Right)
        driveButtons[BUTTON_RIGHT]->setDown(true);
    else
        QWidget::keyPressEvent(event);
}

void CDriveWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
        driveButtons[BUTTON_FWD]->setDown(false);
    else if (event->key() == Qt::Key_Down)
        driveButtons[BUTTON_BWD]->setDown(false);
    else if (event->key() == Qt::Key_Left)
        driveButtons[BUTTON_LEFT]->setDown(false);
    else if (event->key() == Qt::Key_Right)
        driveButtons[BUTTON_RIGHT]->setDown(false);
    else
        QWidget::keyReleaseEvent(event);
}
