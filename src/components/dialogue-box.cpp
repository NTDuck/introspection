#include <components.hpp>

#include <SDL.h>

#include <auxiliaries.hpp>


void IngameDialogueBox::BMPFont::Map::insert(char c, Unit glyphWidth) {
    data.insert(std::make_pair(c, glyphWidth));
}

SDL_Rect IngameDialogueBox::BMPFont::Map::operator[](char c) const {
    // Consider index-based for loop instead?
    auto it = data.find(c);
    if (it == data.end()) return {};
    int glyphWidth = it->second;

    int glyphPos = 0;   // The sum of all "previous" glyphWidths
    for (const auto& pair : data) {
        if (pair.first >= c) break;
        glyphPos += pair.second;
    }

    return { glyphPos, 0, glyphWidth, glyphHeight };
}


IngameDialogueBox::BMPFont::BMPFont(ComponentPreset const& preset, SDL_Point const& spacing) : mPreset(preset), mSpacing(spacing) {}

IngameDialogueBox::BMPFont::~BMPFont() {
    // `~Map()` automatically called
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

void IngameDialogueBox::BMPFont::load(TTF_Font* font) {
    mSrcSize.y = TTF_FontHeight(font);
    mSrcRectsMap.setGlyphHeight(mSrcSize.y);

    const std::string chars = getChars();
    for (const auto& c : chars) if (TTF_GlyphIsProvided32(font, c) != 0) registerCharToMap(c);

    if (mTexture != nullptr) SDL_DestroyTexture(mTexture);
    mTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mSrcSize.x, mSrcSize.y);

    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTexture);
    for (const auto& c : chars) registerCharToTexture(c);
    SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);
}

void IngameDialogueBox::BMPFont::setRenderTarget(SDL_Texture* targetTexture) {
    if (targetTexture == nullptr) return;
    mTargetTexture = targetTexture;
    SDL_QueryTexture(mTargetTexture, nullptr, nullptr, &mTargetTextureSize.x, &mTargetTextureSize.y);

    clear();
}

/**
 * @brief Render char `c` to `texture` in common dialogue text style.
 * @note Do not ever use a different `target`.
*/
void IngameDialogueBox::BMPFont::render(char c) const {
    if (c == ' ') {
        mTargetTextureOrigin.x += mSrcSize.y / 2 + mSpacing.x;
        return;
    }

    SDL_Rect chrSrcRect = mSrcRectsMap[c];
    if (!chrSrcRect.w) return;

    SDL_Rect chrDestRect;
    chrDestRect.w = chrSrcRect.w;
    chrDestRect.h = chrSrcRect.h;

    if (mTargetTextureOrigin.x + chrDestRect.w > mTargetTextureSize.x) {   // End of line
        if (mTargetTextureOrigin.y + chrDestRect.h > mTargetTextureSize.y) { clear(); return; }   // End of space
        mTargetTextureOrigin.x = 0;
        mTargetTextureOrigin.y += chrDestRect.h + mSpacing.y;
    }

    mTargetTextureOrigin.x += chrDestRect.w + mSpacing.x;
    chrDestRect.x = mTargetTextureOrigin.x;
    chrDestRect.y = mTargetTextureOrigin.y;

    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTargetTexture);
    SDL_RenderCopy(globals::renderer, mTexture, &chrSrcRect, &chrDestRect);
    SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);
}

void IngameDialogueBox::BMPFont::clear() const {
    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTargetTexture);
    SDL_RenderClear(globals::renderer);
    utils::setRendererDrawColor(globals::renderer, mPreset.backgroundColor);
    SDL_RenderFillRect(globals::renderer, nullptr);
    SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);

    mTargetTextureOrigin = { 0, 0 };
}

/**
 * @brief Retrieve all supported chars.
 * @note Must be ORDERED for `Map` to work.
*/
std::string IngameDialogueBox::BMPFont::getChars() const {
    std::string chars;
    const std::string symbols = "!?#%^/&*()-+,.:;`\\_=<>@\"\'$";
    chars.reserve(26 * 2 + 10 + symbols.size());

    for (char c = 'a'; c <= 'z'; ++c) chars += c;
    for (char c = 'A'; c <= 'Z'; ++c) chars += c;
    for (char c = '0'; c <= '9'; ++c) chars += c;
    chars.append(symbols);

    std::sort(chars.begin(), chars.end());   // Really, really, really important!

    return chars;
}

