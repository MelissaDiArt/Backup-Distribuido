#include "mythread.h"

mythread::mythread()
{
}

mythread::~mythread()
{

}

void mythread::setWC(QWaitCondition *wait)
{
    wcond = wait;
}

void mythread::send(QString Dir)
{
        QString Dir_= Dir;
        QDirIterator iterator(Dir,QDir::NoDot  | QDir::NoDotDot | QDir::Hidden | QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);

        QProcess *myProcess = new QProcess(this);
        QStringList arguments = (QStringList() << "-sbc" << Dir);
        myProcess->setProgram("du");
        myProcess->setArguments(arguments);
        myProcess->start();
        myProcess->waitForReadyRead();
        QByteArray aux = myProcess->readAllStandardOutput();
        aux.remove(aux.indexOf("\t"), aux.size() - aux.indexOf("\t"));
        time = 0;

        emit sendpackage(QByteArray("Total s:").append(QByteArray().setNum(aux.toLongLong())));

        myProcess->close();
        delete myProcess;

        emit totalsize(aux.toLongLong());
        emit name(Dir_.remove(0,Dir_.lastIndexOf("/")+1));

        emit sendpackage(QByteArray("First D:").append(Dir_));

        qint64 sendfile = 4096;
        lock.lock();

        while(iterator.hasNext())
        {
            iterator.next();
            QFileInfo fileInfo = iterator.fileInfo();

            if(fileInfo.isFile())
            {
                myfile = new QFile(fileInfo.absoluteFilePath());
                qint64 n = 0;
                if(myfile->open(QIODevice::ReadOnly))
                {
                    emit sendpackage(QByteArray("Path:").append(myfile->fileName().remove(0, Dir.lastIndexOf("/")+1)).append(" Size:").append(QString::number(myfile->size())));

                    while(n < myfile->size())
                    {
                      QByteArray data = myfile->read(3145728);
                      secuence_reference_f.push_back(QPair<qint64,qint64>(secuence_number,myfile->pos()));
                      int block = 0;
                      while(block < data.size())
                      {
                         QByteArray fileBlock(data);
                         if((block+512) < data.size())
                         {
                             fileBlock.remove(block+512, fileBlock.size()-(block+512));
                         }
                         fileBlock.remove(0,block);
                         emit sendpackage(QByteArray("SN:").append(QByteArray().setNum(secuence_number).append(QByteArray("//").append(fileBlock))));
                         secuence_number+=1;
                         wcond->wait(&lock, time);
                         qApp->processEvents();
                         block +=512;
                         sendfile += fileBlock.size();
                         emit actualsize(sendfile);

                      }

                      if(myfile->pos() == myfile->size()){
                        wcond->wait(&lock);
                      }

                      n = myfile->pos();
                    }
                }

            } else if (fileInfo.isDir()) {

                 emit sendpackage(QByteArray("Directoryy:").append(QByteArray("SN:")).append(QByteArray().setNum(secuence_number)).append(QByteArray("//")).append(fileInfo.absoluteFilePath().remove(0, Dir.lastIndexOf("/")+1)));

                 QByteArray aux;
                 aux.append(fileInfo.absoluteFilePath().remove(0, Dir.lastIndexOf("/")+1));
                 secuence_reference_d.push_back(QPair<qint64,QByteArray>(secuence_number,aux));
                 secuence_number+=1;
                 sendfile +=4096;
                 emit actualsize(sendfile);
            }
        }

        lock.unlock();
}

void mythread::package(QByteArray package)
{
    bool done = false;
    bool stop = false;
    package.remove(0,package.indexOf("Package:")+8);
    for(int i=0;(i<secuence_reference_d.size()-1)&&(!stop);i++)
    {
        if((package.toLongLong()>=secuence_reference_d[i].first)&&(package.toLongLong()<secuence_reference_d[i+1].first))
        {
            emit sendpackage(QByteArray("Directoryy:").append(QByteArray("SN:")).append(secuence_reference_d[i].first).append(QByteArray("//")).append(secuence_reference_d[i].second));
            done = true;
            stop = true;
        }
    }

    if(!done)
    {
        for(int i=0;i<secuence_reference_f.size();i++)
        {
            if((package.toLongLong()>=secuence_reference_f[i].first)&&(package.toLongLong()<secuence_reference_f[i+1].first))
            {
                secuence_number = secuence_reference_f[i].first;
                myfile->seek(secuence_reference_f[i].second);
                time += 1;
                wcond->wakeAll();
            }
        }
    }
}
