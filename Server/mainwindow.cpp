#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mySocket = new QUdpSocket(this);
    QObject::connect(mySocket,&QUdpSocket::readyRead,this,&MainWindow::read);
    ui->QuitPushButton->setEnabled(false);
    clientnumber=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_ConnectPushButton_clicked()
{
    bool aux = mySocket-> bind(QHostAddress::LocalHost, ui->PortSpinBox->value());
    if (aux)
    {
        QMessageBox::information(this,tr("Server"),tr("Established connection.\n"),QMessageBox::Ok);
        ui->ConnectPushButton->setEnabled(false);
        ui->QuitPushButton->setEnabled(true);

    }else{       
        QMessageBox::critical(this,tr("Server"),tr("Unable to connect.\n"),QMessageBox::Ok);
    }
}

void MainWindow::read()
{
    QNetworkDatagram datagram = mySocket->receiveDatagram();
    QHostAddress clientaddress = datagram.senderAddress();
    quint16 clientport = datagram.senderPort();
    QByteArray cmessage = datagram.data();
    QByteArray okmessage("ServerOkey");
    QByteArray waitmessage("WaitConfirm");
    QByteArray readymessage("ReadyToSend");
    if(cmessage=="HellowWorld")
    {
        mySocket->writeDatagram(okmessage,clientaddress,clientport);
        QMessageBox::information(this,tr("Server"),tr("Message received.\n"),QMessageBox::Ok);
        clientnumber+=1;

    } else if(cmessage.startsWith("IWantSend")){

        mySocket->writeDatagram(waitmessage,clientaddress,clientport);
        QMessageBox::information(this,tr("Server"),tr("Message for send received.\n"),QMessageBox::Ok);
        cmessage.remove(0,9);
        if(cmessage.toInt()==clientnumber){
            mySocket->writeDatagram(readymessage,clientaddress,clientport);
        }
    }

}

void MainWindow::on_QuitPushButton_clicked()
{
    mySocket->close();
    ui->QuitPushButton->setEnabled(false);
    ui->ConnectPushButton->setEnabled(true);
}
