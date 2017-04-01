#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mySocket = new QUdpSocket(this);
    QObject::connect(mySocket,&QUdpSocket::readyRead,this,&MainWindow::read);
    ui->SendPushButton->setEnabled(false);
    ui->QuitPushButton->setEnabled(false);
    ui->ConnectPushButton->setEnabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ConnectPushButton_clicked()
{
    QByteArray message("HellowWorld");
    QHostAddress address(ui->AddressLineEdit->text());
    quint16 sport(ui->ServerPortSpinBox->value());
    qint64 aux = mySocket->writeDatagram(message,address,sport);
    if(aux == -1)
    {
        QMessageBox::critical(this,tr("Client"),tr("Write socket error.\n"),QMessageBox::Ok);
    } else {
        QMessageBox::information(this,tr("Client"),tr("Message sent.\n"),QMessageBox::Ok);
    }
}

void MainWindow::read()
{
    QNetworkDatagram datagram = mySocket->receiveDatagram();
    QByteArray cmessage = datagram.data();
    if(cmessage=="ServerOkey") {
        QMessageBox::information(this,tr("Client"),tr("Message received.\n"),QMessageBox::Ok);
        ui->ConnectPushButton->setEnabled(false);
        ui->QuitPushButton->setEnabled(true);
        ui->SendPushButton->setEnabled(true);

    } else {
        QMessageBox::critical(this,tr("Client"),tr("Could not be connect to server.\n"),QMessageBox::Ok);
    }

    if (cmessage=="WaitConfirm")
    {
        QMessageBox::information(this,tr("Client"),tr("Message for wait received.\n"),QMessageBox::Ok);
    } else {
        QMessageBox::critical(this,tr("Client"),tr("Could not be sent to server.\n"),QMessageBox::Ok);
    }

    if(cmessage=="ReadyToSend")
    {
        send();
    }
}

void MainWindow::send()
{

}

void MainWindow::on_QuitPushButton_clicked()
{
    mySocket->close();
    ui->QuitPushButton->setEnabled(false);
    ui->ConnectPushButton->setEnabled(true);
}

void MainWindow::on_SAPushButton_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"/home");
    ui->SAddressLineEdit->setText(filename);
}

void MainWindow::on_DDPushButton_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"/home");
    ui->DAddressLineEdit->setText(filename);
}

void MainWindow::on_SendPushButton_clicked()
{
    QByteArray message("IWantSend 2");
    QHostAddress address(ui->AddressLineEdit->text());
    quint16 sport(ui->ServerPortSpinBox->value());
    qint64 aux = mySocket->writeDatagram(message,address,sport);

    if(ui->SAddressLineEdit->text()=="") {
        QMessageBox::information(this,tr("Client"),tr("You must selecct a directory to send.\n"),QMessageBox::Ok);

    } else if (ui->DAddressLineEdit->text()=="") {
        ui->DAddressLineEdit->setText(ui->SAddressLineEdit);

    } else {

        if(aux == -1)
        {
            QMessageBox::critical(this,tr("Client"),tr("Write socket error.\n"),QMessageBox::Ok);
        } else {
            QMessageBox::information(this,tr("Client"),tr("Message for send sent.\n"),QMessageBox::Ok);
            ui->SendPushButton->setEnabled(false);
        }
    }
}
