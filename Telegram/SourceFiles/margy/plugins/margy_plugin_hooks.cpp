#include "margy/plugins/margy_plugin_hooks.h"
#include "margy/plugins/margy_plugin_host.h"
#include "margy/margy_config.h"

namespace Margy::Plugins::Hooks {

QString OnSend(const QString &text, int64_t chatId, bool *outCancelled) {
	if (!Config::Instance().pluginsEnabled()) {
		if (outCancelled) {
			*outCancelled = false;
		}
		return text;
	}
	return Host::Instance().onSend(text, chatId, outCancelled);
}

void OnMessage(const QString &text, int64_t dialogId, int32_t messageId, bool out) {
	if (!Config::Instance().pluginsEnabled()) {
		return;
	}
	Host::Instance().onMessage(text, dialogId, messageId, out);
}

void OnChatOpened(int64_t dialogId) {
	if (!Config::Instance().pluginsEnabled()) {
		return;
	}
	Host::Instance().chatOpened(dialogId);
}

void OnInputTextChanged(
		const QString &fieldId,
		const QString &text,
		const QString &oldText,
		int cursor,
		float charW,
		float lineH,
		int padL,
		int padT) {
	if (!Config::Instance().pluginsEnabled() || !Config::Instance().pluginHooksEnabled()) {
		return;
	}
	Host::Instance().onInputChange(fieldId, text, oldText, cursor, charW, lineH, padL, padT);
}

void OnInputStep(const QString &fieldId) {
	if (!Config::Instance().pluginsEnabled() || !Config::Instance().pluginHooksEnabled()) {
		return;
	}
	Host::Instance().onInputStep(fieldId);
}

} // namespace Margy::Plugins::Hooks
