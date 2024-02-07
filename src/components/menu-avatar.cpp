// #include <components.hpp>

// #include <SDL.h>
// #include <auxiliaries.hpp>


// MenuAvatar::MenuAvatar(tile::EntitiesTilesetData& tilesetData, const double destRectModifier) : texture(tilesetData.texture), srcRect(SDL_Rect{ 0, 0, tilesetData.srcSize.x * tilesetData.animationSize.x, tilesetData.srcSize.y * tilesetData.animationSize.y }), destRectModifier(destRectModifier) {}

// void MenuAvatar::render() const {
//     SDL_RenderCopy(globals::renderer, texture, &srcRect, &destRect);
// }

// void MenuAvatar::onWindowChange() {
//     destRect.h = globals::windowSize.y * destRectModifier;
//     destRect.w = srcRect.w / srcRect.h * destRect.h;
//     destRect.x = (globals::windowSize.x - destRect.w) >> 1;
//     destRect.y = (globals::windowSize.y - destRect.h) >> 1;
// }