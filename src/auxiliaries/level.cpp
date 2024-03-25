#include <auxiliaries.hpp>

#include <string>
#include <type_traits>


/**
 * @see https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
*/
std::optional<level::Name> level::stoln(std::string const& s) {
    #define IMPL(ln) \
    case static_cast<unsigned int>(ln):\
        return std::make_optional<Name>(ln);

    switch (hstr(s.c_str())) {
        IMPL(Name::kLevelWhiteSpace)
        IMPL(Name::kLevelBegin)
        IMPL(Name::kLevelForest_0)
        IMPL(Name::kLevelForest_1)
        IMPL(Name::kLevelForest_2)
        IMPL(Name::kLevelForest_3)
        IMPL(Name::kLevelForest_4)
        IMPL(Name::kLevelDeprecatedTutorial_0)
        IMPL(Name::kLevelDeprecatedTutorial_1)
        IMPL(Name::kLevelAnte)
        IMPL(Name::kLevelPaene)
        IMPL(Name::kLevelUmbra)
        IMPL(Name::kLevelBreakroomInitial)
        IMPL(Name::kLevelBedroom)

        default: return std::nullopt;
    }
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

        auto ln = stoln(name_v);
        if (!ln.has_value()) continue;

        mUMap.insert(std::make_pair(ln.value(), source_v));
    }
}

