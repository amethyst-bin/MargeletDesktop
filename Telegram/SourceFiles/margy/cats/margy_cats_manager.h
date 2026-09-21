#pragma once

#include <QString>
#include <vector>
#include <memory>

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

private:
	CatsManager();
	~CatsManager() = default;

	void loadCached();
	void parseJson(const QByteArray &data);
	void fetchRemote();

	std::vector<Cat> _cats;
};

} // namespace Margy::Cats
