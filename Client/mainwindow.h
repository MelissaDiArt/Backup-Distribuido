#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QFileDialog>

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

private slots:
    void on_ConnectPushButton_clicked();

    void on_QuitPushButton_clicked();

    void on_SAPushButton_clicked();

    void on_DDPushButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket* mySocket;
};

#endif // MAINWINDOW_H