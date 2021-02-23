#include "mainwindow.h"

#include <QApplication>
#include "audio/audioadapter.h"

int main(int argc, char *argv[])
{
    InitializeAudioSystem

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    auto r = a.exec();

    ShutdownAudioSystem

    return r;
}
