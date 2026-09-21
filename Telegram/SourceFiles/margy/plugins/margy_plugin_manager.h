#pragma once

#include "margy/plugins/margy_plugin_types.h"

#include <rpl/producer.h>
#include <rpl/event_stream.h>
#include <QString>
#include <vector>
#include <optional>
#include <map>

namespace Margy::Plugins {

class Manager final {
public:
	[[nodiscard]] static Manager &Instance();

	[[nodiscard]] QString pluginsPath() const;
	[[nodiscard]] QString filesPath(const QString &pluginId) const;

	[[nodiscard]] std::vector<PluginManifest> installedPlugins() const;
	[[nodiscard]] std::optional<PluginManifest> plugin(const QString &id) const;

	bool installPlugin(const QString &marpPath, QString *outError = nullptr);
	bool uninstallPlugin(const QString &id);

	[[nodiscard]] bool isEnabled(const QString &id) const;
	void setEnabled(const QString &id, bool enabled);

	void log(const QString &plugin, const QString &text, bool isError = false);
	[[nodiscard]] const std::vector<ConsoleLine> &console() const;
	void clearConsole();
	[[nodiscard]] rpl::producer<ConsoleLine> consoleUpdates();
	[[nodiscard]] rpl::producer<> pluginsUpdated();

	void declareSettings(const QString &id, const std::vector<SettingRow> &rows);
	[[nodiscard]] std::vector<SettingRow> settings(const QString &id) const;
	[[nodiscard]] bool hasSettings(const QString &id) const;

private:
	Manager();
	~Manager() = default;

	void reloadInstalled();
	void ensureHostScript();

	std::vector<PluginManifest> _installed;
	std::vector<ConsoleLine> _console;
	rpl::event_stream<ConsoleLine> _consoleStream;
	rpl::event_stream<> _pluginsUpdated;
	std::map<QString, std::vector<SettingRow>> _declaredSettings;
};

} // namespace Margy::Plugins
