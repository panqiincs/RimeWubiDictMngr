#include "rime_wubi_dict_mngr.h"

RimeWubiDictMngr::RimeWubiDictMngr()
{
    int n = loadHanziCode("./core/hanzi_code.txt");
    qDebug() << "In constructor, loaded" << n << "hanzi code items.";
    n = loadWordFreq("./core/word_freq_total_above_1k.txt");
    qDebug() << "In constructor, loaded" << n << "word freq items.";
}

bool isSmaller(const QPair<QString, QPair<QString, size_t>> &p1,
               const QPair<QString, QPair<QString, size_t>> &p2)
{
    QString code1   = p1.second.first;
    QString code2   = p2.second.first;
    if (code1 < code2) {
        return true;
    } else if (code1 > code2) {
        return false;
    }

    size_t  weight1 = p1.second.second;
    size_t  weight2 = p2.second.second;
    if (weight1 > weight2) {
        return true;
    } else if (weight1 < weight2) {
        return false;
    }

    QString word1   = p1.first;
    QString word2   = p2.first;
    return (word1 < word2);
}

int RimeWubiDictMngr::loadMainDict(const QString &filename)
{
    QFile infile(filename);
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "In loadMainDict(), openning file" << filename <<  "failed!";
        return -1;
    }

    main_dict.clear();
    main_dict_set.clear();

    // 添加编码表中的所有汉字到字典
    for (auto it = hanzi_code.cbegin(); it != hanzi_code.cend(); ++it) {
        QString hanzi = it.key();
        if (main_dict_set.contains(hanzi)) {
            continue;
        }

        QVector<QPair<QString, size_t> > code_weight = getHanziCodeWeight(hanzi);
        for (auto &cw : code_weight) {
            QPair<QString, QPair<QString, size_t>> item(hanzi, cw);
            main_dict.push_back(item);
            main_dict_set.insert(hanzi);
        }
    }

    QTextStream in(&infile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (list.size() != 3) {
            continue;
        }
        QString word = list[0];
        if (main_dict_set.contains(word)) {
            continue;
        }
        QString code = list[1].toLower();
        if (!isWordValid(word) || !isCodeValid(code)) {
            qDebug() << "In loadMainDict(), invaid dict item:" << line;
            continue;
        }
        size_t weight = list[2].toULong();

        QPair<QString, size_t> code_weight(code, weight);
        QPair<QString, QPair<QString, size_t>> item(word, code_weight);
        main_dict.push_back(item);
        main_dict_set.insert(word);
    }
    infile.close();

    return main_dict.size();
}

int RimeWubiDictMngr::saveMainDict(const QString &filename)
{
    std::sort(main_dict.begin(), main_dict.end(), isSmaller);

    int ret = saveDict(main_dict, filename);

    return ret;
}

int RimeWubiDictMngr::saveUserDict(const QString &filename)
{
    int ret = saveDict(user_dict, filename);

    return ret;
}

int RimeWubiDictMngr::extendMainDict(const QString &filename, RimeWubiDictMngr::add_mode_t mode)
{
    Q_ASSERT(!word_freq.empty());
    Q_ASSERT(!hanzi_code.empty());

    int ret = loadUserWords(filename);
    if (ret == -1) {
        qDebug() << "In extendMainDict(), load user words failed!";
        return -1;
    }

    user_dict.clear();

    size_t old_size = main_dict_set.size();

    for (auto word : user_words) {
        if (main_dict_set.contains(word)) {
            continue;
        }

        size_t weight;
        if (word_set.contains(word)) {
            weight = word_freq.value(word);
            if (weight < THRESHOLD) {
                continue;
            }
        } else {
            if (mode == ADD_HIGHFREQ) {
                continue;
            } else if (mode == ADD_EVERYONE) {
                weight = THRESHOLD;
            }
        }

        QString code = getCizuCode(word);
        QPair<QString, size_t> code_weight(code, weight);
        QPair<QString, QPair<QString, size_t>> item(word, code_weight);

        user_dict.push_back(item);

        main_dict.push_back(item);
        main_dict_set.insert(word);
    }

    size_t new_size = main_dict_set.size();

    return (new_size - old_size);
}

QVector<QPair<QString, size_t> > RimeWubiDictMngr::getHanziCodeWeight(const QString &hz)
{
    Q_ASSERT(isHanzi(hz));

    QVector<QPair<QString, size_t>> res;

    QStringList all_code = getHanziAllCode(hz);
    // 找到最小长度的编码
    int min_len = 5;
    for (auto &code : all_code) {
        if (code.length() < min_len) {
            min_len = code.length();
        }
    }
    Q_ASSERT(min_len > 0 && min_len < 5);

    // 编码最短的权重最大，编码最长权重越小
    size_t freq = word_freq.value(hz);
    for (auto &code : all_code) {
        float factor = 1.0 - (code.length()-min_len) * 0.2;
        size_t weight = (size_t)(factor * freq);
        if (weight < THRESHOLD) {
            weight = 0;
        }
        QPair<QString, size_t> item(code, weight);
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
    user_words_set.clear();

    QTextStream in(&infile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        for (auto &word : list) {
            if (!isWordValid(word) || isHanzi(word) || user_words_set.contains(word)) {
                continue;
            }
            user_words.append(word);
            user_words_set.insert(word);
        }
    }
    infile.close();

    return user_words_set.size();
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
        QString hanzicode1 = getHanziFullcode(cz[0]);
        QString hanzicode2 = getHanziFullcode(cz[1]);
        QString hanzicode3 = getHanziFullcode(cz[2]);
        QString hanzicode4 = getHanziFullcode(cz[cz.length()-1]);
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

bool RimeWubiDictMngr::isCodeValid(const QString &c)
{
    if (c.length() < 1 || c.length() > 4) {
        return false;
    }

    for (int i = 0; i < c.length(); ++i) {
        if (!c[i].isLetter()) {
            return false;
        }
    }

    return true;
}

bool RimeWubiDictMngr::isHanzi(const QString &wd)
{
    return (wd.length() == 1);
}

int RimeWubiDictMngr::saveDict(QVector<QPair<QString, QPair<QString, size_t> > > &dict, const QString &filename)
{
    QFile outfile(filename);
    if (!outfile.open(QIODevice::ReadWrite | QFile::Truncate)) {
        qDebug() << "In saveDict(), openning file" << filename << "failed!";
        return -1;
    }

    QTextStream ts(&outfile);
    for (int index = 0; index < dict.size(); ++index) {
        QString word  = dict.at(index).first;
        QString code  = dict.at(index).second.first;
        size_t weight = dict.at(index).second.second;
        ts << word << "\t" << code << "\t" << weight << endl;
    }
    outfile.close();

    return dict.size();
}

