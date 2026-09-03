#include "urltools.h"
namespace UrlTools {
QUrl fromUserInput(const QString &input)
{
    const QString trimmed = input.trimmed();
    return trimmed.isEmpty() ? QUrl() : QUrl::fromUserInput(trimmed);
}
bool isHttpUrl(const QUrl &url)
{
    return url.isValid() && !url.host().isEmpty() &&
           (url.scheme().compare(QStringLiteral("http"), Qt::CaseInsensitive) == 0 ||
            url.scheme().compare(QStringLiteral("https"), Qt::CaseInsensitive) == 0);
}
}
