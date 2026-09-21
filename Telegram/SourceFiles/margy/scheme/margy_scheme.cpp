#include "margy/scheme/margy_scheme.h"
#include "margy/settings/margy_settings_section.h"
#include "margy/badges/margy_badge_gallery_box.h"
#include "margy/badges/margy_badge_box.h"
#include "margy/cats/margy_cats_box.h"
#include "margy/fonts/margy_fonts.h"
#include "margy/gradient/margy_gradient.h"
#include "margy/wall/margy_wall_box.h"
#include "margy/donate/margy_donate_box.h"
#include "margy/proxy/margy_proxy.h"
#include "core/application.h"
#include "core/click_handler_types.h"
#include "window/window_session_controller.h"
#include "window/window_controller.h"

#include <QUrl>
#include <QUrlQuery>
#include <QDesktopServices>

namespace Margy::Scheme {

bool HandleUrl(const QString &url, const QVariant &context) {
	if (!url.startsWith(u"margy://"_q, Qt::CaseInsensitive)) {
		return false;
	}

	const auto parsedUrl = QUrl(url);
	const auto host = parsedUrl.host().toLower();
	const auto path = parsedUrl.path().toLower();
	const auto target = host.isEmpty() ? path : host;

	const auto my = context.value<ClickHandlerContext>();
	const auto controller = my.sessionWindow.get()
		? my.sessionWindow.get()
		: Core::App().activePrimaryWindow()
		? Core::App().activePrimaryWindow()->sessionController()
		: Core::App().activeWindow()
		? Core::App().activeWindow()->sessionController()
		: nullptr;
	const auto window = controller ? controller->widget() : nullptr;

	if (target.isEmpty() || target == u"settings"_q || target == u"margy"_q) {
		if (controller) {
			controller->showSettings(Settings::MargySettingsId());
			return true;
		}
	} else if (target == u"badges"_q) {
		Badges::BadgeGalleryBox::Show(window);
		return true;
	} else if (target == u"badge"_q) {
		const auto query = QUrlQuery(parsedUrl.query());
		const auto peerStr = query.queryItemValue(u"peer"_q);
		if (!peerStr.isEmpty()) {
			const auto peerId = peerStr.toLongLong();
			Badges::BadgeBox::Show(window, peerId);
			return true;
		}
	} else if (target == u"cats"_q) {
		Cats::CatsBox::Show(window);
		return true;
	} else if (target == u"fonts"_q) {
		Fonts::FontsBox::Show(window);
		return true;
	} else if (target == u"gradient"_q) {
		Gradient::GradientBox::Show(window);
		return true;
	} else if (target == u"wall"_q) {
		const auto query = QUrlQuery(parsedUrl.query());
		const auto peerStr = query.queryItemValue(u"peer"_q);
		const auto peerId = peerStr.isEmpty() ? 0 : peerStr.toLongLong();
		Wall::WallBox::Show(window, peerId);
		return true;
	} else if (target == u"donate"_q) {
		Donate::DonateBox::Show(window);
		return true;
	} else if (target == u"proxy"_q) {
		Proxy::ConnectCommunityProxy();
		return true;
	} else if (target == u"channel"_q) {
		QDesktopServices::openUrl(QUrl(u"https://t.me/margeletter"_q));
		return true;
	} else if (target == u"forum"_q) {
		QDesktopServices::openUrl(QUrl(u"https://t.me/margeletforum"_q));
		return true;
	} else if (target == u"source"_q) {
		QDesktopServices::openUrl(QUrl(u"https://github.com/amethyst-bin/MargeletDesktop"_q));
		return true;
	}

	return false;
}

} // namespace Margy::Scheme
