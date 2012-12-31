#ifndef DRIVEWIDGET_H
#define DRIVEWIDGET_H

#include <QWidget>

class QPushButton;

class CDriveWidget : public QWidget
{
public:
    enum EDriveDirection
    {
        DRIVE_NONE = (1<<0),
        DRIVE_FWD = (1<<1),
        DRIVE_BWD = (1<<2),
        DRIVE_LEFT = (1<<3),
        DRIVE_RIGHT = (1<<4)
    };
    Q_DECLARE_FLAGS(DriveFlags, EDriveDirection)

private:
    Q_OBJECT

    enum { BUTTON_FWD=0, BUTTON_BWD, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_END };

    QPushButton *driveButtons[BUTTON_END];

    QPushButton *createDriveButton(const QIcon &icon);
    bool canPressKey(int key) const;
    void updateDriveDir(void);

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

public:
    explicit CDriveWidget(QWidget *parent = 0);
    
signals:
    void directionChanged(DriveFlags);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CDriveWidget::DriveFlags)

#endif // DRIVEWIDGET_H
