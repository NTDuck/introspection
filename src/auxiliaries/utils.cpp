#include <auxiliaries/utils.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <unordered_map>

#include <SDL.h>
#include <pugixml/pugixml.hpp>
#include <zlib/zlib.h>

#include <auxiliaries/globals.hpp>


bool operator==(const SDL_Point& first, const SDL_Point& second) {
    return first.x == second.x && first.y == second.y;
}

bool operator!=(const SDL_Point& first, const SDL_Point& second) {
    return !(first == second);
}

bool operator<(const SDL_Point& first, const SDL_Point& second) {
    return (first.y < second.y) || (first.y == second.y && first.x < second.x);
}

SDL_Point operator+(const SDL_Point& first, const SDL_Point& second) {
    return {first.x + second.x, first.y + second.y};
}

SDL_Point operator-(const SDL_Point& first, const SDL_Point& second) {
    return {first.x - second.x, first.y - second.y};
}


template <typename Iterable, typename Function, typename... Args>
void utils::iterate(const Iterable& iterable, Function&& function, Args&&... args) {
    for (const auto& element : iterable) std::invoke(std::forward<Function>(function), element, std::forward<Args>(args)...);
}

/**
 * @brief Convert a `float` to type `int`. Achieve a similar effect to `std::floor`.
 * @note Susceptible to data loss.
*/
int utils::castFloatToInt(float f) { return static_cast<int>(std::lroundf(f)); }

/**
 * @brief Retrieve a binary outcome. Models a Bernoulli distribution.
 * @see https://en.wikipedia.org/wiki/Bernoulli_distribution
 * @see https://en.wikipedia.org/wiki/Mersenne_Twister
*/
int utils::generateRandomBinary(const double probability) {
    std::random_device rd;
    std::mt19937 mt(rd());  // Mersenne Twister 19937 engine

    std::bernoulli_distribution dist(probability);   // Bernoulli trial
    return dist(mt);
}

/**
 * @brief Calculate the distance between two `SDL_Point`.
*/
double utils::calculateDistance(const SDL_Point& first, const SDL_Point& second) {
    auto sub = first - second;
    return std::sqrt(std::pow(sub.x, 2) + std::pow(sub.y, 2));
}

/**
 * @brief Convert a string representing a hex color value to `SDL_Color`.
*/
SDL_Color utils::SDL_ColorFromHexString(const std::string& hexString) {
    // Convert hexadecimal string to unsigned integer
    uint32_t ARGB = std::stoul(hexString.substr(1), nullptr, 16);

    // Isolate each component (8 bits) then mask out redundancies (via bitwise AND, to ensure valid range 0 - 255)
    uint8_t alpha = (ARGB >> 24) & 0xff;
    uint8_t red = (ARGB >> 16) & 0xff;
    uint8_t green = (ARGB >> 8) & 0xff;
    uint8_t blue = ARGB & 0xff;

    return {red, green, blue, alpha};
}

