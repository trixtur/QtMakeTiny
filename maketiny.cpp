#include "maketiny.h"
#include "ui_maketiny.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QClipboard>
//#define _DEBUG_


MakeTiny::MakeTiny(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MakeTiny)
{
    baseURL = "http://tinyurl.com/api-create.php";
    ui->setupUi(this);
    ui->serviceCombo->addItem("Default");
    reverseManager = new QNetworkAccessManager(this);
    page = new QWebPage(this);
    frame = page->mainFrame();
    manager = new QNetworkAccessManager(this);
}

MakeTiny::~MakeTiny()
{
    delete manager;
    delete reverseManager;
    delete page;
    delete ui;
}

void MakeTiny::createActions()
{
    connect(ui->actionE_xit, SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->mktny_button,SIGNAL(pressed()),this,SLOT(MkTny()));
    connect(ui->inputUrl,SIGNAL(returnPressed()),this,SLOT(MkTny()));
    connect(ui->tinyURL_Input,SIGNAL(returnPressed()),this,SLOT(Reverse()));
    connect(ui->rev_button,SIGNAL(pressed()),this,SLOT(Reverse()));
    manager->connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(PageData(QNetworkReply*)));
    reverseManager->connect(reverseManager, SIGNAL(finished(QNetworkReply*)),this,SLOT(CallReverse(QNetworkReply*)));
    frame->connect(frame,SIGNAL(loadFinished(bool)),this,SLOT(FrameLoad(bool)));
    connect(ui->tinyUrl_output,SIGNAL(selectionChanged()),this,SLOT(CopyToClip()));

}

void MakeTiny::CopyToClip()
{
    QClipboard *clip = QApplication::clipboard();
    if(ui->tinyUrl_output->hasFocus() && ui->tinyUrl_output->text() != "")
    {
        clip->setText(ui->tinyUrl_output->text(), clip->Clipboard );
        ui->statusBar->clearMessage();
        ui->statusBar->showMessage("\""+(ui->tinyUrl_output->text()+"\" Copied to clipboard..."), 2000);
    }
}

void MakeTiny::Reverse()
{
    frame->load(QUrl(ui->tinyURL_Input->text().toAscii()));
    ui->LongURL_Output->clear();
    ui->LongURL_Output->append("Working...");
}

void MakeTiny::FrameLoad(bool loaded)
{
    QString mrequest;
    mrequest.append(ui->tinyURL_Input->text());
    reverseManager->get(QNetworkRequest(QUrl(mrequest.toAscii())));
}

void MakeTiny::CallReverse(QNetworkReply *reply)
{
    if(reply->rawHeaderList().contains("Location"))
    {
        ui->LongURL_Output->clear();
        ui->LongURL_Output->append(frame->title().toAscii()+"\n");
        ui->LongURL_Output->append(reply->rawHeader("Location"));
    }
    else
    {
        ui->LongURL_Output->append("Error: Provided URL is not Redirected.");
    }
}

void MakeTiny::MkTny()
{
    QString mrequest;
    mrequest.append("url=");
    MakeTiny::request.setUrl(QUrl(baseURL));
    mrequest.append(ui->inputUrl->text());
    manager->post(QNetworkRequest(QUrl(baseURL)), mrequest.toAscii());
}

void MakeTiny::PageData(QNetworkReply *reply)
{
    QString str;
    if(reply->error() != QNetworkReply::NoError)
    {
        #ifdef _DEBUG_
        qDebug() << "Request failed, " << reply->errorString();
        #endif
        return;
    }
    #ifdef _DEBUG_
    qDebug() << "Request succeeded";
    qDebug() << "There are " << reply->bytesAvailable() << "bytes available.";
    #endif

    if(reply->error() != QNetworkReply::NoError)
    {
        str.append(" ");
        str.append(reply->errorString());
    }
    else
    {
       str.append(reply->readAll());
    }

    ui->tinyUrl_output->clear();
    ui->tinyUrl_output->insert(str);
}
