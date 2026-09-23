#pragma once

#include <QPixmap>
#include <QColor>
#include <QRect>
#include <QPainter>

#include <QString>

namespace Margy::Badges {

[[nodiscard]] QPixmap GenerateBadgeIcon(
	const QColor &color,
	int size = 20,
	qreal devicePixelRatio = 1.0,
	const QString &customIconId = QString());

void PaintBadgeIcon(
	QPainter &p,
	const QRect &rect,
	const QColor &color,
	const QString &customIconId = QString());

} // namespace Margy::Badges
