#include "margy/seizure/margy_seizure.h"
#include "margy/margy_config.h"

#include <QDateTime>
#include <cmath>

namespace Margy::Seizure {

bool IsEnabled() {
	return Config::Instance().seizureMode();
}

void SetEnabled(bool enabled) {
	Config::Instance().setSeizureMode(enabled);
}

QColor CurrentColor() {
	if (!IsEnabled()) {
		return QColor(u"#8dd1b0"_q);
	}
	const auto msecs = QDateTime::currentMSecsSinceEpoch();
	const auto hue = std::fmod(msecs / 2500.0, 1.0);
	return QColor::fromHsvF(hue, 0.75, 0.95);
}

QString CurrentColorCss() {
	return CurrentColor().name();
}

} // namespace Margy::Seizure
