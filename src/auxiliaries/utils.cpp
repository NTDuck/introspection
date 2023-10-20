#include <iostream>
#include <numeric>
#include <array>
#include <fstream>
#include <vector>
#include <random>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/globals.h>
#include <auxiliaries/utils.h>


namespace utils {
    /**
     * @brief Read a JSON file.
    */
    void readJSON(const std::string path, json& data) {
        std::ifstream file;
        file.open(path);
        if (!file.is_open()) return;

        file >> data;
        file.close();
    }

    /**
     * @brief Parse configuration data from the dedicated JSON file.
    */
    void parseConfigData(const std::string path, Flags& flags, SDL_Rect& dims, int& frameRate) {
        json configData;
        utils::readJSON(path, configData);

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
}