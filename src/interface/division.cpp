#include <interface.hpp>

#include <cstring>
#include <functional>

#include <SDL.h>
#include <SDL_ttf.h>

#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


/**
 * @brief The hasher functor. Use `kCenter`.
 * @see https://stackoverflow.com/questions/4238122/hash-function-for-floats
 * @see https://en.wikipedia.org/wiki/MurmurHash
*/
std::size_t Division::Hasher::operator()(const Division* pointer) const {
    // auto murmur = [](float f) {
    //     f ^= f >> 13;
    //     f *= 0x5bd1e995;
    //     return f ^ (f >> 15);
    // }

    auto hash = [](float f) {
        std::size_t s;
        std::memcpy(&s, &f, sizeof(float));
        return s & 0xfffff000;
    };

    return hash(pointer->kCenter.x) ^ (hash(pointer->kCenter.y) << 1);
}

/**
 * @brief The equality operator. Use `kCenter`.
*/
bool Division::EqualityOperator::operator()(const Division* first, const Division* second) const {
    return first->kCenter.x == second->kCenter.x && first->kCenter.y == second->kCenter.y;
}

Division::Division(const std::string& content, const SDL_FPoint& center) : content(content), kCenter(center) {}

Division::~Division() {
    SDL_DestroyTexture(outerTexture);
    SDL_DestroyTexture(innerTexture);
    outerTexture = innerTexture = nullptr;
}

void Division::initialize() {
    font = TTF_OpenFont(globals::config::kOmoriFontSecondPath.c_str(), size);
    // if (font == nullptr) TTF_GetError();
}

void Division::deinitialize() {
    TTF_CloseFont(font);
    font = nullptr;
}

void Division::render() {
    SDL_RenderCopy(globals::renderer, outerTexture, nullptr, &outerDestRect);
    SDL_RenderCopy(globals::renderer, innerTexture, nullptr, &innerDestRect);
}

/**
 * @brief Reset all `SDL_Texture*` and their corresponding `SDL_Rect`.
*/
void Division::onWindowChange() {
    if (outerTexture != nullptr) SDL_DestroyTexture(outerTexture);
    if (innerTexture != nullptr) SDL_DestroyTexture(innerTexture);
    loadOuterTexture();
    loadInnerTexture();
}

void Division::editContent(const std::string& nextContent) {
    content = nextContent;
    loadInnerTexture();
}

/**
 * @note Attempt to recreate OMORI buttons.
*/
void Division::loadOuterTexture() {
    constexpr static float kUnit = 2.0f / 32.0f;

    auto modifyRect = [](SDL_Rect& rect, float ratio) {
        int delta = utils::castFloatToInt(std::min(rect.w, rect.h) / 2 * ratio);
        rect.x += delta;
        rect.y += delta;
        rect.w -= delta * 2;
        rect.h -= delta * 2;
    };

    outerDestRect.w = size * 10;
    outerDestRect.h = size * 2;
    outerDestRect.x = utils::castFloatToInt(globals::windowSize.x * kCenter.x - outerDestRect.w / 2);
    outerDestRect.y = utils::castFloatToInt(globals::windowSize.y * kCenter.y - outerDestRect.h / 2);

    outerTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, 
    SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, outerDestRect.w, outerDestRect.h);

    SDL_SetRenderTarget(globals::renderer, outerTexture);
    SDL_Rect arbitraryRect = outerDestRect;
    arbitraryRect.x = arbitraryRect.y = 0;

    auto fillRect = [&](float multiplier, const SDL_Color& color) {
        modifyRect(arbitraryRect, kUnit * multiplier);
        utils::setRendererDrawColor(globals::renderer, color);
        SDL_RenderFillRect(globals::renderer, &arbitraryRect);
    };

    fillRect(0.0f, globals::config::kDefaultBlackColor);
    fillRect(0.5f, globals::config::kDefaultWhiteColor);
    fillRect(2.0f, globals::config::kDefaultBlackColor);

    SDL_SetRenderTarget(globals::renderer, nullptr);
}

void Division::loadInnerTexture() {
    // Render text at high quality
    SDL_Surface* innerSurface = TTF_RenderUTF8_Blended(font, content.c_str(), globals::config::kDefaultWhiteColor);
    if (innerSurface == nullptr) return;

    innerTexture = SDL_CreateTextureFromSurface(globals::renderer, innerSurface);
    
    // Register `innerSurface` dimensions to `innerDestRect`
    innerDestRect.w = innerSurface->w;
    innerDestRect.h = innerSurface->h;
    innerDestRect.x = utils::castFloatToInt(globals::windowSize.x * kCenter.x - innerDestRect.w / 2);
    innerDestRect.y = utils::castFloatToInt(globals::windowSize.y * kCenter.y - innerDestRect.h / 2);

    SDL_FreeSurface(innerSurface);
}


TTF_Font* Division::font = nullptr;