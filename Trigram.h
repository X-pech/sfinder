#ifndef TRIGRAM_H
#define TRIGRAM_H

#include <QString>
#include <QSet>
#include <QtCore/QVector>

struct Trigram {
    static const int TEXT_CEIL = 20000;
    QString path;
    QSet<quint32> trigrams;

    bool check(Trigram const &other);

    void from_file(QString const &str);

    void from_string(QString const &s);

    void clear();

};

#endif // TRIGRAM_H
