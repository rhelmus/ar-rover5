#ifndef DRIVEWIDGET_H
#define DRIVEWIDGET_H

#include "../../shared/shared.h"

#include <QWidget>

class QComboBox;
class QPushButton;
class QSpinBox;

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

    QWidget *createKeypad(void);
    QWidget *createSpeedWidget(void);
    QWidget *createDriveWidget(void);

private slots:
    void sendDriveUpdate(CDriveWidget::DriveFlags df);

public:
    explicit CDriveWidget(QWidget *parent = 0);

signals:
    void driveUpdate(CDriveWidget::DriveFlags, int);
    void driveContReq(int, int, EMotorDirection);
    void driveDistReq(int, int, EMotorDirection);
    void turnContReq(int, int, ETurnDirection);
    void turnAngleReq(int, int, ETurnDirection);
    void stopDriveReq(void);
};

class CDriveKeypad : public QWidget
{
    Q_OBJECT

    enum { BUTTON_FWD=0, BUTTON_BWD, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_END };

    QPushButton *driveButtons[BUTTON_END];
    QTimer *updateTimer;
#ifdef MECANUM_MOVEMENT
    bool controlPressed;
#endif

    QPushButton *createDriveButton(const QIcon &icon);
    bool canPressKey(int key) const;

private slots:
    void updateDriveDir(void);

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

public:
    explicit CDriveKeypad(QWidget *parent = 0);
    
signals:
    void driveUpdate(CDriveWidget::DriveFlags);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CDriveWidget::DriveFlags)

#endif // DRIVEWIDGET_H
