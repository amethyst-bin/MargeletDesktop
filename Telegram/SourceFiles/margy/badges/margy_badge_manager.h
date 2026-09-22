#pragma once

#include "margy/badges/margy_badge_types.h"

#include <QObject>
#include <vector>
#include <optional>
#include <memory>

class QNetworkAccessManager;

namespace Margy::Badges {

class Manager final : public QObject {
	Q_OBJECT

public:
	[[nodiscard]] static Manager &Instance();

	[[nodiscard]] bool enabled() const;
	void setEnabled(bool enabled);

	[[nodiscard]] std::optional<Badge> of(int64_t peerId) const;
	[[nodiscard]] std::optional<Badge> of(const QString &username) const;
	[[nodiscard]] std::optional<Badge> of(int64_t peerId, const QString &username) const;
	[[nodiscard]] std::vector<Badge> all(int64_t peerId) const;
	[[nodiscard]] bool has(int64_t peerId) const;
	[[nodiscard]] std::vector<Badge> list() const;

	void refresh();

Q_SIGNALS:
	void badgesUpdated();

private:
	Manager();
	~Manager() override;

	void loadCache();
	void saveCache(const QByteArray &bytes);
	bool parseJson(const QByteArray &bytes);

	bool _enabled = true;
	std::vector<Badge> _badges;
	std::unique_ptr<QNetworkAccessManager> _network;
};

[[nodiscard]] inline std::optional<Badge> Of(int64_t peerId) {
	return Manager::Instance().of(peerId);
}

[[nodiscard]] inline std::optional<Badge> Of(const QString &username) {
	return Manager::Instance().of(username);
}

[[nodiscard]] inline std::optional<Badge> Of(int64_t peerId, const QString &username) {
	return Manager::Instance().of(peerId, username);
}

[[nodiscard]] inline bool Has(int64_t peerId) {
	return Manager::Instance().has(peerId);
}

} // namespace Margy::Badges
