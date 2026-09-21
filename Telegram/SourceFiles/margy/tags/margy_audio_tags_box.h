#pragma once

#include "margy/tags/margy_audio_tags.h"

#include <QDialog>

namespace Margy::Tags {

class AudioTagsBox final : public QDialog {
	Q_OBJECT

public:
	explicit AudioTagsBox(
		QWidget *parent,
		const QString &filePath,
		const QString &initialTitle = QString(),
		const QString &initialArtist = QString());
	~AudioTagsBox() override = default;

	static void Show(
		QWidget *parent,
		const QString &filePath,
		const QString &initialTitle = QString(),
		const QString &initialArtist = QString());

private:
	void setupUi();
	void apply();

	QString _filePath;
	QString _title;
	QString _artist;
	QByteArray _cover;
};

} // namespace Margy::Tags
