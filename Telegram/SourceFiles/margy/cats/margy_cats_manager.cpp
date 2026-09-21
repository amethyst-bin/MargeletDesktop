#include "margy/cats/margy_cats_manager.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRandomGenerator>

namespace Margy::Cats {
namespace {

constexpr auto kCatsUrl = "https://raw.githubusercontent.com/narezany/margelet/main/cats.json";
constexpr auto kRawBaseUrl = "https://raw.githubusercontent.com/narezany/margelet/main/";

QString CacheDirPath() {
	const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/margy_cache/cats";
	QDir().mkpath(dir);
	return dir;
}

QString CatsJsonCachePath() {
	return CacheDirPath() + "/cats.json";
}

const char kDefaultCatsJson[] = R"([
  {"photo":"res/margelet_cat_1.jpg","name":"walnut","name_ru":"грецкий орех","from":"@narezany"},
  {"photo":"res/margelet_cat_2.jpg","name":"ыжвдвдыбабв😡😡😡","from":"@egorkagds"},
  {"photo":"res/margelet_cat_3.jpg","name":"КОГДА.","from":"@egorkagds"},
  {"photo":"res/margelet_cat_4.jpg","name":"Он просто чёрный кот","from":"@relikexx"},
  {"photo":"res/margelet_cat_5.jpg","name":"Чорт","from":"@cynesaa"},
  {"photo":"res/margelet_cat_6.jpg","name":"Жирныч веня 🤓","from":"@cynesaa"},
  {"photo":"res/margelet_cat_7.jpg","name":"короче прикольни челик ток немного жирни ну это хорошо оке","from":"@kanilad66"},
  {"photo":"res/margelet_cat_8.jpg","name":"Серое котость","from":"@Гравка"},
  {"photo":"res/margelet_cat_9.jpg","name":"Клёпа :3","from":"@ynhur"}
])";

} // namespace

CatsManager &CatsManager::Instance() {
	static CatsManager instance;
	return instance;
}

CatsManager::CatsManager() {
	loadCached();
	fetchRemote();
}

void CatsManager::loadCached() {
	auto file = QFile(CatsJsonCachePath());
	if (file.open(QIODevice::ReadOnly)) {
		parseJson(file.readAll());
	}
	if (_cats.empty()) {
		parseJson(QByteArray(kDefaultCatsJson));
	}
}

void CatsManager::parseJson(const QByteArray &data) {
	const auto doc = QJsonDocument::fromJson(data);
	if (!doc.isArray()) {
		return;
	}
	std::vector<Cat> list;
	for (const auto &val : doc.array()) {
		if (!val.isObject()) {
			continue;
		}
		const auto obj = val.toObject();
		const auto photo = obj.value("photo").toString();
		if (photo.isEmpty()) {
			continue;
		}
		list.push_back(Cat{
			.photo = photo,
			.name = obj.value("name").toString(),
			.nameRu = obj.value("name_ru").toString(),
			.from = obj.value("from").toString(),
		});
	}
	if (!list.empty()) {
		_cats = std::move(list);
	}
}

void CatsManager::fetchRemote() {
	auto nam = new QNetworkAccessManager();
	const auto reply = nam->get(QNetworkRequest(QUrl(kCatsUrl)));
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		reply->deleteLater();
		nam->deleteLater();
		if (reply->error() == QNetworkReply::NoError) {
			const auto data = reply->readAll();
			parseJson(data);
			auto cache = QFile(CatsJsonCachePath());
			if (cache.open(QIODevice::WriteOnly)) {
				cache.write(data);
			}
		}
	});
}

void CatsManager::refresh() {
	fetchRemote();
}

const std::vector<Cat> &CatsManager::cats() const {
	return _cats;
}

Cat CatsManager::randomCat() const {
	if (_cats.empty()) {
		return Cat{
			.photo = u"res/margelet_cat_9.jpg"_q,
			.name = u"Клёпа :3"_q,
			.nameRu = u"Клёпа :3"_q,
			.from = u"@narezany"_q,
		};
	}
	const auto idx = QRandomGenerator::global()->bounded(int(_cats.size()));
	return _cats[idx];
}

QString CatsManager::localPhotoPath(const Cat &cat) const {
	const auto filename = QFileInfo(cat.photo).fileName();
	const auto localPath = CacheDirPath() + '/' + filename;
	if (QFile::exists(localPath)) {
		return localPath;
	}

	// Trigger async download if missing
	auto nam = new QNetworkAccessManager();
	const auto url = QUrl(kRawBaseUrl + cat.photo);
	const auto reply = nam->get(QNetworkRequest(url));
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		reply->deleteLater();
		nam->deleteLater();
		if (reply->error() == QNetworkReply::NoError) {
			auto file = QFile(localPath);
			if (file.open(QIODevice::WriteOnly)) {
				file.write(reply->readAll());
			}
		}
	});

	return QString();
}

} // namespace Margy::Cats
