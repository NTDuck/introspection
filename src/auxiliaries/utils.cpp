#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

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
     * @brief Converts a `std::string` representing a hex color value to `SDL_Color`.
    */
    SDL_Color SDL_ColorFromHexString(const std::string& hexString) {
        // Convert hexadecimal string to unsigned integer
        uint32_t ARGB = std::stoul(hexString.substr(1), nullptr, 16);

        // Isolate each component (8 bits) then mask out redundancies (via bitwise AND, to ensure valid range 0 - 255)
        uint8_t alpha = (ARGB >> 24) & 0xff;
        uint8_t red = (ARGB >> 16) & 0xff;
        uint8_t green = (ARGB >> 8) & 0xff;
        uint8_t blue = ARGB & 0xff;

        return SDL_Color({red, green, blue, alpha});
    }

    /**
     * @brief Register level initialization data to a `LevelMapping`.
    */
    void loadLevelsData(const std::string jsonPath, LevelMapping& mapping) {
        json data;
        utils::readJSON(config::LEVELS_PATH.string(), data);

        auto levels = data.find("levels");
        if (levels == data.end() || !levels.value().is_array()) return;

        // int count = 0;

        for (const auto& level : levels.value()) {
            if (!level.is_object()) continue;
            auto name = level.find("name");
            auto source = level.find("source");
            if (name == level.end() || source == level.end() || !name.value().is_string() || !source.value().is_string()) continue;
            mapping[name.value()] = source.value();
        }
    }

    /**
     * @brief Parse level-related data from a JSON file. Converted from TMX file, preferably.
     * @note Only `csv` and `zlib-compressed base64` are supported.
    */
    void loadLevelData(Level& currentLevel, const json& data) {
        for (auto& tileRow : currentLevel.tileCollection) for (auto& tile : tileRow) tile.clear();

        // Update global variables
        auto tileDestCountWidth = data.find("width");
        auto tileDestCountHeight = data.find("height");
        if (tileDestCountWidth == data.end() || tileDestCountHeight == data.end() || !tileDestCountWidth.value().is_number_integer() || !tileDestCountHeight.value().is_number_integer()) return;

        globals::TILE_DEST_COUNT = {tileDestCountWidth.value(), tileDestCountHeight.value()};
        globals::TILE_DEST_SIZE = {globals::WINDOW_SIZE.x / globals::TILE_DEST_COUNT.x, globals::WINDOW_SIZE.y / globals::TILE_DEST_COUNT.y};
        globals::OFFSET = {
            (globals::WINDOW_SIZE.x - globals::TILE_DEST_COUNT.x * globals::TILE_DEST_SIZE.x) / 2,
            (globals::WINDOW_SIZE.y - globals::TILE_DEST_COUNT.y * globals::TILE_DEST_SIZE.y) / 2,
        };

        auto backgroundColor = data.find("backgroundcolor");
        if (backgroundColor != data.end() && !backgroundColor.value().is_string()) return;
        currentLevel.backgroundColor = (backgroundColor != data.end() ? utils::SDL_ColorFromHexString(backgroundColor.value()) : config::DEFAULT_BACKGROUND_COLOR);

        // Emplace gids into `tileCollection`. Executed per layer.
        currentLevel.tileCollection.resize(globals::TILE_DEST_COUNT.y);
        for (auto& tileRow : currentLevel.tileCollection) tileRow.resize(globals::TILE_DEST_COUNT.x);

        auto layers = data.find("layers");
        if (layers == data.end() || !layers.value().is_array()) return;

        for (const auto& layer : layers.value()) {
            // Prevent registering non-tilelayers e.g. object layers
            auto type = layer.find("type");
            if (type == layer.end() || !type.value().is_string() || type.value() != "tilelayer") continue;

            // Load GIDs
            auto layerData = layer.find("data");
            if (layerData == layer.end() || !(layerData.value().is_string() || layerData.value().is_array())) continue;

            std::vector<int> GIDsCollection;
            auto encoding = layer.find("encoding");
            auto compression = layer.find("compression");

            if ((encoding == layer.end() || encoding.value() == "csv") && compression == layer.end()) {   // csv
                for (const auto& GID : layerData.value()) GIDsCollection.emplace_back(GID);
            } else if (encoding != layer.end() && encoding.value() == "base64") {
                std::string decoded = utils::base64Decode(layerData.value());
                if (compression.value() == "zlib") {   // zlib-compressed base64
                    GIDsCollection = utils::zlibDecompress<int>(decoded);
                } else return;
            } else return;

            for (int y = 0; y < globals::TILE_DEST_COUNT.y; ++y) for (int x = 0; x < globals::TILE_DEST_COUNT.x; ++x) currentLevel.tileCollection[y][x].emplace_back(GIDsCollection[y * globals::TILE_DEST_COUNT.x + x]);
        }
    }

    /**
     * @brief Parse tileset-related data from a XML file. Also compatible with XML-like formats, including Tiled's TSX.
    */
    void loadTilesetData(SDL_Renderer* renderer, TilesetDataCollection& tilesetDataCollection, const json& data) {
        tilesetDataCollection.clear();

        auto tilesets = data.find("tilesets");
        if (tilesets == data.end() || !tilesets.value().is_array()) return;

        for (const auto& tileset : tilesets.value()) {
            TilesetData tilesetData;

            auto firstGID = tileset.find("firstgid");
            if (firstGID == tileset.end() || !firstGID.value().is_number_integer()) continue;
            tilesetData.firstGID = firstGID.value();

            auto _xmlPath = tileset.find("source");
            if (_xmlPath == tileset.end() || !_xmlPath.value().is_string()) continue;
            std::string xmlPath = _xmlPath.value();
            utils::cleanRelativePath(xmlPath);

            pugi::xml_document document;
            pugi::xml_parse_result result = document.load_file((config::TILED_ASSETS_PATH / xmlPath).string().c_str());   // All tilesets should be located in "assets/.tiled/"
            if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

            // Parse nodes
            pugi::xml_node tilesetNode = document.child("tileset");
            pugi::xml_node propertiesNode = tilesetNode.child("properties");
            pugi::xml_node imageNode = tilesetNode.child("image");

            if (tilesetNode.empty() || imageNode.empty()) continue;

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
            tilesetData.texture = IMG_LoadTexture(renderer, (config::ASSETS_PATH / path).string().c_str());

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