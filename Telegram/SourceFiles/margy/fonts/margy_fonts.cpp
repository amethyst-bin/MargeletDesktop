#include "margy/fonts/margy_fonts.h"
#include "margy/margy_config.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QFont>

namespace Margy::Fonts {

QStringList AvailableEmojiFontPacks() {
	return {
		u"Default"_q,
		u"Twemoji"_q,
		u"JoyPixels"_q,
		u"Apple Color Emoji"_q,
		u"Segoe UI Emoji"_q,
		u"Noto Color Emoji"_q,
	};
}

void ApplyConfiguredFonts() {
	const auto customFamily = Config::Instance().customFont();
	if (!customFamily.isEmpty()) {
		auto font = QGuiApplication::font();
		font.setFamily(customFamily);
		QGuiApplication::setFont(font);
	}

	const auto emojiPack = Config::Instance().emojiFont();
	if (emojiPack != u"Default"_q && !emojiPack.isEmpty()) {
		QFont::insertSubstitution(u"Segoe UI Emoji"_q, emojiPack);
		QFont::insertSubstitution(u"Apple Color Emoji"_q, emojiPack);
		QFont::insertSubstitution(u"Noto Color Emoji"_q, emojiPack);
	}
}

FontsBox::FontsBox(QWidget *parent)
: QDialog(parent) {
	setupUi();
}

void FontsBox::Show(QWidget *parent) {
	auto box = new FontsBox(parent);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->open();
}

void FontsBox::setupUi() {
	setWindowTitle(u"Настройки шрифтов и эмодзи"_q);
	setFixedSize(400, 360);

	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(16, 16, 16, 16);
	layout->setSpacing(12);

	const auto titleLabel = new QLabel(u"Шрифт интерфейса:"_q, this);
	layout->addWidget(titleLabel);

	const auto fontCombo = new QComboBox(this);
	fontCombo->addItem(u"Системный (по умолчанию)"_q, QString());
	const auto families = QFontDatabase::families();
	for (const auto &family : families) {
		fontCombo->addItem(family, family);
	}
	const auto currentCustom = Config::Instance().customFont();
	if (!currentCustom.isEmpty()) {
		const auto idx = fontCombo->findData(currentCustom);
		if (idx >= 0) {
			fontCombo->setCurrentIndex(idx);
		}
	}
	layout->addWidget(fontCombo);

	const auto emojiLabel = new QLabel(u"Шрифт и набор эмодзи (Twemoji):"_q, this);
	layout->addWidget(emojiLabel);

	const auto emojiCombo = new QComboBox(this);
	const auto emojiPacks = AvailableEmojiFontPacks();
	for (const auto &pack : emojiPacks) {
		emojiCombo->addItem(pack, pack);
	}
	const auto currentEmoji = Config::Instance().emojiFont();
	const auto emojiIdx = emojiCombo->findData(currentEmoji);
	if (emojiIdx >= 0) {
		emojiCombo->setCurrentIndex(emojiIdx);
	}
	layout->addWidget(emojiCombo);

	const auto previewBoxLabel = new QLabel(u"Предпросмотр:"_q, this);
	layout->addWidget(previewBoxLabel);

	const auto preview = new QLabel(
		u"Margy Desktop 🐱✨🚀\nСъешь ещё этих мягких булок 🐾\n0123456789"_q,
		this);
	preview->setAlignment(Qt::AlignCenter);
	preview->setStyleSheet(
		u"background-color: #1e1e22; border-radius: 8px; padding: 12px; color: white;"_q);
	layout->addWidget(preview);

	const auto updatePreview = [=] {
		const auto selFont = fontCombo->currentData().toString();
		auto font = preview->font();
		if (!selFont.isEmpty()) {
			font.setFamily(selFont);
		}
		preview->setFont(font);
	};
	connect(fontCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, updatePreview);

	const auto buttonsLayout = new QHBoxLayout();
	buttonsLayout->addStretch();

	const auto cancelBtn = new QPushButton(u"Отмена"_q, this);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
	buttonsLayout->addWidget(cancelBtn);

	const auto applyBtn = new QPushButton(u"Применить"_q, this);
	applyBtn->setDefault(true);
	connect(applyBtn, &QPushButton::clicked, this, [=] {
		Config::Instance().setCustomFont(fontCombo->currentData().toString());
		Config::Instance().setEmojiFont(emojiCombo->currentData().toString());
		ApplyConfiguredFonts();
		accept();
	});
	buttonsLayout->addWidget(applyBtn);

	layout->addLayout(buttonsLayout);
}

} // namespace Margy::Fonts
