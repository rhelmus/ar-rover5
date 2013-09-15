#include "drivewidget.h"
#include "../../control-base/utils.h"

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QStyle>
#include <QTimer>
#include <QToolButton>


CDriveWidget::CDriveWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout(this);

    hbox->addWidget(createKeypad());
    hbox->addWidget(createSpeedWidget());
    hbox->addWidget(createDriveWidget());
}

QWidget *CDriveWidget::createKeypad()
{
    QFrame *ret = new QFrame;
    ret->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QHBoxLayout *hbox = new QHBoxLayout(ret);

    QPushButton *but = new QPushButton;
    but->setIconSize(QSize(50, 50));
    but->setIcon(QIcon(":/resources/arrowleft.png"));
    hbox->addWidget(but);

    hbox->addWidget(but = new QPushButton);
    but->setIconSize(QSize(50, 50));
    but->setIcon(QIcon(":/resources/arrowright.png"));

    return ret;
}

QWidget *CDriveWidget::createSpeedWidget()
{
    QWidget *ret = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(ret);

    QLabel *label = new QLabel("Speed");
    label->setAlignment(Qt::AlignCenter);
    vbox->addWidget(label);

    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(MIN_MOTOR_POWER, MAX_MOTOR_POWER);
    slider->setValue(70);
    vbox->addWidget(slider, 0, Qt::AlignHCenter);

    vbox->addWidget(motorPowerSpinBox = new QSpinBox);
    motorPowerSpinBox->setRange(MIN_MOTOR_POWER, MAX_MOTOR_POWER);
    motorPowerSpinBox->setValue(slider->value());

    connect(slider, SIGNAL(valueChanged(int)), motorPowerSpinBox, SLOT(setValue(int)));
    connect(motorPowerSpinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));

    return ret;
}

QWidget *CDriveWidget::createDriveWidget()
{
    QFrame *ret = new QFrame;
    ret->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *vbox = new QVBoxLayout(ret);

    QPushButton *but = new QPushButton;
    but->setIconSize(QSize(65, 65));
    but->setIcon(QIcon(":resources/car-fwd.png"));
    vbox->addWidget(but);

    vbox->addWidget(but = new QPushButton);
    but->setIconSize(QSize(65, 65));
    but->setIcon(QIcon(":resources/car-bwd.png"));

    return ret;
}

void CDriveWidget::sendDriveUpdate(CDriveWidget::DriveFlags df)
{
    emit driveUpdate(df, motorPowerSpinBox->value());
}


CDriveKeypad::CDriveKeypad(QWidget *parent) : QWidget(parent)
#ifdef MECANUM_MOVEMENT
    , controlPressed(false)
#endif
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::StrongFocus);
    QGridLayout *grid = new QGridLayout(this);


    driveButtons[BUTTON_FWD] = createDriveButton(QIcon(":resources/arrowup.png"));
    grid->addWidget(driveButtons[BUTTON_FWD], 0, 1);

    driveButtons[BUTTON_BWD] = createDriveButton(QIcon(":resources/arrowdown.png"));
    grid->addWidget(driveButtons[BUTTON_BWD], 1, 1);

    driveButtons[BUTTON_LEFT] = createDriveButton(QIcon(":resources/arrowleft.png"));
    grid->addWidget(driveButtons[BUTTON_LEFT], 1, 0);

    driveButtons[BUTTON_RIGHT] = createDriveButton(QIcon(":resources/arrowright.png"));
    grid->addWidget(driveButtons[BUTTON_RIGHT], 1, 2);


    // Make sure buttons don't steal (keyboard) focus
    for (int i=0; i<BUTTON_END; ++i)
        driveButtons[i]->setFocusPolicy(Qt::NoFocus);

    updateTimer = new QTimer(this);
    updateTimer->setInterval(200);
    connect(updateTimer, SIGNAL(timeout()), SLOT(updateDriveDir()));
}

QPushButton *CDriveKeypad::createDriveButton(const QIcon &icon)
{
    QPushButton *ret = new QPushButton;
    ret->setIcon(icon);
    ret->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ret->setIconSize(QSize(35, 35));
    return ret;
}

bool CDriveKeypad::canPressKey(int key) const
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

void CDriveKeypad::updateDriveDir()
{
    CDriveWidget::DriveFlags dir = CDriveWidget::DRIVE_NONE;

    if (driveButtons[BUTTON_FWD]->isDown())
        dir |= CDriveWidget::DRIVE_FWD;
    if (driveButtons[BUTTON_BWD]->isDown())
        dir |= CDriveWidget::DRIVE_BWD;
    if (driveButtons[BUTTON_LEFT]->isDown())
        dir |= CDriveWidget::DRIVE_LEFT;
    if (driveButtons[BUTTON_RIGHT]->isDown())
        dir |= CDriveWidget::DRIVE_RIGHT;
#ifdef MECANUM_MOVEMENT
    if (controlPressed && (dir != CDriveWidget::DRIVE_NONE))
        dir |= CDriveWidget::DRIVE_TRANSLATE;
#endif

    emit driveUpdate(dir);

    qDebug() << "update drive:" << dir;

    if (dir == CDriveWidget::DRIVE_NONE)
        updateTimer->stop();
}

void CDriveKeypad::keyPressEvent(QKeyEvent *event)
{
    if (!canPressKey(event->key()))
        return;

#ifdef MECANUM_MOVEMENT
    controlPressed = (event->modifiers() & Qt::ControlModifier);
#endif

    if (event->key() == Qt::Key_Up)
        driveButtons[BUTTON_FWD]->setDown(true);
    else if (event->key() == Qt::Key_Down)
        driveButtons[BUTTON_BWD]->setDown(true);
    else if (event->key() == Qt::Key_Left)
        driveButtons[BUTTON_LEFT]->setDown(true);
    else if (event->key() == Qt::Key_Right)
        driveButtons[BUTTON_RIGHT]->setDown(true);
    else
    {
        QWidget::keyPressEvent(event);
        return;
    }

    if (!updateTimer->isActive())
        updateTimer->start();
}

void CDriveKeypad::keyReleaseEvent(QKeyEvent *event)
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
