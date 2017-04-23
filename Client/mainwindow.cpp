#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mySocket = new QUdpSocket(this);
    QObject::connect(mySocket,&QUdpSocket::readyRead,this,&MainWindow::read);
    ui->AddressLineEdit->setText("127.0.0.1");
    ui->ConnectPushButton->setEnabled(true);
    ui->SendPushButton->setEnabled(false);
    ui->QuitPushButton->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//Boton para establecer la conexion.

void MainWindow::on_ConnectPushButton_clicked()
{
    QByteArray message("HellowWorld");
    QHostAddress address(ui->AddressLineEdit->text());
    quint16 sport(ui->ServerPortSpinBox->value());

    if((ui->ClientPortSpinBox->value()!=sport)||(QHostAddress::LocalHost!=ui->AddressLineEdit->text())) {
        mySocket->bind(QHostAddress::LocalHost, ui->ClientPortSpinBox->value());

        qint64 aux = mySocket->writeDatagram(message,address,sport);

        if(aux == -1)
        {
            QMessageBox::critical(this,tr("Client"),tr("Write socket error.\n"),QMessageBox::Ok);

        } else {

            QMessageBox::information(this,tr("Client"),tr("Message sent.\n"),QMessageBox::Ok);
        }

    } else {

        QMessageBox::information(this,tr("Client"),tr("Can't be connected.\n"),QMessageBox::Ok);
    }
}

void MainWindow::read()
{
    QNetworkDatagram datagram = mySocket->receiveDatagram();
    QByteArray cmessage = datagram.data();
    if(cmessage=="ServerOkey")
    {
        QMessageBox::information(this,tr("Client"),tr("Message received.\n"),QMessageBox::Ok);
        ui->ConnectPushButton->setEnabled(false);
        ui->QuitPushButton->setEnabled(true);

    } else if (cmessage=="WaitConfirm") {
        QMessageBox::information(this,tr("Client"),tr("Message for wait received.\n"),QMessageBox::Ok);

    } else if(cmessage=="ReadyToSend") {
        QDir Dir(ui->SAddressLineEdit->text());
        if(Dir.exists())
        {
            send();
        } else {
            QMessageBox::information(this,tr("Client"), tr("Directory doesn't exisits\n"),QMessageBox::Ok);
        }

    } else {
        QMessageBox::information(this,tr("Client"),cmessage,QMessageBox::Ok);
    }
}

void MainWindow::send()
{
    QString Dir(ui->SAddressLineEdit->text());
    QString Dir_= Dir;
    quint16 sport(ui->ServerPortSpinBox->value());
    QHostAddress address(ui->AddressLineEdit->text());
    QDirIterator iterator(Dir,QDir::NoDot  | QDir::NoDotDot | QDir::Hidden | QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);

    qint16 sended = mySocket->writeDatagram(QByteArray("First D:").append(Dir_.remove(0,Dir_.lastIndexOf("/")+1)),address,sport);

    while(iterator.hasNext())
    {
        iterator.next();
        QFileInfo fileInfo = iterator.fileInfo();
        if(fileInfo.isFile())
        {
            QFile file(fileInfo.absoluteFilePath());
            int n = 0;
            if(file.open(QIODevice::ReadOnly))
            {
                sended = mySocket->writeDatagram(QByteArray("Path:").append(file.fileName().remove(0, Dir_.lastIndexOf("/")+1).append(" Size:").append(QString::number(file.size()))),address,sport);
                while(n < file.size())
                {
                  QByteArray data = file.read(3145728);
                  int block = 0;
                  while(block < data.size())
                  {
                     QByteArray fileBlock(data);
                     if((block+512) < data.size())
                     {
                         fileBlock.remove(block+512, fileBlock.size()-(block+512));
                     }
                     fileBlock.remove(0,block);
                     sended = mySocket->writeDatagram(fileBlock,address,sport);
                     if(sended == -1)
                     {
                         QMessageBox::warning(this,tr("Error"),tr("Can't send file"),QMessageBox::Ok);
                     } else {

                         block +=512;
                     }
                  }
                  n+=3145728;
                }
            }
        } else if (fileInfo.isDir()) {

             qint16 sended = mySocket->writeDatagram(QByteArray("Directory"),address,sport);
             if(sended == -1){
                 QMessageBox::warning(this,tr("Error"),tr("Can't send directory"),QMessageBox::Ok);
             }

        }
    }
}

//Boton para cerrar la conexion
void MainWindow::on_QuitPushButton_clicked()
{
    mySocket->close();
    ui->QuitPushButton->setEnabled(false);
    ui->SendPushButton->setEnabled(false);
    ui->ConnectPushButton->setEnabled(true);
    ui->DDPushButton->setEnabled(true);
    ui->SAPushButton->setEnabled(true);
}

//Boton para especificar la ruta a enviar.
void MainWindow::on_SAPushButton_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"/home");
    ui->SAddressLineEdit->setText(filename);
    if(filename.size() != 0)
    {
       ui->DDPushButton->setEnabled(false);
       ui->SendPushButton->setEnabled(true);
    }
}

//Boton para especificar la ruta para guardar.
void MainWindow::on_DDPushButton_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"/home");
    ui->DAddressLineEdit->setText(filename);
    if(filename.size() != 0)
    {
        ui->SAPushButton->setEnabled(false);
        ui->SendPushButton->setEnabled(false);
    }
}

//Boton para enviar un mensaje de quiero enviar
void MainWindow::on_SendPushButton_clicked()
{
    QByteArray message("IWantSend");
    QString clientNumber;
    QHostAddress address(ui->AddressLineEdit->text());
    quint16 sport(ui->ServerPortSpinBox->value());

    clientNumber.setNum(ui->ClientNumberSpinBox->value());
    message.append(clientNumber);

    if(ui->SAddressLineEdit->text() == "")
    {
        QMessageBox::information(this,tr("Client"),tr("You must selecct a directory to send.\n"),QMessageBox::Ok);

    } else {

        qint64 aux = mySocket->writeDatagram(message,address,sport);

        if(aux == -1)
        {
            QMessageBox::critical(this,tr("Client"),tr("Write socket error.\n"),QMessageBox::Ok);

        } else {

            QMessageBox::information(this,tr("Client"),tr("Message for send sent.\n"),QMessageBox::Ok);
            ui->SendPushButton->setEnabled(false);
        }
    }
}
