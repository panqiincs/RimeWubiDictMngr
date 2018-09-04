#include <QCoreApplication>
#include "rime_wubi_dict_mngr.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RimeWubiDictMngr rwdm;
    int n = rwdm.loadMainDict("./core/wubi06.dict.yaml");
    qDebug() << "In main(), loaded" << n << "items from dict.";
    rwdm.saveMainDict("out.txt");

    return a.exec();
}
