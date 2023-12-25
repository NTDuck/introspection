#include <auxiliaries.hpp>


std::size_t level::EntityLevelData::Hasher::operator()(level::EntityLevelData const& instance) const {
    return std::hash<SDL_Point>{}(instance.destCoords);
}

bool level::EntityLevelData::EqualityOperator::operator()(level::EntityLevelData const& first, level::EntityLevelData const& second) const {
    return first.destCoords == second.destCoords;
}

/**
 * @brief An alternative to the constructor. Populate data members based on JSON data.
 * @note Requires JSON data to be in proper format before being called.
*/
void level::EntityLevelData::initialize(json const& entityJSONLeveData) {
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

void level::TeleporterLevelData::initialize(json const& entityJSONLevelData) {
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

void level::LevelData::initialize(json const& JSONLayerData) {
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