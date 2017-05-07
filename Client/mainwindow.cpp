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
    ui->SAddressLineEdit->setEnabled(true);
    ui->DAddressLineEdit->setEnabled(true);
    ui->AddressLineEdit->setEnabled(true);
    ui->ServerPortSpinBox->setEnabled(true);
    ui->ClientPortSpinBox->setEnabled(true);
    ui->ClientNumberSpinBox->setEnabled(true);
    ui->SAPushButton->setEnabled(true);

    myfile = NULL;
    acc = 0;
    size = 0;
    secuence_number = 0;
    temp = false;
    ihave = true;

    progressWindows = new NoMainWindows(this);

    connect(progressWindows,&NoMainWindows::ready,this,&MainWindow::enable);
    connect(&nothread,&mythread::sendpackage,this,&MainWindow::spackage);
    connect(&nothread,&mythread::totalsize,this,&MainWindow::tsize);
    connect(&nothread,&mythread::actualsize,this,&MainWindow::asize);
    connect(&nothread,&mythread::name,this,&MainWindow::name);
    connect(this, &MainWindow::wantsend, &nothread, &mythread::send);
    connect(this, &MainWindow::package, &nothread, &mythread::package);

    nothread.setWC(&wcond);
    nothread.moveToThread(&thread);
    thread.start();
    
}

MainWindow::~MainWindow()
{
    delete ui;
    delete progressWindows;

    if(myfile!=NULL)
    {
        myfile->close();
        delete myfile;
    }
}

//Boton para establecer la conexion.
void MainWindow::on_ConnectPushButton_clicked()
{
    QByteArray message("HellowWorld");
    QHostAddress address(ui->AddressLineEdit->text());
    quint16 sport(ui->ServerPortSpinBox->value());
    QTimer *timer = new QTimer();
    timer->setInterval(5000);
    connect(timer,&QTimer::timeout,this, [=](){
        QMessageBox::critical(this,tr("Client Error"),tr("Can't connect to server.\n"),QMessageBox::Ok);
        timer->deleteLater();
    });


    if(((ui->ClientPortSpinBox->value()!=sport)||(QHostAddress::LocalHost!=ui->AddressLineEdit->text())))
    {
        if((ui->SAddressLineEdit->text()!="")||(ui->DAddressLineEdit->text()!=""))
        {
            if(mySocket->state()==QAbstractSocket::BoundState)
            {
                mySocket->close();
            }

            if(mySocket->bind(QHostAddress::LocalHost, ui->ClientPortSpinBox->value())){

                qint64 sended = mySocket->writeDatagram(message,address,sport);

                if(sended == -1)
                {
                    QMessageBox::critical(this,tr("Client Error"),tr("Write socket error.\n"),QMessageBox::Ok);

                }
            }else{

                QMessageBox::critical(this,tr("Client Error"),mySocket->errorString(),QMessageBox::Ok);
            }

        } else {

            QMessageBox::information(this,tr("Client Error"),tr("You have to change a directory.\n"),QMessageBox::Ok);
        }

    } else {

        QMessageBox::information(this,tr("Client Error"),tr("Can't be connected.\n"),QMessageBox::Ok);
    }
}


