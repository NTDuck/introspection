#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

#include <SDL.h>
#include <pugixml/pugixml.hpp>
#include <zlib/zlib.h>

#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


namespace utils {
    /**
     * @brief Define/re-define hash function objects for needy classes.
     * @note Required for `std::unordered_set` and `std::unordered_map`.
    */
    namespace hashers {
        std::size_t SDL_Point_Hasher::operator()(const SDL_Point& obj) const { return std::hash<int>()(obj.x) ^ (std::hash<int>()(obj.y) << 1); }
    }

    /**
     * @brief Define/re-define common operators needed for `std::set`, `std::map`, `std::unordered_set`, `std::unordered_map`.
    */
    namespace operators {
        bool SDL_Point_Equality_Operator::operator()(const SDL_Point& first, const SDL_Point& second) const { return first.x == second.x && first.y == second.y; }

        bool SDL_Point_Less_Than_Operator::operator()(const SDL_Point& first, const SDL_Point& second) const { return (first.y < second.y) || (first.y == second.y && first.x < second.x); }
    }

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
    void readJSON(const std::filesystem::path path, json& data) {
        std::ifstream file;
        file.open(path);
        if (!file.is_open()) return;

        file >> data;
        file.close();
    }

    /**
     * @brief Remove leading dots (`.`) and slashes (`/` `\`) in a `std::filesystem::path`.
     * @note Fall back to string manipulation for `std::filesystem` methods (`canonical()`, `lexically_normal()`, etc.) fails inexplicably.
    */
    void cleanRelativePath(std::filesystem::path& path) {
        std::string s = path.string();

        size_t found = s.find("../");
        while (found != std::string::npos) {
            s = s.substr(found + 3);
            found = s.find("../");
        }

        path = s;
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
    void loadLevelsData(leveldata::LevelMapping& mapping) {
        json data;
        utils::readJSON(globals::config::LEVELS_PATH.string(), data);

        auto levels = data.find("levels");
        if (levels == data.end() || !levels.value().is_array()) return;

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
     * @note This handles A LOT.
     * @note Only `csv` and `zlib-compressed base64` are supported.
    */
    void loadLevelData(leveldata::LevelData& currentLevel, const json& data) {
        // Clear current level data
        for (auto& tileRow : currentLevel.tileCollection) for (auto& tile : tileRow) tile.clear();
        currentLevel.teleportersLevelData.clear();

        // Update global variables
        auto tileDestCountWidth = data.find("width");
        auto tileDestCountHeight = data.find("height");
        if (tileDestCountWidth == data.end() || tileDestCountHeight == data.end() || !tileDestCountWidth.value().is_number_integer() || !tileDestCountHeight.value().is_number_integer()) return;

        globals::tileDestCount = {tileDestCountWidth.value(), tileDestCountHeight.value()};
        globals::tileDestSize = {globals::windowSize.x / globals::tileDestCount.x, globals::windowSize.y / globals::tileDestCount.y};
        globals::windowOffset = {
            (globals::windowSize.x - globals::tileDestCount.x * globals::tileDestSize.x) / 2,
            (globals::windowSize.y - globals::tileDestCount.y * globals::tileDestSize.y) / 2,
        };

        auto backgroundColor = data.find("backgroundcolor");
        if (backgroundColor != data.end() && !backgroundColor.value().is_string()) return;
        currentLevel.backgroundColor = (backgroundColor != data.end() ? utils::SDL_ColorFromHexString(backgroundColor.value()) : globals::config::DEFAULT_BACKGROUND_COLOR);

        // Emplace gids into `tileCollection`. Executed per layer.
        currentLevel.tileCollection.resize(globals::tileDestCount.y);
        for (auto& tileRow : currentLevel.tileCollection) tileRow.resize(globals::tileDestCount.x);

        auto layers = data.find("layers");
        if (layers == data.end() || !layers.value().is_array()) return;

        for (const auto& layer : layers.value()) {
            // Prevent registering non-tilelayers e.g. object layers
            auto type = layer.find("type");
            if (type == layer.end() || !type.value().is_string()) continue;

            // Could use `std::unordered_map` for cleaner implementation
            if (type.value() == "tilelayer") {
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

                for (int y = 0; y < globals::tileDestCount.y; ++y) for (int x = 0; x < globals::tileDestCount.x; ++x) currentLevel.tileCollection[y][x].emplace_back(GIDsCollection[y * globals::tileDestCount.x + x]);

            } else if (type.value() == "objectgroup") {
                auto objects = layer.find("objects");
                if (objects == layer.end() || !objects.value().is_array()) continue;
                
                for (const auto& object : objects.value()) {
                    auto type = object.find("type");
                    if (type == object.end() || !type.value().is_string()) continue;

                    if (type.value() == "player") {
                        auto playerDestCoordsX = object.find("x");
                        auto playerDestCoordsY = object.find("y");
                        auto playerDestSizeWidth = object.find("width");
                        auto playerDestSizeHeight = object.find("height");

                        if (playerDestCoordsX == object.end() || playerDestCoordsY == object.end() || playerDestSizeWidth == object.end() || playerDestSizeHeight == object.end() || !playerDestCoordsX.value().is_number_integer() || !playerDestCoordsY.value().is_number_integer() || !playerDestSizeWidth.value().is_number_integer() || !playerDestSizeHeight.value().is_number_integer()) continue;

                        globals::currentLevelData.playerLevelData.destCoords = {int(playerDestCoordsX.value()) / int(playerDestSizeWidth.value()), int(playerDestCoordsY.value()) / int(playerDestSizeHeight.value())};

                    } else if (type.value() == "teleporter") {
                        auto teleporterDestCoordsX = object.find("x");
                        auto teleporterDestCoordsY = object.find("y");
                        auto teleporterDestSizeWidth = object.find("width");
                        auto teleporterDestSizeHeight = object.find("height");

                        if (teleporterDestCoordsX == object.end() || teleporterDestCoordsY == object.end() || teleporterDestSizeWidth == object.end() || teleporterDestSizeHeight == object.end() || !teleporterDestCoordsX.value().is_number_integer() || !teleporterDestCoordsY.value().is_number_integer() || !teleporterDestSizeWidth.value().is_number_integer() || !teleporterDestSizeHeight.value().is_number_integer()) continue;

                        leveldata::TeleporterData teleporter;

                        teleporter.destCoords = {int(teleporterDestCoordsX.value()) / int(teleporterDestSizeWidth.value()), int(teleporterDestCoordsY.value()) / int(teleporterDestSizeHeight.value())};

                        auto teleporterProperties = object.find("properties");
                        if (teleporterProperties == object.end() || !teleporterProperties.value().is_array()) continue;

                        for (const auto& property : teleporterProperties.value()) {
                            auto name = property.find("name");
                            auto value = property.find("value");

                            if (name == property.end() || value == property.end() || !value.value().is_string()) continue;

                            if (name.value() == "target-dest-coords") {
                                auto& value_ = value.value();
                                if (!value_.is_object()) continue;

                                auto targetDestCoordsX = value_.find("x");
                                auto targetDestCoordsY = value_.find("y");
                                if (targetDestCoordsX == value_.end() || targetDestCoordsY == value_.end() || !targetDestCoordsX.value().is_number() || !targetDestCoordsY.value().is_number()) continue;

                                teleporter.targetDestCoords.x = targetDestCoordsX.value();
                                teleporter.targetDestCoords.y = targetDestCoordsY.value();

                            } else if (name.value() == "target-level") {
                                if (!value.value().is_string()) continue;
                                teleporter.targetLevel = value.value();
                            }
                        }

                        currentLevel.teleportersLevelData.insert(teleporter);
                    }
                }
            }
        }
    }

    /**
     * @brief Call `init()` method of all tilelayer-tileset.
    */
    void loadTilesetsData(SDL_Renderer* renderer, tiledata::TilelayerTilesetData::TilelayerTilesetDataCollection& tilesetDataCollection, const json& data) {
        for (auto& tilesetData : tilesetDataCollection) tilesetData.dealloc();   // Prevent memory leak
        tilesetDataCollection.clear();

        auto tilesets = data.find("tilesets");
        if (tilesets == data.end() || !tilesets.value().is_array()) return;

        for (const auto& tileset : tilesets.value()) {
            tiledata::TilelayerTilesetData tilesetData;
            tilesetData.init(tileset, renderer);
            tilesetDataCollection.emplace_back(tilesetData);
        }
    }

    bool compareByFirstGID(const tiledata::TilelayerTilesetData& first, const tiledata::TilelayerTilesetData& second) {
        return first.firstGID < second.firstGID;
    }

    /**
     * @brief Retrieve the `TilelayerTilesetData` associated with a `GID`.
     * @note Requires further optimization to reduce time complexity.
     * @todo Try binary search.
    */
    tiledata::TilelayerTilesetData getTilesetData(tiledata::TilelayerTilesetData::TilelayerTilesetDataCollection& tilesetDataCollection, int gid) {
        for (const auto& tilesetData : tilesetDataCollection) if (tilesetData.firstGID <= gid && gid < tilesetData.firstGID + tilesetData.srcCount.x * tilesetData.srcCount.y) return tilesetData;
        return tiledata::TilelayerTilesetData();
    }
}

namespace tiledata {
    /**
     * @brief Parse a tileset's data from a loaded `pugi::xml_document`.
     * @note Also loads the `texture`.
     * @note Requires `document` to be successfully loaded from a XML file.
    */
    void BaseTilesetData::init(pugi::xml_document& document, SDL_Renderer* renderer) {
        // Parse nodes
        pugi::xml_node tilesetNode = document.child("tileset");
        pugi::xml_node imageNode = tilesetNode.child("image");

        if (tilesetNode.empty() || imageNode.empty()) return;   // A tileset can have no properties

        // Dimensions
        auto srcCountX = tilesetNode.attribute("columns");
        auto srcCountSum = tilesetNode.attribute("tilecount");
        auto srcSizeX = tilesetNode.attribute("tilewidth");
        auto srcSizeY = tilesetNode.attribute("tileheight");

        if (srcCountX == nullptr || srcCountSum == nullptr || srcSizeX == nullptr || srcSizeY == nullptr) return;

        srcCount = {srcCountX.as_int(), srcCountSum.as_int() / srcCountX.as_int()};
        srcSize = {srcSizeX.as_int(), srcSizeY.as_int()};

        // Load texture
        auto src = imageNode.attribute("source");
        if (src == nullptr) return;
        std::filesystem::path path(src.value());
        utils::cleanRelativePath(path);
        texture = IMG_LoadTexture(renderer, (globals::config::ASSETS_PATH / path).string().c_str());   // Should also check whether path exists
    }

    /**
     * @brief Parse a tilelayer-tileset's data from a loaded `nlohmann::json`.
     * @note Also loads the `texture` and populate `firstGID`.
     * @note `firstGID` comes only from Tiled Map JSON files.
    */
    void TilelayerTilesetData::init(const json& tileset, SDL_Renderer* renderer) {
        auto firstGID_ = tileset.find("firstgid");
        if (firstGID_ == tileset.end() || !firstGID_.value().is_number_integer()) return;
        firstGID = firstGID_.value();

        auto src = tileset.find("source");
        if (src == tileset.end() || !src.value().is_string()) return;
        std::filesystem::path xmlPath(src.value());
        utils::cleanRelativePath(xmlPath);

        pugi::xml_document document;
        pugi::xml_parse_result result = document.load_file((globals::config::TILED_ASSETS_PATH / xmlPath).c_str());   // All tilesets should be located in "assets/.tiled/"
        if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

        BaseTilesetData::init(document, renderer);

        // Properties
        pugi::xml_node propertiesNode = document.child("tileset").child("properties");
        if (propertiesNode.empty()) return;

        for (pugi::xml_node propertyNode = document.child("tileset").child("properties").child("property"); propertyNode; propertyNode = propertyNode.next_sibling("property")) {
            if (propertyNode.empty()) continue;

            auto name = propertyNode.attribute("name");
            auto type = propertyNode.attribute("type");

            if (name == nullptr) continue;

            if (type == nullptr || std::strcmp(type.as_string(), "class")) {
                auto value = propertyNode.attribute("value");
                if (value == nullptr) continue;
                properties.insert(std::make_pair(name.as_string(), value.as_string()));
            }
        }
    }

    /**
     * @brief Parse a animated-entities-tileset's data from a loaded `pugi::xml_document`.
     * @note Use `std::strcmp()` instead of `std::string()` in C-string comparison for slight performance gains.
    */
    void AnimatedEntitiesTilesetData::init(pugi::xml_document& document, SDL_Renderer* renderer) {
        BaseTilesetData::init(document, renderer);

        pugi::xml_node propertiesNode = document.child("tileset").child("properties");
        if (propertiesNode.empty()) return;

        for (pugi::xml_node propertyNode = document.child("tileset").child("properties").child("property"); propertyNode; propertyNode = propertyNode.next_sibling("property")) {
            if (propertyNode.empty()) continue;

            auto name = propertyNode.attribute("name");
            auto type = propertyNode.attribute("type");

            if (name == nullptr) continue;
            
            if (type == nullptr || std::strcmp(type.as_string(), "class")) {
                auto value = propertyNode.attribute("value");
                if (value == nullptr) continue;
                if (!std::strcmp(name.as_string(), "animation-update-rate")) {
                    animationUpdateRate = value.as_int();
                } else if (!std::strcmp(name.as_string(), "animation-width")) {
                    animationSize.x = value.as_int();
                } else if (!std::strcmp(name.as_string(), "animation-height")) {
                    animationSize.y = value.as_int();
                }
            } else {
                auto propertytype = propertyNode.attribute("propertytype");
                pugi::xml_node animationsNode = propertyNode.child("properties");
                if (propertytype == nullptr || std::strcmp(propertytype.as_string(), "animation") || animationsNode.empty()) continue;

                auto animationType = AnimatedEntitiesTilesetData::animationTypeMapping.find(name.as_string());
                if (animationType == AnimatedEntitiesTilesetData::animationTypeMapping.end()) continue;

                AnimatedEntitiesTilesetData::Animation animation;

                for (pugi::xml_node animationNode = animationsNode.child("property"); animationNode; animationNode = animationNode.next_sibling("property")) {
                    if (animationNode.empty()) continue;

                    auto name_ = animationNode.attribute("name");
                    auto value_ = animationNode.attribute("value");
                    if (name_ == nullptr || value_ == nullptr) continue;
                    
                    if (!std::strcmp(name_.as_string(), "startGID")) {
                        animation.startGID = value_.as_int();
                    } else if (!std::strcmp(name_.as_string(), "stopGID")) {
                        animation.stopGID = value_.as_int();
                    } else if (!std::strcmp(name_.as_string(), "isPermanent")) {
                        animation.isPermanent = value_.as_bool();
                    }
                }

                // Does this need to be inserted instead?
                animationMapping.emplace(std::make_pair(animationType -> second, animation));
            }
        }
    }
}