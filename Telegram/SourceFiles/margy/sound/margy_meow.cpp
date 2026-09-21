#include "margy/sound/margy_meow.h"
#include "margy/sound/margelet_meow_bytes.h"
#include "margy/margy_config.h"
#include "media/audio/media_audio.h"
#include "media/audio/media_audio_track.h"

#include <QWidget>
#include <QTimer>

namespace Margy::Sound {

void PlayMeow() {
	if (!Config::Instance().meowEnabled()) {
		return;
	}
	Config::Instance().setMeowHeard(true);

	static std::unique_ptr<Media::Audio::Track> track;
	if (!track) {
		track = Media::Audio::Current().createTrack();
		track->fillFromData(bytes::make_vector(kMeowData));
	}
	if (track) {
		track->playOnce();
	}
}

void TriggerMeow(QWidget *target) {
	PlayMeow();
	if (target) {
		const auto orig = target->geometry();
		target->setGeometry(orig.x(), orig.y() + 2, orig.width(), orig.height());
		QTimer::singleShot(80, target, [=] {
			target->setGeometry(orig.x(), orig.y() - 1, orig.width(), orig.height());
			QTimer::singleShot(80, target, [=] {
				target->setGeometry(orig);
			});
		});
	}
}

} // namespace Margy::Sound
