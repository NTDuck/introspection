#include <iostream>
#include <fstream>

#include <pugixml/pugixml.hpp>

#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>


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
     * @brief Parse tileset-related data from a XML file. Tailored for XML-like formats, including Tiled's TSX.
    */
    void parseTilesetData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& jsonData) {
        tilesetDataCollection.clear();

        for (const auto& data : jsonData["tilesets"]) {
            const int first_gid = data["firstgid"];
            const std::string xmlPath = data["source"];

            pugi::xml_document document;
            pugi::xml_parse_result result = document.load_file(("assets/.tiled/" + xmlPath).c_str());   // All tilesets should be located in "assets/.tiled/"
            if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

            // Parse nodes
            pugi::xml_node tilesetNode = document.child("tileset");
            pugi::xml_node imageNode = tilesetNode.child("image");

            SDL_Point tileSrcCount = {
                tilesetNode.attribute("columns").as_int(),
                tilesetNode.attribute("tilecount").as_int() / tilesetNode.attribute("columns").as_int(),
            };
            SDL_Point tileSrcSize = {
                // image.attribute("width").as_int() / tileSrcCount.x,
                // image.attribute("height").as_int() / tileSrcCount.y,
                tilesetNode.attribute("tilewidth").as_int(),
                tilesetNode.attribute("tileheight").as_int(),
            };

            // Construct proper path from provided path
            std::string tilePath = imageNode.attribute("source").value();
            tilePath = "assets" + tilePath.substr(2);

            // Load texture
            SDL_Texture* texture = IMG_LoadTexture(renderer, tilePath.c_str());

            // Register to collection
            tilesetDataCollection.push_back({texture, tilePath, first_gid, tileSrcCount, tileSrcSize});
        }
    }
}