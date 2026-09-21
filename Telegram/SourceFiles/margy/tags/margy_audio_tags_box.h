#pragma once

#include "ui/layers/box_content.h"
#include <QString>
#include <QByteArray>

namespace Ui {
class InputField;
} // namespace Ui

namespace Margy::Tags {

class AudioTagsBox final : public ::Ui::BoxContent {
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

protected:
	void prepare() override;

private:
	void apply();

	QString _filePath;
	QString _title;
	QString _artist;
	QByteArray _cover;
	::Ui::InputField *_titleInput = nullptr;
	::Ui::InputField *_artistInput = nullptr;
};

} // namespace Margy::Tags
