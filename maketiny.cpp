#include "maketiny.h"
#include "ui_maketiny.h"

#include <QApplication>
#include <QClipboard>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHBoxLayout>
#include <QVBoxLayout>

Q_LOGGING_CATEGORY(tinyLog, "maketiny.network")

MakeTiny::MakeTiny(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MakeTiny),
      networkManager(new QNetworkAccessManager(this)),
      baseUrl(QStringLiteral("https://tinyurl.com/api-create.php")),
      securityBackendUrl(qEnvironmentVariable("MAKETINY_BACKEND_URL", QStringLiteral("http://127.0.0.1:8787")))
{
    ui->setupUi(this);
    auto *centralLayout = new QVBoxLayout(ui->centralWidget);
    centralLayout->setContentsMargins(8, 8, 8, 8);
    centralLayout->addWidget(ui->tabWidget);

    auto *makeLayout = new QVBoxLayout(ui->tab);
    makeLayout->setContentsMargins(8, 8, 8, 8);
    makeLayout->addWidget(ui->inputUrl);
    makeLayout->addWidget(ui->layoutWidget);
    makeLayout->addWidget(ui->tinyUrl_output);
    makeLayout->addWidget(ui->label);
    makeLayout->addStretch();

    auto *reverseLayout = new QVBoxLayout(ui->tab_2);
    reverseLayout->setContentsMargins(8, 8, 8, 8);
    auto *reverseInputLayout = new QHBoxLayout;
    reverseInputLayout->addWidget(ui->tinyURL_Input);
    reverseInputLayout->addWidget(ui->rev_button);
    reverseLayout->addLayout(reverseInputLayout);
    reverseLayout->addWidget(ui->LongURL_Output);

    ui->serviceCombo->addItem(tr("TinyURL"));
    connect(ui->actionE_xit, &QAction::triggered, this, &QWidget::close);
    connect(ui->mktny_button, &QPushButton::clicked, this, &MakeTiny::makeTiny);
    connect(ui->inputUrl, &QLineEdit::returnPressed, this, &MakeTiny::makeTiny);
    connect(ui->tinyURL_Input, &QLineEdit::returnPressed, this, &MakeTiny::reverseLookup);
    connect(ui->rev_button, &QPushButton::clicked, this, &MakeTiny::reverseLookup);
    connect(ui->tinyUrl_output, &QLineEdit::selectionChanged, this, &MakeTiny::copyToClipboard);
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        const QString operation = reply->property("operation").toString();
        if (operation == QStringLiteral("check-shorten"))
            handleSecurityReply(reply, false);
        else if (operation == QStringLiteral("check-reverse"))
            handleSecurityReply(reply, true);
        else
            handleShortenReply(reply);
    });
    ui->statusBar->showMessage(tr("Ready"));
    qCInfo(tinyLog) << "application initialized" << "service" << baseUrl
                    << "security backend" << securityBackendUrl;
}

MakeTiny::~MakeTiny() { delete ui; }

void MakeTiny::makeTiny()
{
    const QUrl url = UrlTools::fromUserInput(ui->inputUrl->text());
    if (!UrlTools::isHttpUrl(url)) {
        showError(tr("Enter a valid HTTP or HTTPS URL."));
        return;
    }
    submitSecurityCheck(url, false);
}

void MakeTiny::sendShortenRequest(const QUrl &url)
{
    QNetworkRequest request(baseUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    QNetworkReply *reply = networkManager->post(request,
        QByteArrayLiteral("url=") + QUrl::toPercentEncoding(url.toString()));
    reply->setProperty("operation", QStringLiteral("shorten"));
    setBusy(true);
    ui->tinyUrl_output->clear();
    ui->statusBar->showMessage(tr("Creating short URL…"));
    qCInfo(tinyLog) << "shorten request after security check" << url;
}

void MakeTiny::reverseLookup()
{
    const QUrl url = UrlTools::fromUserInput(ui->tinyURL_Input->text());
    if (!UrlTools::isHttpUrl(url)) {
        showError(tr("Enter a valid HTTP or HTTPS short URL."));
        return;
    }
    submitSecurityCheck(url, true);
}

void MakeTiny::submitSecurityCheck(const QUrl &url, bool reverse)
{
    pendingUrl = url.toString();
    QNetworkRequest request(securityBackendUrl.resolved(QUrl(QStringLiteral("/api/url-check"))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    const QJsonObject payload{{QStringLiteral("url"), pendingUrl}};
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    reply->setProperty("operation", reverse ? QStringLiteral("check-reverse") : QStringLiteral("check-shorten"));
    setBusy(true);
    if (reverse) ui->LongURL_Output->setPlainText(tr("Checking URL safety…"));
    else ui->tinyUrl_output->clear();
    ui->statusBar->showMessage(tr("Checking URL safety…"));
    qCInfo(tinyLog) << "security check request" << url << "reverse" << reverse;
}

void MakeTiny::handleSecurityReply(QNetworkReply *reply, bool reverse)
{
    setBusy(false);
    if (reply->error() != QNetworkReply::NoError) {
        showError(tr("Security check unavailable: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }
    const QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    const QJsonObject result = document.object();
    const QString verdict = result.value(QStringLiteral("verdict")).toString();
    if (verdict == QStringLiteral("malicious")) {
        showError(tr("Blocked: the URL was flagged as potentially harmful."));
        qCWarning(tinyLog) << "blocked URL" << pendingUrl << result.value(QStringLiteral("findings"));
    } else if (verdict != QStringLiteral("safe")) {
        showError(tr("URL safety could not be confirmed."));
        qCWarning(tinyLog) << "inconclusive URL check" << pendingUrl << result.value(QStringLiteral("errors"));
    } else if (reverse) {
        const QJsonArray chain = result.value(QStringLiteral("chain")).toArray();
        ui->LongURL_Output->setPlainText(chain.isEmpty() ? pendingUrl : chain.last().toString());
        ui->statusBar->showMessage(tr("Redirect resolved and checked"));
    } else {
        sendShortenRequest(QUrl(pendingUrl));
    }
    reply->deleteLater();
}

void MakeTiny::copyToClipboard()
{
    if (!ui->tinyUrl_output->hasFocus() || ui->tinyUrl_output->text().isEmpty()) return;
    QApplication::clipboard()->setText(ui->tinyUrl_output->text());
    ui->statusBar->showMessage(tr("URL copied to clipboard"), 2000);
    qCInfo(tinyLog) << "short URL copied";
}

void MakeTiny::handleShortenReply(QNetworkReply *reply)
{
    setBusy(false);
    const QByteArray body = reply->readAll().trimmed();
    if (reply->error() != QNetworkReply::NoError || body.isEmpty()) {
        showError(reply->error() == QNetworkReply::NoError ? tr("The shortening service returned no URL.")
                                                           : reply->errorString());
        qCWarning(tinyLog) << "shorten failed" << reply->error() << reply->errorString();
    } else {
        ui->tinyUrl_output->setText(QString::fromUtf8(body));
        ui->statusBar->showMessage(tr("Short URL created"));
        qCInfo(tinyLog) << "shorten succeeded" << reply->url();
    }
    reply->deleteLater();
}

void MakeTiny::setBusy(bool busy)
{
    ui->mktny_button->setEnabled(!busy);
    ui->rev_button->setEnabled(!busy);
}

void MakeTiny::showError(const QString &message)
{
    ui->statusBar->showMessage(message);
    qCWarning(tinyLog) << message;
}
