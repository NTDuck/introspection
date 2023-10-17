#include <fstream>
#include <string>

#include <SDL.h>

#include <game.h>
#include <auxiliaries/defs.h>


void parseConfigData(const std::string path, Flags& flags, SDL_Rect& dims, int& frameRate) {
    std::ifstream configFile;
    json configData;
    
    configFile.open(path);
    if (!configFile.is_open()) return;
    configFile >> configData;
    configFile.close();

    flags = {
        configData["flags"]["init"],
        configData["flags"]["window"],
        configData["flags"]["renderer"],
        configData["flags"]["image"],
        configData["flags"]["hints"],
    };
    dims = {
        configData["dims"]["x"],
        configData["dims"]["y"],
        configData["dims"]["w"],
        configData["dims"]["h"],
    };
    frameRate = configData["frameRate"];

    // std::ofstream configFile(path);
    // if (!configFile.is_open()) return 1;

    // configFile << configData.dump(2);
    // configFile.close();
}

int main(int argc, char* args[]) {
    Flags flags; SDL_Rect dims; int frameRate; std::string title = "8964";
    parseConfigData("config.json", flags, dims, frameRate);

    Game game(flags, dims, frameRate, title);
    game.start();

    return 0;
}