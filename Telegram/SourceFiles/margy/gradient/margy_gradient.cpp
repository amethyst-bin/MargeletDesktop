#include "margy/gradient/margy_gradient.h"
#include "margy/margy_config.h"

#include <QPainter>
#include <QLinearGradient>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QCheckBox>

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

Colors ForPeer(int64_t peerId) {
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
	QLinearGradient grad(rect.topLeft(), rect.bottomRight());
	grad.setColorAt(0.0, colors.first);
	grad.setColorAt(1.0, colors.second);
	p.fillRect(rect, grad);
}

GradientBox::GradientBox(QWidget *parent)
: QDialog(parent) {
	const auto cur = Parse(Config::Instance().profileGradient());
	if (cur.valid) {
		_first = cur.first;
		_second = cur.second;
	}
	setupUi();
}

void GradientBox::Show(QWidget *parent) {
	auto box = new GradientBox(parent);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->open();
}

void GradientBox::setupUi() {
	setWindowTitle(u"Градиент профиля Margy"_q);
	setFixedSize(380, 420);

	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(16, 16, 16, 16);
	layout->setSpacing(12);

	const auto enableCheck = new QCheckBox(
		u"Включить кастомный градиент профиля"_q,
		this);
	enableCheck->setChecked(Config::Instance().profileGradientEnabled());
	layout->addWidget(enableCheck);

	const auto previewLabel = new QLabel(u"Предпросмотр градиента:"_q, this);
	layout->addWidget(previewLabel);

	_preview = new QWidget(this);
	_preview->setFixedHeight(120);
	_preview->setStyleSheet(u"border-radius: 12px;"_q);
	layout->addWidget(_preview);

	const auto colorsLayout = new QHBoxLayout();
	const auto color1Btn = new QPushButton(u"Цвет 1"_q, this);
	connect(color1Btn, &QPushButton::clicked, this, [=] {
		const auto color = QColorDialog::getColor(_first, this, u"Выбор первого цвета"_q);
		if (color.isValid()) {
			_first = color;
			updatePreview();
		}
	});
	colorsLayout->addWidget(color1Btn);

	const auto color2Btn = new QPushButton(u"Цвет 2"_q, this);
	connect(color2Btn, &QPushButton::clicked, this, [=] {
		const auto color = QColorDialog::getColor(_second, this, u"Выбор второго цвета"_q);
		if (color.isValid()) {
			_second = color;
			updatePreview();
		}
	});
	colorsLayout->addWidget(color2Btn);
	layout->addLayout(colorsLayout);

	const auto presetsLabel = new QLabel(u"Готовые пресеты:"_q, this);
	layout->addWidget(presetsLabel);

	const auto presetsLayout = new QHBoxLayout();
	const auto preset1 = new QPushButton(u"Margy Mint"_q, this);
	connect(preset1, &QPushButton::clicked, this, [=] {
		_first = QColor(u"#8DD1B0"_q);
		_second = QColor(u"#B7A8E0"_q);
		updatePreview();
	});
	presetsLayout->addWidget(preset1);

	const auto preset2 = new QPushButton(u"Sunset"_q, this);
	connect(preset2, &QPushButton::clicked, this, [=] {
		_first = QColor(u"#FF7E5F"_q);
		_second = QColor(u"#FEB47B"_q);
		updatePreview();
	});
	presetsLayout->addWidget(preset2);

	const auto preset3 = new QPushButton(u"Ocean"_q, this);
	connect(preset3, &QPushButton::clicked, this, [=] {
		_first = QColor(u"#2E3192"_q);
		_second = QColor(u"#1BFFFF"_q);
		updatePreview();
	});
	presetsLayout->addWidget(preset3);
	layout->addLayout(presetsLayout);

	updatePreview();

	const auto buttonsLayout = new QHBoxLayout();
	buttonsLayout->addStretch();

	const auto cancelBtn = new QPushButton(u"Отмена"_q, this);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
	buttonsLayout->addWidget(cancelBtn);

	const auto saveBtn = new QPushButton(u"Сохранить"_q, this);
	saveBtn->setDefault(true);
	connect(saveBtn, &QPushButton::clicked, this, [=] {
		Config::Instance().setProfileGradientEnabled(enableCheck->isChecked());
		Config::Instance().setProfileGradient(
			_first.name().mid(1).toUpper() + '-' + _second.name().mid(1).toUpper());
		accept();
	});
	buttonsLayout->addWidget(saveBtn);

	layout->addLayout(buttonsLayout);
}

void GradientBox::updatePreview() {
	if (!_preview) {
		return;
	}
	const auto css = QString(
		u"background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2); border-radius: 12px;"_q)
		.arg(_first.name())
		.arg(_second.name());
	_preview->setStyleSheet(css);
}

} // namespace Margy::Gradient
