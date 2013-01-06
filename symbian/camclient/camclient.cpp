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
    : QMainWindow(parent), camera(0), videoSurface(0), tcpReadBlockSize(0),
      camFrameDelay(1000 / 15), lastServerPort(0)
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
    menuBar()->addAction("Connect to ip", this, SLOT(connectToServerIP()));

    //initCamera();
    // Delayed cam init for correct landscape view
    QTimer::singleShot(1000, this, SLOT(initCamera()));

    initTcp();
}

CCamClient::~CCamClient()
{
    if (camera)
    {
        videoSurface->stop();
        camera->stop();
    }
}

void CCamClient::initTcp()
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(connected()), SLOT(handleConnected()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(serverHasData()));

    tcpReconnectTimer = new QTimer(this);
    tcpReconnectTimer->setInterval(2500);
    connect(tcpReconnectTimer, SIGNAL(timeout()), SLOT(tryReconnect()));
    connect(tcpSocket, SIGNAL(disconnected()), tcpReconnectTimer, SLOT(start()));
    connect(tcpSocket, SIGNAL(connected()), tcpReconnectTimer, SLOT(stop()));
}

void CCamClient::paintEvent(QPaintEvent *)
{
    if (videoSurface && videoSurface->isActive())
    {
        QPainter painter(this);
        const QImage frame(videoSurface->getFrame());
        painter.drawImage(QPoint(0, 0), frame);
//        painter.drawImage(0, 0, videoSurface->getFrame(), 0, 0, width(), height());
    }
}

void CCamClient::updateVideo()
{
    if (camera && (tcpSocket->state() == QTcpSocket::ConnectedState) &&
        (lastFrameTime.isNull() || (lastFrameTime.elapsed() > camFrameDelay)))
    {
        QBuffer imbuffer;
        QImageWriter imwriter(&imbuffer, "jpg");
        imwriter.write(videoSurface->getFrame());

        CTcpMsgComposer tcpmsg(MSG_CAMFRAME);
        tcpmsg << imbuffer.buffer();
        tcpSocket->write(tcpmsg);
        tcpSocket->flush();

        lastFrameTime.start();
    }

    repaint();
}

void CCamClient::initCamera()
{
    camera = new QCamera(this);
    camera->setCaptureMode(QCamera::CaptureStillImage);

    QMediaService *ms = camera->service();
    QVideoRendererControl *vrc = ms->requestControl<QVideoRendererControl*>();
    vrc->setSurface(videoSurface = new CVideoSurface(this));

    if (camera->state() == QCamera::ActiveState)
        camera->stop();

    camera->start();
}

void CCamClient::connectToServer()
{
    tcpSocket->abort();
    tcpSocket->connectToHost("192.168.1.40", 40000);
}

void CCamClient::connectToServerIP()
{
    QDialog dialog(this);
    dialog.setModal(true);

    QVBoxLayout *vbox = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel("IP address");
    vbox->addWidget(label);

    QLineEdit *ipedit = new QLineEdit("192.168.1.66");
    ipedit->setInputMask("000.000.000.000;_");
    vbox->addWidget(ipedit);

    QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                  QDialogButtonBox::Cancel);
    connect(bbox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(bbox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    vbox->addWidget(bbox);

    if (dialog.exec() == QDialog::Accepted)
    {
        tcpSocket->abort();
        tcpSocket->connectToHost(ipedit->text(), 40000);
    }
}

void CCamClient::handleConnected()
{
    lastServerAddress = tcpSocket->peerAddress();
    lastServerPort = tcpSocket->peerPort();
}

void CCamClient::serverHasData()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_7);

    while (true)
    {
        if (tcpReadBlockSize == 0)
        {
            if (tcpSocket->bytesAvailable() < (int)sizeof(uint32_t))
                return;

            in >> tcpReadBlockSize;
        }

        if (tcpSocket->bytesAvailable() < tcpReadBlockSize)
            return;

        parseTcp(in);
        tcpReadBlockSize = 0;
    }
}

void CCamClient::tryReconnect()
{
    if (tcpSocket->state() == QTcpSocket::UnconnectedState)
    {
        if (!lastServerAddress.isNull() && (lastServerPort != 0))
        {
            tcpSocket->abort();
            tcpSocket->connectToHost(lastServerAddress, lastServerPort);
        }
    }
    else
        tcpReconnectTimer->stop();
}

void CCamClient::parseTcp(QDataStream &stream)
{
    uint8_t tmp;
    stream >> tmp;

    const EMessage msg = static_cast<EMessage>(tmp);

    if (msg == MSG_SETZOOM)
    {
        qreal z;
        stream >> z;
        camera->focus()->zoomTo(1.0, z);
//        QMessageBox::information(this, "heuh", QString("Cam zoom: %1/%2/%3").arg(z).arg(camera->focus()->maximumDigitalZoom()).arg(camera->focus()->maximumOpticalZoom()));
    }
    else // Unknown data
    {
        stream.skipRawData(tcpReadBlockSize-1);
    }
//    QMessageBox::information(this, "heuh", QString("Received msg: %1 (%2 bytes)\n").arg(msg).arg(tcpReadBlockSize));
}
