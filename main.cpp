#include <QCoreApplication>
#include "rime_wubi_dict_mngr.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RimeWubiDictMngr rwdm;

    QString dict_file = ":/resource/wubi06.dict.yaml";
    int n = rwdm.loadMainDict(dict_file);
    qDebug() << "In main(), loaded" << n << "items to main dict from" << dict_file;

    QStringList user_files = {
        ":/words/ciku_qq.txt",
        ":/words/user.txt"
    };

    for (auto &file : user_files) {
        int n_items = rwdm.expandMainDict(file, RimeWubiDictMngr::ADD_ALL);
        qDebug() << "In main(), loaded" << n_items << "new words from" << file;
        //n_items = rwdm.saveUserDict("user.txt");
        //qDebug() << "In main(), saved" << n_items << "user words";
    }

    QString save_file = "/home/krist/all.txt";
    n = rwdm.saveMainDict(save_file);
    qDebug() << "In main(), saved" << n << "items from main dict to" << save_file;

    return a.exec();
}
