# pragma once
# include <Siv3D.hpp>

class ScriptConfig
{
public:
	bool load(const FilePath& path);

	Vec2 positionOf(const String& key) const;
	double scaleOf(const String& key) const;

	bool hasPosition(const String& key) const;
	bool hasScale(const String& key) const;

private:
	void setDefaults();
	void loadPosition(const JSON& json, const String& key);
	void loadScale(const JSON& json, const String& key);

	HashTable<String, Vec2> positions;
	HashTable<String, double> scales;
};

ScriptConfig& GetScriptConfig();
