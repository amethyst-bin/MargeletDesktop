#pragma once

#include <QPixmap>
#include <QColor>
#include <QRect>
#include <QPainter>

namespace Margy::Badges {

[[nodiscard]] QPixmap GenerateBadgeIcon(const QColor &color, int size = 20, qreal devicePixelRatio = 1.0);

void PaintBadgeIcon(QPainter &p, const QRect &rect, const QColor &color);

} // namespace Margy::Badges
