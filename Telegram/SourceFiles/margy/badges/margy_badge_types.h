#pragma once

#include <QString>
#include <QColor>
#include <vector>
#include <cstdint>

namespace Margy::Badges {

struct Badge {
	int64_t peerId = 0;
	QString titleEn;
	QString titleRu;
	QString aboutEn;
	QString aboutRu;
	QColor color = QColor(0x8D, 0xD1, 0xB0);
	QString url;
	QString username;

	[[nodiscard]] QString title(bool isRussian = true) const {
		if (isRussian && !titleRu.isEmpty()) {
			return titleRu;
		}
		return titleEn.isEmpty() ? titleRu : titleEn;
	}

	[[nodiscard]] QString about(bool isRussian = true) const {
		if (isRussian && !aboutRu.isEmpty()) {
			return aboutRu;
		}
		return aboutEn.isEmpty() ? aboutRu : aboutEn;
	}
};

[[nodiscard]] inline QColor ParseColor(const QString &value, const QColor &fallback = QColor(0x8D, 0xD1, 0xB0)) {
	if (value.isEmpty()) {
		return fallback;
	}
	QString hex = value.trimmed();
	if (hex.startsWith('#')) {
		hex = hex.mid(1);
	}
	bool ok = false;
	const auto num = hex.toULongLong(&ok, 16);
	if (!ok) {
		return fallback;
	}
	if (hex.length() <= 6) {
		const auto r = static_cast<int>((num >> 16) & 0xFF);
		const auto g = static_cast<int>((num >> 8) & 0xFF);
		const auto b = static_cast<int>(num & 0xFF);
		return QColor(r, g, b, 255);
	} else {
		const auto a = static_cast<int>((num >> 24) & 0xFF);
		const auto r = static_cast<int>((num >> 16) & 0xFF);
		const auto g = static_cast<int>((num >> 8) & 0xFF);
		const auto b = static_cast<int>(num & 0xFF);
		return QColor(r, g, b, a);
	}
}

inline const std::vector<Badge>& BuiltInBadges() {
	static const std::vector<Badge> kBuiltIn = {
		Badge{
			.peerId = 7811378656LL,
			.titleEn = "Margelet Desktop Owner",
			.titleRu = "Владелец Margelet Desktop",
			.aboutEn = "Official owner and developer of Margelet Desktop.",
			.aboutRu = "Официальный создатель и разработчик Margelet Desktop.",
			.color = QColor(0x8D, 0xD1, 0xB0),
			.url = "https://t.me/margydesktop",
			.username = "tinytosha",
		},
		Badge{
			.peerId = -1002271810484LL,
			.titleEn = "Official Margelet Desktop Channel",
			.titleRu = "Официальный канал Margelet Desktop",
			.aboutEn = "Official Telegram channel for Margelet Desktop.",
			.aboutRu = "Официальный канал форка Margelet Desktop.",
			.color = QColor(0x8D, 0xD1, 0xB0),
			.url = "https://t.me/margydesktop",
			.username = "margydesktop",
		},
		Badge{
			.peerId = 7826361017LL,
			.titleEn = "Margy creator",
			.titleRu = "Создатель Margy",
			.aboutEn = "Made this fork.",
			.aboutRu = "Сделал этот форк.",
			.color = QColor(0x8D, 0xD1, 0xB0),
			.url = "https://t.me/narezanyinf",
		},
		Badge{
			.peerId = 8675724972LL,
			.titleEn = "Best friend",
			.titleRu = "Лучший друг",
			.aboutEn = "The creator's best friend.",
			.aboutRu = "Лучший друг создателя форка.",
			.color = QColor(0xB7, 0xA8, 0xE0),
			.url = "https://t.me/mizoginichka_y",
		},
		Badge{
			.peerId = -4426743212LL,
			.titleEn = "Official channel",
			.titleRu = "Официальный канал",
			.aboutEn = "The fork's own channel.",
			.aboutRu = "Собственный канал форка.",
			.color = QColor(0x8D, 0xD1, 0xB0),
			.url = "https://t.me/margeletter",
		},
		Badge{
			.peerId = -4436273526LL,
			.titleEn = "Official forum",
			.titleRu = "Официальный форум",
			.aboutEn = "The fork's own chat.",
			.aboutRu = "Собственный чат форка.",
			.color = QColor(0x8D, 0xD1, 0xB0),
			.url = "https://t.me/margeletforum",
		},
		Badge{
			.peerId = 7826361017LL,
			.titleEn = "Cat in Margy",
			.titleRu = "Кот в Margy",
			.aboutEn = "Their cat lives inside the app.",
			.aboutRu = "Его кот живёт внутри приложения.",
			.color = QColor(0xEB, 0xC8, 0x5C),
			.url = "",
		},
		Badge{
			.peerId = 6092720414LL,
			.titleEn = "Cat in Margy",
			.titleRu = "Кот в Margy",
			.aboutEn = "Their cat lives inside the app.",
			.aboutRu = "Его кот живёт внутри приложения.",
			.color = QColor(0xEB, 0xC8, 0x5C),
			.url = "",
		},
		Badge{
			.peerId = 7097556004LL,
			.titleEn = "Cat in Margy",
			.titleRu = "Кот в Margy",
			.aboutEn = "Their cat lives inside the app.",
			.aboutRu = "Его кот живёт внутри приложения.",
			.color = QColor(0xEB, 0xC8, 0x5C),
			.url = "",
		},
		Badge{
			.peerId = 7993344828LL,
			.titleEn = "Cat in Margy",
			.titleRu = "Кот в Margy",
			.aboutEn = "Their cat lives inside the app.",
			.aboutRu = "Его кот живёт внутри приложения.",
			.color = QColor(0xEB, 0xC8, 0x5C),
			.url = "",
		},
		Badge{
			.peerId = 7241514685LL,
			.titleEn = "Cat in Margy",
			.titleRu = "Кот в Margy",
			.aboutEn = "Their cat lives inside the app.",
			.aboutRu = "Его кот живёт внутри приложения.",
			.color = QColor(0xEB, 0xC8, 0x5C),
			.url = "",
		},
		Badge{
			.peerId = 2018673403LL,
			.titleEn = "Cat in Margy",
			.titleRu = "Кот в Margy",
			.aboutEn = "Their cat lives inside the app.",
			.aboutRu = "Его кот живёт внутри приложения.",
			.color = QColor(0xEB, 0xC8, 0x5C),
			.url = "",
		},
	};
	return kBuiltIn;
}

} // namespace Margy::Badges
