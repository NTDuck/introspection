#include <auxiliaries.hpp>

#include <algorithm>
#include <filesystem>

#include <SDL.h>


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
void tile::Data_TilelayerTileset::load(json const& tileset, SDL_Renderer* renderer) {
    auto firstGID_ = tileset.find("firstgid");
    if (firstGID_ == tileset.end() || !firstGID_.value().is_number_integer()) return;
    firstGID = firstGID_.value();

    auto src = tileset.find("source");
    if (src == tileset.end() || !src.value().is_string()) return;
    std::filesystem::path xmlPath(src.value());

    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file((config::path::asset_tiled / utils::cleanRelativePath(xmlPath)).c_str());   // All tilesets should be located in "assets/.tiled/"
    if (!result) return;   // Should be replaced with `result.status` or `pugi::xml_parse_status`

    Data_Generic::load(document, renderer);

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

tile::Data_TilelayerTileset const* tile::Data_TilelayerTilesets::operator[](GID gid) const {
    auto it = std::find_if(mData.begin(), mData.end(), [&](const auto tilelayer) {
        return tilelayer.firstGID <= gid && gid < tilelayer.firstGID + tilelayer.srcCount.x * tilelayer.srcCount.y;
    });
    return it != mData.end() ? &*it : nullptr;
}

/**
 * @brief Read data associated with a tileset used for an entity or an animated object from loaded XML data.
 * @note Use `std::strcmp()` instead of `std::string()` in C-string comparison for slight performance gains.
*/
void tile::Data_Entity::initialize(pugi::xml_document& document, SDL_Renderer* renderer) {
    Data_Generic::load(document, renderer);

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
            } else properties.insert(std::make_pair(name.as_string(), value.as_string()));

        } else {
            auto propertytype = propertyNode.attribute("propertytype");
            pugi::xml_node animationsNode = propertyNode.child("properties");
            if (propertytype == nullptr || std::strcmp(propertytype.as_string(), "animation") || animationsNode.empty()) continue;

            auto animationType = Data_Entity::kAnimationTypeConversionMapping.find(name.as_string());
            if (animationType == Data_Entity::kAnimationTypeConversionMapping.end()) continue;

            Data_Entity::Data_Animation animation;

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
                } else if (!std::strcmp(name_.as_string(), "animation-update-rate-multiplier")) {
                    animation.animationUpdateRateMultiplier = value_.as_double();
                }
            }

            // Does this need to be inserted instead?
            animationMapping.emplace(std::make_pair(animationType->second, animation));
        }
    }
}