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
		} else if (ch == '+' || ch == ' ' || ch == '-' || ch == '(' || ch == ')') {
			result.append(ch);
		} else if (ch == QChar(0x2022)) {
			result.append(ch);
		} else {
			result.append(QChar(0x2022));
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