void IngameDialogueBox::BMPFont::registerCharToMap(char c) {
    SDL_Surface* surface = TTF_RenderUTF8_Solid(sFont, &c, config::preset::dialogue.textColor);   // This is non-deterministic, which is really weird
    if (surface == nullptr) return;
    
    mSrcRectsMap.insert(c, surface->w);
    mSrcSize.x += surface->w;

    SDL_FreeSurface(surface);
}

void IngameDialogueBox::BMPFont::registerCharToTexture(char c) const {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(sFont, &c, mPreset.textColor);
    if (surface == nullptr) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(globals::renderer, surface);

    auto srcRect = mSrcRectsMap[c];
    SDL_RenderCopy(globals::renderer, texture, nullptr, &srcRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


IngameDialogueBox::IngameDialogueBox(SDL_FPoint const& center, ComponentPreset const& preset) : GenericComponent<IngameDialogueBox>(center, preset), GenericBoxComponent<IngameDialogueBox>(center, preset), mBMPFont(preset) {
    mBMPFont.load(sFont);
}

IngameDialogueBox::~IngameDialogueBox() {
    if (mTextTexture != nullptr) {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }
}

void IngameDialogueBox::initialize() {
    sFont = TTF_OpenFont(sFontPath.generic_string().c_str(), sFontSize);
}

void IngameDialogueBox::deinitialize() {
    if (sFont != nullptr) {
        TTF_CloseFont(sFont);
        sFont = nullptr;
    }
}

void IngameDialogueBox::render() const {
    GenericBoxComponent<IngameDialogueBox>::render();
    if (mStatus == Status::kUpdateInProgress) mBMPFont.render(mContent[mCurrProgress]);
    SDL_RenderCopy(globals::renderer, mTextTexture, nullptr, &mTextDestRect);
}

void IngameDialogueBox::onWindowChange() {
    GenericBoxComponent<IngameDialogueBox>::onWindowChange();

    const int destOffset = static_cast<double>(std::min(mBoxDestRect.w, mBoxDestRect.h)) * mTextTextureOffsetRatio;
    mTextDestRect = {
        mBoxDestRect.x + destOffset,
        mBoxDestRect.y + destOffset,
        mBoxDestRect.w - 2 * destOffset,
        mBoxDestRect.h - 2 * destOffset,
    };

    if (mTextTexture != nullptr) SDL_DestroyTexture(mTextTexture);
    mTextTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mTextDestRect.w, mTextDestRect.h);

    mBMPFont.setRenderTarget(mTextTexture);
}

void IngameDialogueBox::handleKeyBoardEvent(SDL_Event const& event) {
    switch (event.key.keysym.sym) {
        case ~config::Key::kPlayerInteract:
            if (mStatus != Status::kUpdateComplete) break;
            mBMPFont.clear();
            mStatus = Status::kInactive;
            globals::state = GameState::kIngamePlaying;
            break;
            
        default: break;
    }
}

void IngameDialogueBox::updateProgress() {
    if (mStatus != Status::kUpdateInProgress) return;
    if (mCurrProgress == static_cast<unsigned short int>(mContent.size()) - 1) mStatus = Status::kUpdateComplete; else ++mCurrProgress;
}

void IngameDialogueBox::editContent(std::string const& content) {
    if (mStatus != Status::kInactive || content.empty()) return;

    mCurrProgress = 0;
    mContent = content;

    mStatus = Status::kUpdateInProgress;
    globals::state = GameState::kIngameDialogue;
}


template <>
const double GenericComponent<IngameDialogueBox>::kDestSizeModifier = config::components::dialogue_box::destSizeModifier;

template <>
const SDL_Point GenericComponent<IngameDialogueBox>::kDestRectRatio = config::components::dialogue_box::destRectRatio;

TTF_Font* IngameDialogueBox::sFont = nullptr;

const std::filesystem::path IngameDialogueBox::sFontPath = config::components::dialogue_box::fontPath;