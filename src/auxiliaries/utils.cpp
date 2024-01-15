#include <auxiliaries.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <fstream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <string>
#include <random>
#include <unordered_map>

#include <SDL.h>
#include <pugixml/pugixml.hpp>
#include <zlib/zlib.h>


bool operator==(SDL_Point const& first, SDL_Point const& second) {
    return first.x == second.x && first.y == second.y;
}

bool operator!=(SDL_Point const& first, SDL_Point const& second) {
    return !(first == second);
}

bool operator<(SDL_Point const& first, SDL_Point const& second) {
    return (first.y < second.y) || (first.y == second.y && first.x < second.x);
}

SDL_Point operator+(SDL_Point const& first, SDL_Point const& second) {
    return {first.x + second.x, first.y + second.y};
}

SDL_Point operator-(SDL_Point const& first, SDL_Point const& second) {
    return {first.x - second.x, first.y - second.y};
}

bool operator==(SDL_FPoint const& first, SDL_FPoint const& second) {
    return first.x == second.x && first.y == second.y;
}

std::size_t std::hash<SDL_Point>::operator()(SDL_Point const& instance) const {
    return std::hash<int>{}(instance.x) ^ (std::hash<int>{}(instance.y) << 1);
}

std::size_t std::hash<SDL_FPoint>::operator()(SDL_FPoint const& instance) const {
    return std::hash<float>{}(instance.x) ^ (std::hash<float>{}(instance.y) << 1);
}

/**
 * @brief Convert a `float` to type `int`. Achieve a similar effect to `std::floor`.
 * @note Susceptible to data loss.
*/
int utils::castFloatToInt(const float f) { return static_cast<int>(std::lroundf(f)); }

/**
 * @brief Stringify a `double` with specified precision i.e. digits after the decimal point.
*/
std::string utils::castDoubleToString(const double d, unsigned int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << d;
    return oss.str();
}

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
double utils::calculateDistance(SDL_Point const& first, SDL_Point const& second) {
    auto sub = first - second;
    return std::sqrt(std::pow(sub.x, 2) + std::pow(sub.y, 2));
}

/**
 * @brief Convert a string representing a hex color value to `SDL_Color`.
*/
SDL_Color utils::SDL_ColorFromHexString(std::string const& hexString) {
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
void utils::setRendererDrawColor(SDL_Renderer* renderer, SDL_Color const& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/**
 * @brief Convert a texture to grayscale.
 * @see https://gigi.nullneuron.net/gigilabs/converting-an-image-to-grayscale-using-sdl2/
 * @see https://en.wikipedia.org/wiki/Grayscale
*/
SDL_Texture* utils::createGrayscaleTexture(SDL_Renderer* renderer, SDL_Texture* texture) {
    constexpr auto grayscale = [](SDL_Color const& color) {
        return static_cast<uint8_t>(0.212671f * color.r + 0.715160f * color.g + 0.072169f * color.b);
    };

    // Query texture dimensions
    SDL_Point size;
    SDL_QueryTexture(texture, nullptr, nullptr, &size.x, &size.y);

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, size.x, size.y, 32, SDL_PIXELFORMAT_RGBA32);   // Also needs to be same format?
    if (surface == nullptr) return nullptr;
    SDL_RenderReadPixels(renderer, nullptr, surface->format->format, surface->pixels, surface->pitch);   // Copy texture to surface

    // Convert surface to grayscale
    for (int y = 0; y < size.y; ++y) {
        for (int x = 0; x < size.x; ++x) {
            uint32_t& pixel = *(reinterpret_cast<uint32_t*>(surface->pixels) + y * (size.x) + x);   // Provide access to pixel `(x, y)` of `surface`

            SDL_Color color;
            SDL_GetRGBA(pixel, surface->format, &color.r, &color.g, &color.b, &color.a);   // Extract color from pixel

            uint8_t grayscaledPixel = grayscale(color);   // Create grayscaled pixel from extracted color
            pixel = SDL_MapRGBA(surface->format, grayscaledPixel, grayscaledPixel, grayscaledPixel, color.a);   // Register changes
        }
    }

    // Create new texture from grayscaled surface
    SDL_Texture* grayscaledTexture = SDL_CreateTextureFromSurface(renderer, surface);

    // Clean up
    SDL_FreeSurface(surface);

    return grayscaledTexture;
}

/**
 * @brief Decompress a zlib-compressed string.
 * @param s the zlib-compressed string.
 * @return the decompressed stream represented as a vector of the specified data type.
 * @note Conversion to `std::string` can be done as follows: `std::vector<char> vec = utils::zlibDecompress(compressed); `std::string decompressed(vec.data(), vec.size());`
*/
template <typename T>
std::vector<T> utils::zlibDecompress(std::string const& s) {
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
std::string utils::base64Decode(std::string const& s) {
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
void utils::readJSON(std::filesystem::path const& path, json& data) {
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
    utils::readJSON(config::interface::path.string(), data);

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
void utils::loadLevelData(level::LevelData& currentLevelData, json const& JSONLevelData) {
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
    currentLevelData.backgroundColor = (backgroundColor != JSONLevelData.end() ? utils::SDL_ColorFromHexString(backgroundColor.value()) : config::color::offblack);

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
void utils::loadTilesetsData(SDL_Renderer* renderer, tile::TilelayerTilesetData::Collection& tilesetDataCollection, json const& data) {
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
tile::TilelayerTilesetData const* utils::getTilesetData(const tile::TilelayerTilesetData::Collection& tilesetDataCollection, int gid) {
    auto it = std::find_if(tilesetDataCollection.begin(), tilesetDataCollection.end(), [&](const auto tilesetData) {
        return tilesetData.firstGID <= gid && gid < tilesetData.firstGID + tilesetData.srcCount.x * tilesetData.srcCount.y;
    });
    return it != tilesetDataCollection.end() ? &*it : nullptr;
}

/**
 * @brief Set color modulation on the texture of derived class `T`.
*/
void utils::setTextureRGB(SDL_Texture*& texture, SDL_Color const& color) {
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
}

/**
 * @brief Set both color modulation and alpha modulation on the texture of derived class `T`.
 * @param col represents a standard RGBA value.
*/
void utils::setTextureRGBA(SDL_Texture*& texture, SDL_Color const& color) {
    utils::setTextureRGB(texture, color);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, color.a);
}