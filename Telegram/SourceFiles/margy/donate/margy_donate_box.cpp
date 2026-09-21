#include "margy/donate/margy_donate_box.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGuiApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>

namespace Margy::Donate {

DonateBox::DonateBox(QWidget *parent)
: QDialog(parent) {
	setupUi();
}

void DonateBox::Show(QWidget *parent) {
	auto box = new DonateBox(parent);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->open();
}

void DonateBox::setupUi() {
	setWindowTitle(u"Поддержка Margy 💖"_q);
	setFixedSize(420, 480);

	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(20, 20, 20, 20);
	layout->setSpacing(14);

	const auto title = new QLabel(u"Поддержать разработку Margy ✨"_q, this);
	title->setAlignment(Qt::AlignCenter);
	title->setStyleSheet(u"font-size: 18px; font-weight: bold; color: #8dd1b0;"_q);
	layout->addWidget(title);

	const auto desc = new QLabel(
		u"Margy — бесплатный и открытый клиент, созданный с душой. "
		u"Ваша поддержка мотивирует развивать проект и добавлять новые фичи!"_q,
		this);
	desc->setWordWrap(true);
	desc->setAlignment(Qt::AlignCenter);
	desc->setStyleSheet(u"font-size: 13px; color: #b0b0b8; line-height: 1.4;"_q);
	layout->addWidget(desc);

	// ЮMoney Card
	const auto yoomoneyCard = new QWidget(this);
	yoomoneyCard->setStyleSheet(
		u"background-color: #242428; border-radius: 10px; padding: 10px;"_q);
	const auto yooLayout = new QVBoxLayout(yoomoneyCard);
	yooLayout->setContentsMargins(12, 10, 12, 10);
	yooLayout->setSpacing(6);

	const auto yooTitle = new QLabel(u"💳 ЮMoney (кошелёк):"_q, yoomoneyCard);
	yooTitle->setStyleSheet(u"font-weight: bold; color: #ffffff;"_q);
	yooLayout->addWidget(yooTitle);

	const auto yooRow = new QHBoxLayout();
	const auto yooNumber = new QLabel(u"4100118596660144"_q, yoomoneyCard);
	yooNumber->setStyleSheet(u"font-size: 14px; font-family: monospace; color: #8dd1b0;"_q);
	yooRow->addWidget(yooNumber);
	yooRow->addStretch();

	const auto copyBtn = new QPushButton(u"Копировать"_q, yoomoneyCard);
	copyBtn->setStyleSheet(
		u"background-color: #383842; color: #ffffff; border-radius: 6px; padding: 4px 10px;"_q);
	connect(copyBtn, &QPushButton::clicked, this, [=] {
		QGuiApplication::clipboard()->setText(u"4100118596660144"_q);
		copyBtn->setText(u"Скопировано! ✓"_q);
	});
	yooRow->addWidget(copyBtn);

	const auto openYooBtn = new QPushButton(u"Открыть форму"_q, yoomoneyCard);
	openYooBtn->setStyleSheet(
		u"background-color: #383842; color: #ffffff; border-radius: 6px; padding: 4px 10px;"_q);
	connect(openYooBtn, &QPushButton::clicked, this, [] {
		QDesktopServices::openUrl(QUrl(u"https://yoomoney.ru/to/4100118196133693"_q));
	});
	yooRow->addWidget(openYooBtn);

	yooLayout->addLayout(yooRow);
	layout->addWidget(yoomoneyCard);

	// Roblox Card
	const auto robloxCard = new QWidget(this);
	robloxCard->setStyleSheet(
		u"background-color: #242428; border-radius: 10px; padding: 10px;"_q);
	const auto robloxLayout = new QVBoxLayout(robloxCard);
	robloxLayout->setContentsMargins(12, 10, 12, 10);
	robloxLayout->setSpacing(6);

	const auto robloxTitle = new QLabel(u"🎮 Roblox (Robux / Gamepass):"_q, robloxCard);
	robloxTitle->setStyleSheet(u"font-weight: bold; color: #ffffff;"_q);
	robloxLayout->addWidget(robloxTitle);

	const auto robloxRow = new QHBoxLayout();
	const auto robloxName = new QLabel(u"@narezany"_q, robloxCard);
	robloxName->setStyleSheet(u"font-size: 14px; color: #b7a8e0;"_q);
	robloxRow->addWidget(robloxName);
	robloxRow->addStretch();

	const auto robloxBtn = new QPushButton(u"Открыть Roblox"_q, robloxCard);
	robloxBtn->setStyleSheet(
		u"background-color: #383842; color: #ffffff; border-radius: 6px; padding: 4px 10px;"_q);
	connect(robloxBtn, &QPushButton::clicked, this, [] {
		QDesktopServices::openUrl(QUrl(u"https://www.roblox.com/users/3576778434/profile"_q));
	});
	robloxRow->addWidget(robloxBtn);
	robloxLayout->addLayout(robloxRow);
	layout->addWidget(robloxCard);

	// Telegram Stars Card
	const auto tgCard = new QWidget(this);
	tgCard->setStyleSheet(
		u"background-color: #242428; border-radius: 10px; padding: 10px;"_q);
	const auto tgLayout = new QVBoxLayout(tgCard);
	tgLayout->setContentsMargins(12, 10, 12, 10);
	tgLayout->setSpacing(6);

	const auto tgTitle = new QLabel(u"⭐ Telegram Stars & Связь с автором:"_q, tgCard);
	tgTitle->setStyleSheet(u"font-weight: bold; color: #ffffff;"_q);
	tgLayout->addWidget(tgTitle);

	const auto tgRow = new QHBoxLayout();
	const auto tgAuthor = new QLabel(u"@narezany"_q, tgCard);
	tgAuthor->setStyleSheet(u"font-size: 14px; color: #ffd700;"_q);
	tgRow->addWidget(tgAuthor);
	tgRow->addStretch();

	const auto tgBtn = new QPushButton(u"Написать автору"_q, tgCard);
	tgBtn->setStyleSheet(
		u"background-color: #383842; color: #ffffff; border-radius: 6px; padding: 4px 10px;"_q);
	connect(tgBtn, &QPushButton::clicked, this, [] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/narezany"_q));
	});
	tgRow->addWidget(tgBtn);
	tgLayout->addLayout(tgRow);
	layout->addWidget(tgCard);

	layout->addStretch();

	const auto bottomLayout = new QHBoxLayout();
	bottomLayout->addStretch();

	const auto closeBtn = new QPushButton(u"Закрыть"_q, this);
	connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
	bottomLayout->addWidget(closeBtn);

	layout->addLayout(bottomLayout);
}

} // namespace Margy::Donate
