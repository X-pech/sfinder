#ifndef TRIGRAMWORKER_H
#define TRIGRAMWORKER_H

#include <QObject>
#include <QVector>
#include <Trigram.h>
#include <MainWindow.h>
#include <QThread>

class IndexWorker : public QObject {
Q_OBJECT
private:

    MainWindow * ptr;

public:
    explicit IndexWorker(QObject *parent = nullptr);

public slots:

    void index(QString dir);

    void reindex(QString path);
signals:

    void indexation_started(int files_cnt);

    void reindexation_finished();

    void indexation_finished();

    void start_runner();
};

#endif // TRIGRAMWORKER_H
