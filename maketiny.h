#ifndef MAKETINY_H
#define MAKETINY_H

#include <QMainWindow>
#include <QUrl>
#include <QString>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QObject>
#include <QStringList>
#include <QIODevice>
#include <QWebFrame>
#include <QWebPage>
#include <QClipboard>

namespace Ui {
    class MakeTiny;
}

class MakeTiny : public QMainWindow
{
    Q_OBJECT

public:
    explicit MakeTiny(QWidget *parent = 0);
    ~MakeTiny();
    void createActions();
private slots:
    void MkTny();
    void PageData(QNetworkReply*);
    void CallReverse(QNetworkReply*);
    void Reverse();
    void FrameLoad(bool);
    void CopyToClip();

private:
    Ui::MakeTiny *ui;
    QNetworkAccessManager *manager;
    QNetworkAccessManager *reverseManager;
    QWebFrame *frame;
    QWebPage *page;
    QNetworkRequest request;
    QString baseURL;
};

#endif // MAKETINY_H
