#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QFileDialog>

#include "nomainwindows.h"
#include "mythread.h"

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

    void on_SAddressLineEdit_editingFinished();

    void on_DAddressLineEdit_editingFinished();

public slots:

    void enable();

    void spackage(QByteArray pack);

    void tsize(qint64 size);

    void asize(qint64 size);

    void name(QString name);

signals:
    void wantsend(QString dir);
    void package(QByteArray package);

private:
    Ui::MainWindow *ui;
    QUdpSocket* mySocket;
    QFile* myfile;
    QByteArray size;
    qint64 acc;
    NoMainWindows *progressWindows;
    QProcess *myProcess;
    QVector<QPair<qint64,qint64> > secuence_reference_f;
    QVector<QPair<qint64,QByteArray> > secuence_reference_d;
    qint64 recievefile;
    qint64 secuence_number;
    QMutex lock;
    QWaitCondition wcond;
    mythread nothread;
    QThread thread;
    ulong time;
    bool filecomp;
    bool temp;
    bool ihave;
};

#endif // MAINWINDOW_H
