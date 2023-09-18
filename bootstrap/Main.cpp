#include <memory>

#include "Core/Application.h"
#include "Settings.h"

extern std::unique_ptr<IApplicationListener> CreateApplication();

int main(int argc, char** argv)
{
	Application app(CreateApplication(), { WIDTH, HEIGHT, 32, CAPTION });
	app.Run();
}