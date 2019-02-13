#ifndef RIME_WUBI_DICT_MNGR_H
#define RIME_WUBI_DICT_MNGR_H

#include <QString>
#include <QVector>
#include <QMultiMap>
#include <QMap>
#include <QSet>
#include <QPair>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QChar>
#include <QList>
#include <QRegExp>
#include <QDebug>

/*
 * 命名规范：
 *   hanzi  - 汉字，单字，英文Chinese character太长，不用
 *   cizu   - 词组，由多个汉字组成，用拼音cizu是为了和hanzi对应
 *   word   - 词语，包括单字hanzi和词组cizu
 *   text   - 所有类型的字符，包括汉字，字母，特殊符号以及它们的组合
 *   code   - 字或词对应的编码
 *   freq   - 字或词出现的频率
 *   weight - 字或词在码表中的权重，和词频相关
 *   dict   - 词典，亦称之为码表，对应Rime的wubiXX.dict.yaml文件
 *
 *   load   - 从文件读取到内存
 *   save   - 从内存保存到文件
 */

class RimeWubiDictMngr
{
public:
    RimeWubiDictMngr(const QString &, const QString &);
    RimeWubiDictMngr();

public:
    typedef enum {
        ADD_ALL,
        ADD_HIGH
    } add_mode_t;

public:
    int loadMainDict(const QString &filename);
    int saveMainDict(const QString &filename);
    int saveUserDict(const QString &filename);

    int expandMainDict(const QString &filename, add_mode_t mode);

private:
    bool isSetupSuccess();

    QVector<QPair<QString, size_t> > getHanziCodeWeight(const QString &hz);

    int loadHanziCode(const QString &filename);
    int loadWordFreq(const QString &filename);
    int loadUserWords(const QString &filename);

    QStringList getHanziAllCode(const QString &hz);
    QString     getHanziFullcode(const QString &hz);
    QString     getCizuCode(const QString &cz);

    bool isWordValid(const QString &wd);
    bool isCodeValid(const QString &c);
    bool isHanzi(const QString &wd);

    int saveDict(QVector<QPair<QString, QPair<QString, size_t> > > &dict, const QString &filename);

private:
    bool setup_success = false;

    const size_t THRESHOLD = 1000;

    QMultiMap<QString, QString> hanzi_code;
    QSet<QString>               hanzi_set;

    QMap<QString, size_t> word_freq;
    QSet<QString>         word_set;

    QStringList   user_words;
    QSet<QString> user_words_set;

    QVector<QPair<QString, QPair<QString, size_t> > > main_dict;
    QSet<QString>                                     main_dict_set;

    QVector<QPair<QString, QPair<QString, size_t> > > user_dict;

};

#endif // RIME_WUBI_DICT_MNGR_H
