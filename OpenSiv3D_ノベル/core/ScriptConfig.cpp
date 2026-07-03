# include "core/ScriptConfig.hpp"

namespace
{
	const FilePath DefaultScriptConfigPath()
	{
		return Resource(U"Config/script_config.json");
	}
}

bool ScriptConfig::load(const FilePath& path)
{
	setDefaults();

	if (!FileSystem::Exists(path))
	{
		Logger << U"[warning] ScriptConfig: config file not found: " << path;
		return false;
	}

	const JSON json = JSON::Load(path);
	if (not json)
	{
		Logger << U"[warning] ScriptConfig: failed to load config file: " << path;
		return false;
	}

	for (const auto& key : { U"L", U"C", U"R", U"LL", U"RR" })
	{
		loadPosition(json, key);
	}

	for (const auto& key : { U"\u5927", U"\u4E2D", U"\u5C0F" })
	{
		loadScale(json, key);
	}

	return true;
}

Vec2 ScriptConfig::positionOf(const String& key) const
{
	if (const auto it = positions.find(key); it != positions.end())
	{
		return it->second;
	}

	Logger << U"[warning] ScriptConfig: undefined position key: " << key;
	return Vec2{ 0, 0 };
}

double ScriptConfig::scaleOf(const String& key) const
{
	if (const auto it = scales.find(key); it != scales.end())
	{
		return it->second;
	}

	Logger << U"[warning] ScriptConfig: undefined scale key: " << key;
	return 0.0;
}

bool ScriptConfig::hasPosition(const String& key) const
{
	return positions.contains(key);
}

bool ScriptConfig::hasScale(const String& key) const
{
	return scales.contains(key);
}

void ScriptConfig::setDefaults()
{
	positions.clear();
	scales.clear();

	positions[U"L"] = Vec2{ 384, 1600 };
	positions[U"C"] = Vec2{ 960, 1600 };
	positions[U"R"] = Vec2{ 1536, 1600 };
	positions[U"LL"] = Vec2{ 192, 1600 };
	positions[U"RR"] = Vec2{ 1728, 1600 };

	scales[U"\u5927"] = 2.0;
	scales[U"\u4E2D"] = 0.8;
	scales[U"\u5C0F"] = 0.5;
}

void ScriptConfig::loadPosition(const JSON& json, const String& key)
{
	const auto value = json[U"positions"][key];
	if (!value)
	{
		return;
	}

	positions[key] = Vec2{
		value[0].get<double>(),
		value[1].get<double>()
	};
}

void ScriptConfig::loadScale(const JSON& json, const String& key)
{
	const auto value = json[U"scales"][key];
	if (!value)
	{
		return;
	}

	scales[key] = value.get<double>();
}

ScriptConfig& GetScriptConfig()
{
	static ScriptConfig config;
	static bool loaded = false;

	if (!loaded)
	{
		config.load(DefaultScriptConfigPath());
		loaded = true;
	}

	return config;
}
