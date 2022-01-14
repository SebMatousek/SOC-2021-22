#pragma once

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>
#include <QPixmap>

class FileDownloader : public QObject {
    Q_OBJECT
    QFile *m_file;
    bool m_isReady = true;

public:
    explicit FileDownloader(QObject *parent = 0) : QObject(parent) {}
    virtual ~FileDownloader() { delete m_file; }

    void downloadFileFromURL(const QString &url, const QString &filePath);

signals:
    void downloaded();

private slots:
    void onDownloadFileComplete(QNetworkReply *reply);
};
