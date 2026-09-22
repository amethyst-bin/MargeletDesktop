#include "margy/wall/margy_wall_box.h"
#include "margy/margy_config.h"
#include "boxes/abstract_box.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/ui_utility.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"
#include "styles/style_settings.h"

#include <QSettings>
#include <QDateTime>

namespace Margy::Wall {
namespace {

QString WallSettingsKey(int64_t peerId) {
	return QString(u"wall_%1"_q).arg(peerId);
}

} // namespace

WallBox::WallBox(QWidget *parent, int64_t peerId)
: _peerId(peerId) {
}

void WallBox::Show(QWidget *parent, int64_t peerId) {
	::Ui::show(::Box<WallBox>(peerId));
}

void WallBox::prepare() {
	setTitle(rpl::single(u"Стена Margy 📝"_q));
	setDimensions(st::boxWideWidth, 480);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	_input = content->add(
		object_ptr<::Ui::InputField>(
			content,
			st::defaultInputField,
			rpl::single(u"Напишите что-нибудь тёплое..."_q),
			QString()),
		st::boxRowPadding);

	const auto postBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"Опубликовать запись 🚀"_q),
			st::settingsButton),
		st::boxRowPadding);
	postBtn->setClickedCallback([=] {
		const auto text = _input->getLastText().trimmed();
		if (!text.isEmpty()) {
			addPost(text);
			_input->setText(QString());
		}
	});

	::Ui::AddSkip(content);
	::Ui::AddDivider(content);

	_postsContainer = content->add(
		object_ptr<::Ui::VerticalLayout>(content));

	loadPosts();

	addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		closeBox();
	});
}

void WallBox::loadPosts() {
	if (!_postsContainer) {
		return;
	}
	auto s = QSettings(Config::Instance().settingsFilePath(), QSettings::IniFormat);
	s.beginGroup(WallSettingsKey(_peerId));
	const auto count = s.value(u"count"_q, 0).toInt();

	if (count == 0) {
		_postsContainer->add(
			object_ptr<::Ui::FlatLabel>(
				_postsContainer,
				u"🐾 Margy\nДобро пожаловать на Стену! Здесь можно делиться записями и памятными событиями."_q,
				st::boxLabel),
			st::boxRowPadding);
	} else {
		for (int i = count - 1; i >= 0; --i) {
			const auto text = s.value(QString(u"post_%1_text"_q).arg(i)).toString();
			const auto time = s.value(QString(u"post_%1_time"_q).arg(i)).toString();
			_postsContainer->add(
				object_ptr<::Ui::FlatLabel>(
					_postsContainer,
					time + u"\n"_q + text,
					st::boxLabel),
				st::boxRowPadding);
			::Ui::AddDivider(_postsContainer);
		}
	}
}

void WallBox::addPost(const QString &text) {
	auto s = QSettings(Config::Instance().settingsFilePath(), QSettings::IniFormat);
	s.beginGroup(WallSettingsKey(_peerId));
	const auto count = s.value(u"count"_q, 0).toInt();
	s.setValue(QString(u"post_%1_text"_q).arg(count), text);
	s.setValue(QString(u"post_%1_time"_q).arg(count), QDateTime::currentDateTime().toString(u"dd.MM.yyyy hh:mm"_q));
	s.setValue(u"count"_q, count + 1);
	s.endGroup();
	s.sync();

	loadPosts();
}

} // namespace Margy::Wall
