#ifndef MAKETINY_H
#define MAKETINY_H

#include <QMainWindow>
#include <QNetworkReply>
#include "urltools.h"

namespace Ui { class MakeTiny; }

class MakeTiny : public QMainWindow
{
    Q_OBJECT
public:
    explicit MakeTiny(QWidget *parent = nullptr);
    ~MakeTiny();

private slots:
    void makeTiny();
    void reverseLookup();
    void copyToClipboard();
    void handleShortenReply(QNetworkReply *reply);
    void handleReverseReply(QNetworkReply *reply);

private:
    Ui::MakeTiny *ui;
    QNetworkAccessManager *networkManager;
    QUrl baseUrl;
    void setBusy(bool busy);
    void showError(const QString &message);
};

#endif // MAKETINY_H
