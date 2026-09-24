#include "margy/plugins/margy_plugin_manager.h"
#include "margy/plugins/margy_host_script.h"
#include "margy/plugins/margy_plugin_host.h"
#include "margy/margy_config.h"
#include "settings.h"

#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDateTime>

namespace Margy::Plugins {
namespace {

constexpr auto kMaxConsoleLines = 500;

void AppendToLogFile(const QString &path, const QString &entry) {
	auto file = QFile(path);
	if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
		file.write(entry.toUtf8());
		file.flush();
	}
}

QString SanitizeId(const QString &id) {
	auto res = QString();
	for (const auto ch : id) {
		if (ch.isLetterOrNumber() || ch == '.' || ch == '_' || ch == '-') {
			res.append(ch);
		} else {
			res.append('_');
		}
	}
	return res;
}

} // namespace

Manager &Manager::Instance() {
	static Manager instance;
	return instance;
}

Manager::Manager() {
	ensureHostScript();
	reloadInstalled();
	log(u"margelet"_q, u"Менеджер плагинов инициализирован. Путь: "_q + pluginsPath());
}

QString Manager::pluginsPath() const {
	const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + u"/margelet_plugins"_q;
	QDir().mkpath(dir);
	return dir;
}

QString Manager::filesPath(const QString &pluginId) const {
	const auto safe = SanitizeId(pluginId);
	const auto dir = pluginsPath() + '/' + safe;
	QDir().mkpath(dir);
	return dir;
}

void Manager::ensureHostScript() {
	const auto scriptPath = pluginsPath() + u"/margy_host.py"_q;
	auto file = QFile(scriptPath);
	if (!file.exists() || file.size() != qint64(strlen(kMargyHostScript))) {
		if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			file.write(kMargyHostScript);
		}
	}
}

std::vector<PluginManifest> Manager::installedPlugins() const {
	return _installed;
}

std::optional<PluginManifest> Manager::plugin(const QString &id) const {
	for (const auto &p : _installed) {
		if (p.id == id) {
			return p;
		}
	}
	return std::nullopt;
}

void Manager::reloadInstalled() {
	_installed.clear();
	const auto root = QDir(pluginsPath());
	const auto entries = root.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const auto &info : entries) {
		if (info.fileName().startsWith(u"tmp_"_q)) {
			continue;
		}
		const auto folder = info.absoluteFilePath();
		const auto manifestPath = folder + u"/manifest.json"_q;
		const auto mainPath = folder + u"/main.py"_q;
		if (!QFileInfo::exists(manifestPath)) {
			continue;
		}
		if (!QFileInfo::exists(mainPath)) {
			auto stub = QFile(mainPath);
			if (stub.open(QIODevice::WriteOnly)) {
				stub.write("# MTP plugin stub\ndef on_start():\n    pass\n");
				stub.close();
			}
		}

		auto mFile = QFile(manifestPath);
		if (!mFile.open(QIODevice::ReadOnly)) {
			continue;
		}

		const auto manifestData = mFile.readAll();
		mFile.close();
		const auto doc = QJsonDocument::fromJson(manifestData);
		if (!doc.isObject()) {
			continue;
		}
		const auto obj = doc.object();

		auto manifest = PluginManifest();
		manifest.id = obj[u"id"_q].toString();
		if (manifest.id.isEmpty()) {
			manifest.id = info.fileName();
		}
		manifest.name = obj[u"name"_q].toString();
		manifest.nameEn = obj[u"name_en"_q].toString();
		manifest.version = obj[u"version"_q].toString();
		manifest.author = obj[u"author"_q].toString();
		manifest.description = obj[u"description"_q].toString();
		manifest.descriptionEn = obj[u"description_en"_q].toString();
		manifest.minVersion = obj[u"min_version"_q].toString();
		manifest.folder = folder;

		const auto perms = obj[u"permissions"_q].toArray();
		for (const auto &p : perms) {
			manifest.permissions.push_back(p.toString());
		}

		manifest.usesHooks = std::find(
			manifest.permissions.begin(),
			manifest.permissions.end(),
			u"hooks"_q) != manifest.permissions.end();

		if (!manifest.usesHooks) {
			auto mainFile = QFile(mainPath);
			if (mainFile.open(QIODevice::ReadOnly)) {
				const auto src = QString::fromUtf8(mainFile.readAll());
				if (src.contains(u"margelet.hook("_q)) {
					manifest.usesHooks = true;
				}
			}
		}

		const auto iconPath = folder + u"/icon.png"_q;
		if (QFileInfo::exists(iconPath)) {
			manifest.icon.load(iconPath);
		}

		_installed.push_back(std::move(manifest));
	}
	_pluginsUpdated.fire({});
}

namespace {

bool CopyDirectory(const QString &from, const QString &to) {
	QDir dir(from);
	if (!dir.exists()) {
		return false;
	}
	QDir().mkpath(to);
	for (const auto &entry : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		const auto subFrom = entry.absoluteFilePath();
		const auto subTo = to + '/' + entry.fileName();
		if (!CopyDirectory(subFrom, subTo)) {
			return false;
		}
	}
	for (const auto &entry : dir.entryInfoList(QDir::Files)) {
		const auto fileFrom = entry.absoluteFilePath();
		const auto fileTo = to + '/' + entry.fileName();
		QFile::remove(fileTo);
		if (!QFile::copy(fileFrom, fileTo)) {
			return false;
		}
	}
	return true;
}

} // namespace

