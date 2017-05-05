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
        QMessageBox::information(this,tr("Server"),tr("Connection established.\n"),QMessageBox::Ok);
        ui->ConnectPushButton->setEnabled(false);
        ui->QuitPushButton->setEnabled(true);

    }else{       
        QMessageBox::critical(this,tr("Server"),tr("Unable to connect.\n"),QMessageBox::Ok);
    }

    QTimer *timer = new QTimer();
    QTimer *timer_answer = new QTimer();
    timer->setInterval(10000);
    timer_answer->setInterval(1000);
    connect(timer,&QTimer::timeout,this,[=](){
        QByteArray message("Keep alive");
        for(int i=0; i<clients.size();i++)
        {
            mySocket->writeDatagram(message,clients[i].first,clients[i].second);
        }
        timer_answer->start();
    });

    connect(timer_answer,&QTimer::timeout,this,[=](){
        clients = clients_live;

        for(int i=0; i<clients_send.size();i++)
        {
            if(!clients.contains(clients_send[i]))
            {
                clients_send.remove(i);
                clients_recieve.remove(i);
            }
        }
    });

    timer->start();
}

void MainWindow::read()
{
    while(mySocket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = mySocket->receiveDatagram();
        QHostAddress clientaddress = datagram.senderAddress();
        int clientport = datagram.senderPort();
        QByteArray cmessage = datagram.data();
        QByteArray okmessage("ServerOkey");
        QByteArray waitmessage("WaitConfirm");
        QByteArray readymessage("ReadyToSend");
        bool aux = 0;

        if(cmessage=="HellowWorld")
        {
            mySocket->writeDatagram(okmessage,clientaddress,clientport);
            clientnumber+=1;
            clients.push_back(QPair<QHostAddress,qint16>(clientaddress,clientport));
            if(!clients_send.empty())
            {
                for(int i=0; i<clients_recieve.size()&&(aux == false);i++)
                {
                    if((clients_recieve[i]<=clientnumber-1))
                    {
                        mySocket->writeDatagram(readymessage,clients_send[i].first,clients_send[i].second);
                        aux = true;
                    }
                }
            }


        } else if(cmessage.startsWith("IWantSend"))
        {
            mySocket->writeDatagram(waitmessage,clientaddress,clientport);
            //QMessageBox::information(this,tr("Server"),tr("Message for send received.\n"),QMessageBox::Ok);
            cmessage.remove(0,9);
            if(cmessage.toInt()<=(clientnumber-1))
            {
                mySocket->writeDatagram(readymessage,clientaddress,clientport);

            }else{

                clients_send.push_back(QPair<QHostAddress,qint16>(clientaddress,clientport));
                clients_recieve.push_back(cmessage.toInt());
            }

        }else if(cmessage=="Client Okey:"){

            clients_live.push_back(QPair<QHostAddress,qint16>(clientaddress,clientport));
            QMessageBox::information(this,tr("Server"),cmessage,QMessageBox::Ok);

        } else {

            for(auto Client: clients)
            {
                if(Client.first!=clientaddress||Client.second!=clientport)
                    mySocket->writeDatagram(cmessage,Client.first,Client.second);
            }
        }
    }
}

void MainWindow::on_QuitPushButton_clicked()
{
    mySocket->close();
    ui->QuitPushButton->setEnabled(false);
    ui->ConnectPushButton->setEnabled(true);
    clientnumber=0;
    clients_send.clear();
    clients_recieve.clear();
    clients.clear();
}
