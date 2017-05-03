#ifndef NOMAINWINDOWS_H
#define NOMAINWINDOWS_H

#include <QMainWindow>

namespace Ui {
class NoMainWindows;
}

class NoMainWindows : public QMainWindow
{
    Q_OBJECT

public:
    explicit NoMainWindows(QWidget *parent = 0);
    ~NoMainWindows();

public:

    void setName(QString name);
    void setTotalSize(qint64 size);
    void setActualSize(qint64 size);

private slots:
    void on_FinishPushButton_clicked();

signals:
    void ready();

private:
    Ui::NoMainWindows *ui;
    QString Name;
    qint16 ActualSize;
    qint16 TotalSize;
};

#endif // NOMAINWINDOWS_H
