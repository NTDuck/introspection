#include <auxiliaries.hpp>


level::Name level::stoln(std::string const& s) {
    static const std::unordered_map<std::string, level::Name> ump = {
        { "level-equilibrium", level::Name::kLevelEquilibrium },
        { "level-valley-of-despair", level::Name::kLevelValleyOfDespair },
        { "level-white-space", level::Name::kLevelWhiteSpace },
    };
    auto it = ump.find(s);
    return it != ump.end() ? it->second : Name::null;
}

void level::Map::load(json const& JSONLevelMapData) {
    auto levels_j = JSONLevelMapData.find("levels"); if (levels_j == JSONLevelMapData.end()) return;
    auto levels_v = levels_j.value(); if (!levels_v.is_array()) return;

    for (const auto& level : levels_v) {
        if (!level.is_object()) continue;

        auto name_j = level.find("name"); if (name_j == level.end()) continue;
        auto source_j = level.find("source"); if (source_j == level.end()) continue;
        auto name_v = name_j.value(); if (!name_v.is_string()) continue;
        auto source_v = source_j.value(); if (!source_v.is_string()) continue;

        Name ln = stoln(name_v);
        if (ln == Name::null) continue;

        mUMap.insert(std::make_pair(ln, source_v));
    }
}

std::filesystem::path level::Map::operator[](Name ln) const {
    static const std::filesystem::path root = config::path::asset_tiled;

    auto it = mUMap.find(ln);
    return it != mUMap.end() ? root / it->second : std::filesystem::path{};
}

/**
 * @brief An alternative to the constructor. Populate data members based on JSON data.
 * @note Requires JSON data to be in proper format before being called.
*/
void level::Data_Generic::load(json const& JSONObjectData) {
    auto destCoordsX_j = JSONObjectData.find("x"); if (destCoordsX_j == JSONObjectData.end()) return;
    auto destCoordsY_j = JSONObjectData.find("y"); if (destCoordsY_j == JSONObjectData.end()) return;
    auto destSizeWidth_j = JSONObjectData.find("width"); if (destSizeWidth_j == JSONObjectData.end()) return;
    auto destSizeHeight_j = JSONObjectData.find("height"); if (destSizeHeight_j == JSONObjectData.end()) return;

    // Also check for division by zero
    auto destCoordsX_v = destCoordsX_j.value(); if (!destCoordsX_v.is_number_integer()) return;
    auto destCoordsY_v = destCoordsY_j.value(); if (!destCoordsY_v.is_number_integer()) return;
    auto destSizeWidth_v = destSizeWidth_j.value(); if (!destSizeWidth_v.is_number_integer()) return;
    auto destSizeHeight_v = destSizeHeight_j.value(); if (!destSizeHeight_v.is_number_integer()) return;

    destCoords = {
        static_cast<int>(destCoordsX_v) / static_cast<int>(destSizeWidth_v),
        static_cast<int>(destCoordsY_v) / static_cast<int>(destSizeHeight_v),
    };
}

void level::Data_Teleporter::load(json const& JSONObjectData) {
    Data_Generic::load(JSONObjectData);

    auto properties_j = JSONObjectData.find("properties"); if (properties_j == JSONObjectData.end()) return;
    auto properties_v = properties_j.value(); if (!properties_v.is_array()) return;

    for (const auto& property : properties_v) {
        auto name_j = property.find("name"); if (name_j == property.end()) continue;
        auto value_j = property.find("value"); if (value_j == property.end()) continue;
        auto name_v = name_j.value(); if (!name_v.is_string()) continue;
        auto value_v = value_j.value();

        switch (hs(static_cast<std::string>(name_v).c_str())) {
            case hs("target-dest-coords"): {
                if (!value_v.is_object()) break;

                auto targetDestCoordsX_j = value_v.find("x"); if (targetDestCoordsX_j == value_v.end()) break;
                auto targetDestCoordsY_j = value_v.find("y"); if (targetDestCoordsY_j == value_v.end()) break;
                auto targetDestCoordsX_v = targetDestCoordsX_j.value(); if (!targetDestCoordsX_v.is_number_integer()) break;
                auto targetDestCoordsY_v = targetDestCoordsY_j.value(); if (!targetDestCoordsY_v.is_number_integer()) break;

                targetDestCoords = { targetDestCoordsX_v, targetDestCoordsY_v };
                break; }

            case hs("target-level"): {
                if (!value_v.is_string()) break;

                level::Name ln = level::stoln(value_v);
                if (ln != Name::null) targetLevel = ln;
                break; }

            default: break;
        }
    }
}

std::vector<level::Data_Generic*> level::Data::get(std::string const& key) {
    auto it = dependencies.find(key);
    return it != dependencies.end() ? it->second : std::vector<level::Data_Generic*>{};
}

void level::Data::insert(std::string const& key, Data_Generic* data) {
    auto it = dependencies.find(key);
    if (it == dependencies.end()) dependencies.insert(std::make_pair(key, std::vector<Data_Generic*>{ data }));
    else it->second.push_back(data);
}

void level::Data::erase(std::string const& key) {
    auto it = dependencies.find(key);
    if (it == dependencies.end()) return;
    for (auto& p : it->second) delete p;
    dependencies.erase(it);
}

std::string level::Data::getProperty(std::string const& key) {
    auto it = properties.find(key);
    return it != properties.end() ? it->second : std::string{};
}

void level::Data::setProperty(std::string const& key, std::string const& property) {
    auto it = properties.find(key);
    if (it == properties.end()) properties.insert(std::make_pair(key, property));
    else it->second = property;
}

