# pragma once
# include "Common.hpp"

class EndRoll {
public:

	Array<String> strs;

	Stopwatch stopwatch;

	Font font{ 30 };

	static constexpr double size = 50;
	double speed = 50;

	EndRoll(const FilePath& path);

	void start();

	void draw()const;

	bool isRunning()const;

	void speedchange(const double& speed_);

private:
	double limit()const;

	double movement()const;
};

