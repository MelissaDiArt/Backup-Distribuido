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

private:
    Ui::NoMainWindows *ui;
};

#endif // NOMAINWINDOWS_H
