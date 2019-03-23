#include "IndexWorker.h"
#include <QThread>
#include <QException>
#include <QThreadPool>
#include <QDirIterator>
#include <indexrunner.h>
#include <MainWindow.h>
#include <QDir>
#include <algorithm>
#include <Trigram.h>
#include <QDebug>

IndexWorker::IndexWorker(QObject *parent) : QObject(nullptr) {
    // TODO: rework?
    ptr = qobject_cast<MainWindow*>(parent);
}

void IndexWorker::index(QString dir) {
    QDirIterator it(dir, QDir::NoSymLinks | QDir::Hidden | QDir::Files, QDirIterator::Subdirectories);
    QVector<QString> files;
    int cnt = 0;
    while (it.hasNext()) {
        if(QThread::currentThread()->isInterruptionRequested()) {
            emit indexation_finished();
            return;
        }
        files.push_back(it.next());
        cnt++;
    }
    emit indexation_started(cnt);

    auto runner_thread = new QThread();
    auto runner = new IndexRunner(files);
    runner->moveToThread(runner_thread);
    connect(runner, &IndexRunner::progress, ptr, &MainWindow::add_progress);
    connect(runner, &IndexRunner::get_trigrams, ptr, &MainWindow::get_trigrams);
    connect(this, &IndexWorker::start_runner, runner, &IndexRunner::run);
    runner_thread->start();
    emit start_runner();
    while(!runner_thread->wait(50)) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            runner_thread->requestInterruption();
            runner_thread->wait();
            delete runner_thread;
            emit indexation_finished();
            return;
        }
    }
    runner_thread->quit();
    runner_thread->wait();
    delete runner_thread;
    emit indexation_finished();
}

void IndexWorker::reindex(QString path) {
    QVector<QString> file;
    file.push_back(path);
    auto runner_thread = new QThread();
    auto runner = new IndexRunner(file);
    runner->moveToThread(runner_thread);
    connect(runner, &IndexRunner::progress, ptr, &MainWindow::add_progress);
    connect(runner, &IndexRunner::get_trigrams, ptr, &MainWindow::get_trigrams);
    connect(this, &IndexWorker::start_runner, runner, &IndexRunner::run);
    runner_thread->start();
    emit start_runner();
    while(!runner_thread->wait(50)) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            runner_thread->requestInterruption();
            runner_thread->wait();
            delete runner_thread;
            emit reindexation_finished();
            return;
        }
    }
    runner_thread->quit();
    runner_thread->wait();
    delete runner_thread;
    emit reindexation_finished();
}
