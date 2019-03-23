#ifndef INDEXRUNNER_H
#define INDEXRUNNER_H

#include <QObject>
#include <QRunnable>
#include <QVector>
#include <QString>
#include <QThread>
#include <Trigram.h>

class IndexRunner : public QObject {
Q_OBJECT

private:
    QVector<QString> files;

public:
    IndexRunner(QVector<QString> _files);

signals:

    void progress();

    void get_trigrams(Trigram);

public slots:
    void run();
};

#endif // INDEXRUNNER_H
