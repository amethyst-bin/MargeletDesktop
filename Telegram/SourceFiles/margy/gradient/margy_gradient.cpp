#include "margy/gradient/margy_gradient.h"
#include "margy/margy_config.h"
#include "boxes/abstract_box.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/ui_utility.h"
#include "ui/painter.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"
#include "styles/style_settings.h"

#include <QPainter>
#include <QLinearGradient>
#include <QRegularExpression>
#include <QColorDialog>

namespace Margy::Gradient {
namespace {

const auto kPattern = QRegularExpression(
	u"(?:#margy_gradient\\s+)?([0-9A-Fa-f]{6})-([0-9A-Fa-f]{6})\\b"_q);

} // namespace

Colors Parse(const QString &text) {
	const auto match = kPattern.match(text);
	if (!match.hasMatch()) {
		return Colors{};
	}
	const auto c1 = '#' + match.captured(1);
	const auto c2 = '#' + match.captured(2);
	return Colors{
		.first = QColor(c1),
		.second = QColor(c2),
		.valid = true,
	};
}

QString Format(const QColor &first, const QColor &second) {
	return QString(u"#margy_gradient %1-%2"_q)
		.arg(first.name().mid(1).toUpper())
		.arg(second.name().mid(1).toUpper());
}

Colors ForPeer(int64_t) {
	if (Config::Instance().profileGradientEnabled()) {
		const auto parsed = Parse(Config::Instance().profileGradient());
		if (parsed.valid) {
			return parsed;
		}
	}
	return Colors{};
}

void Paint(QPainter &p, const QRect &rect, const Colors &colors) {
	if (!colors.valid) {
		return;
	}
	auto grad = QLinearGradient(rect.topLeft(), rect.bottomRight());
	grad.setColorAt(0.0, colors.first);
	grad.setColorAt(1.0, colors.second);
	p.fillRect(rect, grad);
}

GradientBox::GradientBox(QWidget *parent) {
	_first = QColor(0x8D, 0xD1, 0xB0);
	_second = QColor(0x6B, 0xA4, 0xFF);
	const auto cur = Parse(Config::Instance().profileGradient());
	if (cur.valid) {
		_first = cur.first;
		_second = cur.second;
	}
	_enabled = Config::Instance().profileGradientEnabled();
}

void GradientBox::Show(QWidget *parent) {
	::Ui::show(::Box<GradientBox>());
}

void GradientBox::prepare() {
	setTitle(rpl::single(u"Градиент профиля Margy"_q));
	setDimensions(st::boxWideWidth, 490);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	// Preview widget first for visual feedback
	const auto preview = content->add(
		object_ptr<::Ui::FixedHeightWidget>(content, 110),
		st::boxRowPadding);

	const auto enableCheck = content->add(
		object_ptr<::Ui::Checkbox>(
			content,
			u"Включить кастомный градиент профиля"_q,
			_enabled,
			st::defaultCheckbox,
			st::defaultToggle),
		st::boxRowPadding);
	enableCheck->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		_enabled = checked;
		preview->update();
	}, content->lifetime());

	preview->paintRequest(
	) | rpl::on_next([=](const QRect &clip) {
		Painter p(preview);
		p.setRenderHint(QPainter::Antialiasing);
		const auto r = preview->rect();
		QPainterPath path;
		path.addRoundedRect(r, 10, 10);
		p.setClipPath(path);
		if (_enabled) {
			auto grad = QLinearGradient(0, 0, preview->width(), preview->height());
			grad.setColorAt(0.0, _first);
			grad.setColorAt(1.0, _second);
			p.fillPath(path, grad);
		} else {
			p.fillPath(path, QColor(0x24, 0x24, 0x28));
		}
		p.setClipping(false);
		p.setPen(QColor(255, 255, 255, 50));
		p.drawRoundedRect(r.adjusted(0, 0, -1, -1), 10, 10);

		p.setFont(st::boxTitle.style.font);
		p.setPen(Qt::white);
		p.drawText(r.adjusted(14, 12, -14, -12), Qt::AlignLeft | Qt::AlignTop, _enabled ? u"Превью градиента"_q : u"Градиент выключен"_q);

		p.setFont(st::boxDividerLabel.style.font);
		p.setPen(QColor(255, 255, 255, 200));
		const auto hexText = _enabled ? QString(u"%1  →  %2"_q).arg(_first.name().toUpper()).arg(_second.name().toUpper()) : QString();
		p.drawText(r.adjusted(14, 12, -14, -12), Qt::AlignLeft | Qt::AlignBottom, hexText);
	}, preview->lifetime());

	const auto c1Btn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"Выбрать начальный цвет"_q),
			st::settingsButton),
		st::boxRowPadding);
	c1Btn->setClickedCallback([=] {
		const auto c = QColorDialog::getColor(_first, nullptr, u"Начальный цвет"_q);
		if (c.isValid()) {
			_first = c;
			preview->update();
		}
	});

	const auto c2Btn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"Выбрать конечный цвет"_q),
			st::settingsButton),
		st::boxRowPadding);
	c2Btn->setClickedCallback([=] {
		const auto c = QColorDialog::getColor(_second, nullptr, u"Конечный цвет"_q);
		if (c.isValid()) {
			_second = c;
			preview->update();
		}
	});

	const auto applyPreset = [=](QColor c1, QColor c2) {
		_first = c1;
		_second = c2;
		_enabled = true;
		enableCheck->setChecked(true);
		preview->update();
	};

	const auto margyPreset = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"Пресет: Margy (Бирюзовый - Синий)"_q),
			st::settingsButton),
		st::boxRowPadding);
	margyPreset->setClickedCallback([=] {
		applyPreset(QColor(0x8D, 0xD1, 0xB0), QColor(0x6B, 0xA4, 0xFF));
	});

	const auto sunsetPreset = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"Пресет: Sunset (Оранжевый - Розовый)"_q),
			st::settingsButton),
		st::boxRowPadding);
	sunsetPreset->setClickedCallback([=] {
		applyPreset(QColor(0xFF, 0x51, 0x2F), QColor(0xDD, 0x24, 0x76));
	});

	addButton(rpl::single(u"Сохранить"_q), [=] {
		Config::Instance().setProfileGradientEnabled(_enabled);
		Config::Instance().setProfileGradient(Format(_first, _second));
		closeBox();
	});

	addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		closeBox();
	});
}

} // namespace Margy::Gradient
