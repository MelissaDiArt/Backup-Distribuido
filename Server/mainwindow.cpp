#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ConnectPushButton_clicked()
{
    mySocket = new QUdpSocket(this);
    bool aux = mySocket-> bind(QHostAddress::LocalHost, ui->PortSpinBox->value());
    if (aux)
    {
        QMessageBox::information(this,tr("Server"),tr("Established connection.\n"),QMessageBox::Ok);
    }else{
        QMessageBox::critical(this,tr("Server"),tr("Unable to connect.\n"),QMessageBox::Ok);
    }

}

