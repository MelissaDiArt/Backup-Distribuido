#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QDirIterator>
#include <QProcess>
#include <QMutex>
#include <QPair>
#include <QVector>
#include <QWaitCondition>
#include <QApplication>

class mythread : public QObject
{
    Q_OBJECT

signals:
    void sendpackage(QByteArray pack);
    void totalsize(qint64 size);
    void actualsize(qint64 size);
    void name(QString name);

public:
    mythread();
    ~mythread();
    void setWC(QWaitCondition *wait);

public slots:
    void send(QString Dir);
    void package(QByteArray package);

private:
    QMutex lock;
    QWaitCondition *wcond;
    ulong time;
    QVector<QPair<qint64,QByteArray> > secuence_reference_d;
    QVector<QPair<qint64,qint64> > secuence_reference_f;
    qint64 secuence_number;
    QFile *myfile;
};

#endif // MYTHREAD_H
