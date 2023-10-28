#include <fstream>
#include <string>

#include <SDL.h>
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
     * @brief Parse level-related data from a JSON file. Converted from TMX file, preferably.
    */
    void loadLevelData(Level& levelData, json& data) {
        for (auto& tileRow : levelData.tileCollection) for (auto& tile : tileRow) tile.clear();

        // Update global variables
        globals::TILE_DEST_COUNT = {data["width"], data["height"]};
        globals::TILE_DEST_SIZE = {globals::WINDOW_SIZE.x / globals::TILE_DEST_COUNT.x, globals::WINDOW_SIZE.y / globals::TILE_DEST_COUNT.y};
        globals::OFFSET = {
            (globals::WINDOW_SIZE.x - globals::TILE_DEST_COUNT.x * globals::TILE_DEST_SIZE.x) / 2,
            (globals::WINDOW_SIZE.y - globals::TILE_DEST_COUNT.y * globals::TILE_DEST_SIZE.y) / 2,
        };

        levelData.tileCollection.resize(globals::TILE_DEST_COUNT.y);
        for (auto& tileRow : levelData.tileCollection) tileRow.resize(globals::TILE_DEST_COUNT.x);

        // Emplace gids into `tileCollection`. Executed per layer.
        for (const auto& layer : data["layers"]) {
            // Prevent registering non-tilelayers e.g. object layers
            if (layer["type"] != "tilelayer") continue;

            // Load GIDs
            json data = layer["data"];
            for (int y = 0; y < globals::TILE_DEST_COUNT.y; ++y) {
                for (int x = 0; x < globals::TILE_DEST_COUNT.x; ++x) {
                    levelData.tileCollection[y][x].emplace_back(data[y * globals::TILE_DEST_COUNT.x + x]);
                }
            }
        }
    }

    /**
     * @brief Parse tileset-related data from a XML file. Also compatible with XML-like formats, including Tiled's TSX.
    */
    void loadTilesetData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& jsonData) {
        tilesetDataCollection.clear();

        for (const auto& data : jsonData["tilesets"]) {
            TilesetData tilesetData;

            tilesetData.firstGID = data["firstgid"];
            const std::string xmlPath = data["source"];

            pugi::xml_document document;
            pugi::xml_parse_result result = document.load_file(("assets/.tiled/" + xmlPath).c_str());   // All tilesets should be located in "assets/.tiled/"
            if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

            // Parse nodes
            pugi::xml_node tilesetNode = document.child("tileset");
            pugi::xml_node propertiesNode = tilesetNode.child("properties");
            pugi::xml_node imageNode = tilesetNode.child("image");

            tilesetData.TILE_SRC_COUNT = {
                tilesetNode.attribute("columns").as_int(),
                tilesetNode.attribute("tilecount").as_int() / tilesetNode.attribute("columns").as_int(),
            };
            tilesetData.TILE_SRC_SIZE = {
                // image.attribute("width").as_int() / tileSrcCount.x,
                // image.attribute("height").as_int() / tileSrcCount.y,
                tilesetNode.attribute("tilewidth").as_int(),
                tilesetNode.attribute("tileheight").as_int(),
            };
            
            // Custom properties
            for (pugi::xml_node propertyNode = propertiesNode.child("property"); propertyNode; propertyNode = propertyNode.next_sibling("property")) tilesetData.properties[propertyNode.attribute("name").as_string()] = propertyNode.attribute("value").as_string();

            // Load texture
            std::string path = imageNode.attribute("source").value();
            tilesetData.texture = IMG_LoadTexture(renderer, ("assets" + path.substr(2)).c_str());

            // Register to collection
            tilesetDataCollection.emplace_back(tilesetData);
        }
    }

    TilesetData getTilesetData(int gid) {
        for (const auto& TILESET_DATA : globals::TILESET_COLLECTION) if (TILESET_DATA.firstGID <= gid && gid < TILESET_DATA.firstGID + TILESET_DATA.TILE_SRC_COUNT.x * TILESET_DATA.TILE_SRC_COUNT.y) return TILESET_DATA;
        return {nullptr,  0, {0, 0}, {0, 0}, {}};   // Throw some trash value if not found
    }
}