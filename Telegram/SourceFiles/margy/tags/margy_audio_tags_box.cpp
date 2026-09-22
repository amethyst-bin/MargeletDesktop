#include "margy/tags/margy_audio_tags_box.h"
#include "margy/tags/margy_audio_tags.h"
#include "boxes/abstract_box.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/ui_utility.h"
#include "ui/toast/toast.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"
#include "styles/style_settings.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QFile>

namespace Margy::Tags {

AudioTagsBox::AudioTagsBox(
	QWidget *parent,
	const QString &filePath,
	const QString &initialTitle,
	const QString &initialArtist)
: _filePath(filePath)
, _title(initialTitle)
, _artist(initialArtist) {
}

void AudioTagsBox::Show(
		QWidget *parent,
		const QString &filePath,
		const QString &initialTitle,
		const QString &initialArtist) {
	::Ui::show(::Box<AudioTagsBox>(filePath, initialTitle, initialArtist));
}

void AudioTagsBox::prepare() {
	setTitle(rpl::single(u"Редактор аудио тегов"_q));
	setDimensions(st::boxWideWidth, 380);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	content->add(
		object_ptr<::Ui::FlatLabel>(
			content,
			u"Файл: "_q + QFileInfo(_filePath).fileName(),
			st::boxLabel),
		st::boxRowPadding);

	_titleInput = content->add(
		object_ptr<::Ui::InputField>(
			content,
			st::defaultInputField,
			rpl::single(u"Название трека"_q),
			_title),
		st::boxRowPadding);

	_artistInput = content->add(
		object_ptr<::Ui::InputField>(
			content,
			st::defaultInputField,
			rpl::single(u"Исполнитель"_q),
			_artist),
		st::boxRowPadding);

	const auto coverBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"Выбрать обложку..."_q),
			st::settingsButton),
		st::boxRowPadding);
	coverBtn->setClickedCallback([=] {
		const auto path = QFileDialog::getOpenFileName(
			nullptr,
			u"Выбор обложки"_q,
			QString(),
			u"Изображения (*.jpg *.jpeg *.png)"_q);
		if (!path.isEmpty()) {
			auto imgFile = QFile(path);
			if (imgFile.open(QIODevice::ReadOnly)) {
				_cover = imgFile.readAll();
				::Ui::Toast::Show(this, u"Обложка выбрана!"_q);
			}
		}
	});

	addButton(rpl::single(u"Сохранить"_q), [=] {
		_title = _titleInput->getLastText().trimmed();
		_artist = _artistInput->getLastText().trimmed();
		apply();
		closeBox();
	});

	addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		closeBox();
	});
}

void AudioTagsBox::apply() {
	TagData data;
	data.title = _title;
	data.artist = _artist;
	data.cover = _cover;
	WriteTags(_filePath, data);
}

} // namespace Margy::Tags