/**
 * @brief Allow `SDL_Color` to be passed into `SDL_SetRendererDrawColor()` instead of `uint8_t`.
*/
void utils::setRendererDrawColor(SDL_Renderer* renderer, const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/**
 * @brief Decompress a zlib-compressed string.
 * @param s the zlib-compressed string.
 * @return the decompressed stream represented as a vector of the specified data type.
 * @note Conversion to `std::string` can be done as follows: `std::vector<char> vec = utils::zlibDecompress(compressed); `std::string decompressed(vec.data(), vec.size());`
*/
template <typename T>
std::vector<T> utils::zlibDecompress(const std::string& s) {
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

    // "Clear" `stream` by setting all bytes to `0`
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
 * @brief Decrypt a base64-encrypted string.
 * @param s the base-64 encrypted string.
*/
std::string utils::base64Decode(const std::string& s) {
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
 * @brief Read a JSON file.
*/
void utils::readJSON(const std::filesystem::path& path, json& data) {
    std::ifstream file;
    file.open(path);
    if (!file.is_open()) return;

    file >> data;
    file.close();
}

/**
 * @brief Remove leading dots (`.`) and slashes (`/` `\`) in a `std::filesystem::path`.
 * @note Fall back to string manipulation since `std::filesystem` methods (`canonical()`, `lexically_normal()`, etc.) fails inexplicably.
*/
void utils::cleanRelativePath(std::filesystem::path& path) {
    std::string s = path.string();

    size_t found = s.find("../");
    while (found != std::string::npos) {
        s = s.substr(found + 3);
        found = s.find("../");
    }

    path = s;
}

/**
 * @brief Register data associated with levels initialization to a temporary storage.
 * @param mapping the aforementioned storage.
*/
void utils::loadLevelsData(level::LevelMapping& mapping) {
    json data;
    utils::readJSON(globals::config::kConfigPathLevel.string(), data);

    auto levels = data.find("levels");
    if (levels == data.end() || !levels.value().is_array()) return;

    for (const auto& level : levels.value()) {
        if (!level.is_object()) continue;

        auto name = level.find("name");
        auto source = level.find("source");
        if (name == level.end() || source == level.end() || !name.value().is_string() || !source.value().is_string()) continue;

        auto result = level::kLevelNameConversionMapping.find(name.value());
        if (result == level::kLevelNameConversionMapping.end()) continue;

        mapping[result->second] = source.value();
    }
}

/**
 * @brief Read data associated with a level from a JSON file (preferrably converted from a Tiled TMX file).
 * @note Only `csv` and `zlib-compressed base64` are supported.
 * @todo Reimplementation should be made to adhere to SoC. (this is a true mess)
*/
void utils::loadLevelData(level::LevelData& currentLevelData, const json& JSONLevelData) {
    // Clear current level data
    currentLevelData.deinitialize();

    // Update global variables
    auto tileDestCountWidth = JSONLevelData.find("width");
    auto tileDestCountHeight = JSONLevelData.find("height");
    if (tileDestCountWidth == JSONLevelData.end() || tileDestCountHeight == JSONLevelData.end() || !tileDestCountWidth.value().is_number_integer() || !tileDestCountHeight.value().is_number_integer()) return;

    globals::tileDestCount = {tileDestCountWidth.value(), tileDestCountHeight.value()};
    globals::tileDestSize.x = globals::tileDestSize.y = 1 << int(log2(std::min(globals::windowSize.x / globals::tileDestCount.x, globals::windowSize.y / globals::tileDestCount.y)));   // The closest power of 2
    globals::windowOffset = {
        (globals::windowSize.x - globals::tileDestCount.x * globals::tileDestSize.x) / 2,
        (globals::windowSize.y - globals::tileDestCount.y * globals::tileDestSize.y) / 2,
    };

    auto backgroundColor = JSONLevelData.find("backgroundcolor");
    if (backgroundColor != JSONLevelData.end() && !backgroundColor.value().is_string()) return;
    currentLevelData.backgroundColor = (backgroundColor != JSONLevelData.end() ? utils::SDL_ColorFromHexString(backgroundColor.value()) : globals::config::kDefaultBackgroundColor);

    // Emplace gids into `tileCollection`. Executed per layer.
    currentLevelData.tileCollection.resize(globals::tileDestCount.y);
    for (auto& tileRow : currentLevelData.tileCollection) tileRow.resize(globals::tileDestCount.x);

    auto layers = JSONLevelData.find("layers");
    if (layers == JSONLevelData.end() || !layers.value().is_array()) return;

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

            for (int y = 0; y < globals::tileDestCount.y; ++y) for (int x = 0; x < globals::tileDestCount.x; ++x) currentLevelData.tileCollection[y][x].emplace_back(GIDsCollection[y * globals::tileDestCount.x + x]);

        } else if (type.value() == "objectgroup") {
            currentLevelData.initialize(layer);
        }
    }
}

/**
 * @brief Initialize all tilelayer tilesets associated with the current level.
*/
void utils::loadTilesetsData(SDL_Renderer* renderer, tile::TilelayerTilesetData::Collection& tilesetDataCollection, const json& data) {
    for (auto& tilesetData : tilesetDataCollection) tilesetData.deinitialize();   // Prevent memory leak
    tilesetDataCollection.clear();

    auto tilesets = data.find("tilesets");
    if (tilesets == data.end() || !tilesets.value().is_array()) return;

    for (const auto& tileset : tilesets.value()) {
        tile::TilelayerTilesetData tilesetData;
        tilesetData.initialize(tileset, renderer);
        tilesetDataCollection.emplace_back(tilesetData);
    }
}

/**
 * @brief Retrieve the `TilelayerTilesetData` associated with a `GID`.
 * @todo Optimization should be made to reduce time complexity. Try binary search.
*/
const tile::TilelayerTilesetData* utils::getTilesetData(const tile::TilelayerTilesetData::Collection& tilesetDataCollection, int gid) {
    auto it = std::find_if(tilesetDataCollection.begin(), tilesetDataCollection.end(), [&](const auto tilesetData) {
        return tilesetData.firstGID <= gid && gid < tilesetData.firstGID + tilesetData.srcCount.x * tilesetData.srcCount.y;
    });
    return it != tilesetDataCollection.end() ? &*it : nullptr;
}

/**
 * @brief Read data associated with a tileset from loaded XML data.
 * @note Also loads the `texture`.
 * @note Requires `document` to be successfully loaded from a XML file.
*/
void tile::BaseTilesetData::initialize(pugi::xml_document& document, SDL_Renderer* renderer) {
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

    std::filesystem::path path(src.as_string());
    utils::cleanRelativePath(path);
    texture = IMG_LoadTexture(renderer, (globals::config::kAssetPath / path).string().c_str());   // Should also check whether path exists
}

/**
 * @brief Read data associated with a tilelayer tileset from loaded JSON data.
 * @note Also loads the `texture` and populate `firstGID`.
 * @note `firstGID` is contained only in Tiled Map JSON files.
*/
void tile::TilelayerTilesetData::initialize(const json& tileset, SDL_Renderer* renderer) {
    auto firstGID_ = tileset.find("firstgid");
    if (firstGID_ == tileset.end() || !firstGID_.value().is_number_integer()) return;
    firstGID = firstGID_.value();

    auto src = tileset.find("source");
    if (src == tileset.end() || !src.value().is_string()) return;
    std::filesystem::path xmlPath(src.value());
    utils::cleanRelativePath(xmlPath);

    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file((globals::config::kTiledAssetPath / xmlPath).c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

    BaseTilesetData::initialize(document, renderer);

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
 * @brief Read data associated with a tileset used for an entity or an animated object from loaded XML data.
 * @note Use `std::strcmp()` instead of `std::string()` in C-string comparison for slight performance gains.
*/
void tile::EntitiesTilesetData::initialize(pugi::xml_document& document, SDL_Renderer* renderer) {
    BaseTilesetData::initialize(document, renderer);

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

            auto animationType = EntitiesTilesetData::kAnimationTypeConversionMapping.find(name.as_string());
            if (animationType == EntitiesTilesetData::kAnimationTypeConversionMapping.end()) continue;

            EntitiesTilesetData::Animation animation;

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
            animationMapping.emplace(std::make_pair(animationType->second, animation));
        }
    }
}

/**
 * @brief An alternative to the constructor. Populate data members based on JSON data.
 * @note Requires JSON data to be in proper format before being called.
*/
void level::EntityLevelData::initialize(const json& entityJSONLeveData) {
    auto destCoordsX = entityJSONLeveData.find("x");
    auto destCoordsY = entityJSONLeveData.find("y");
    auto destSizeWidth = entityJSONLeveData.find("width");
    auto destSizeHeight = entityJSONLeveData.find("height");

    // Also check for division by zero
    if (destCoordsX == entityJSONLeveData.end() || destCoordsY == entityJSONLeveData.end() || destSizeWidth == entityJSONLeveData.end() || destSizeHeight == entityJSONLeveData.end() || !destCoordsX.value().is_number_integer() || !destCoordsY.value().is_number_integer() || !destSizeWidth.value().is_number_integer() || !destSizeHeight.value().is_number_integer()) return;

    destCoords = {
        int(destCoordsX.value()) / int(destSizeWidth.value()),
        int(destCoordsY.value()) / int(destSizeHeight.value()),
    };
}

void level::TeleporterLevelData::initialize(const json& entityJSONLevelData) {
    EntityLevelData::initialize(entityJSONLevelData);

    auto teleporterProperties = entityJSONLevelData.find("properties");
    if (teleporterProperties == entityJSONLevelData.end() || !teleporterProperties.value().is_array()) return;

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

            targetDestCoords = {targetDestCoordsX.value(), targetDestCoordsY.value()};

        } else if (name.value() == "target-level") {
            if (!value.value().is_string()) continue;
            auto result = level::kLevelNameConversionMapping.find(value.value());
            if (result == level::kLevelNameConversionMapping.end()) continue;
            targetLevel = result->second;
        }
    }
}

