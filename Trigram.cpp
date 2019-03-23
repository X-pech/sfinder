#include "Trigram.h"

#include <QFile>
#include <QDataStream>
#include <QtCore/QThread>

bool Trigram::check(Trigram const &other) {
    return (other.trigrams.contains(trigrams));
}

void Trigram::from_file(QString const &str) {
    trigrams.clear();

    path = str;
    QFile file(str);
    if (!(file.permissions() & QFileDevice::ReadUser) || !file.open(QIODevice::ReadOnly)) {
        return;
    }
    QDataStream in(&file);
    char buffer[1024 * 4];
    int len = 1024 * 4;
    int cur_len;
    quint32 cur_trigram = 0;
    while ((cur_len = in.readRawData(buffer, len)) > 0) {

        for (int i = 0; i < cur_len; i++) {

            if (QThread::currentThread()->isInterruptionRequested()) {
                break;
            }

            cur_trigram <<= 16;
            cur_trigram >>= 8;
            cur_trigram += static_cast<quint8>(buffer[i]);

            trigrams.insert(cur_trigram);
        }

        if (trigrams.size() > TEXT_CEIL) {
            trigrams.clear();
            return;
        }

    }
}

void Trigram::from_string(const QString &s) {
    trigrams.clear();
    quint32 cur_trigram = 0;
    auto arr = s.toUtf8();

    if (arr.size() < 3)
        return;

    cur_trigram += static_cast<quint8>(arr[0]);
    cur_trigram <<= 8;
    cur_trigram += static_cast<quint8>(arr[1]);

    for (int i = 2; i < arr.size(); i++) {

        cur_trigram <<= 16;
        cur_trigram >>= 8;
        cur_trigram += static_cast<quint8>(arr[i]);

        trigrams.insert(cur_trigram);

        if (trigrams.size() > TEXT_CEIL) {
            trigrams.clear();
            return;
        }

    }
}

void Trigram::clear() {
    trigrams.clear();
}
