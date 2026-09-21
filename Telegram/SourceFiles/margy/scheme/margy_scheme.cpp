#include "margy/scheme/margy_scheme.h"
#include "margy/settings/margy_settings_section.h"
#include "margy/badges/margy_badge_gallery_box.h"
#include "margy/badges/margy_badge_box.h"
#include "margy/cats/margy_cats_box.h"
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

	const auto command = url.mid(u"margy://"_q.size()).trimmed();
	const auto parsedUrl = QUrl(url);
	const auto host = parsedUrl.host().toLower();
	const auto path = parsedUrl.path().toLower();
	const auto target = host.isEmpty() ? path : host;

	const auto my = context.value<ClickHandlerContext>();
	const auto controller = my.sessionWindow.get()
		? my.sessionWindow.get()
		: Core::App().lastActivePrimaryWindow()
		? Core::App().lastActivePrimaryWindow()->sessionController()
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
