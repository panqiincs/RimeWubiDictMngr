#include <QCoreApplication>
#include "rime_wubi_dict_mngr.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RimeWubiDictMngr rwdm;

    QString dict_file = "./core/wubi06.dict.yaml";
    int n = rwdm.loadMainDict(dict_file);
    qDebug() << "In main(), loaded" << n << "items to main dict from" << dict_file;

    QStringList user_files = {
        "freeime.txt",
        "qq_wubi.txt",
        "vocab.txt",
        "chengyu_1.txt",
        "chengyu_2.txt"
    };

    for (auto &file : user_files) {
        file = "./user/" + file;
        int n_items = rwdm.expandMainDict(file, RimeWubiDictMngr::ADD_HIGHFREQ);
        qDebug() << "In main(), loaded" << n_items << "new words from" << file;
    }

    QString save_file = "all.txt";
    n = rwdm.saveMainDict(save_file);
    qDebug() << "In main(), saved" << n << "items from main dict to" << save_file;

    return a.exec();
}
