#include "margy/tags/margy_audio_tags.h"

#include <QFile>
#include <QDataStream>
#include <cstdint>

namespace Margy::Tags {
namespace {

void WriteSynchsafe(QByteArray &out, uint32_t value) {
	out.append(char((value >> 21) & 0x7F));
	out.append(char((value >> 14) & 0x7F));
	out.append(char((value >> 7) & 0x7F));
	out.append(char(value & 0x7F));
}

void WriteInt(QByteArray &out, uint32_t value) {
	out.append(char((value >> 24) & 0xFF));
	out.append(char((value >> 16) & 0xFF));
	out.append(char((value >> 8) & 0xFF));
	out.append(char(value & 0xFF));
}

void WriteFrame(QByteArray &out, const char *id, const QByteArray &body) {
	out.append(id, 4);
	WriteInt(out, uint32_t(body.size()));
	out.append(char(0));
	out.append(char(0));
	out.append(body);
}

QByteArray MakeTextFrameBody(const QString &text) {
	QByteArray body;
	body.append(char(1)); // UTF-16 with BOM
	body.append(char(0xFF));
	body.append(char(0xFE));
	const auto utf16 = text.utf16();
	const auto len = text.size();
	body.append(reinterpret_cast<const char*>(utf16), len * sizeof(char16_t));
	return body;
}

QByteArray MakePictureFrameBody(const QByteArray &jpeg) {
	QByteArray body;
	body.append(char(0)); // ISO-8859-1
	body.append("image/jpeg", 10);
	body.append(char(0));
	body.append(char(3)); // Cover (front)
	body.append(char(0)); // Empty description terminator
	body.append(jpeg);
	return body;
}

int64_t OldTagLength(QFile &in) {
	const auto head = in.peek(10);
	if (head.size() < 10) {
		return 0;
	}
	if (head[0] != 'I' || head[1] != 'D' || head[2] != '3') {
		return -1; // No old tag
	}
	int64_t size = 0;
	for (int i = 6; i < 10; ++i) {
		size = (size << 7) | (uint8_t(head[i]) & 0x7F);
	}
	return size + 10; // Include header itself
}

} // namespace

bool WriteTags(
		const QString &srcPath,
		const QString &dstPath,
		const TagInfo &tags) {
	auto src = QFile(srcPath);
	if (!src.open(QIODevice::ReadOnly)) {
		return false;
	}
	auto dst = QFile(dstPath);
	if (!dst.open(QIODevice::WriteOnly)) {
		return false;
	}

	QByteArray frames;
	if (!tags.title.trimmed().isEmpty()) {
		WriteFrame(frames, "TIT2", MakeTextFrameBody(tags.title.trimmed()));
	}
	if (!tags.artist.trimmed().isEmpty()) {
		WriteFrame(frames, "TPE1", MakeTextFrameBody(tags.artist.trimmed()));
	}
	if (!tags.cover.isEmpty()) {
		WriteFrame(frames, "APIC", MakePictureFrameBody(tags.cover));
	}

	QByteArray header;
	header.append("ID3", 3);
	header.append(char(3)); // ID3v2.3
	header.append(char(0));
	header.append(char(0));
	WriteSynchsafe(header, uint32_t(frames.size()));

	if (dst.write(header) != header.size()) {
		return false;
	}
	if (!frames.isEmpty() && dst.write(frames) != frames.size()) {
		return false;
	}

	const auto oldLen = OldTagLength(src);
	if (oldLen > 0) {
		src.seek(oldLen);
	} else {
		src.seek(0);
	}

	constexpr qint64 kBufferSize = 64 * 1024;
	while (!src.atEnd()) {
		const auto chunk = src.read(kBufferSize);
		if (chunk.isEmpty() || dst.write(chunk) != chunk.size()) {
			return false;
		}
	}

	return true;
}

} // namespace Margy::Tags
