#include "margy/badges/margy_badge_icon.h"
#include "margy/badges/margy_badge_icons_data.h"

#include <QPainterPath>
#include <map>
#include <tuple>

namespace Margy::Badges {
namespace {

using IconKey = std::tuple<QRgb, int, int, QString>;
std::map<IconKey, QPixmap> kIconCache;

} // namespace

QPixmap GenerateBadgeIcon(const QColor &color, int size, qreal devicePixelRatio, const QString &customIconId) {
	if (size <= 0) {
		size = 20;
	}
	if (devicePixelRatio <= 0.0) {
		devicePixelRatio = 1.0;
	}

	const auto ratioInt = static_cast<int>(devicePixelRatio * 100);
	const auto key = IconKey(color.rgb(), size, ratioInt, customIconId);
	const auto it = kIconCache.find(key);
	if (it != kIconCache.end()) {
		return it->second;
	}

	const auto pixelSize = static_cast<int>(size * devicePixelRatio);
	QPixmap pixmap(pixelSize, pixelSize);
	pixmap.fill(Qt::transparent);
	pixmap.setDevicePixelRatio(devicePixelRatio);

	if (customIconId == u"kent"_q || customIconId == u"yoxi"_q) {
		const auto img = (customIconId == u"kent"_q)
			? GetKentBadgeImage()
			: GetYoxiBadgeImage();
		QPainter p(&pixmap);
		p.setRenderHint(QPainter::Antialiasing);
		p.setRenderHint(QPainter::SmoothPixmapTransform);
		p.drawImage(QRectF(0, 0, size, size), img);
		kIconCache[key] = pixmap;
		return pixmap;
	}

	{
		QPainter p(&pixmap);
		p.setRenderHint(QPainter::Antialiasing);

		const auto scale = size / 24.0;
		const auto mapPt = [&](qreal x, qreal y) -> QPointF {
			return QPointF(x * scale, y * scale);
		};

		// 1. Background rounded rect
		const auto radius = 5.0 * scale;
		QPainterPath bgPath;
		bgPath.addRoundedRect(QRectF(0, 0, size, size), radius, radius);
		p.fillPath(bgPath, color);

		// 2. Left wing (#FFFFFF)
		QPainterPath leftWing;
		leftWing.moveTo(mapPt(12.0, 6.64));
		leftWing.lineTo(mapPt(6.12, 14.99));
		leftWing.lineTo(mapPt(11.57, 13.14));
		leftWing.closeSubpath();
		p.fillPath(leftWing, QColor(0xFF, 0xFF, 0xFF));

		// 3. Right wing (#EEF3FA)
		QPainterPath rightWing;
		rightWing.moveTo(mapPt(12.0, 6.64));
		rightWing.lineTo(mapPt(17.88, 14.99));
		rightWing.lineTo(mapPt(12.43, 13.14));
		rightWing.closeSubpath();
		p.fillPath(rightWing, QColor(0xEE, 0xF3, 0xFA));

		// 4. Keel (#CCD5E9)
		QPainterPath keel;
		keel.moveTo(mapPt(12.0, 6.64));
		keel.lineTo(mapPt(11.57, 13.14));
		keel.lineTo(mapPt(12.0, 14.25));
		keel.lineTo(mapPt(12.43, 13.14));
		keel.closeSubpath();
		p.fillPath(keel, QColor(0xCC, 0xD5, 0xE9));
	}

	kIconCache[key] = pixmap;
	return pixmap;
}

void PaintBadgeIcon(QPainter &p, const QRect &rect, const QColor &color, const QString &customIconId) {
	if (rect.isEmpty()) {
		return;
	}
	const auto size = std::min(rect.width(), rect.height());
	const auto ratio = p.device() ? p.device()->devicePixelRatioF() : 1.0;
	const auto icon = GenerateBadgeIcon(color, size, ratio, customIconId);

	const auto x = rect.x() + (rect.width() - size) / 2;
	const auto y = rect.y() + (rect.height() - size) / 2;
	p.drawPixmap(x, y, icon);
}

} // namespace Margy::Badges
