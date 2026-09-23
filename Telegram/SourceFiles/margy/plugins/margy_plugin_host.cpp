#include "margy/plugins/margy_plugin_host.h"
#include "margy/plugins/margy_plugin_manager.h"
#include "margy/margy_config.h"

#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QStandardPaths>

namespace Margy::Plugins {

Host &Host::Instance() {
	static Host instance;
	return instance;
}

Host::Host() = default;

Host::~Host() {
	stop();
}

QString Host::findPython() const {
	const auto candidates = std::vector<QString>{
		u"C:/Users/nulis/AppData/Local/Python/pythoncore-3.14-64/python.exe"_q,
		u"C:/Program Files/Python314/python.exe"_q,
		u"C:/Program Files/Python313/python.exe"_q,
		u"C:/Program Files/Python312/python.exe"_q,
		u"python.exe"_q,
		u"python3.exe"_q,
		u"py.exe"_q,
		u"/usr/bin/python3"_q,
		u"/usr/local/bin/python3"_q,
		u"python3"_q,
		u"python"_q,
	};
	for (const auto &path : candidates) {
		if (path.contains('/') && QFileInfo::exists(path)) {
			return path;
		}
	}
	// Fall back to PATH search
	return u"python"_q;
}

void Host::start() {
	if (_running || !Config::Instance().pluginsEnabled()) {
		return;
	}
	initProcess();
}

void Host::stop() {
	if (_process) {
		Manager::Instance().log(u"margelet"_q, u"Остановка процесса плагинов"_q);
		_process->disconnect();
		_process->terminate();
		if (!_process->waitForFinished(1000)) {
			_process->kill();
		}
		_process = nullptr;
	}
	_running = false;
	_runningPlugins.clear();
}

void Host::restart() {
	stop();
	start();
}

bool Host::isRunning() const {
	return _running;
}

bool Host::isPluginRunning(const QString &id) const {
	return _runningPlugins.find(id) != _runningPlugins.end();
}

void Host::initProcess() {
	const auto pythonPath = findPython();
	Manager::Instance().log(u"margelet"_q, u"Поиск Python: выбран путь "_q + pythonPath);
	const auto hostScript = Manager::Instance().pluginsPath() + u"/margy_host.py"_q;
	if (!QFileInfo::exists(hostScript)) {
		Manager::Instance().log(u"margelet"_q, u"Скрипт margy_host.py не найден"_q, true);
		return;
	}

	_process = std::make_unique<QProcess>();
	_process->setProgram(pythonPath);
	_process->setArguments(QStringList{ u"-u"_q, hostScript });

	QObject::connect(_process.get(), &QProcess::readyReadStandardOutput, [=] {
		while (_process && _process->canReadLine()) {
			const auto line = QString::fromUtf8(_process->readLine()).trimmed();
			if (!line.isEmpty()) {
				processLine(line);
			}
		}
	});

	QObject::connect(_process.get(), &QProcess::readyReadStandardError, [=] {
		if (_process) {
			const auto err = QString::fromUtf8(_process->readAllStandardError()).trimmed();
			if (!err.isEmpty()) {
				Manager::Instance().log(u"python-err"_q, err, true);
			}
		}
	});

	QObject::connect(_process.get(), &QProcess::finished, [=](int exitCode) {
		_running = false;
		_runningPlugins.clear();
		Manager::Instance().log(
			u"margelet"_q,
			u"Процесс плагинов завершился с кодом "_q + QString::number(exitCode),
			exitCode != 0);
	});

	_process->start();
	if (!_process->waitForStarted(3000)) {
		Manager::Instance().log(u"margelet"_q, u"Не удалось запустить Python: "_q + pythonPath + u" ("_q + _process->errorString() + u")"_q, true);
		_process = nullptr;
		return;
	}

	_running = true;
	Manager::Instance().log(u"margelet"_q, u"Процесс Python запущен (PID: "_q + QString::number(_process->processId()) + u")"_q);

	// Prepare init message with enabled plugins
	auto pluginsArray = QJsonArray();
	for (const auto &p : Manager::Instance().installedPlugins()) {
		if (Manager::Instance().isEnabled(p.id)) {
			auto obj = QJsonObject();
			obj[u"id"_q] = p.id;
			obj[u"name"_q] = p.name;
			obj[u"folder"_q] = p.folder;
			pluginsArray.append(obj);
			_runningPlugins.insert(p.id);
		}
	}

	auto initObj = QJsonObject();
	initObj[u"cmd"_q] = u"init"_q;
	initObj[u"hooks_enabled"_q] = Config::Instance().pluginHooksEnabled();
	initObj[u"plugins"_q] = pluginsArray;

	sendCommand(QString::fromUtf8(QJsonDocument(initObj).toJson(QJsonDocument::Compact)));
}

void Host::sendCommand(const QString &jsonLine) {
	if (_process && _running && _process->state() == QProcess::Running) {
		const auto data = jsonLine.toUtf8() + '\n';
		_process->write(data);
	}
}

void Host::processLine(const QString &line) {
	const auto doc = QJsonDocument::fromJson(line.toUtf8());
	if (!doc.isObject()) {
		return;
	}
	const auto obj = doc.object();
	const auto op = obj[u"op"_q].toString();

	if (op == u"ready"_q) {
		Manager::Instance().log(
			u"margelet"_q,
			u"Движок плагинов готов ("_q + obj[u"python_version"_q].toString() + u")"_q);
	} else if (op == u"log"_q) {
		Manager::Instance().log(
			obj[u"plugin"_q].toString(),
			obj[u"text"_q].toString(),
			obj[u"error"_q].toBool());
	} else if (op == u"declare_settings"_q) {
		const auto pluginId = obj[u"plugin"_q].toString();
		const auto settingsArray = obj[u"settings"_q].toArray();
		auto rows = std::vector<SettingRow>();
		for (const auto &val : settingsArray) {
			const auto rowObj = val.toObject();
			const auto kind = rowObj[u"kind"_q].toString();
			auto row = SettingRow();
			if (kind == u"header"_q) {
				row.type = SettingType::Header;
			} else if (kind == u"note"_q) {
				row.type = SettingType::Note;
			} else if (kind == u"switch"_q) {
				row.type = SettingType::Switch;
			} else if (kind == u"text"_q) {
				row.type = SettingType::Text;
			} else if (kind == u"choice"_q) {
				row.type = SettingType::Choice;
			} else if (kind == u"action"_q) {
				row.type = SettingType::Action;
			}
			row.key = rowObj[u"key"_q].toString();
			row.title = rowObj[u"title"_q].toString();
			if (rowObj[u"default"_q].isBool()) {
				row.defaultValue = rowObj[u"default"_q].toBool() ? u"1"_q : u"0"_q;
			} else {
				row.defaultValue = rowObj[u"default"_q].toString();
			}
			row.danger = rowObj[u"danger"_q].toBool();
			for (const auto &opt : rowObj[u"options"_q].toArray()) {
				row.options.push_back(opt.toString());
			}
			rows.push_back(std::move(row));
		}
		Manager::Instance().declareSettings(pluginId, rows);
	} else if (op == u"set_pref"_q) {
		const auto pluginId = obj[u"plugin"_q].toString();
		const auto key = obj[u"key"_q].toString();
		const auto value = obj[u"value"_q].toVariant().toString();
		Config::Instance().setPluginPref(pluginId, key, value);
	} else if (op == u"anim_frame"_q) {
		const auto fieldId = obj[u"field_id"_q].toString();
		auto frame = TypingAnimFrame();
		frame.hasAnimation = obj[u"active"_q].toBool();
		const auto sparksArray = obj[u"sparks"_q].toArray();
		for (const auto &sparkVal : sparksArray) {
			const auto sparkObj = sparkVal.toObject();
			auto spark = SparkParticle();
			spark.x = float(sparkObj[u"x"_q].toDouble());
			spark.y = float(sparkObj[u"y"_q].toDouble());
			spark.radius = float(sparkObj[u"r"_q].toDouble());
			spark.color = uint32_t(sparkObj[u"color"_q].toVariant().toULongLong());
			spark.alpha = float(sparkObj[u"alpha"_q].toDouble());
			frame.sparks.push_back(spark);
		}
		const auto cursorObj = obj[u"cursor"_q].toObject();
		frame.cursor.visible = cursorObj[u"visible"_q].toBool();
		frame.cursor.active = cursorObj[u"active"_q].toBool();

		_lastFrames[fieldId] = frame;
		_lastFieldId = fieldId;
		_typingStream.fire(std::move(frame));
	}
}

void Host::launchPlugin(const PluginManifest &plugin) {
	Manager::Instance().log(u"margelet"_q, u"Запуск плагина: "_q + plugin.name + u" ("_q + plugin.id + u")"_q);
	if (!_running) {
		start();
		return;
	}
	auto obj = QJsonObject();
	obj[u"cmd"_q] = u"run"_q;
	obj[u"id"_q] = plugin.id;
	obj[u"name"_q] = plugin.name;
	obj[u"folder"_q] = plugin.folder;
	sendCommand(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
	_runningPlugins.insert(plugin.id);
}

void Host::chatOpened(int64_t chatId) {
	if (!_running) {
		start();
	}
	if (!_running) {
		return;
	}
	auto obj = QJsonObject();
	obj[u"cmd"_q] = u"chat_opened"_q;
	obj[u"chat_id"_q] = double(chatId);
	sendCommand(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

QString Host::onSend(const QString &text, bool *outCancelled) {
	if (outCancelled) {
		*outCancelled = false;
	}
	// Synchronous dispatch is optional; currently pass-through
	return text;
}

void Host::onMessage(const QString &text, int64_t dialogId, int32_t messageId, bool out) {
	if (!_running) {
		start();
	}
	if (!_running) {
		return;
	}
	auto obj = QJsonObject();
	obj[u"cmd"_q] = u"message"_q;
	obj[u"text"_q] = text;
	obj[u"dialog_id"_q] = double(dialogId);
	obj[u"msg_id"_q] = messageId;
	obj[u"out"_q] = out;
	sendCommand(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

void Host::onSettingChanged(const QString &pluginId, const QString &key, const QString &value) {
	if (!_running) {
		return;
	}
	auto obj = QJsonObject();
	obj[u"cmd"_q] = u"setting"_q;
	obj[u"plugin"_q] = pluginId;
	obj[u"key"_q] = key;
	obj[u"value"_q] = value;
	sendCommand(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

void Host::onInputChange(
		const QString &fieldId,
		const QString &text,
		const QString &oldText,
		int cursor,
		float charW,
		float lineH,
		int padL,
		int padT) {
	if (!_running) {
		start();
	}
	if (!_running) {
		return;
	}
	auto obj = QJsonObject();
	obj[u"cmd"_q] = u"input_change"_q;
	obj[u"field_id"_q] = fieldId;
	obj[u"text"_q] = text;
	obj[u"old_text"_q] = oldText;
	obj[u"cursor"_q] = cursor;
	obj[u"char_w"_q] = double(charW);
	obj[u"line_h"_q] = double(lineH);
	obj[u"pad_l"_q] = padL;
	obj[u"pad_t"_q] = padT;
	sendCommand(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

void Host::onInputStep(const QString &fieldId) {
	if (!_running) {
		return;
	}
	auto obj = QJsonObject();
	obj[u"cmd"_q] = u"input_step"_q;
	obj[u"field_id"_q] = fieldId;
	sendCommand(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

rpl::producer<TypingAnimFrame> Host::typingAnimation(const QString &fieldId) {
	return _typingStream.events();
}

bool Host::unpackArchive(const QString &archivePath, const QString &targetDir) {
	QDir().mkpath(targetDir);

	auto tarProc = QProcess();
	tarProc.start(u"tar.exe"_q, { u"-xf"_q, archivePath, u"-C"_q, targetDir });
	if (tarProc.waitForFinished(10000) && tarProc.exitCode() == 0) {
		return true;
	}

	const auto pythonPath = findPython();
	if (!pythonPath.isEmpty()) {
		auto pyProc = QProcess();
		pyProc.start(pythonPath, { u"-m"_q, u"zipfile"_q, u"-e"_q, archivePath, targetDir });
		if (pyProc.waitForFinished(10000) && pyProc.exitCode() == 0) {
			return true;
		}
	}

	auto psProc = QProcess();
	const auto psCmd = QString(u"Expand-Archive -LiteralPath '%1' -DestinationPath '%2' -Force"_q)
		.arg(archivePath, targetDir);
	psProc.start(u"powershell.exe"_q, { u"-NoProfile"_q, u"-Command"_q, psCmd });
	return psProc.waitForFinished(15000) && (psProc.exitCode() == 0);
}

} // namespace Margy::Plugins
