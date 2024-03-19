#include <auxiliaries.hpp>

#include <algorithm>
#include <optional>
#include <filesystem>

#include <SDL.h>


std::string tile::Data_Generic::getProperty(std::string const& key) {
    auto it = properties.find(key);
    return it != properties.end() ? it->second : std::string{};
}

void tile::Data_Generic::setProperty(std::string const& key, std::string const& property) {
    auto it = properties.find(key);
    if (it == properties.end()) properties.insert(std::make_pair(key, property));
    else it->second = property;
}

/**
 * @brief Read data associated with a tileset from loaded XML data.
 * @note Also loads the `texture`.
 * @note Requires `document` to be successfully loaded from a XML file.
*/
void tile::Data_Generic::load(pugi::xml_document const& XMLTilesetData, SDL_Renderer* renderer) {
    // Parse nodes
    auto tileset_n = XMLTilesetData.child("tileset"); if (tileset_n.empty()) return;
    auto image_n = tileset_n.child("image"); if (image_n.empty()) return;

    // Dimensions
    auto srcCountX_a = tileset_n.attribute("columns"); if (srcCountX_a == nullptr) return;
    auto srcCountSum_a = tileset_n.attribute("tilecount"); if (srcCountSum_a == nullptr) return;
    auto srcSizeX_a = tileset_n.attribute("tilewidth"); if (srcSizeX_a == nullptr) return;
    auto srcSizeY_a = tileset_n.attribute("tileheight"); if (srcSizeY_a == nullptr) return;

    srcCount = {
        srcCountX_a.as_int(),
        srcCountSum_a.as_int() / srcCountX_a.as_int(),
    };
    srcSize = {
        srcSizeX_a.as_int(),
        srcSizeY_a.as_int(),
    };

    // Texture
    auto source_a = image_n.attribute("source"); if (source_a == nullptr) return;

    std::filesystem::path path(source_a.as_string());
    texture = IMG_LoadTexture(renderer, (config::path::asset / utils::cleanRelativePath(path)).string().c_str());   // Should also check whether path exists
}

void tile::Data_Generic::clear() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

