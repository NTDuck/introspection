#include <auxiliaries.hpp>

#include <algorithm>
#include <array>
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
    return { first.x + second.x, first.y + second.y };
}

SDL_Point operator-(SDL_Point const& first, SDL_Point const& second) {
    return { first.x - second.x, first.y - second.y };
}

SDL_Point operator-(SDL_Point const& instance) {
    return { -instance.x, -instance.y };
}

SDL_Point operator~(SDL_Point const& instance) {
    return { instance.y, instance.x };
}

bool operator==(SDL_FPoint const& first, SDL_FPoint const& second) {
    return first.x == second.x && first.y == second.y;
}

/**
 * @brief Rotate a floating point by the specified angle, counterclockwise.
 * @note Behold, linear algebra! Use `<cmath.h>`'s `M_PI` or `std::acos(-1)`.
 * @see https://en.wikipedia.org/wiki/Rotation_matrix
 * @see https://www.modernescpp.com/index.php/c-core-guidelines-rules-for-conversions-and-casts/
*/
SDL_FPoint operator<<(SDL_FPoint const& instance, float rad) {
    return {
        instance.x * std::cos(rad) - instance.y * std::sin(rad),
        instance.x * std::sin(rad) + instance.y * std::cos(rad),
    };
}

SDL_FPoint operator<<(SDL_Point const& instance, float rad) {
    return {
        instance.x * std::cos(rad) - instance.y * std::sin(rad),
        instance.x * std::sin(rad) + instance.y * std::cos(rad),
    };    
}

/**
 * @brief Rotate a floating point by the specified angle, clockwise.
*/
SDL_FPoint operator>>(SDL_FPoint const& instance, float rad) {
    return instance << -rad;
}

SDL_FPoint operator>>(SDL_Point const& instance, float rad) {
    return instance << -rad;
}

std::size_t std::hash<SDL_Point>::operator()(SDL_Point const& instance) const {
    return std::hash<int>{}(instance.x) ^ (std::hash<int>{}(instance.y) << 1);
}

std::size_t std::hash<SDL_FPoint>::operator()(SDL_FPoint const& instance) const {
    return std::hash<float>{}(instance.x) ^ (std::hash<float>{}(instance.y) << 1);
}


template <typename K, typename V>
utils::LRUCache<K, V>::LRUCache(std::size_t size) : kSize(size) {}

template <typename K, typename V>
std::optional<V> utils::LRUCache<K, V>::at(K const& k) {
    auto it = mHashmap.find(k);
    if (it == mHashmap.end()) return std::nullopt;

    // Move to front of deque
    push_front_impl(it);

    return std::optional<V>(mHashmap[k].value);
}

template <typename K, typename V>
void utils::LRUCache<K, V>::insert(K const& k, V const& v) {
    auto it = mHashmap.find(k);

    if (it != mHashmap.end()) {
        it->second.value = v;   // Update associated value
        push_front_impl(it);   // Move to front of queue
    } else {
        if (mDeque.size() == kSize) {   // Cache is full
            // Evict LRU item (at the back of queue)
            K lru = mDeque.back();
            mDeque.pop_back();
            mHashmap.erase(lru);   // Remove corresponding entry from the hashmap
        }
        mDeque.push_front(k);   // Insert item to the front of queue
        mHashmap.insert(std::make_pair(k, U{ v, mDeque.begin() }));   // Register hashmap entry
    }
}

template <typename K, typename V>
void utils::LRUCache<K, V>::clear() {
    mHashmap.clear();
    mDeque.clear();
}

template <typename K, typename V>
void utils::LRUCache<K, V>::push_front_impl(typename std::unordered_map<K, U>::iterator it) {
    mDeque.erase(it->second.iterator);
    mDeque.push_front(it->first);
    it->second.iterator = mDeque.begin();
}


template class utils::LRUCache<tile::GID, tile::Data_TilelayerTileset>;


/**
 * @brief Convert a `float` to type `int`. Achieve a similar effect to `std::floor`.
 * @note Susceptible to data loss.
*/
int utils::ftoi(const float f) { return static_cast<int>(std::lroundf(f)); }

/**
 * @brief Stringify a `double` with specified precision i.e. digits after the decimal point.
*/
std::string utils::dtos(const double d, unsigned int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << d;
    return oss.str();
}

SDL_Point utils::fpttopt(SDL_FPoint const& fpt) {
    return { utils::ftoi(fpt.x), utils::ftoi(fpt.y) };
}

