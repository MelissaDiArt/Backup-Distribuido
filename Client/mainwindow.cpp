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
    myfile = NULL;
    acc = 0;
    size = 0;
    temp = false;
    ihave = true;

    progressWindows = new NoMainWindows(this);

    connect(progressWindows,&NoMainWindows::ready,this,&MainWindow::enable);

    secuence_number = 0;
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


    if((ui->ClientPortSpinBox->value()!=sport)||(QHostAddress::LocalHost!=ui->AddressLineEdit->text()))
    {
        if(mySocket->bind(QHostAddress::LocalHost, ui->ClientPortSpinBox->value())){

            qint64 aux = mySocket->writeDatagram(message,address,sport);

            if(aux == -1)
            {
                QMessageBox::critical(this,tr("Client Error"),tr("Write socket error.\n"),QMessageBox::Ok);

            }
        }else{
            QMessageBox::critical(this,tr("Client Error"),mySocket->errorString(),QMessageBox::Ok);
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

    } else if(cmessage=="ReadyToSend") {

        QDir Dir(ui->SAddressLineEdit->text());

        if(Dir.exists())
        {
            send();
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
            recievefile += acc;
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
                qint64 sended = mySocket->writeDatagram(QByteArray("Directory completed:"),QHostAddress(Dir),sport);
                if(sended == -1)
                {
                    QMessageBox::critical(this,tr("Client Error"),tr("Directory incompleted\n"),QMessageBox::Ok);
                }
                recievefile +=4096;
            }
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
        cmessage1.remove(cmessage1.indexOf("//"),cmessage1.size()-cmessage.indexOf("//")+2);
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

        bool done = false;
        bool stop = false;
        cmessage.remove(0,cmessage1.indexOf("Package:")+8);
        for(int i=0;(i<secuence_reference_d.size()-1)&&(!stop);i++)
        {
            if((cmessage.toLongLong()>=secuence_reference_d[i].first)&&(cmessage.toLongLong()<secuence_reference_d[i+1].first))
            {
                qint16 sended = mySocket->writeDatagram(QByteArray("Directoryy:").append(QByteArray("SN:")).append(secuence_reference_d[i].first).append(QByteArray("//")).append(secuence_reference_d[i].second),QHostAddress(Dir),sport);
                if(sended == -1)
                {
                    QMessageBox::critical(this,tr("Client Error"),tr("Can't resend package left to server\n"),QMessageBox::Ok);
                }
                done = true;
                stop = true;
            }
        }

        if(!done)
        {
            for(int i=0;i<secuence_reference_f.size();i++)
            {
                if((cmessage.toLongLong()>=secuence_reference_f[i].first)&&(cmessage.toLongLong()<secuence_reference_f[i+1].first))
                {
                    myfile->seek(secuence_reference_f[i].second);

                }
            }
        }
    } else if(cmessage =="File completed:") {

    } else if(cmessage == "Directory completed:"){

    }
}

void MainWindow::send()
{
    QString Dir(ui->SAddressLineEdit->text());
    QString Dir_= Dir;
    quint16 sport(ui->ServerPortSpinBox->value());
    QHostAddress address(ui->AddressLineEdit->text());
    QDirIterator iterator(Dir,QDir::NoDot  | QDir::NoDotDot | QDir::Hidden | QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);
    qint64 secuence_number = 0;

    myProcess = new QProcess(this);
    QStringList arguments = (QStringList() << "-sbc" << Dir);
    myProcess->setProgram("du");
    myProcess->setArguments(arguments);
    myProcess->start();
    myProcess->waitForReadyRead();
    QByteArray aux = myProcess->readAllStandardOutput();
    aux.remove(aux.indexOf("\t"), aux.size() - aux.indexOf("\t"));

    mySocket->writeDatagram(QByteArray("Total s:").append(QByteArray().setNum(aux.toLongLong())),address,sport);

    myProcess->close();
    delete myProcess;

    progressWindows->setTotalSize(aux.toLongLong());
    progressWindows->setName(Dir_.remove(0,Dir_.lastIndexOf("/")+1));
    progressWindows->setWindowTitle("Sending");

    qint16 sended = mySocket->writeDatagram(QByteArray("First D:").append(Dir_),address,sport);

    this->setEnabled(false);
    progressWindows->show();
    progressWindows->setEnabled(true);

    qint64 sendfile = 4096;

    while(iterator.hasNext())
    {
        iterator.next();
        QFileInfo fileInfo = iterator.fileInfo();

        if(fileInfo.isFile())
        {
            QFile file(fileInfo.absoluteFilePath());
            qint64 n = 0;
            if(file.open(QIODevice::ReadOnly))
            {
                sended = mySocket->writeDatagram(QByteArray("Path:").append(file.fileName().remove(0, Dir.lastIndexOf("/")+1)).append(" Size:").append(QString::number(file.size())),address,sport);

                while(n < file.size())
                {
                  QByteArray data = file.read(3145728);
                  secuence_reference_f.push_back(QPair<qint64,qint64>(secuence_number,file.pos()));
                  int block = 0;
                  while(block < data.size())
                  {
                     QByteArray fileBlock(data);
                     if((block+512) < data.size())
                     {
                         fileBlock.remove(block+512, fileBlock.size()-(block+512));
                     }
                     fileBlock.remove(0,block);
                     sended = mySocket->writeDatagram(QByteArray("SN:").append(QByteArray().setNum(secuence_number).append(QByteArray("//").append(fileBlock))),address,sport);
                     secuence_number+=1;
                     qApp->processEvents();
                     if(sended == -1)
                     {
                         QMessageBox::warning(this,tr("Client Error"),tr("Can't send file"),QMessageBox::Ok);
                     } else {

                         block +=512;
                         sendfile += fileBlock.size();
                         progressWindows->setActualSize(sendfile);
                     }
                  }
                  n+=3145728;
                }
            }

        } else if (fileInfo.isDir()) {

             qint16 sended = mySocket->writeDatagram(QByteArray("Directoryy:").append(QByteArray("SN:")).append(QByteArray().setNum(secuence_number)).append(QByteArray("//")).append(fileInfo.absoluteFilePath().remove(0, Dir.lastIndexOf("/")+1)),address,sport);

             QByteArray aux;
             aux.append(fileInfo.absoluteFilePath().remove(0, Dir.lastIndexOf("/")+1));
             secuence_reference_d.push_back(QPair<qint64,QByteArray>(secuence_number,aux));
             secuence_number+=1;
             if(sended == -1)
             {
                 QMessageBox::warning(this,tr("Client Error"),tr("Can't send directory"),QMessageBox::Ok);
             }
             sendfile +=4096;
             progressWindows->setActualSize(sendfile);
        }
    }

    if(sended == -1)
    {
        QMessageBox::critical(this,tr("Client Error"),tr("Send error"),QMessageBox::Ok);
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
    ui->DAddressLineEdit->setText("");
    ui->ClientNumberSpinBox->setValue(0);
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
