#pragma once

#include <QString>
#include <QVariant>

namespace Margy::Scheme {

bool HandleUrl(const QString &url, const QVariant &context = QVariant());

} // namespace Margy::Scheme
