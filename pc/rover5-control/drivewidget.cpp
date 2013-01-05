#include "drivewidget.h"
#include "utils.h"

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QStyle>
#include <QTimer>


CDriveWidget::CDriveWidget(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::StrongFocus);

    QHBoxLayout *hbox = new QHBoxLayout(this);

    hbox->addWidget(createKeypad());
    hbox->addWidget(createContinuousDriveWidget());
    hbox->addWidget(createSpeedWidget());
}

QWidget *CDriveWidget::createKeypad()
{
    QFrame *ret = new QFrame;
    ret->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QVBoxLayout *vbox = new QVBoxLayout(ret);

    CDriveKeypad *keypad = new CDriveKeypad;
    vbox->addWidget(keypad);
    connect(keypad, SIGNAL(driveUpdate(CDriveWidget::DriveFlags)),
            SLOT(sendDriveUpdate(CDriveWidget::DriveFlags)));

    return ret;
}

QWidget *CDriveWidget::createContinuousDriveWidget()
{
    QFrame *ret = new QFrame;
    ret->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QGridLayout *grid = new QGridLayout(ret);

    grid->addWidget(contDriveModeCombo = new QComboBox, 0, 0);
    contDriveModeCombo->addItems(QStringList() << "Drive FWD" << "Drive BWD" <<
                                                  "Turn left" << "Turn right");
    connect(contDriveModeCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(updateContDriveMode(int)));

    grid->addWidget(contDriveSpinBox = new QSpinBox, 0, 1);

    grid->addWidget(contDriveDurationCombo = new QComboBox, 0, 2);
    contDriveDurationCombo->addItems(QStringList() << "Continuously" <<
                                     "centimeter" << "seconds");
    connect(contDriveDurationCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(updateContDriveDuration(int)));

    QPushButton *button = new QPushButton("Go!");
    connect(button, SIGNAL(clicked()), SLOT(sendContDrive()));
    grid->addWidget(button, 0, 3);

    grid->addWidget(button = new QPushButton("Stop"), 1, 3);
    connect(button, SIGNAL(clicked()), SIGNAL(stopDriveReq()));

    // Apply initial settings
    updateContDriveMode(contDriveModeCombo->currentIndex());
    updateContDriveDuration(contDriveDurationCombo->currentIndex());

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
    slider->setRange(MIN_POWER, MAX_POWER);
    vbox->addWidget(slider, 0, Qt::AlignHCenter);

    vbox->addWidget(motorPowerSpinBox = new QSpinBox);
    motorPowerSpinBox->setRange(MIN_POWER, MAX_POWER);

    connect(slider, SIGNAL(valueChanged(int)), motorPowerSpinBox, SLOT(setValue(int)));
    connect(motorPowerSpinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));

    return ret;
}

void CDriveWidget::sendDriveUpdate(CDriveWidget::DriveFlags df)
{
    emit driveUpdate(df, motorPowerSpinBox->value());
}

void CDriveWidget::updateContDriveMode(int index)
{
    if (index < 2)
    {
        contDriveDurationCombo->setItemText(1, "centimeters");
        contDriveSpinBox->setMaximum(10000);
        contDriveSpinBox->setWrapping(false);
    }
    else
    {
        contDriveDurationCombo->setItemText(1, "degrees");
        contDriveSpinBox->setMaximum(359);
        contDriveSpinBox->setWrapping(true);
    }
}

void CDriveWidget::updateContDriveDuration(int index)
{
    contDriveSpinBox->setEnabled(index != 0);
}

void CDriveWidget::sendContDrive()
{
    if (contDriveModeCombo->currentIndex() < 2) // Drive FWD/BWD
    {
        const EMotorDirection dir =
                (contDriveModeCombo->currentIndex() == 0) ? DIR_FWD : DIR_BWD;

        if (contDriveDurationCombo->currentIndex() == 0) // continuous
            emit driveContReq(motorPowerSpinBox->value(), 0, dir);
        else if (contDriveDurationCombo->currentIndex() == 1) // distance
            emit driveDistReq(motorPowerSpinBox->value(), contDriveSpinBox->value(), dir);
        else // drive for a specified time
            emit driveContReq(motorPowerSpinBox->value(), contDriveSpinBox->value(), dir);

    }
    else // Rotate left/right
    {
        const ETurnDirection dir =
                (contDriveModeCombo->currentIndex() == 2) ? DIR_LEFT : DIR_RIGHT;

        if (contDriveDurationCombo->currentIndex() == 0) // continuous
            emit turnContReq(motorPowerSpinBox->value(), 0, dir);
        else if (contDriveDurationCombo->currentIndex() == 1) // turn angle
            emit turnAngleReq(motorPowerSpinBox->value(), contDriveSpinBox->value(), dir);
        else // turn for a specified time
            emit turnContReq(motorPowerSpinBox->value(), contDriveSpinBox->value(), dir);
    }
}

void CDriveWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Plus)
        motorPowerSpinBox->stepBy(5);
    else if (event->key() == Qt::Key_Minus)
        motorPowerSpinBox->stepBy(-5);
    else
        QWidget::keyPressEvent(event);
}


CDriveKeypad::CDriveKeypad(QWidget *parent) : QWidget(parent)
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
    CDriveWidget::DriveFlags dir =CDriveWidget:: DRIVE_NONE;

    if (driveButtons[BUTTON_FWD]->isDown())
        dir |= CDriveWidget::DRIVE_FWD;
    if (driveButtons[BUTTON_BWD]->isDown())
        dir |= CDriveWidget::DRIVE_BWD;
    if (driveButtons[BUTTON_LEFT]->isDown())
        dir |= CDriveWidget::DRIVE_LEFT;
    if (driveButtons[BUTTON_RIGHT]->isDown())
        dir |= CDriveWidget::DRIVE_RIGHT;

    emit driveUpdate(dir);

    qDebug() << "update drive:" << dir;

    if (dir == CDriveWidget::DRIVE_NONE)
        updateTimer->stop();
}

void CDriveKeypad::keyPressEvent(QKeyEvent *event)
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
