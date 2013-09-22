#include "drivewidget.h"
#include "../../control-base/utils.h"

#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QSpinBox>
#include <QTimer>


namespace {

QPushButton *createDriveButton(const QIcon &icon)
{
    QPushButton *ret = new QPushButton;
    ret->setIconSize(QSize(65, 65));
    ret->setIcon(icon);
    ret->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ret->setAutoRepeat(true);
    ret->setAutoRepeatInterval(200);
    ret->setCheckable(true);

    return ret;
}


}

CDriveWidget::CDriveWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout(this);

    driveButtonSignalMapper = new QSignalMapper(this);
    connect(driveButtonSignalMapper, SIGNAL(mapped(QWidget *)),
            SLOT(updateDriveButtonStates(QWidget *)));

    hbox->addWidget(createSteerPad());
    hbox->addWidget(createSpeedWidget());
    hbox->addWidget(createDrivePad());

    driveUpdateTimer = new QTimer(this);
    driveUpdateTimer->setInterval(200);
    connect(driveUpdateTimer, SIGNAL(timeout()), SLOT(sendDriveUpdate()));
}

QWidget *CDriveWidget::createSteerPad()
{
    QFrame *ret = new QFrame;
    ret->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    ret->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QHBoxLayout *hbox = new QHBoxLayout(ret);

    hbox->addWidget(leftButton = createDriveButton(QIcon(":/resources/arrowleft.png")));
    connect(leftButton, SIGNAL(clicked()), SLOT(enableUpdateDriveTimer()));
    connect(leftButton, SIGNAL(clicked()), driveButtonSignalMapper, SLOT(map()));
    driveButtonSignalMapper->setMapping(leftButton, leftButton);

    hbox->addWidget(rightButton = createDriveButton(QIcon(":/resources/arrowright.png")));
    connect(rightButton, SIGNAL(clicked()), SLOT(enableUpdateDriveTimer()));
    connect(rightButton, SIGNAL(clicked()), driveButtonSignalMapper, SLOT(map()));
    driveButtonSignalMapper->setMapping(rightButton, rightButton);

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

QWidget *CDriveWidget::createDrivePad()
{
    QFrame *ret = new QFrame;
    ret->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    ret->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout *vbox = new QVBoxLayout(ret);

    vbox->addWidget(fwdButton = createDriveButton(QIcon(":/resources/car-fwd.png")));
    connect(fwdButton, SIGNAL(clicked()), SLOT(enableUpdateDriveTimer()));
    connect(fwdButton, SIGNAL(clicked()), driveButtonSignalMapper, SLOT(map()));
    driveButtonSignalMapper->setMapping(fwdButton, fwdButton);

    vbox->addWidget(bwdButton = createDriveButton(QIcon(":/resources/car-bwd.png")));
    connect(bwdButton, SIGNAL(clicked()), SLOT(enableUpdateDriveTimer()));
    connect(bwdButton, SIGNAL(clicked()), driveButtonSignalMapper, SLOT(map()));
    driveButtonSignalMapper->setMapping(bwdButton, bwdButton);

    return ret;
}

void CDriveWidget::enableUpdateDriveTimer()
{
    if (!driveUpdateTimer->isActive())
        driveUpdateTimer->start();
}

void CDriveWidget::updateDriveButtonStates(QWidget *ob)
{
    if (ob == leftButton)
        rightButton->setChecked(false);
    else if (ob == rightButton)
        leftButton->setChecked(false);
    else if (ob == fwdButton)
        bwdButton->setChecked(false);
    else if (ob == bwdButton)
        fwdButton->setChecked(false);
}

void CDriveWidget::sendDriveUpdate()
{
    qDebug() << "d update";

    DriveFlags df = DRIVE_NONE;

    if (leftButton->isChecked())
        df |= DRIVE_LEFT;
    else if (rightButton->isChecked())
        df |= DRIVE_RIGHT;

    if (fwdButton->isChecked())
        df |= DRIVE_FWD;
    else if (bwdButton->isChecked())
        df |= DRIVE_BWD;

    emit driveUpdate(df, motorPowerSpinBox->value());

    if (df == DRIVE_NONE)
        driveUpdateTimer->stop();
}
