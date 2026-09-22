#include "margy/markup/margy_markup.h"
#include "data/stickers/data_custom_emoji.h"

#include <vector>

namespace Margy::Markup {
namespace {

constexpr char16_t kOpen = 0x2060;
constexpr char16_t kClose = 0x2061;
constexpr char16_t kTrit = 0x2062;
constexpr int kTrits = 3;

constexpr int kKindTrits = 2;
constexpr int kValueTrits = 3;
constexpr int kMarkLen = 1 + kKindTrits + kValueTrits;

constexpr int kKindSize = 0;
constexpr int kKindDim = 1;
constexpr int kKindRainbow = 2;
constexpr int kKindButton = 3;
constexpr int kKindEmoji = 4;
constexpr int kKindOutline = 5;

constexpr int kLenTrits = 5;
constexpr int kByteTrits = 6;

const auto kHeader = u"<! Message looks better with @margeletter! >"_q;

inline bool IsTrit(QChar c) {
	return c.unicode() >= kTrit && c.unicode() < (kTrit + kTrits);
}

inline bool AllTrits(const QString &text, int from, int count) {
	if (from < 0 || from + count > text.size()) {
		return false;
	}
	for (int i = 0; i < count; ++i) {
		if (!IsTrit(text[from + i])) {
			return false;
		}
	}
	return true;
}

inline int DecodeNumber(const QString &text, int at, int count) {
	int value = 0;
	int mul = 1;
	for (int i = 0; i < count; ++i) {
		value += (text[at + i].unicode() - kTrit) * mul;
		mul *= kTrits;
	}
	return value;
}

inline bool HasPayload(int kind) {
	return kind == kKindButton || kind == kKindEmoji;
}

struct Run {
	int kind = 0;
	int value = 0;
	int start = 0;
	int end = 0;
	QByteArray payload;

	[[nodiscard]] QString text() const {
		return QString::fromUtf8(payload);
	}
};

std::vector<Run> Parse(const QString &text) {
	std::vector<Run> runs;
	if (text.size() < 4) {
		return runs;
	}

	struct OpenTag {
		int kind = 0;
		int value = 0;
		int start = 0;
		QByteArray payload;
	};

	std::vector<OpenTag> stack;

	for (int i = 0; i < text.size(); ++i) {
		const auto c = text[i];
		if (c.unicode() == kOpen && i + kMarkLen <= text.size() && AllTrits(text, i + 1, kKindTrits + kValueTrits)) {
			const auto kind = DecodeNumber(text, i + 1, kKindTrits);
			const auto value = DecodeNumber(text, i + 1 + kKindTrits, kValueTrits);
			int after = i + kMarkLen;
			QByteArray payload;
			if (HasPayload(kind)) {
				if (after + kLenTrits > text.size() || !AllTrits(text, after, kLenTrits)) {
					continue;
				}
				const auto len = DecodeNumber(text, after, kLenTrits);
				after += kLenTrits;
				if (after + len * kByteTrits > text.size() || !AllTrits(text, after, len * kByteTrits)) {
					continue;
				}
				payload.resize(len);
				for (int b = 0; b < len; ++b) {
					payload[b] = static_cast<char>(DecodeNumber(text, after + b * kByteTrits, kByteTrits));
				}
				after += len * kByteTrits;
			}
			stack.push_back(OpenTag{
				.kind = kind,
				.value = value,
				.start = after,
				.payload = std::move(payload),
			});
			i = after - 1;
		} else if (c.unicode() == kClose) {
			if (!stack.empty()) {
				auto top = std::move(stack.back());
				stack.pop_back();
				runs.push_back(Run{
					.kind = top.kind,
					.value = top.value,
					.start = top.start,
					.end = i,
					.payload = std::move(top.payload),
				});
			}
		}
	}
	return runs;
}

} // namespace

bool Has(const QString &text) {
	return text.contains(QChar(kOpen)) || text.contains(kHeader);
}

void Process(TextWithEntities &textWithEntities) {
	if (!Has(textWithEntities.text)) {
		return;
	}

	const auto headerIndex = textWithEntities.text.indexOf(kHeader);
	if (headerIndex >= 0) {
		int removeStart = headerIndex;
		if (removeStart > 0 && textWithEntities.text[removeStart - 1] == '\n') {
			--removeStart;
		}
		int removeEnd = headerIndex + kHeader.size();
		if (removeEnd < textWithEntities.text.size() && textWithEntities.text[removeEnd] == '\n') {
			++removeEnd;
		}
		const auto removedLen = removeEnd - removeStart;
		textWithEntities.text.remove(removeStart, removedLen);

		for (auto it = textWithEntities.entities.begin(); it != textWithEntities.entities.end();) {
			if (it->offset() >= removeEnd) {
				*it = EntityInText(it->type(), it->offset() - removedLen, it->length(), it->data());
				++it;
			} else if (it->offset() + it->length() <= removeStart) {
				++it;
			} else {
				it = textWithEntities.entities.erase(it);
			}
		}
	}

	const auto runs = Parse(textWithEntities.text);
	for (const auto &run : runs) {
		if (run.start >= run.end || run.start < 0 || run.end > textWithEntities.text.size()) {
			continue;
		}
		const auto length = run.end - run.start;
		if (run.kind == kKindButton) {
			const auto url = run.text();
			if (!url.isEmpty()) {
				textWithEntities.entities.push_back(
					EntityInText(EntityType::CustomUrl, run.start, length, url));
			}
		} else if (run.kind == kKindEmoji) {
			bool ok = false;
			const auto docId = run.text().trimmed().toULongLong(&ok);
			if (ok && docId != 0) {
				textWithEntities.entities.push_back(
					EntityInText(EntityType::CustomEmoji, run.start, length, Data::SerializeCustomEmojiId(docId)));
			}
		} else if (run.kind == kKindOutline) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Bold, run.start, length));
		} else if (run.kind == kKindDim) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Italic, run.start, length));
		}
	}
}

} // namespace Margy::Markup
