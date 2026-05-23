# include "EndRoll.hpp"

EndRoll::EndRoll(const FilePath& path) {
	TextReader reader{ path };
	strs = reader.readAll().split(U'\n');
}

void EndRoll::start() {
	stopwatch.start();
}

void EndRoll::draw()const {

	const double y = Scene::Size().y + size / 2 - Min(movement(), limit());
	for (auto i : step(strs.size())) {
		font(strs[i]).drawAt(40, Scene::Center().x, y + i * size);
	}
}

bool EndRoll::isRunning()const {
	return stopwatch.isRunning() && movement() <= limit();
}

double EndRoll::limit()const {
	return (strs.size() - 1) * size + Scene::Center().y;
}

double EndRoll::movement()const {
	return stopwatch.sF() * speed;
}

void EndRoll::speedchange(const double& speed_)
{
	speed = speed_;
}
