#include "game.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Game::Game()
{
	m_Registry = std::make_shared<Registry>();
}

Game::~Game()
{
}

void Game::init()
{
	m_Window = std::make_shared<Window>();
	if (!m_Window->init(1280, 960, "saudade"))
	{
		Logger::log(1, "%s error: Window init error\n", __FUNCTION__);
	}

	m_Projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
}

void Game::setup(int level)
{
	loadLevel(level);
	m_PreviousSeconds = glfwGetTime();
}

void Game::loadLevel(int level)
{
}


void Game::run()
{
	setup(1);
	while(!glfwWindowShouldClose(m_Window->get()))
	{
		processInput();
		m_Window->mainLoop();
		update();
		render();
	}
}


void Game::processInput()
{
}

void Game::update()
{
}

void Game::render()
{

}

void Game::cleanup()
{
	m_Window->cleanup();
}