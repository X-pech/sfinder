#include "indexrunner.h"
#include <Trigram.h>
#include <QDebug>

IndexRunner::IndexRunner(QVector<QString> _files) : files(_files) {}

void IndexRunner::run() {
    for (QString s : files) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }

        Trigram trigram;
        trigram.from_file(s);

        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }

        emit get_trigrams(trigram);
        emit progress();
    }

    QThread::currentThread()->quit();
}
