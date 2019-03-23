#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "indexrunner.h"
#include <QFileDialog>
#include <QVector>
#include <QDebug>
#include <QPair>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QThreadPool>
#include <IndexWorker.h>
#include <SearchWorker.h>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {

    ui->setupUi(this);
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse_button_pushed()));
    //connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::search_button_pushed);
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(search_button_pushed()));
    connect(ui->indexButton, SIGNAL(clicked()), this, SLOT(index_pushed()));
    connect(ui->cancelIndexButton, SIGNAL(clicked()), this, SLOT(cancel_pushed()));
    connect(ui->cancelSearchButton, SIGNAL(clicked()), this, SLOT(cancel_pushed()));

    changed = 0;
    ui->number->setText(QString::number(changed));
    connect(&watcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::dir_changed);

    qRegisterMetaType<Trigram>("Trigram");
    qRegisterMetaType<QVector<Trigram>>("QVector<Trigram>");
    qRegisterMetaType<QVector<QPair<int, int>>>("QVector<QPair<int,int>>");
}

MainWindow::~MainWindow() {
    thread.requestInterruption();
    thread.quit();
    thread.wait();
}

void MainWindow::browse_button_pushed() {
    ui->dirEdit->setText(QFileDialog::getExistingDirectory());
    change_enabled(true, true, false, false, false);
}

void MainWindow::search_button_pushed() {
    if (ui->stringEdit->text().trimmed().size() == 0) {
        ui->statusBar->showMessage(QString("Empty string"));
        return;
    }
    if (data.empty()) {
        ui->statusBar->showMessage(QString("Index not finished"));
        return;
    }
    if (!QDir(dir).exists()) {
        ui->statusBar->showMessage("Directory not found");
        return;
    }
    ui->searchWidget->clear();
    change_enabled(false, false, false, false, true);
    if (changed > 0) {
        ui->number->setText("0");
    }
    timer.start();
    auto *worker = new SearchWorker();
    worker->moveToThread(&thread);
    connect(this, &MainWindow::start_search, worker, &SearchWorker::search);
    connect(worker, &SearchWorker::search_started, this, &MainWindow::thread_started);
    connect(worker, &SearchWorker::progress, this, &MainWindow::add_progress);
    connect(worker, &SearchWorker::get_search_res, this, &MainWindow::get_search_results);
    connect(worker, &SearchWorker::search_finished, this, &MainWindow::thread_finished);
    connect(worker, &SearchWorker::search_finished, this, &MainWindow::search_finished);
    // Бага может быть: создается new Worker -- именно в самого себя
    connect(worker, &SearchWorker::search_finished, worker, &QObject::deleteLater);
    thread.start();
    emit start_search(ui->stringEdit->text().trimmed(), data);
}


void MainWindow::get_search_results(QString file, QVector<QPair<int, int> > places) {
    auto *item = new QTreeWidgetItem(ui->searchWidget);
    ui->searchWidget->addTopLevelItem(item);
    item->setText(0, QString::number(places.size()) + " entries: " + QDir(dir).relativeFilePath(file));
    for (auto i : places) {
        auto *child = new QTreeWidgetItem(item);
        child->setText(0, "At: " + QString::number(i.first) + ":" + QString::number(i.second));
        item->addChild(child);
    }
}

void MainWindow::index_pushed() {
    if (!QDir::isAbsolutePath(ui->dirEdit->text()) || !QDir(ui->dirEdit->text()).exists()) {
        ui->statusBar->showMessage(QString("Invalid path"));
        return;
    }
    dir = ui->dirEdit->text();
    ui->searchWidget->clear();
    if (!watcher.files().isEmpty()) watcher.removePaths(watcher.files());
    timer.start();
    changed = 0;
    ui->number->setText(QString::number(changed));

    change_enabled(false, false, true, false, false);
    data.clear();
    data.resize(0);
    data.squeeze();
    auto *worker = new IndexWorker(this);
    worker->moveToThread(&thread);
    connect(this, &MainWindow::start_indexation, worker, &IndexWorker::index);
    connect(worker, &IndexWorker::indexation_started, this, &MainWindow::thread_started);
    connect(worker, &IndexWorker::indexation_finished, this, &MainWindow::thread_finished);
    connect(worker, &IndexWorker::indexation_finished, worker, &QObject::deleteLater);
    connect(worker, &IndexWorker::indexation_finished, this, &MainWindow::indexation_finished);
    thread.start();
    emit start_indexation(dir);
}

void MainWindow::get_trigrams(Trigram tr) {
    data.push_back(tr);
    watcher.addPath(tr.path);
}

void MainWindow::thread_started(int files_cnt) {
    ui->progressBar->setRange(0, files_cnt);
    ui->progressBar->setValue(0);
}

void MainWindow::add_progress() {
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void MainWindow::thread_finished() {
    ui->statusBar->showMessage("Time elapsed: " + QString::number(timer.elapsed() / 1000.0) + QString(" sec."));
    thread.quit();
    thread.wait();
}

void MainWindow::cancel_pushed() {
    thread.requestInterruption();

    change_enabled(true, true, false, true);
}

void MainWindow::dir_changed(const QString &path) {
    changed++;
    ui->number->setText(QString::number(changed) + " START SEARCH AGAIN!");
    ui->searchWidget->clear();
    change_enabled(); //false, false, false);

    for (auto &i : data) {
        if (i.path == path) {
            i.clear();
        }
    }

    auto *worker = new IndexWorker(this);
    worker->moveToThread(&thread);
    connect(this, &MainWindow::start_reindex, worker, &IndexWorker::reindex);
    connect(worker, &IndexWorker::indexation_started, this, &MainWindow::thread_started);
    connect(worker, &IndexWorker::reindexation_finished, this, &MainWindow::thread_finished);
    connect(worker, &IndexWorker::reindexation_finished, worker, &QObject::deleteLater);
    connect(worker, &IndexWorker::reindexation_finished, this, &MainWindow::reindexation_finished);
    thread.start();
    emit start_reindex(path);
}

void MainWindow::reindexation_finished() {
    change_enabled(true, false, false, true, false);
}

void MainWindow::change_enabled(const bool &browse, const bool &index, const bool &cind, const bool &search, const bool &csrch) {
  ui->browseButton->setEnabled(browse);
  ui->indexButton->setEnabled(index);
  ui->cancelIndexButton->setEnabled(cind);
  ui->searchButton->setEnabled(search);
  ui->cancelSearchButton->setEnabled(csrch);
}

void MainWindow::indexation_finished() {
    change_enabled(true, false, false, true, false);
}

void MainWindow::search_finished() {
    change_enabled(true, false, false, true, false);
}
