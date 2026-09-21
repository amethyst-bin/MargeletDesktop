#include "margy/badges/margy_badge_manager.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>

namespace Margy::Badges {
namespace {

constexpr auto kRemoteBadgesUrl = "https://raw.githubusercontent.com/narezany/margelet/main/badges.json";
constexpr auto kCacheFileName = "margy_badges.json";

QString CacheFilePath() {
	const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir().mkpath(dir);
	return dir + '/' + kCacheFileName;
}

} // namespace

Manager &Manager::Instance() {
	static Manager instance;
	return instance;
}

Manager::Manager()
: _network(std::make_unique<QNetworkAccessManager>(this)) {
	loadCache();
	refresh();
}

Manager::~Manager() = default;

bool Manager::enabled() const {
	return _enabled;
}

void Manager::setEnabled(bool enabled) {
	if (_enabled != enabled) {
		_enabled = enabled;
		Q_EMIT badgesUpdated();
	}
}

std::optional<Badge> Manager::of(int64_t peerId) const {
	if (!_enabled) {
		return std::nullopt;
	}
	for (const auto &badge : _badges) {
		if (badge.peerId == peerId) {
			return badge;
		}
	}
	return std::nullopt;
}

std::vector<Badge> Manager::all(int64_t peerId) const {
	std::vector<Badge> result;
	if (!_enabled) {
		return result;
	}
	for (const auto &badge : _badges) {
		if (badge.peerId == peerId) {
			result.push_back(badge);
		}
	}
	return result;
}

bool Manager::has(int64_t peerId) const {
	return of(peerId).has_value();
}

std::vector<Badge> Manager::list() const {
	std::vector<Badge> result;
	for (const auto &badge : _badges) {
		bool exists = false;
		for (const auto &added : result) {
			if (added.titleEn == badge.titleEn && added.titleRu == badge.titleRu) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			result.push_back(badge);
		}
	}
	return result;
}

void Manager::loadCache() {
	auto file = QFile(CacheFilePath());
	if (file.open(QIODevice::ReadOnly)) {
		const auto bytes = file.readAll();
		if (parseJson(bytes)) {
			return;
		}
	}
	_badges = BuiltInBadges();
}

void Manager::saveCache(const QByteArray &bytes) {
	auto file = QFile(CacheFilePath());
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		file.write(bytes);
	}
}

bool Manager::parseJson(const QByteArray &bytes) {
	QJsonParseError error{};
	const auto doc = QJsonDocument::fromJson(bytes, &error);
	if (error.error != QJsonParseError::NoError || !doc.isArray()) {
		return false;
	}
	const auto array = doc.array();
	if (array.isEmpty()) {
		return false;
	}

	std::vector<Badge> parsed;
	parsed.reserve(array.size());

	for (const auto &val : array) {
		if (!val.isObject()) {
			continue;
		}
		const auto obj = val.toObject();
		const auto peer = obj.value("peer").toVariant().toLongLong();
		if (peer == 0) {
			continue;
		}
		Badge badge;
		badge.peerId = peer;
		badge.titleEn = obj.value("title").toString();
		badge.titleRu = obj.value("title_ru").toString();
		badge.aboutEn = obj.value("about").toString();
		badge.aboutRu = obj.value("about_ru").toString();
		badge.color = ParseColor(obj.value("color").toString());
		badge.url = obj.value("url").toString();
		parsed.push_back(std::move(badge));
	}

	if (parsed.empty()) {
		return false;
	}

	_badges = std::move(parsed);
	return true;
}

void Manager::refresh() {
	auto request = QNetworkRequest(QUrl(kRemoteBadgesUrl));
	request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

	const auto reply = _network->get(request);
	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		reply->deleteLater();
		if (reply->error() != QNetworkReply::NoError) {
			return;
		}
		const auto data = reply->readAll();
		if (parseJson(data)) {
			saveCache(data);
			Q_EMIT badgesUpdated();
		}
	});
}

} // namespace Margy::Badges
