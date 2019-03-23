#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "Trigram.h"
#include <QVector>
#include <QFileSystemWatcher>
#include <QDir>
#include <QString>
#include <QTime>
#include <QMap>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

public slots:

    void browse_button_pushed();

    //search
    void search_button_pushed();

    void get_search_results(QString file, QVector<QPair<int, int> > places);

    //index
    void index_pushed();

    void get_trigrams(Trigram);

    //both
    void thread_started(int files_cnt);

    void add_progress();

    void thread_finished();

    void cancel_pushed();

    void dir_changed(const QString &path);

    void search_finished();

    void indexation_finished();

    void reindexation_finished();

signals:

    void start_search(QString str, QVector<Trigram> data);

    void start_indexation(QString dir);

    void start_reindex(QString path);

private:
    void change_enabled(const bool &browse = false, const bool &index = false, const bool &cind = false,
                        const bool &search = false, const bool &csrch = false);

    Ui::MainWindow *ui;
    QThread thread;
    QString dir;
    QVector<Trigram> data;
    QTime timer;
    QFileSystemWatcher watcher;
    int changed;
};

#endif // MAINWINDOW_H
