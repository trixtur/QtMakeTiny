#ifndef URLTOOLS_H
#define URLTOOLS_H
#include <QUrl>
namespace UrlTools {
QUrl fromUserInput(const QString &input);
bool isHttpUrl(const QUrl &url);
}
#endif // URLTOOLS_H