void level::LevelData::initialize(const json& JSONLayerData) {
    auto objects = JSONLayerData.find("objects");
    if (objects == JSONLayerData.end() || !objects.value().is_array()) return;
    
    for (const auto& object : objects.value()) {
        auto type = object.find("type");
        if (type == object.end() || !type.value().is_string()) continue;

        if (type.value() == "player") {
            level::PlayerLevelData data;
            data.initialize(object);
            playerLevelData = data;
        } else if (type.value() == "teleporter") {
            level::TeleporterLevelData data;
            data.initialize(object);
            teleportersLevelData.insert(data);
        } else if (type.value() == "slime") {
            level::SlimeLevelData data;
            data.initialize(object);
            slimesLevelData.insert(data);
        }
    }
}

void level::LevelData::deinitialize() {
    for (auto& tileRow : tileCollection) for (auto& tile : tileRow) tile.clear();
    backgroundColor = globals::config::kDefaultBackgroundColor;
    teleportersLevelData.clear();
    slimesLevelData.clear();
}

/**
 * @brief Calculate the finalized physical damage dealt to entity `passive` by entity `active`.
 * @note Defined here to prevent circular dependencies.
*/
int EntitySecondaryStats::calculateFinalizedPhysicalDamage(EntitySecondaryStats& active, EntitySecondaryStats& passive) {
    return std::round(active.PhysicalDamage * (1 - passive.PhysicalDefense) * (utils::generateRandomBinary(active.CriticalChance) ? active.CriticalDamage : 1));
}

/**
 * @brief Calculate the finalized magic damage dealt to entity `passive` by entity `active`.
 * @note Defined here to prevent circular dependencies.
*/
int EntitySecondaryStats::calculateFinalizedMagicDamage(EntitySecondaryStats& active, EntitySecondaryStats& passive) {
    return std::round(active.MagicDamage * (1 - passive.MagicDefense) * (utils::generateRandomBinary(active.CriticalChance) ? active.CriticalDamage : 1));
}