void MainWindow::read()
{
    QNetworkDatagram datagram = mySocket->receiveDatagram();
    QByteArray cmessage = datagram.data();
    QByteArray cmessage1;    
    QByteArray aux = 0;
    QString Dir(ui->AddressLineEdit->text());
    quint16 sport(ui->ServerPortSpinBox->value());
    QByteArray answer("Client Okey:");


    if(cmessage=="ServerOkey")
    {
        temp = true;
        ui->ConnectPushButton->setEnabled(false);
        ui->QuitPushButton->setEnabled(true);
        ui->AddressLineEdit->setEnabled(false);
        ui->ServerPortSpinBox->setEnabled(false);
        ui->ClientPortSpinBox->setEnabled(false);


    } else if(cmessage=="ReadyToSend") {

        QDir Dir(ui->SAddressLineEdit->text());

        if(Dir.exists())
        {
            emit wantsend(ui->SAddressLineEdit->text());
        } else {
            QMessageBox::information(this,tr("Client"), tr("Directory doesn't exisits\n"),QMessageBox::Ok);
        }

    } else if (cmessage=="WaitConfirm"){


    } else if (cmessage.startsWith("Total s:")){
        cmessage.remove(0,8);
        aux = cmessage;
        progressWindows->setTotalSize(aux.toLongLong());

    } else if(cmessage.startsWith("First D:")) {

        this->setEnabled(false);
        progressWindows->setWindowTitle("Reciving");
        progressWindows->show();
        progressWindows->setEnabled(true);
        cmessage.remove(0,8);
        QDir DDir(ui->DAddressLineEdit->text().append("/").append(cmessage));        
        progressWindows->setName(cmessage);

        if(!DDir.exists())
        {
            DDir.mkpath(ui->DAddressLineEdit->text().append("/").append(cmessage));
        }
        recievefile =4096;

        progressWindows->setActualSize(recievefile);

    } else if(cmessage.startsWith("Path:")) {

        if(size.toLongLong() == acc)
        {
            acc = 0;
            QByteArray toremovemessage = cmessage;
            cmessage.remove(0,5);
            cmessage.remove(cmessage.indexOf("Size:")-1,cmessage.size()-(cmessage.indexOf("Size:")-1));

            size = toremovemessage.remove(0,toremovemessage.size()-(toremovemessage.lastIndexOf("Size:")-1));
            size = toremovemessage.remove(0,toremovemessage.indexOf(":")+1);

            if(myfile!=NULL)
            {
                myfile->close();
            }

            myfile = new QFile(ui->DAddressLineEdit->text().append("/").append(cmessage));
            myfile->open(QIODevice::ReadWrite);

        } else {
            QMessageBox::critical(this,tr("Client Error"),tr("Last file incompleted\n"),QMessageBox::Ok);
        }

        progressWindows->setActualSize(recievefile);

    } else if(cmessage.startsWith("Directoryy:")){

        cmessage1 = cmessage;
        cmessage1.remove(cmessage1.indexOf("//"),cmessage1.size()-cmessage1.indexOf("//"));
        cmessage1.remove(0,cmessage1.lastIndexOf(":")+1);
        cmessage.remove(0,cmessage.indexOf("//")+2);

        if(cmessage1.toLongLong()==secuence_number)
        {
            QDir DiDir(ui->DAddressLineEdit->text().append("/").append(cmessage));
            if(!DiDir.exists())
            {
                DiDir.mkpath(ui->DAddressLineEdit->text().append("/").append(cmessage));

            }
            recievefile +=4096;
            progressWindows->setActualSize(recievefile);
            secuence_number+=1;
            ihave = true;

        } else if(ihave==true){

            qint64 sended = mySocket->writeDatagram(QByteArray("Package:").append(cmessage1),QHostAddress(Dir),sport);
            if(sended == -1)
            {
                QMessageBox::critical(this,tr("Client Error"),tr("Can't send package left to server\n"),QMessageBox::Ok);
            }
            ihave = false;
        }

    } else if(cmessage=="Keep alive"){

        qint64 sended = mySocket->writeDatagram(answer,QHostAddress(Dir),sport);
        if(sended == -1)
        {
            QMessageBox::critical(this,tr("Client Error"),tr("Can't keep alive answer\n"),QMessageBox::Ok);
        }

    } else if (cmessage.startsWith("SN:")) {

        cmessage1 = cmessage;
        cmessage.remove(0,cmessage.indexOf("//")+2);
        cmessage1.remove(cmessage1.indexOf("//"),cmessage1.size()-cmessage1.indexOf("//"));
        cmessage1.remove(0,cmessage1.indexOf(":")+1);

        if(cmessage1.toLongLong()==secuence_number)
        {
            if(myfile->exists())
            {
                if(myfile->isWritable()){
                    qint64 byt = myfile->write(cmessage);
                    acc += byt;
                    if(byt != cmessage.size())
                    {
                        QMessageBox::information(this, tr("Client Error"),tr("Can't write"), QMessageBox::Ok);
                    }else{
                        recievefile += byt;
                        progressWindows->setActualSize(recievefile);
                    }
                    if(size.toLongLong() == acc)
                    {
                        myfile->close();
                        qint64 sended = mySocket->writeDatagram(QByteArray("File completed:"),QHostAddress(Dir),sport);
                        if(sended == -1)
                        {
                            QMessageBox::critical(this,tr("Client Error"),tr("File incompleted\n"),QMessageBox::Ok);
                        }
                    }
                }
            }
            secuence_number+=1;
            ihave = true;

        } else if(ihave==true){

            qint64 sended = mySocket->writeDatagram(QByteArray("Package:").append(cmessage1),QHostAddress(Dir),sport);
            if(sended == -1)
            {
                QMessageBox::critical(this,tr("Client Error"),tr("Can't send package left to server\n"),QMessageBox::Ok);
            }
            ihave = false;
        }
    } else if(cmessage.startsWith("Package:")){
        emit package(cmessage);
    } else if(cmessage =="File completed:") {
        wcond.wakeAll();
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
    ui->SAddressLineEdit->setText("");
    ui->SAddressLineEdit->setEnabled(true);
    ui->DAddressLineEdit->setText("");
    ui->DAddressLineEdit->setEnabled(true);
    ui->ClientNumberSpinBox->setValue(0);
    ui->AddressLineEdit->setEnabled(true);
    ui->ServerPortSpinBox->setEnabled(true);
    ui->ClientPortSpinBox->setEnabled(true);
    ui->ClientNumberSpinBox->setEnabled(true);
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
    ui->SAddressLineEdit->setEnabled(false);
    ui->DAddressLineEdit->setEnabled(false);
    ui->ClientNumberSpinBox->setEnabled(false);
    ui->SAPushButton->setEnabled(false);
    QByteArray message("IWantSend");
    QString clientNumber;
    QHostAddress address(ui->AddressLineEdit->text());
    quint16 sport(ui->ServerPortSpinBox->value());

    clientNumber.setNum(ui->ClientNumberSpinBox->value());
    message.append(clientNumber);

    if(ui->SAddressLineEdit->text() == "")
    {
        QMessageBox::information(this,tr("Client"),tr("You must selecct a directory to send.\n"),QMessageBox::Ok);
        ui->SAPushButton->setEnabled(true);

    } else {

        qint64 aux = mySocket->writeDatagram(message,address,sport);

        if(aux == -1)
        {
            QMessageBox::critical(this,tr("Client Error"),tr("Write socket error.\n"),QMessageBox::Ok);

        } else {

            ui->SendPushButton->setEnabled(false);
        }
    }
}

void MainWindow::enable()
{
    this->setEnabled(true);
}

void MainWindow::spackage(QByteArray pack)
{
    qint64 aux = mySocket->writeDatagram(pack,QHostAddress(ui->AddressLineEdit->text()),ui->ServerPortSpinBox->value());

    if(aux == -1)
    {
        QMessageBox::critical(this,tr("Client Error"),tr("Write socket error.\n"),QMessageBox::Ok);

    }
}

void MainWindow::tsize(qint64 size)
{
    progressWindows->setWindowTitle("Sending");
    progressWindows->setTotalSize(size);
    this->setEnabled(false);
    progressWindows->show();
    progressWindows->setEnabled(true);
}

void MainWindow::asize(qint64 size)
{
    progressWindows->setActualSize(size);
}

void MainWindow::name(QString name)
{
    progressWindows->setName(name);
}

void MainWindow::on_SAddressLineEdit_editingFinished()
{
    QDir Dir(ui->SAddressLineEdit->text());
    if(Dir.exists())
    {
        ui->SendPushButton->setEnabled(true);
    }
}


void MainWindow::on_DAddressLineEdit_editingFinished()
{
    ui->ClientNumberSpinBox->setEnabled(false);
    ui->SendPushButton->setEnabled(false);

}
