#include <QtTest>
#include "urltools.h"

class UrlToolsTest : public QObject
{
    Q_OBJECT
private slots:
    void acceptsHttpAndHttps();
    void rejectsUnsupportedOrEmptyUrls();
    void trimsUserInput();
};

void UrlToolsTest::acceptsHttpAndHttps()
{
    QVERIFY(UrlTools::isHttpUrl(UrlTools::fromUserInput("https://example.com/path")));
    QVERIFY(UrlTools::isHttpUrl(UrlTools::fromUserInput("http://example.com")));
}

void UrlToolsTest::rejectsUnsupportedOrEmptyUrls()
{
    QVERIFY(!UrlTools::isHttpUrl(UrlTools::fromUserInput("")));
    QVERIFY(!UrlTools::isHttpUrl(UrlTools::fromUserInput("ftp://example.com/file")));
    QVERIFY(!UrlTools::isHttpUrl(UrlTools::fromUserInput("not a URL")));
}

void UrlToolsTest::trimsUserInput()
{
    const QUrl url = UrlTools::fromUserInput("  https://example.com  ");
    QCOMPARE(url.toString(), QStringLiteral("https://example.com"));
}

QTEST_APPLESS_MAIN(UrlToolsTest)
#include "urltools_test.moc"
