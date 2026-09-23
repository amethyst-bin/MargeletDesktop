#pragma once

#include "margy/plugins/margy_plugin_types.h"

#include <rpl/producer.h>
#include <rpl/event_stream.h>
#include <QString>
#include <memory>
#include <set>
#include <map>

class QProcess;

namespace Margy::Plugins {

class Host final {
public:
	[[nodiscard]] static Host &Instance();

	void start();
	void stop();
	void restart();
	[[nodiscard]] bool isRunning() const;
	[[nodiscard]] bool isPluginRunning(const QString &id) const;

	void launchPlugin(const PluginManifest &plugin);
	void stopPlugin(const QString &id);
	void chatOpened(int64_t chatId);
	[[nodiscard]] QString onSend(const QString &text, bool *outCancelled = nullptr);
	void onMessage(const QString &text, int64_t dialogId, int32_t messageId, bool out);
	void onSettingChanged(const QString &pluginId, const QString &key, const QString &value);

	void onInputChange(
		const QString &fieldId,
		const QString &text,
		const QString &oldText,
		int cursor,
		float charW,
		float lineH,
		int padL,
		int padT);

	void onInputStep(const QString &fieldId);

	[[nodiscard]] rpl::producer<TypingAnimFrame> typingAnimation(const QString &fieldId);
	[[nodiscard]] bool unpackArchive(const QString &archivePath, const QString &targetDir);

private:
	Host();
	~Host();

	void initProcess();
	[[nodiscard]] QString findPython() const;
	void processLine(const QString &line);
	void sendCommand(const QString &jsonLine);

	std::unique_ptr<QProcess> _process;
	bool _running = false;
	std::set<QString> _runningPlugins;
	rpl::event_stream<TypingAnimFrame> _typingStream;
	std::map<QString, TypingAnimFrame> _lastFrames;
	QString _lastFieldId;
};

} // namespace Margy::Plugins
