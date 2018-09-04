#include "rime_wubi_dict_mngr.h"

RimeWubiDictMngr::RimeWubiDictMngr()
{

}

QVector<QPair<QString, size_t> > RimeWubiDictMngr::calWordCodeWeight(const QString &wd, RimeWubiDictMngr::add_mode_t mode)
{
    QVector<QPair<QString, size_t>> res;

    size_t freq = word_freq.value(wd);
    QPair<QString, size_t> item;

    if (isHanzi(wd)) {  // 单字
        QStringList all_code = getHanziAllCode(wd);
        // 找到最小长度的编码
        int min_len = 5;
        for (auto &code : all_code) {
            if (code.length() < min_len) {
                min_len = code.length();
            }
        }
        Q_ASSERT(min_len > 0 && min_len < 5);

        // 编码最短的权重最大，编码最长权重越小
        for (auto &code : all_code) {
            float factor = 1.0 - (code.length()-min_len) * 0.2;
            size_t weight = (size_t)(factor * freq);
            if (weight < 1000) {
                weight = 0;
            }

            item.first = code;
            item.second = weight;
            res.push_back(item);
        }
    } else {  // 词组
        QString one_code = getCizuCode(wd);

        item.first = one_code;
        item.second = freq;
        res.push_back(item);
    }

    return res;
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

bool RimeWubiDictMngr::isHanzi(const QString &wd)
{
    return (wd.length() == 1);
}

bool RimeWubiDictMngr::isCizu(const QString &wd)
{
    return (wd.length() > 1);
}