bool Manager::installPlugin(const QString &marpPath, QString *outError) {
	if (!QFileInfo::exists(marpPath)) {
		if (outError) {
			*outError = u"Файл не найден"_q;
		}
		return false;
	}

	const auto stagingDir = pluginsPath() + u"/tmp_staging"_q;
	QDir(stagingDir).removeRecursively();
	QDir().mkpath(stagingDir);

	if (!Host::Instance().unpackArchive(marpPath, stagingDir)) {
		if (outError) {
			*outError = u"Не удалось распаковать архив"_q;
		}
		QDir(stagingDir).removeRecursively();
		return false;
	}

	auto effectiveDir = stagingDir;
	auto manifestPath = effectiveDir + u"/manifest.json"_q;
	if (!QFile::exists(manifestPath)) {
		const auto subdirs = QDir(stagingDir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (const auto &sd : subdirs) {
			const auto subManifest = sd.absoluteFilePath() + u"/manifest.json"_q;
			if (QFile::exists(subManifest)) {
				effectiveDir = sd.absoluteFilePath();
				manifestPath = subManifest;
				break;
			}
		}
	}

	auto mFile = QFile(manifestPath);
	if (!mFile.open(QIODevice::ReadOnly)) {
		if (outError) {
			*outError = u"Архив не содержит manifest.json"_q;
		}
		QDir(stagingDir).removeRecursively();
		return false;
	}

	const auto manifestData = mFile.readAll();
	mFile.close();

	const auto doc = QJsonDocument::fromJson(manifestData);
	if (!doc.isObject()) {
		if (outError) {
			*outError = u"Некорректный manifest.json"_q;
		}
		QDir(stagingDir).removeRecursively();
		return false;
	}

	const auto obj = doc.object();
	const auto id = obj[u"id"_q].toString();
	if (id.isEmpty()) {
		if (outError) {
			*outError = u"В manifest.json не указан id"_q;
		}
		QDir(stagingDir).removeRecursively();
		return false;
	}

	const auto targetDir = filesPath(id);
	QDir(targetDir).removeRecursively();
	QDir().mkpath(targetDir);
	if (!CopyDirectory(effectiveDir, targetDir)) {
		if (outError) {
			*outError = u"Не удалось скопировать файлы плагина"_q;
		}
		QDir(stagingDir).removeRecursively();
		return false;
	}
	const auto targetMain = targetDir + u"/main.py"_q;
	if (!QFile::exists(targetMain)) {
		auto stub = QFile(targetMain);
		if (stub.open(QIODevice::WriteOnly)) {
			stub.write("# MTP plugin stub\ndef on_start():\n    pass\n");
			stub.close();
		}
	}
	QDir(stagingDir).removeRecursively();

	setEnabled(id, true);
	reloadInstalled();

	const auto p = plugin(id);
	if (p) {
		Host::Instance().launchPlugin(*p);
	}
	return true;
}

bool Manager::uninstallPlugin(const QString &id) {
	Host::Instance().stopPlugin(id);
	Config::Instance().setPluginEnabled(id, false);
	const auto target = filesPath(id);
	if (QFileInfo::exists(target)) {
		QDir(target).removeRecursively();
		reloadInstalled();
		_pluginsUpdated.fire({});
		return true;
	}
	reloadInstalled();
	_pluginsUpdated.fire({});
	return false;
}

bool Manager::isEnabled(const QString &id) const {
	return Config::Instance().isPluginEnabled(id);
}

void Manager::setEnabled(const QString &id, bool enabled) {
	Config::Instance().setPluginEnabled(id, enabled);
	log(u"margelet"_q, (enabled ? u"Включение плагина: "_q : u"Отключение плагина: "_q) + id);
	if (enabled) {
		const auto p = plugin(id);
		if (p) {
			Host::Instance().launchPlugin(*p);
		}
	} else {
		Host::Instance().stopPlugin(id);
	}
	_pluginsUpdated.fire({});
}

void Manager::log(const QString &plugin, const QString &text, bool isError) {
	auto line = ConsoleLine();
	line.timestamp = QDateTime::currentMSecsSinceEpoch();
	line.plugin = plugin;
	line.text = text;
	line.isError = isError;

	_console.push_back(line);
	while (_console.size() > kMaxConsoleLines) {
		_console.erase(_console.begin());
	}
	_consoleStream.fire(std::move(line));

	const auto timeStr = QDateTime::fromMSecsSinceEpoch(line.timestamp).toString(u"yyyy-MM-dd hh:mm:ss.zzz"_q);
	const auto entry = u"[%1] [%2] [%3] %4\n"_q
		.arg(timeStr)
		.arg(plugin)
		.arg(isError ? u"ERROR"_q : u"INFO"_q)
		.arg(text);

	AppendToLogFile(cWorkingDir() + u"plugins_log.txt"_q, entry);
	AppendToLogFile(pluginsPath() + u"/plugins.log"_q, entry);
}

const std::vector<ConsoleLine> &Manager::console() const {
	return _console;
}

void Manager::clearConsole() {
	_console.clear();
}

rpl::producer<ConsoleLine> Manager::consoleUpdates() {
	return _consoleStream.events();
}

rpl::producer<> Manager::pluginsUpdated() {
	return _pluginsUpdated.events();
}

void Manager::declareSettings(const QString &id, const std::vector<SettingRow> &rows) {
	_declaredSettings[id] = rows;
}

std::vector<SettingRow> Manager::settings(const QString &id) const {
	const auto it = _declaredSettings.find(id);
	if (it != _declaredSettings.end()) {
		return it->second;
	}
	return {};
}

bool Manager::hasSettings(const QString &id) const {
	const auto it = _declaredSettings.find(id);
	return (it != _declaredSettings.end()) && !it->second.empty();
}

} // namespace Margy::Plugins
