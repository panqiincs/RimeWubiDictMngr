#include <QCoreApplication>
#include "rime_wubi_dict_mngr.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RimeWubiDictMngr rwdm;

    int n = rwdm.loadMainDict("./core/wubi06.dict.yaml");
    qDebug() << "In main(), loaded" << n << "items from dict.";

    n = rwdm.extendMainDict("./user/total.txt", RimeWubiDictMngr::ADD_HIGHFREQ);
    qDebug() << "In main(), loaded" << n << "items from user words.";

    n = rwdm.saveMainDict("out.txt");
    qDebug() << "In main(), saved" << n << "items from main dict";

    return a.exec();
}
