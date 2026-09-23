#pragma once

#include <QString>
#include <cstdint>

namespace Margy::Plugins::Hooks {

[[nodiscard]] QString OnSend(const QString &text, int64_t chatId = 0, bool *outCancelled = nullptr);
void OnMessage(const QString &text, int64_t dialogId, int32_t messageId, bool out);
void OnChatOpened(int64_t dialogId);

void OnInputTextChanged(
	const QString &fieldId,
	const QString &text,
	const QString &oldText,
	int cursor,
	float charW,
	float lineH,
	int padL,
	int padT);

void OnInputStep(const QString &fieldId);

} // namespace Margy::Plugins::Hooks
