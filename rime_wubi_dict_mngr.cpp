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

QStringList RimeWubiDictMngr::getHanziAllCode(const QString &hz)
{
    Q_ASSERT(hz.length() == 1);

    QStringList res;

    for (auto it = hanzi_code.lowerBound(hz); it != hanzi_code.upperBound(hz); ++it) {
        res.append(it.value());
    }

    return res;
}

QString RimeWubiDictMngr::getHanziFullcode(const QString &hz)
{
    QString res;

    QStringList code_list = getHanziAllCode(hz);
    for (auto it = code_list.cbegin(); it != code_list.cend(); ++it) {
        if (it->length() > res.length()) {
            res = *it;
        }
    }

    return res;
}

QString RimeWubiDictMngr::getCizuCode(const QString &cz)
{
    Q_ASSERT(cz.length() > 1);

    QString res;

    size_t word_len = cz.length();
    if (word_len == 2) {
        // 二字词：取每个字全码的前两个码组成，共4码
        QString hanzicode1 = getHanziFullcode(cz[0]);
        QString hanzicode2 = getHanziFullcode(cz[1]);
        res.append(hanzicode1[0]);
        res.append(hanzicode1[1]);
        res.append(hanzicode2[0]);
        res.append(hanzicode2[1]);
    } else if (word_len == 3) {
        // 三字词：前两个字，各取第一个码，最后一字取前两个码，共4码
        QString hanzicode1 = getHanziFullcode(cz[0]);
        QString hanzicode2 = getHanziFullcode(cz[1]);
        QString hanzicode3 = getHanziFullcode(cz[2]);
        res.append(hanzicode1[0]);
        res.append(hanzicode2[0]);
        res.append(hanzicode3[0]);
        res.append(hanzicode3[1]);
    } else {
        // 四字词或四字以上的词：
        // 取第一、二、三及最后一个汉字的第一码，共4码
        QString hanzicode1 =getHanziFullcode(cz[0]);
        QString hanzicode2 =getHanziFullcode(cz[1]);
        QString hanzicode3 =getHanziFullcode(cz[2]);
        QString hanzicode4 =getHanziFullcode(cz[cz.length()-1]);
        res.append(hanzicode1[0]);
        res.append(hanzicode2[0]);
        res.append(hanzicode3[0]);
        res.append(hanzicode4[0]);
    }

    return res;
}

bool RimeWubiDictMngr::isWordValid(const QString &wd)
{
    if (wd.size() == 0) {
        return false;
    }

    for (int i = 0; i < wd.length(); ++i) {
        if (!hanzi_set.contains(QString(wd[i]))) {
            return false;
        }
    }

    return true;
}