std::optional<std::filesystem::path> level::Map::operator[](Name ln) const {
    static const std::filesystem::path root = config::path::asset_tiled;

    auto it = mUMap.find(ln);
    return it != mUMap.end() ? std::make_optional<std::filesystem::path>(root / it->second) : std::nullopt;
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

void level::Data_Interactable::load(json const& JSONObjectData) {
    Data_Generic::load(JSONObjectData);

    auto properties_j = JSONObjectData.find("properties"); if (properties_j == JSONObjectData.end()) return;
    auto properties_v = properties_j.value(); if (!properties_v.is_array()) return;

    for (const auto& property : properties_v) {
        auto name_j = property.find("name"); if (name_j == property.end()) continue;
        auto value_j = property.find("value"); if (value_j == property.end()) continue;

        auto name_v = name_j.value(); if (!name_v.is_string() || static_cast<std::string>(name_v).find("dialogue")) continue;   // != 0
        // propertytype: "dialogue"
        auto value_v = value_j.value(); if (!value_v.is_object()) continue;

        auto content_j = value_v.find("content"); if (content_j == value_v.end()) continue;
        auto group_index_j = value_v.find("group-index"); if (group_index_j == value_v.end()) continue;
        auto index_j = value_v.find("index"); if (index_j == value_v.end()) continue;

        auto content_v = content_j.value(); if (!content_v.is_string()) continue;
        auto group_index_v = group_index_j.value();
        auto index_v = index_j.value();

        const unsigned short int group_index = static_cast<unsigned short int>(group_index_v);
        const unsigned short int index = static_cast<unsigned short int>(index_v);

        // Prevent segmentation fault
        if (group_index > static_cast<unsigned short int>(dialogues.size()) - 1) dialogues.resize(group_index + 1);
        if (index > static_cast<unsigned short int>(dialogues[group_index].size()) - 1) dialogues[group_index].resize(index + 1);

        dialogues[group_index][index] = content_v;
    }
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

        switch (hstr(static_cast<std::string>(name_v).c_str())) {
            // case hstr("target-dest-coords"): {
            //     if (!value_v.is_object()) break;

            //     auto targetDestCoordsX_j = value_v.find("x"); if (targetDestCoordsX_j == value_v.end()) break;
            //     auto targetDestCoordsY_j = value_v.find("y"); if (targetDestCoordsY_j == value_v.end()) break;
            //     auto targetDestCoordsX_v = targetDestCoordsX_j.value(); if (!targetDestCoordsX_v.is_number_integer()) break;
            //     auto targetDestCoordsY_v = targetDestCoordsY_j.value(); if (!targetDestCoordsY_v.is_number_integer()) break;

            //     targetDestCoords = { targetDestCoordsX_v, targetDestCoordsY_v };
            //     break; }

            case hstr("target-dest-coords-x"):
                if (!value_v.is_number_integer()) break;
                targetDestCoords.x = value_v;
                break;

            case hstr("target-dest-coords-y"):
                if (!value_v.is_number_integer()) break;
                targetDestCoords.y = value_v;
                break;

            case hstr("target-level"): {
                if (!value_v.is_string()) break;

                auto ln = level::stoln(value_v);
                if (ln.has_value()) targetLevel = ln.value();
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

template <>
std::string level::Data::getProperty<std::string>(std::string const& key) {
    auto it = properties.find(key);
    return it != properties.end() ? it->second : std::string{};
}

template <>
int level::Data::getProperty<int>(std::string const& key) {
    return std::stoi(getProperty<std::string>(key));
}

template <>
double level::Data::getProperty<double>(std::string const& key) {
    return std::stod(getProperty<std::string>(key));
}

template <>
float level::Data::getProperty<float>(std::string const& key) {
    return std::stof(getProperty<std::string>(key));
}

template <>
long level::Data::getProperty<long>(std::string const& key) {
    return std::stol(getProperty<std::string>(key));
}

template <>
long long level::Data::getProperty<long long>(std::string const& key) {
    return std::stoll(getProperty<std::string>(key));
}

template <>
bool level::Data::getProperty<bool>(std::string const& key) {
    switch (hstr(getProperty<std::string>(key).c_str())) {
        case hstr("true"):
        case hstr("1"):
            return true;

        default: return false;
    }
}

template <typename T>
void level::Data::setProperty(std::string const& key, T const& property) {
    if constexpr(std::is_same_v<T, std::string>) properties[key] = property;
    else properties[key] = std::to_string(property);
}

template void level::Data::setProperty<bool>(std::string const&, bool const&);
template void level::Data::setProperty<int>(std::string const&, int const&);

void level::Data::eraseProperty(std::string const& key) {
    auto it = properties.find(key);
    if (it == properties.end()) return;
    properties.erase(it);
}

void level::Data::load(json const& JSONLevelData) {
    clear();   // Prevent undefined behaviour

    loadProperties(JSONLevelData);
    loadLayers(JSONLevelData);
    loadTilelayerTilesets(JSONLevelData);
}

void level::Data::loadProperties(json const& JSONLevelData) {
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

    auto backgroundColor_j = JSONLevelData.find("backgroundcolor");
    if (backgroundColor_j != JSONLevelData.end()) {
        auto backgroundColor_v = backgroundColor_j.value();
        if (backgroundColor_v.is_string()) backgroundColor = utils::hextocol(backgroundColor_v);
    }

    auto properties_j = JSONLevelData.find("properties"); if (properties_j == JSONLevelData.end()) return;
    auto properties_v = properties_j.value();

    for (const auto& property : properties_v) {
        auto name_j = property.find("name"); if (name_j == property.end()) continue;
        auto name_v = name_j.value(); if (!name_v.is_string()) continue;
        auto value_j = property.find("value"); if (value_j == property.end()) continue;
        auto value_v = value_j.value();

        switch (hstr(static_cast<std::string>(name_v).c_str())) {
            case hstr("viewport-height"):
                viewportHeight = value_v;
                break;

            default:
                setProperty(std::string(name_v), std::string(value_v));
        }
    }
}

void level::Data::loadLayers(json const& JSONLevelData) {
    auto layers_j = JSONLevelData.find("layers"); if (layers_j == JSONLevelData.end()) return;
    auto layers_v = layers_j.value(); if (!layers_v.is_array()) return;

    for (const auto& layer : layers_v) {
        auto type_j = layer.find("type"); if (type_j == layer.end()) continue;
        auto type_v = type_j.value(); if (!type_v.is_string()) continue;

        switch (hstr(static_cast<std::string>(type_v).c_str())) {
            case hstr("tilelayer"): loadTileLayer(layer); break;
            case hstr("objectgroup"): loadObjectLayer(layer); break;
            default: break;
        }
    }
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
        if (compression_j.value() == "zlib") GIDs = utils::zlibDecompress<int>(decoded);   // zlib-compressed base64
        else return;
    } else return;

    for (int y = 0; y < tileDestCount.y; ++y) for (int x = 0; x < tileDestCount.x; ++x) tiles[y][x].emplace_back(GIDs[y * tileDestCount.x + x]);

    // Collision layer
    auto name_j = JSONLayerData.find("name"); if (name_j == JSONLayerData.end()) return;
    auto name_v = name_j.value(); if (!name_v.is_string()) return;

    collisionTilelayer.resize(tileDestCount.y);
    for (auto& row : collisionTilelayer) row.resize(tileDestCount.x, 0);

    if (name_v == "static-collision") {
        for (int y = 0; y < tileDestCount.y; ++y) for (int x = 0; x < tileDestCount.x; ++x) collisionTilelayer[y][x] = GIDs[y * tileDestCount.x + x];
    }
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

        switch (hstr(static_cast<std::string>(type_v).c_str())) {
            case hstr(config::entities::placeholder_interactable::typeID):
            case hstr(config::entities::omori_laptop::typeID):
            case hstr(config::entities::omori_mewo::typeID):
            case hstr(config::entities::omori_cat_0::typeID):
            case hstr(config::entities::omori_cat_1::typeID):
            case hstr(config::entities::omori_cat_2::typeID):
            case hstr(config::entities::omori_cat_3::typeID):
            case hstr(config::entities::omori_cat_4::typeID):
            case hstr(config::entities::omori_cat_5::typeID):
            case hstr(config::entities::omori_cat_6::typeID):
            case hstr(config::entities::omori_cat_7::typeID):
                data = new Data_Interactable();
                break;

            case hstr(config::entities::placeholder_teleporter::typeID):
            case hstr(config::entities::teleporter_red_hand_throne::typeID):
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
    tiles.clear();
    tiles.shrink_to_fit();
    collisionTilelayer.clear();
    collisionTilelayer.shrink_to_fit();

    // Default properties
    viewportHeight = config::interface::viewportHeight;
    backgroundColor = config::color::offblack;

    for (auto& pair : dependencies) for (auto& p : pair.second) delete p;
    dependencies.clear();
    properties.clear();
}


level::Data level::data;