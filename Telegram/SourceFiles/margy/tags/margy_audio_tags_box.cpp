#include "margy/tags/margy_audio_tags_box.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>

namespace Margy::Tags {

AudioTagsBox::AudioTagsBox(
	QWidget *parent,
	const QString &filePath,
	const QString &initialTitle,
	const QString &initialArtist)
: QDialog(parent)
, _filePath(filePath)
, _title(initialTitle)
, _artist(initialArtist) {
	setupUi();
}

void AudioTagsBox::Show(
		QWidget *parent,
		const QString &filePath,
		const QString &initialTitle,
		const QString &initialArtist) {
	auto box = new AudioTagsBox(parent, filePath, initialTitle, initialArtist);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->open();
}

void AudioTagsBox::setupUi() {
	setWindowTitle(u"Редактор тегов аудио"_q);
	setMinimumWidth(380);

	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(20, 20, 20, 20);
	layout->setSpacing(12);

	const auto fileLabel = new QLabel(
		u"Файл: "_q + QFileInfo(_filePath).fileName(),
		this);
	fileLabel->setStyleSheet(u"color: #888888; font-size: 12px;"_q);
	layout->addWidget(fileLabel);

	const auto titleLabel = new QLabel(u"Название трека:"_q, this);
	layout->addWidget(titleLabel);

	const auto titleEdit = new QLineEdit(_title, this);
	titleEdit->setPlaceholderText(u"Введите название песни"_q);
	layout->addWidget(titleEdit);

	const auto artistLabel = new QLabel(u"Исполнитель:"_q, this);
	layout->addWidget(artistLabel);

	const auto artistEdit = new QLineEdit(_artist, this);
	artistEdit->setPlaceholderText(u"Введите имя артиста"_q);
	layout->addWidget(artistEdit);

	const auto coverBtn = new QPushButton(u"Выбрать обложку..."_q, this);
	layout->addWidget(coverBtn);

	connect(coverBtn, &QPushButton::clicked, this, [=] {
		const auto path = QFileDialog::getOpenFileName(
			this,
			u"Выбор обложки"_q,
			QString(),
			u"Изображения (*.jpg *.jpeg *.png)"_q);
		if (!path.isEmpty()) {
			auto imgFile = QFile(path);
			if (imgFile.open(QIODevice::ReadOnly)) {
				_cover = imgFile.readAll();
				coverBtn->setText(u"Обложка выбрана!"_q);
			}
		}
	});

	const auto buttonsLayout = new QHBoxLayout();
	buttonsLayout->addStretch();

	const auto cancelBtn = new QPushButton(u"Отмена"_q, this);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
	buttonsLayout->addWidget(cancelBtn);

	const auto saveBtn = new QPushButton(u"Сохранить"_q, this);
	saveBtn->setDefault(true);
	connect(saveBtn, &QPushButton::clicked, this, [=] {
		_title = titleEdit->text();
		_artist = artistEdit->text();
		apply();
	});
	buttonsLayout->addWidget(saveBtn);

	layout->addLayout(buttonsLayout);
}

void AudioTagsBox::apply() {
	TagInfo info{
		.title = _title,
		.artist = _artist,
		.cover = _cover,
	};
	const auto tempDst = _filePath + u".tagged.tmp"_q;
	if (WriteTags(_filePath, tempDst, info)) {
		QFile::remove(_filePath);
		QFile::rename(tempDst, _filePath);
		accept();
	} else {
		QFile::remove(tempDst);
		QMessageBox::warning(
			this,
			u"Ошибка"_q,
			u"Не удалось записать теги в аудиофайл."_q);
	}
}

} // namespace Margy::Tags
