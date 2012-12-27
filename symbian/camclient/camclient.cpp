#include "camclient.h"
#include "../../shared/shared.h"
#include "../../shared/tcputils.h"
#include "videosurface.h"

#include <QCamera>
#include <QtGui>
#include <QSystemScreenSaver>
#include <QTcpSocket>
#include <QVideoRendererControl>

CCamClient *CCamClient::instance = 0;

CCamClient::CCamClient(QWidget *parent)
    : QMainWindow(parent), tcpReadBlockSize(0), camFrameDelay(1000 / 15)
{
    instance = this;

    setWindowTitle("Camera client");

    setAutoFillBackground(false);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    (new QtMobility::QSystemScreenSaver(this))->setScreenSaverInhibit();

    QWidget *cw = new QWidget(this);
    cw->setPalette(palette);
    setCentralWidget(cw);

    menuBar()->addAction("Connect", this, SLOT(connectToServer()));

    initCamera();
    initTcp();
}

CCamClient::~CCamClient()
{
    videoSurface->stop();
    camera->stop();
}

void CCamClient::initCamera()
{
    camera = new QCamera(this);
    camera->setCaptureMode(QCamera::CaptureVideo);

    QMediaService *ms = camera->service();
    QVideoRendererControl *vrc = ms->requestControl<QVideoRendererControl*>();
    vrc->setSurface(videoSurface = new CVideoSurface(this));

    if (camera->state() == QCamera::ActiveState)
        camera->stop();

    camera->start();
}

void CCamClient::initTcp()
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(serverHasData()));
}

void CCamClient::paintEvent(QPaintEvent *)
{
    if (videoSurface->isActive())
    {
        QPainter painter(this);
        const QImage frame(videoSurface->getFrame());
        painter.drawImage(QPoint(0, 0), frame);
//        painter.drawImage(0, 0, videoSurface->getFrame(), 0, 0, width(), height());
    }
}

void CCamClient::updateVideo()
{
    if ((tcpSocket->state() == QTcpSocket::ConnectedState) &&
        (lastFrameTime.isNull() || (lastFrameTime.elapsed() > camFrameDelay)))
    {
        lastFrameTime.start();

        QBuffer imbuffer;
        QImageWriter imwriter(&imbuffer, "jpg");
        imwriter.write(videoSurface->getFrame());

        CTcpMsgComposer tcpMsg(MSG_CAMFRAME);
        tcpMsg << imbuffer.buffer();
        tcpSocket->write(tcpMsg);
        tcpSocket->flush();
    }

    repaint();
}

void CCamClient::connectToServer()
{
    tcpSocket->abort();
    tcpSocket->connectToHost("192.168.1.40", 40000);
}

void CCamClient::serverHasData()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_7);

    while (true)
    {
        if (tcpReadBlockSize == 0)
        {
            if (tcpSocket->bytesAvailable() < (int)sizeof(quint32))
                return;

            in >> tcpReadBlockSize;
        }

        if (tcpSocket->bytesAvailable() < tcpReadBlockSize)
            return;

        parseTcp(in);
        tcpReadBlockSize = 0;
    }
}

void CCamClient::parseTcp(QDataStream &stream)
{
    QString msg;
    stream >> msg;

//    QMessageBox::information(this, "heuh", QString("Received msg: %1 (%2 bytes)\n").arg(msg).arg(tcpReadBlockSize));
}