/**
 * @brief Convert a string representing a hex color value to `SDL_Color`.
*/
SDL_Color utils::hextocol(std::string const& hexString) {
    uint32_t ARGB = std::stoul(hexString.substr(1), nullptr, 16);   // Base 16
    SDL_Color color;

    // Isolate each component (8 bits) then mask out redundancies (via bitwise AND, to ensure valid range 0 - 255)
    color.a = (ARGB >> 24) & 0xff;
    color.r = (ARGB >> 16) & 0xff;
    color.g = (ARGB >> 8) & 0xff;
    color.b = ARGB & 0xff;

    return color;
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
 * @brief Allow `SDL_Color` to be passed into `SDL_SetRendererDrawColor()` instead of `uint8_t`.
*/
void utils::setRendererDrawColor(SDL_Renderer* renderer, SDL_Color const& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/**
 * @see https://stackoverflow.com/questions/75873908/how-to-copy-a-texture-to-another-texture-without-pointing-to-the-same-texture
*/
SDL_Texture* utils::duplicateTexture(SDL_Renderer* renderer, SDL_Texture* texture) {
    uint32_t format;
    SDL_Point size;
    SDL_BlendMode blendMode;
    SDL_Texture* cachedRenderTarget = nullptr;
    SDL_Texture* duplicatedTexture = nullptr;

    // Get all properties from the texture we are duplicating
    SDL_QueryTexture(texture, &format, nullptr, &size.x, &size.y);
    SDL_GetTextureBlendMode(texture, &blendMode);

    // Save the current rendering target (will be NULL if it is the current window)
    cachedRenderTarget = SDL_GetRenderTarget(renderer);

    // Create a new texture with the same properties as the one we are duplicating
    duplicatedTexture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, size.x, size.y);

    // Set its blending mode and make it the render target
    SDL_SetTextureBlendMode(duplicatedTexture, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, duplicatedTexture);

    // Render the full original texture onto the new one
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Change the blending mode of the new texture to the same as the original one
    SDL_SetTextureBlendMode(duplicatedTexture, blendMode);

    // Restore the render target
    SDL_SetRenderTarget(renderer, cachedRenderTarget);

    // Return the new texture
    return duplicatedTexture;
}

/**
 * @brief Convert a texture to grayscale.
 * @see https://gigi.nullneuron.net/gigilabs/converting-an-image-to-grayscale-using-sdl2/
 * @see https://en.wikipedia.org/wiki/Grayscale
*/
SDL_Texture* utils::createGrayscaleTexture(SDL_Renderer* renderer, SDL_Texture* texture, double intensity) {
    constexpr auto grayscale = [](SDL_Color const& color) {
        return static_cast<uint8_t>(0.212671f * color.r + 0.715160f * color.g + 0.072169f * color.b);
    };

    if (intensity <= 0 || texture == nullptr) return texture;
    if (intensity > 1) intensity = 1;

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

            uint8_t grayscaledPixel = grayscale(color);   // Create grayscaled pixel (100% intensity) from extracted color
            color.r = (1 - intensity) * color.r + intensity * grayscaledPixel;
            color.g = (1 - intensity) * color.g + intensity * grayscaledPixel;
            color.b = (1 - intensity) * color.b + intensity * grayscaledPixel;

            pixel = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);   // Register changes
        }
    }

    // Create new texture from grayscaled surface
    SDL_Texture* grayscaledTexture = SDL_CreateTextureFromSurface(renderer, surface);

    // Clean up
    SDL_FreeSurface(surface);

    return grayscaledTexture;
}

/**
 * @brief Set color modulation on the texture of derived class `T`.
*/
void utils::setTextureRGB(SDL_Texture* texture, SDL_Color const& color) {
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
}

/**
 * @brief Set both color modulation and alpha modulation on the texture of derived class `T`.
 * @param col represents a standard RGBA value.
*/
void utils::setTextureRGBA(SDL_Texture* texture, SDL_Color const& color) {
    utils::setTextureRGB(texture, color);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, color.a);
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

template std::vector<int> utils::zlibDecompress<int>(std::string const& s);

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
void utils::fetch(std::filesystem::path const& path, json& data) {
    std::ifstream file;
    file.open(path);
    if (!file.is_open()) return;

    data = json::parse(file);
    file.close();
}

/**
 * @brief Remove leading dots (`.`) and slashes (`/` `\`) in a `std::filesystem::path`.
 * @note Fall back to string manipulation since `std::filesystem` methods (`canonical()`, `lexically_normal()`, etc.) fails inexplicably.
*/
std::filesystem::path utils::cleanRelativePath(std::filesystem::path const& path) {
    static constexpr const char* junk = "../";
    static constexpr const int junk_size = 3;

    std::string repr = path.string();

    size_t found = repr.find(junk);
    while (found != std::string::npos) {
        repr = repr.substr(found + junk_size);
        found = repr.find(junk);
    }

    return repr;   // Implicit
}