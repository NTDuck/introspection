#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <SDL.h>
#include <pugixml/pugixml.hpp>
#include <zlib/zlib.h>

#include <auxiliaries/globals.hpp>
#include <auxiliaries/utils.hpp>


namespace utils {
    /**
     * @brief Decrypt a base64-encrypted string.
    */
    std::string base64Decode(const std::string s) {
        const std::string b64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";   // Characters used in base64 encoding alphabet

        // Map each base64 character to its corresponding index
        std::vector<int> reverseMapping(256, -1);
        for (int i = 0; i < 64; ++i) reverseMapping[b64chars[i]] = i;

        std::string output;
        int bits = 0;
        int bitCount = 0;
        int value = 0;

        // Process each character in `s`
        for (const auto& c : s) {
            if (reverseMapping[c] == -1) continue;   // Skip non-base64 characters

            value = (value << 6) | reverseMapping[c];   // Retrieve the base64 character's index (representing a 6-bit value) from `reverseMapping`, then "appended" to `value` (shifting existing bits left by 6 and add the newfound 6-bit value)
            bitCount += 6;
            bits <<= 6;   // Accomodate new bits

            while (bitCount >= 8) {   // Enough bits to form a byte
                output += char((value >> (bitCount - 8)) & 0xFF);   // Extract the most significant byte i.e. 8 bits from `value`, then append to `output`
                bitCount -= 8;
            }
        }

        return output;
    }

    /**
     * @brief Decompress a zlib-compressed `std::string`.
     * @note Conversion to `std::string` can be done as follows:
     * @note `std::vector<char> vec = utils::zlibDecompress(compressed);`
     * @note `std::string decompressed(vec.data(), vec.size());`
    */
    template <typename T>
    std::vector<T> zlibDecompress(const std::string s) {
        std::vector<T> decompressed;   // Avoid guessing decompressed data size
        unsigned char buffer[sizeof(T)];   // Temporarily hold decompressed data in bytes

        // Initialize zlib stream
        z_stream stream;
        // Default memory allocation/deallocation
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        // Input data is not yet available
        stream.avail_in = 0;
        stream.next_in = Z_NULL;

        // "Clears" `stream` by setting all bytes to `0`
        std::memset(&stream, 0, sizeof(stream));
        
        // Initialize zlib for inflation i.e. decompression
        int ret = inflateInit(&stream);
        if (ret != Z_OK) return {};

        stream.avail_in = s.size();
        stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(s.c_str()));   // Point to input memory location, also convert `const char*` to `Bytef*` to match zlib data type

        do {
            stream.avail_out = sizeof(buffer);
            stream.next_out = buffer;
            ret = inflate(&stream, Z_NO_FLUSH);   // Perform decompression

            if (ret != Z_OK && ret != Z_STREAM_END) {
                inflateEnd(&stream);
                return {};   // throw/std::cerr/log
            }

            decompressed.push_back(*reinterpret_cast<T*>(buffer));   // Cast the buffer to desired data type `T`
        } while (ret != Z_STREAM_END);

        // Cleanup
        inflateEnd(&stream);
        return decompressed;
    }

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
     * @brief Remove leading dots (`.`) and slashes (`/``\`) in a path-like `std::string`.
    */
    void cleanRelativePath(std::string& path) {
        size_t found = path.find("../");

        while (found != std::string::npos) {
            path = path.substr(found + 3);
            found = path.find("../");
        }
    }

    /**
     * @brief Parse level-related data from a JSON file. Converted from TMX file, preferably.
     * @note Only `csv` and `zlib-compressed base64` are supported.
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
            json rawData = layer["data"];
            std::vector<int> data;
            auto encoding = layer.find("encoding");
            auto compression = layer.find("compression");

            if ((encoding == layer.end() || encoding.value() == "csv") && compression == layer.end()) {   // csv
                for (const auto& GID : rawData) data.emplace_back(GID);
            } else if (encoding != layer.end() && encoding.value() == "base64") {
                std::string decoded = utils::base64Decode(rawData);
                if (compression.value() == "zlib") {   // zlib-compressed base64
                    data = utils::zlibDecompress<int>(decoded);
                } else return;
            } else return;

            for (int y = 0; y < globals::TILE_DEST_COUNT.y; ++y) for (int x = 0; x < globals::TILE_DEST_COUNT.x; ++x) levelData.tileCollection[y][x].emplace_back(data[y * globals::TILE_DEST_COUNT.x + x]);
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
            pugi::xml_parse_result result = document.load_file((config::DIR_TILESETS + xmlPath).c_str());   // All tilesets should be located in "assets/.tiled/"
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
            utils::cleanRelativePath(path);
            tilesetData.texture = IMG_LoadTexture(renderer, (config::DIR_ASSETS + path).c_str());

            // Register to collection
            tilesetDataCollection.emplace_back(tilesetData);
        }
    }

    /**
     * @brief Retrieve the `TilesetData` associated with a `GID`.
    */
    TilesetData getTilesetData(int gid) {
        for (const auto& TILESET_DATA : globals::TILESET_COLLECTION) if (TILESET_DATA.firstGID <= gid && gid < TILESET_DATA.firstGID + TILESET_DATA.TILE_SRC_COUNT.x * TILESET_DATA.TILE_SRC_COUNT.y) return TILESET_DATA;
        return {nullptr,  0, {0, 0}, {0, 0}, {}};   // Throw some trash value if not found
    }
}