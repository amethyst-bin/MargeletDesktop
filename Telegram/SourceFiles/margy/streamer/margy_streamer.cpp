#include "margy/streamer/margy_streamer.h"
#include "margy/margy_config.h"

namespace Margy::Streamer {

QString MaskPhone(const QString &phone) {
	if (phone.isEmpty()) {
		return phone;
	}
	QString result;
	result.reserve(phone.size());
	for (const auto &ch : phone) {
		if (ch.isDigit()) {
			result.append(QChar(0x2022));
		} else {
			result.append(ch);
		}
	}
	return result;
}

bool ShouldHideUsername(bool isSelf) {
	if (!StreamerMode()) {
		return false;
	}
	return isSelf ? StreamerHidesUsername() : StreamerHidesOthers();
}

QString MaskUsername(const QString &username, bool isSelf) {
	if (username.isEmpty() || !ShouldHideUsername(isSelf)) {
		return username;
	}
	return u"•••••••"_q;
}

} // namespace Margy::Streamer
