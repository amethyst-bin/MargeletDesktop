#include "margy/markup/margy_markup.h"
#include "data/stickers/data_custom_emoji.h"

#include <vector>
#include <algorithm>

namespace Margy::Markup {
namespace {

constexpr char16_t kOpen = 0x2060;
constexpr char16_t kClose = 0x2061;
constexpr char16_t kTrit = 0x2062;
constexpr int kTrits = 3;

constexpr int kKindTrits = 2;
constexpr int kValueTrits = 3;
constexpr int kMarkLen = 1 + kKindTrits + kValueTrits;

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

inline QString EncodeNumber(int value, int count) {
	QString result;
	result.reserve(count);
	for (int i = 0; i < count; ++i) {
		const auto trit = value % kTrits;
		value /= kTrits;
		result.push_back(QChar(kTrit + trit));
	}
	return result;
}

inline bool HasPayload(int kind) {
	return kind == kKindButton || kind == kKindEmoji;
}

inline int ClampIndex(int val, int maxVal) {
	return (val < 0) ? 0 : (val > maxVal) ? maxVal : val;
}

struct Run {
	int kind = 0;
	int value = 0;
	int start = 0;
	int end = 0;
	int openTagStart = 0;
	int openTagEnd = 0;
	int closeTagPos = 0;
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
		int openTagStart = 0;
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
				.openTagStart = i,
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
					.openTagStart = top.openTagStart,
					.openTagEnd = top.start,
					.closeTagPos = i,
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

QString Open(int kind, int value, const QByteArray &payload) {
	QString result;
	result.reserve(1 + kKindTrits + kValueTrits + (payload.isEmpty() ? 0 : (kLenTrits + payload.size() * kByteTrits)));
	result.push_back(QChar(kOpen));
	result += EncodeNumber(kind, kKindTrits);
	result += EncodeNumber(value, kValueTrits);
	if (HasPayload(kind)) {
		result += EncodeNumber(payload.size(), kLenTrits);
		for (int b = 0; b < payload.size(); ++b) {
			result += EncodeNumber(static_cast<uchar>(payload[b]), kByteTrits);
		}
	}
	return result;
}

QString Close() {
	return QString(QChar(kClose));
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
	if (runs.empty() && !textWithEntities.text.contains(QChar(kOpen))) {
		return;
	}

	const auto origSize = textWithEntities.text.size();
	std::vector<bool> removed(origSize, false);

	for (const auto &run : runs) {
		for (int i = run.openTagStart; i < run.openTagEnd && i < origSize; ++i) {
			removed[i] = true;
		}
		if (run.closeTagPos >= 0 && run.closeTagPos < origSize) {
			removed[run.closeTagPos] = true;
		}
	}

	for (int i = 0; i < origSize; ++i) {
		const auto u = textWithEntities.text[i].unicode();
		if (u == kOpen || u == kClose || (u >= kTrit && u < kTrit + kTrits)) {
			removed[i] = true;
		}
	}

	QString cleanText;
	cleanText.reserve(origSize);
	std::vector<int> oldToNew(origSize + 1, 0);

	for (int i = 0; i < origSize; ++i) {
		oldToNew[i] = cleanText.size();
		if (!removed[i]) {
			cleanText.push_back(textWithEntities.text[i]);
		}
	}
	oldToNew[origSize] = cleanText.size();

	const auto maxIndex = int(origSize);
	for (auto it = textWithEntities.entities.begin(); it != textWithEntities.entities.end();) {
		const auto start = oldToNew[ClampIndex(it->offset(), maxIndex)];
		const auto end = oldToNew[ClampIndex(it->offset() + it->length(), maxIndex)];
		if (end > start) {
			*it = EntityInText(it->type(), start, end - start, it->data());
			++it;
		} else {
			it = textWithEntities.entities.erase(it);
		}
	}

	for (const auto &run : runs) {
		const auto start = oldToNew[ClampIndex(run.start, maxIndex)];
		const auto end = oldToNew[ClampIndex(run.end, maxIndex)];
		const auto length = end - start;
		if (length <= 0) {
			continue;
		}
		if (run.kind == kKindButton) {
			const auto url = run.text();
			if (!url.isEmpty()) {
				textWithEntities.entities.push_back(
					EntityInText(EntityType::CustomUrl, start, length, url));
			}
		} else if (run.kind == kKindEmoji) {
			bool ok = false;
			const auto docId = run.text().trimmed().toULongLong(&ok);
			if (ok && docId != 0) {
				textWithEntities.entities.push_back(
					EntityInText(EntityType::CustomEmoji, start, length, Data::SerializeCustomEmojiId(docId)));
			}
		} else if (run.kind == kKindOutline) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Bold, start, length));
		} else if (run.kind == kKindDim) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Italic, start, length));
		} else if (run.kind == kKindRainbow) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Underline, start, length));
		} else if (run.kind == kKindSize) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Bold, start, length));
		}
	}

	textWithEntities.text = std::move(cleanText);
}

} // namespace Margy::Markup
