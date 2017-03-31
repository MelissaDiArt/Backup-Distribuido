#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mySocket = new QUdpSocket(this);
    QObject::connect(mySocket,&QUdpSocket::readyRead,this,&MainWindow::read);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ConnectPushButton_clicked()
{
    ui->QuitPushButton->setEnabled(false);
    QByteArray message("HellowWorld");
    QHostAddress address(ui->AddressLineEdit->text());
    quint16 sport(ui->ServerPortSpinBox->value());
    qint64 aux = mySocket->writeDatagram(message,address,sport);
    if(aux == -1){
        QMessageBox::critical(this,tr("Client"),tr("Write socket error.\n"),QMessageBox::Ok);
    } else {
        QMessageBox::information(this,tr("Client"),tr("Message sent.\n"),QMessageBox::Ok);
        ui->ConnectPushButton->setEnabled(false);
        ui->QuitPushButton->setEnabled(true);
    }
}

void MainWindow::read()
{
    QNetworkDatagram datagram = mySocket->receiveDatagram();
    QByteArray cmessage = datagram.data();
    if(cmessage=="ServerOkey") {
        QMessageBox::information(this,tr("Client"),tr("Message received.\n"),QMessageBox::Ok);
    } else {
        QMessageBox::critical(this,tr("Client"),tr("Could not be connect to server.\n"),QMessageBox::Ok);
    }
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
