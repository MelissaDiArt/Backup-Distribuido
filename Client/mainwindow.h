#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QFileDialog>

#include "nomainwindows.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void read();
    void send();

private slots:
    void on_ConnectPushButton_clicked();

    void on_QuitPushButton_clicked();

    void on_SAPushButton_clicked();

    void on_DDPushButton_clicked();

    void on_SendPushButton_clicked();

public slots:

    void enable();

private:
    Ui::MainWindow *ui;
    QUdpSocket* mySocket;
    QFile* myfile;
    QByteArray size;
    qint64 acc;
    NoMainWindows *progressWindows;
    QProcess *myProcess;
    bool temp;
    bool ihave;
    QVector<QPair<qint64,qint64> > secuence_reference_f;
    QVector<QPair<qint64,QByteArray> > secuence_reference_d;
    qint64 recievefile;
    qint64 secuence_number;


};

#endif // MAINWINDOW_H
