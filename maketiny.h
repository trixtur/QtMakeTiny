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
    void handleSecurityReply(QNetworkReply *reply, bool reverse);

private:
    Ui::MakeTiny *ui;
    QNetworkAccessManager *networkManager;
    QUrl baseUrl;
    QUrl securityBackendUrl;
    QString pendingUrl;
    void setBusy(bool busy);
    void showError(const QString &message);
    void submitSecurityCheck(const QUrl &url, bool reverseLookup);
    void sendShortenRequest(const QUrl &url);
};

#endif // MAKETINY_H
