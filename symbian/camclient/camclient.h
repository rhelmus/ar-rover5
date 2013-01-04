#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

class QCamera;
class QTcpSocket;

class CVideoSurface;

class CCamClient : public QMainWindow
{
    Q_OBJECT

    static CCamClient *instance;

    QCamera *camera;
    CVideoSurface *videoSurface;
    uint32_t tcpReadBlockSize;
    uint32_t camFrameDelay;
    QTime lastFrameTime;
    QTcpSocket *tcpSocket;

    void initTcp(void);
    void parseTcp(QDataStream &stream);

private slots:
    void initCamera(void);
    void connectToServer(void);
    void serverHasData(void);

protected:
    virtual void paintEvent(QPaintEvent *);

public:
    explicit CCamClient(QWidget *parent = 0);
    virtual ~CCamClient();

    static CCamClient *getInstance(void) { return instance; }

    void updateVideo(void);
};

#endif // MAINWINDOW_H
