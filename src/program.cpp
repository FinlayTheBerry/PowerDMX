#include "powerdmx.h"
#include "cliengine.h"
#include <iostream>
#include <string>

bool QuitRequested = false;

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    while (!QuitRequested)
    {
        std::string command = CliEngine::ReadLine();
        std::cout << CliEngine::RunCommand(command) << std::endl;
    }
    return 0;
}