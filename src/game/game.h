#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>

#include "ecs/ecs.h"
#include "window.h"

class Game
{
public:
	Game();
	~Game();

	void init();
	void run();
	void setup(int level);
	void cleanup();
private:
	std::shared_ptr<Registry> m_Registry;
	//std::unique_ptr<EventBus> m_EventBus;

	int WINDOW_WIDTH = 2048;
	int WINDOW_HEIGHT = 1536;
	const int FPS = 60;
	const double FIXED_TIMESTEP = 1.0 / FPS;

	std::shared_ptr<Window> m_Window;

	double m_PreviousSeconds;
	
	glm::mat4 m_Projection;

	std::vector<Entity> m_Entities;

	static double m_MouseX, m_MouseY;
	static bool m_MouseClick;

	bool debug = true;
private:
	void loadLevel(int level);
	void processInput();
	void update();
	void render();
};

#endif