/**
 * @brief Read data associated with a tilelayer tileset from loaded JSON data.
 * @note Also loads the `texture` and populate `firstGID`.
 * @note `firstGID` is contained only in Tiled Map JSON files.
*/
void tile::Data_TilelayerTileset::load(json const& JSONTileLayerData, SDL_Renderer* renderer) {
    auto firstGID_j = JSONTileLayerData.find("firstgid"); if (firstGID_j == JSONTileLayerData.end()) return;
    auto firstGID_v = firstGID_j.value(); if (!firstGID_v.is_number_integer()) return;
    firstGID = firstGID_j.value();

    auto source_j = JSONTileLayerData.find("source"); if (source_j == JSONTileLayerData.end()) return;
    auto source_v = source_j.value(); if (!source_v.is_string()) return;
    std::filesystem::path xmlPath(source_j.value());

    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file((config::path::asset_tiled / utils::cleanRelativePath(xmlPath)).c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

    Data_Generic::load(document, renderer);

    // Properties
    auto tileset_n = document.child("tileset");
    auto properties_n = tileset_n.child("properties"); if (properties_n.empty()) return;

    for (auto property_n = properties_n.child("property"); property_n; property_n = property_n.next_sibling("property")) {
        if (property_n.empty()) continue;

        auto name_a = property_n.attribute("name"); if (name_a == nullptr) continue;
        auto type_a = property_n.attribute("type");

        if (type_a == nullptr || std::strcmp(type_a.as_string(), "class")) {
            auto value_a = property_n.attribute("value");
            if (value_a == nullptr) continue;
            setProperty(name_a.as_string(), value_a.as_string());
        }
    }
}

void tile::Data_TilelayerTilesets::load(json const& JSONLevelData, SDL_Renderer* renderer) {
    for (auto& tilelayer : mData) tilelayer.clear();   // Necessary?
    mData.clear();

    auto tilesets_j = JSONLevelData.find("tilesets"); if (tilesets_j == JSONLevelData.end()) return;
    auto tilesets_v = tilesets_j.value(); if (!tilesets_v.is_array()) return;

    for (const auto& tileset : tilesets_v) {
        tile::Data_TilelayerTileset tilelayer;
        tilelayer.load(tileset, renderer);
        mData.emplace_back(tilelayer);
    }
}

std::optional<tile::Data_TilelayerTileset> tile::Data_TilelayerTilesets::operator[](GID gid) const {
    // auto it = std::find_if(mData.begin(), mData.end(), [&](const auto& tilelayer) {
    //     return tilelayer.firstGID <= gid && gid < tilelayer.firstGID + tilelayer.srcCount.x * tilelayer.srcCount.y;
    // });
    // return it != mData.end() ? std::optional<Data_TilelayerTileset>(*it) : std::nullopt;

    auto it = std::lower_bound(mData.begin(), mData.end(), gid, [](const Data_TilelayerTileset& tilelayer, GID gid) {
        return tilelayer.firstGID + tilelayer.srcCount.x * tilelayer.srcCount.y <= gid;   // First `tilelayer` whose `firstGID` is not less than `gid`
    });   // Use `std::lower_bound` (O(log n)) in lieu of `std::find_if` (O(n)) since `mData` is sorted by `firstGID`

    if (it != mData.begin() && (it == mData.end() || it->firstGID > gid)) --it;   // Adjust flaw-susceptible search result so that condition `tilelayer.firstGID <= gid` is met

    // if (it != mData.end() && it->firstGID <= gid && gid < it->firstGID + it->srcCount.x * it->srcCount.y) return std::optional<Data_TilelayerTileset>(*it);
    // else return std::nullopt;
    return it != mData.end() ? std::optional<Data_TilelayerTileset>(*it) : std::nullopt;
}

/**
 * @note Expected input format: "animation-`animation_repr`=`direction_repr`" (for multi-directional tilesets), otherwise "animation-`animation_repr`".
*/
std::pair<std::optional<tile::Data_EntityTileset::Animation>, std::optional<SDL_Point>> tile::Data_EntityTileset::stoan(std::string const& s) {
    static auto parse = [](std::string const& input, std::string& animation_repr, char& direction_repr) {
        auto pos = input.rfind('=');
        if (pos != std::string::npos) {
            animation_repr = input.substr(0, pos);
            direction_repr = input.at(++pos);
        } else {
            animation_repr = input;
            direction_repr = '\0';
        }
    };

    // Extract embedded data
    std::string animation_repr;
    char direction_repr;
    parse(s, animation_repr, direction_repr);

    // Process extracted data
    std::pair<std::optional<Animation>, std::optional<SDL_Point>> data;

    // Register direction
    switch (direction_repr) {
        case 'n':
            data.second = std::make_optional<SDL_Point>({ 0, -1 }); 
            break;
        case 's':
            data.second = std::make_optional<SDL_Point>({ 0, 1 }); 
            break;
        case 'w':
            data.second = std::make_optional<SDL_Point>({ -1, 0 }); 
            break;
        case 'e':
            data.second = std::make_optional<SDL_Point>({ 1, 0 }); 
            break;

        default:
            data.second = std::nullopt;
    }

    // Register animation
    static const std::unordered_map<std::string, Animation> ump = {
        { "animation-idle", Animation::kIdle },
        { "animation-attack-meele", Animation::kAttackMeele },
        { "animation-attack-ranged", Animation::kAttackRanged },
        { "animation-death", Animation::kDeath },
        { "animation-run", Animation::kRun },
        { "animation-walk", Animation::kWalk },
        { "animation-damaged", Animation::kDamaged },
    };
    auto it = ump.find(animation_repr);
    if (it == ump.end()) return std::make_pair(std::nullopt, std::nullopt);

    data.first = std::make_optional(it->second);
    return data;
}

void tile::Data_EntityTileset::Data_Animation::load(pugi::xml_node const& XMLAnimationNode) {
    for (auto animation_n = XMLAnimationNode.child("property"); animation_n; animation_n = animation_n.next_sibling("property")) {
        if (animation_n.empty()) continue;

        auto name_a = animation_n.attribute("name"); if (name_a == nullptr) continue;
        auto value_a = animation_n.attribute("value"); if (value_a == nullptr) continue;
        
        std::string name_v = name_a.as_string();
        switch (hs(name_v.c_str())) {
            case hs("startGID"):
                startGID = value_a.as_int();
                break;
            case hs("stopGID"):
                stopGID = value_a.as_int();
                break;
            case hs("animation-update-rate-multiplier"):
                updateRateMultiplier = value_a.as_double();
                break;
            default: break;
        }
    }
}

/**
 * @brief Read data associated with a tileset used for an entity or an animated object from loaded XML data.
 * @note Use `std::strcmp()` instead of `std::string()` in C-string comparison for slight performance gains.
*/
void tile::Data_EntityTileset::load(pugi::xml_document const& XMLTilesetData, SDL_Renderer* renderer) {
    Data_Generic::load(XMLTilesetData, renderer);

    auto tileset_n = XMLTilesetData.child("tileset"); if (tileset_n.empty()) return;
    auto properties_n = tileset_n.child("properties"); if (properties_n.empty()) return;

    for (auto property_n = properties_n.child("property"); property_n; property_n = property_n.next_sibling("property")) {
        if (property_n.empty()) continue;

        auto name_a = property_n.attribute("name"); if (name_a == nullptr) continue;
        auto type_a = property_n.attribute("type");
        
        if (type_a == nullptr || std::strcmp(type_a.as_string(), "class")) {
            auto value_a = property_n.attribute("value"); if (value_a == nullptr) continue;
            std::string name_v = name_a.as_string();

            switch (hs(name_v.c_str())) {
                case hs("animation-update-rate"):
                    animationUpdateRate = value_a.as_int();
                    break;
                case hs("animation-width"):
                    animationSize.x = value_a.as_int();
                    break;
                case hs("animation-height"):
                    animationSize.y = value_a.as_int();
                    break;
                case hs("multidirectional"):
                    isMultiDirectional = value_a.as_bool();
                    break;
                default:
                    setProperty(name_v, value_a.as_string());
            }
        } else {
            auto propertytype_a = property_n.attribute("propertytype"); if (propertytype_a == nullptr || std::strcmp(propertytype_a.as_string(), "animation")) continue;
            auto animations_n = property_n.child("properties"); if (animations_n.empty()) continue;

            auto pair = stoan(name_a.as_string()); if (!pair.first.has_value()) continue;   // Note that `isMultidirectional` is not correctly defined at the point of parsing

            Data_Animation data;
            data.load(animations_n);

            mUMap.insert(std::make_pair(std::make_pair(pair.first.value(), pair.second.value_or(kDefaultDirection)), data));
        }
    }
}

tile::Data_EntityTileset::Data_Animation const& tile::Data_EntityTileset::at(Animation animation, SDL_Point const& direction) const {
    static Data_Animation nullopt_repr{};
    auto it = mUMap.find(std::make_pair(animation, direction));
    return it != mUMap.end() ? it->second : nullopt_repr;    
}

std::size_t tile::Data_EntityTileset::hash::operator()(std::pair<Animation, SDL_Point> const& instance) const {
    return std::hash<char>{}(static_cast<char>(instance.first)) ^ std::hash<SDL_Point>{}(instance.second);
}

bool tile::Data_EntityTileset::equal_to::operator()(std::pair<Animation, SDL_Point> const& first, std::pair<Animation, SDL_Point> const& second) const {
    return first.first == second.first && first.second == second.second;
}