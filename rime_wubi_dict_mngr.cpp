#include "rime_wubi_dict_mngr.h"

RimeWubiDictMngr::RimeWubiDictMngr()
{

}

int RimeWubiDictMngr::loadHanziCode(const QString &filename)
{
    QFile infile(filename);
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "In loadHanziCode(), openning file" << filename <<  "failed!";
        return -1;
    }

    QTextStream in(&infile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (list.size() != 2) {
            qDebug() << "In loadHanziCode(), invalid line: " << line;
            continue;
        }

        QString hanzi = list[0];            // 字
        QString code  = list[1].toLower();  // 编码
        hanzi_code.insert(hanzi, code);
        hanzi_set.insert(hanzi);
    }
    infile.close();

    return hanzi_set.size();
}

int RimeWubiDictMngr::loadWordFreq(const QString &filename)
{
    QFile infile(filename);
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "In loadWordFreq(), openning file" << filename <<  "failed!";
        return -1;
    }

    QTextStream in(&infile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (list.size() != 2) {
            qDebug() << "In loadWordFreq(), invalid line: " << line;
            continue;
        }

        QString word = list[0];
        if (!isWordValid(word)) {
            qDebug() << "In loadWordFreq()," << word << "contains non-hanzi";
            continue;
        }
        size_t weight = list[1].toULong();
        word_freq[word] = weight;
        word_set.insert(word);
    }
    infile.close();

    return word_set.size();
}

int RimeWubiDictMngr::loadUserWords(const QString &filename)
{
    QFile infile(filename);
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "In loadWordFreq(), openning file" << filename <<  "failed!";
        return -1;
    }

    user_words.clear();

    QTextStream in(&infile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        for (auto &word : list) {
            if (!isWordValid(word) || user_words.contains(word)) {
                continue;
            }
            user_words.append(word);
        }
    }
    infile.close();

    return user_words.size();
}
