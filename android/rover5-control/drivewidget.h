#ifndef DRIVEWIDGET_H
#define DRIVEWIDGET_H

#include "../../shared/shared.h"

#include <QWidget>

class QComboBox;
class QPushButton;
class QSignalMapper;
class QSpinBox;
class QTimer;

class CDriveWidget : public QWidget
{
public:
    enum EDriveDirection
    {
        DRIVE_NONE = 0,
        DRIVE_FWD = (1<<0),
        DRIVE_BWD = (1<<1),
        DRIVE_LEFT = (1<<2),
        DRIVE_RIGHT = (1<<3)
#ifdef MECANUM_MOVEMENT
        , DRIVE_TRANSLATE = (1<<4)
#endif
    };
    Q_DECLARE_FLAGS(DriveFlags, EDriveDirection)

private:
    Q_OBJECT

    QSpinBox *motorPowerSpinBox;
    QSignalMapper *driveButtonSignalMapper;
    QPushButton *leftButton, *rightButton, *fwdButton, *bwdButton;
    QTimer *driveUpdateTimer;

    QWidget *createSteerPad(void);
    QWidget *createSpeedWidget(void);
    QWidget *createDrivePad(void);

private slots:
    void enableUpdateDriveTimer(void);
    void updateDriveButtonStates(QWidget *ob);
    void sendDriveUpdate(void);

public:
    explicit CDriveWidget(QWidget *parent = 0);

signals:
    void driveUpdate(CDriveWidget::DriveFlags, int);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CDriveWidget::DriveFlags)

#endif // DRIVEWIDGET_H
