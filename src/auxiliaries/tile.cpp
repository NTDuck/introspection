#include <auxiliaries.hpp>

#include <algorithm>
#include <filesystem>

#include <SDL.h>


/**
 * @brief Read data associated with a tileset from loaded XML data.
 * @note Also loads the `texture`.
 * @note Requires `document` to be successfully loaded from a XML file.
*/
void tile::Data_Generic::load(pugi::xml_document& XMLTilesetData, SDL_Renderer* renderer) {
    // Parse nodes
    pugi::xml_node tilesetNode = XMLTilesetData.child("tileset");
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
    texture = IMG_LoadTexture(renderer, (config::path::asset / path).string().c_str());   // Should also check whether path exists
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
void tile::Data_Tilelayer::initialize(json const& tileset, SDL_Renderer* renderer) {
    auto firstGID_ = tileset.find("firstgid");
    if (firstGID_ == tileset.end() || !firstGID_.value().is_number_integer()) return;
    firstGID = firstGID_.value();

    auto src = tileset.find("source");
    if (src == tileset.end() || !src.value().is_string()) return;
    std::filesystem::path xmlPath(src.value());
    utils::cleanRelativePath(xmlPath);

    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file((config::path::asset_tiled / xmlPath).c_str());   // All tilesets should be located in "assets/.tiled/"
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