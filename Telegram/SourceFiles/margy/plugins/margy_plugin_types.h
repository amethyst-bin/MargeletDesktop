#pragma once

#include <QString>
#include <QtGui/QImage>
#include <vector>
#include <cstdint>

namespace Margy::Plugins {

struct PluginManifest final {
	QString id;
	QString name;
	QString nameEn;
	QString version;
	QString author;
	QString description;
	QString descriptionEn;
	QString minVersion;
	std::vector<QString> permissions;
	bool usesHooks = false;
	QImage icon;
	QString folder;

	[[nodiscard]] QString displayName() const {
		return !name.isEmpty() ? name : (!nameEn.isEmpty() ? nameEn : id);
	}

	[[nodiscard]] QString displayDescription() const {
		return !description.isEmpty() ? description : descriptionEn;
	}
};

struct ConsoleLine final {
	int64_t timestamp = 0;
	QString plugin;
	QString text;
	bool isError = false;
};

enum class SettingType {
	Header,
	Note,
	Switch,
	Text,
	Choice,
	Action,
};

struct SettingRow final {
	SettingType type = SettingType::Header;
	QString key;
	QString title;
	QString defaultValue;
	std::vector<QString> options;
	bool danger = false;
};

struct SparkParticle final {
	float x = 0.0f;
	float y = 0.0f;
	float radius = 2.0f;
	uint32_t color = 0xFFFFFFFF;
	float alpha = 1.0f;
};

struct CursorGlideState final {
	float x = 0.0f;
	float y = 0.0f;
	float height = 0.0f;
	bool visible = true;
	bool active = false;
};

struct TypingAnimFrame final {
	bool hasAnimation = false;
	std::vector<SparkParticle> sparks;
	CursorGlideState cursor;
};

} // namespace Margy::Plugins
