#include <memory>

#include "Core/Application.h"

extern std::unique_ptr<IApplicationListener> CreateApplication();

int main(int argc, char** argv)
{
	Application app(CreateApplication());
	app.Run();
}