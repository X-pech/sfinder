#include "SearchWorker.h"
#include <QThread>
#include <QException>
#include <QFile>
#include <QVector>
#include <QTextStream>
#include <string>
#include <algorithm>
#include <search.h>
#include <QDebug>
#include <experimental/algorithm>
#include <experimental/functional>
#include <iostream>
#include <functional>

SearchWorker::SearchWorker(QObject *parent) : QObject(parent) {

}

void SearchWorker::search(QString pattern, QVector<Trigram> data) {
    emit search_started(data.size());
    Trigram pattern_tr;
    pattern_tr.from_string(pattern);
    for (int i = 0; i < data.size(); i++) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit search_finished();
            return;
        }
        if (pattern_tr.check(data[i])) {

            QFile file(data[i].path);
            if (!file.open(QFile::ReadOnly)) {
                emit progress();
                continue;
            }

            QVector<QPair<int, int> > ans;
            QTextStream in(&file);
            in.setCodec("UTF-8");
            QString current_line;
            int lid = 0;
            std::string std_pattern = pattern.toStdString();
            while ((current_line = in.readLine()).size() > 0) {

                if (QThread::currentThread()->isInterruptionRequested()) {
                    emit search_finished();
                    return;
                }

                lid++;

                std::string std_line = current_line.toStdString();
                auto it = std::search(std_line.begin(), std_line.end(), std::boyer_moore_searcher(std_pattern.begin(), std_pattern.end()));
                while (it != std_line.end()) {
                    if (QThread::currentThread()->isInterruptionRequested()) {
                        emit search_finished();
                        return;
                    }
                    int pos = (int)(it - std_line.begin());

                    it = std::search(it + 1, std_line.end(), std::boyer_moore_searcher(std_pattern.begin(), std_pattern.end()));
                    ans.push_back(QPair<int, int>(lid, pos));
                }
            }
            if(ans.size() > 0)
              emit get_search_res(data[i].path, ans);
        }
        emit progress();
    }
    emit search_finished();
}