void level::Data::eraseProperty(std::string const& key) {
    auto it = properties.find(key);
    if (it == properties.end()) return;
    properties.erase(it);
}

void level::Data::load(json const& JSONLevelData) {
    clear();   // Prevent undefined behaviour
    loadMembers(JSONLevelData);

    auto layers_j = JSONLevelData.find("layers"); if (layers_j == JSONLevelData.end()) return;
    auto layers_v = layers_j.value(); if (!layers_v.is_array()) return;

    for (const auto& layer : layers_v) {
        auto type_j = layer.find("type"); if (type_j == layer.end()) continue;
        auto type_v = type_j.value(); if (!type_v.is_string()) continue;

        switch (hs(static_cast<std::string>(type_v).c_str())) {
            case hs("tilelayer"): loadTileLayer(layer); break;
            case hs("objectgroup"): loadObjectLayer(layer); break;
            default: break;
        }
    }

    loadTilelayerTilesets(JSONLevelData);
}

void level::Data::loadMembers(json const& JSONLevelData) {
    auto tileDestCountWidth_j = JSONLevelData.find("width"); if (tileDestCountWidth_j == JSONLevelData.end()) return;
    auto tileDestCountWidth_v = tileDestCountWidth_j.value(); if (!tileDestCountWidth_v.is_number_integer()) return;
    auto tileDestCountHeight_j = JSONLevelData.find("height"); if (tileDestCountHeight_j == JSONLevelData.end()) return;
    auto tileDestCountHeight_v = tileDestCountHeight_j.value(); if (!tileDestCountHeight_v.is_number_integer()) return;
    tileDestCount = { tileDestCountWidth_v, tileDestCountHeight_v };

    tiles.resize(tileDestCount.y);
    for (auto& row : tiles) row.resize(tileDestCount.x);

    auto tileDestSizeWidth_j = JSONLevelData.find("width"); if (tileDestSizeWidth_j == JSONLevelData.end()) return;
    auto tileDestSizeWidth_v = tileDestSizeWidth_j.value(); if (!tileDestSizeWidth_v.is_number_integer()) return;
    auto tileDestSizeHeight_j = JSONLevelData.find("height"); if (tileDestSizeHeight_j == JSONLevelData.end()) return;
    auto tileDestSizeHeight_v = tileDestSizeHeight_j.value(); if (!tileDestSizeHeight_v.is_number_integer()) return;
    tileDestSize = { tileDestSizeWidth_v, tileDestSizeHeight_v };

    auto backgroundColor_j = JSONLevelData.find("backgroundcolor"); if (backgroundColor_j == JSONLevelData.end()) return;
    auto backgroundColor_v = backgroundColor_j.value(); if (!backgroundColor_v.is_string()) return;
    backgroundColor = utils::SDL_ColorFromHexString(backgroundColor_v);
}

/**
 * @brief Load GIDs of a layer.
*/
void level::Data::loadTileLayer(json const& JSONLayerData) {
    auto layer_j = JSONLayerData.find("data"); if (layer_j == JSONLayerData.end()) return;
    auto layer_v = layer_j.value(); if (!(layer_v.is_string() || layer_v.is_array())) return;

    tile::Slice GIDs;
    auto encoding_j = JSONLayerData.find("encoding");
    auto compression_j = JSONLayerData.find("compression");

    if ((encoding_j == JSONLayerData.end() || encoding_j.value() == "csv") && compression_j == JSONLayerData.end()) {   // csv
        for (const auto& GID : layer_j.value()) GIDs.emplace_back(GID);
    } else if (encoding_j != JSONLayerData.end() && encoding_j.value() == "base64") {
        std::string decoded = utils::base64Decode(layer_j.value());
        if (compression_j.value() == "zlib") {   // zlib-compressed base64
            GIDs = utils::zlibDecompress<int>(decoded);
        } else return;
    } else return;

    for (int y = 0; y < tileDestCount.y; ++y) for (int x = 0; x < tileDestCount.x; ++x) tiles[y][x].emplace_back(GIDs[y * tileDestCount.x + x]);
}

/**
 * @brief Load entity info.
*/
void level::Data::loadObjectLayer(json const& JSONLayerData) {
    auto objects_j = JSONLayerData.find("objects"); if (objects_j == JSONLayerData.end()) return;
    auto objects_v = objects_j.value(); if (!objects_v.is_array()) return;

    for (const auto& object : objects_v) {
        auto type_j = object.find("type"); if (type_j == object.end()) continue;
        auto type_v = type_j.value(); if (!type_v.is_string()) continue;

        Data_Generic* data = nullptr;

        switch (hs(static_cast<std::string>(type_v).c_str())) {
            case hs(config::entities::teleporter::typeID):
            case hs(config::entities::teleporter_red_hand_throne::typeID):
                data = new Data_Teleporter();
                break;

            default:
                data = new Data_Generic();   // Assumes that all, unless specified, uses `Data_Generic`
        }

        data->load(object);
        insert(type_v, data);
    }
}

void level::Data::loadTilelayerTilesets(json const& JSONLevelData) {
    tilesets.load(JSONLevelData, globals::renderer);
}

/**
 * @note When entry `key` is removed via `erase(key)`, iterators pointing to next entries are invalidated i.e. undefined behaviour with `for (auto& pair : dependencies) erase(pair.first);`
*/
void level::Data::clear() {
    for (auto& row : tiles) for (auto& tile : row) tile.clear();
    backgroundColor = config::color::offblack;   // Default

    for (auto& pair : dependencies) for (auto& p : pair.second) delete p;
    dependencies.clear();
    properties.clear();
}


level::Data level::data;