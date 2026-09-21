#include "margy/wall/margy_wall_box.h"
#include "margy/margy_config.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QDateTime>
#include <QSettings>

namespace Margy::Wall {
namespace {

QString WallSettingsKey(int64_t peerId) {
	return QString(u"wall_%1"_q).arg(peerId);
}

} // namespace

WallBox::WallBox(QWidget *parent, int64_t peerId)
: QDialog(parent)
, _peerId(peerId) {
	setupUi();
	loadPosts();
}

void WallBox::Show(QWidget *parent, int64_t peerId) {
	auto box = new WallBox(parent, peerId);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->open();
}

void WallBox::setupUi() {
	setWindowTitle(u"Стена Margy 📝"_q);
	setFixedSize(440, 520);

	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(16, 16, 16, 16);
	layout->setSpacing(10);

	const auto header = new QLabel(
		_peerId == 0
			? u"Стена профиля (Дуров, верни стену!)"_q
			: QString(u"Стена пользователя #%1"_q).arg(_peerId),
		this);
	header->setStyleSheet(u"font-size: 16px; font-weight: bold; color: #8dd1b0;"_q);
	layout->addWidget(header);

	_postsList = new QListWidget(this);
	_postsList->setStyleSheet(
		u"QListWidget { background-color: #1a1a1d; border-radius: 8px; padding: 6px; color: #ffffff; }"
		u"QListWidget::item { background-color: #242428; border-radius: 6px; margin: 4px; padding: 8px; }"_q);
	layout->addWidget(_postsList);

	const auto inputLabel = new QLabel(u"Оставить запись на стене:"_q, this);
	layout->addWidget(inputLabel);

	_input = new QTextEdit(this);
	_input->setFixedHeight(70);
	_input->setPlaceholderText(u"Напишите что-нибудь тёплое..."_q);
	_input->setStyleSheet(
		u"background-color: #242428; border: 1px solid #38383f; border-radius: 8px; padding: 6px; color: #ffffff;"_q);
	layout->addWidget(_input);

	const auto buttonsLayout = new QHBoxLayout();
	buttonsLayout->addStretch();

	const auto closeBtn = new QPushButton(u"Закрыть"_q, this);
	connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
	buttonsLayout->addWidget(closeBtn);

	const auto sendBtn = new QPushButton(u"Опубликовать 🚀"_q, this);
	sendBtn->setDefault(true);
	sendBtn->setStyleSheet(u"font-weight: bold; color: #8dd1b0;"_q);
	connect(sendBtn, &QPushButton::clicked, this, [=] {
		const auto text = _input->toPlainText().trimmed();
		if (!text.isEmpty()) {
			addPost(text);
			_input->clear();
		}
	});
	buttonsLayout->addWidget(sendBtn);

	layout->addLayout(buttonsLayout);
}

void WallBox::loadPosts() {
	_postsList->clear();
	QSettings s(Config::Instance().settingsFilePath(), QSettings::IniFormat);
	s.beginGroup(WallSettingsKey(_peerId));
	const auto count = s.value(u"count"_q, 0).toInt();

	if (count == 0) {
		const auto item = new QListWidgetItem(_postsList);
		item->setText(
			u"🐾 Margy Bot\n"
			u"Добро пожаловать на Стену! Здесь можно делиться записями и памятными событиями."_q);
		_postsList->addItem(item);
	} else {
		for (auto i = 0; i < count; ++i) {
			const auto text = s.value(QString(u"post_%1"_q).arg(i)).toString();
			const auto date = s.value(QString(u"date_%1"_q).arg(i)).toString();
			const auto item = new QListWidgetItem(_postsList);
			item->setText(QString(u"📅 %1\n%2"_q).arg(date, text));
			_postsList->addItem(item);
		}
	}
	s.endGroup();
	_postsList->scrollToBottom();
}

void WallBox::addPost(const QString &text) {
	QSettings s(Config::Instance().settingsFilePath(), QSettings::IniFormat);
	s.beginGroup(WallSettingsKey(_peerId));
	auto count = s.value(u"count"_q, 0).toInt();
	const auto nowStr = QDateTime::currentDateTime().toString(u"dd.MM.yyyy hh:mm"_q);

	s.setValue(QString(u"post_%1"_q).arg(count), text);
	s.setValue(QString(u"date_%1"_q).arg(count), nowStr);
	s.setValue(u"count"_q, count + 1);
	s.endGroup();

	const auto item = new QListWidgetItem(_postsList);
	item->setText(QString(u"📅 %1 (Вы)\n%2"_q).arg(nowStr, text));
	_postsList->addItem(item);
	_postsList->scrollToBottom();
}

} // namespace Margy::Wall
