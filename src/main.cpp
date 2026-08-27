#include <memory>
#include <window.h>
#include <logger.h>


int main(int argc, char* argv[])
{
	std::unique_ptr<Window> window = std::make_unique<Window>();

	if (!window->init(1280, 960, "test window"))
	{
		Logger::log(1, "%s error: Window init error\n", __FUNCTION__);
		return -1;
	}

	window->mainLoop();
	window->cleanup();

	return 0;
}