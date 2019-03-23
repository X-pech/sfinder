#ifndef SEARCHWORKER_H
#define SEARCHWORKER_H

#include <Trigram.h>
#include <QFileSystemWatcher>
#include <QObject>

class SearchWorker : public QObject {
Q_OBJECT

public:
    explicit SearchWorker(QObject *parent = nullptr);

private:
    void check();

signals:

    void search_started(int files_cnt);

    void progress();

    void get_search_res(QString file, QVector<QPair<int, int>> places);

    void search_finished();

public slots:

    void search(QString pattern, QVector<Trigram> data);
};

#endif // SEARCHWORKER_H
