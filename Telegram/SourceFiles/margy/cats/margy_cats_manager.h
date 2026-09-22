#pragma once

#include <QString>
#include <vector>
#include <memory>

#include <rpl/event_stream.h>
#include <rpl/producer.h>

namespace Margy::Cats {

struct Cat {
	QString photo;
	QString name;
	QString nameRu;
	QString from;
};

class CatsManager final {
public:
	[[nodiscard]] static CatsManager &Instance();

	void refresh();
	[[nodiscard]] const std::vector<Cat> &cats() const;
	[[nodiscard]] Cat randomCat() const;
	[[nodiscard]] QString localPhotoPath(const Cat &cat) const;
	[[nodiscard]] rpl::producer<QString> photoDownloaded() const {
		return _photoDownloaded.events();
	}

private:
	CatsManager();
	~CatsManager() = default;

	void loadCached();
	void parseJson(const QByteArray &data);
	void fetchRemote();

	std::vector<Cat> _cats;
	rpl::event_stream<QString> _photoDownloaded;
};

} // namespace Margy::Cats
