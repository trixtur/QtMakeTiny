#include "maketiny.h"
#include "ui_maketiny.h"

#include <QApplication>
#include <QClipboard>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

Q_LOGGING_CATEGORY(tinyLog, "maketiny.network")

MakeTiny::MakeTiny(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MakeTiny),
      networkManager(new QNetworkAccessManager(this)),
      baseUrl(QStringLiteral("https://tinyurl.com/api-create.php"))
{
    ui->setupUi(this);
    ui->serviceCombo->addItem(tr("TinyURL"));
    connect(ui->actionE_xit, &QAction::triggered, this, &QWidget::close);
    connect(ui->mktny_button, &QPushButton::clicked, this, &MakeTiny::makeTiny);
    connect(ui->inputUrl, &QLineEdit::returnPressed, this, &MakeTiny::makeTiny);
    connect(ui->tinyURL_Input, &QLineEdit::returnPressed, this, &MakeTiny::reverseLookup);
    connect(ui->rev_button, &QPushButton::clicked, this, &MakeTiny::reverseLookup);
    connect(ui->tinyUrl_output, &QLineEdit::selectionChanged, this, &MakeTiny::copyToClipboard);
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        if (reply->property("operation").toString() == QStringLiteral("reverse"))
            handleReverseReply(reply);
        else
            handleShortenReply(reply);
    });
    ui->statusBar->showMessage(tr("Ready"));
    qCInfo(tinyLog) << "application initialized" << "service" << baseUrl;
}

MakeTiny::~MakeTiny() { delete ui; }

void MakeTiny::makeTiny()
{
    const QUrl url = UrlTools::fromUserInput(ui->inputUrl->text());
    if (!UrlTools::isHttpUrl(url)) {
        showError(tr("Enter a valid HTTP or HTTPS URL."));
        return;
    }
    QNetworkRequest request(baseUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    QNetworkReply *reply = networkManager->post(request,
        QByteArrayLiteral("url=") + QUrl::toPercentEncoding(url.toString()));
    reply->setProperty("operation", QStringLiteral("shorten"));
    setBusy(true);
    ui->tinyUrl_output->clear();
    ui->statusBar->showMessage(tr("Creating short URL…"));
    qCInfo(tinyLog) << "shorten request" << url;
}

void MakeTiny::reverseLookup()
{
    const QUrl url = UrlTools::fromUserInput(ui->tinyURL_Input->text());
    if (!UrlTools::isHttpUrl(url)) {
        showError(tr("Enter a valid HTTP or HTTPS short URL."));
        return;
    }
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply = networkManager->get(request);
    reply->setProperty("operation", QStringLiteral("reverse"));
    setBusy(true);
    ui->LongURL_Output->setPlainText(tr("Following redirect…"));
    qCInfo(tinyLog) << "reverse request" << url;
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

void MakeTiny::handleReverseReply(QNetworkReply *reply)
{
    setBusy(false);
    if (reply->error() != QNetworkReply::NoError) {
        showError(reply->errorString());
        qCWarning(tinyLog) << "reverse failed" << reply->error() << reply->errorString();
    } else {
        ui->LongURL_Output->setPlainText(reply->url().toString());
        ui->statusBar->showMessage(tr("Redirect resolved"));
        qCInfo(tinyLog) << "reverse succeeded" << reply->url();
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
