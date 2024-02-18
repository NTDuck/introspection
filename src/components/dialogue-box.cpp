#include <components.hpp>

#include <SDL.h>

#include <auxiliaries.hpp>


void IngameDialogueBox::BMPFont::Map::insert(char c, Data_Glyph_Storage const& data) {
    mGlyphDataUMap.insert(std::make_pair(c, data));
}

IngameDialogueBox::BMPFont::Data_Glyph_Query IngameDialogueBox::BMPFont::Map::operator[](char c) const {
    Data_Glyph_Query data;

    // Same for every glyph
    data.srcRect.y = 0;
    data.srcRect.h = mGlyphHeight;

    auto it = mGlyphDataUMap.find(c);
    if (it != mGlyphDataUMap.end()) {
        data.srcRect.w = it->second.width;
        data.advance = it->second.advance;
    }

    // The sum of all "previous" glyphWidths
    data.srcRect.x = 0;
    for (const auto& pair : mGlyphDataUMap) {
        if (pair.first >= c) break;
        data.srcRect.x += pair.second.width;
    }

    return data;
}


IngameDialogueBox::BMPFont::BMPFont(ComponentPreset const& preset) : mPreset(preset) {}

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
    for (const auto& c : chars) registerCharToMap(font, c);

    if (mTexture != nullptr) SDL_DestroyTexture(mTexture);
    mTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mSrcSize.x, mSrcSize.y);

    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTexture);
    for (const auto& c : chars) registerCharToTexture(font, c);
    SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);
}

void IngameDialogueBox::BMPFont::setRenderTarget(SDL_Texture*& targetTexture) {
    if (targetTexture == nullptr) return;
    
    mTargetTexture = targetTexture;
    SDL_QueryTexture(mTargetTexture, nullptr, nullptr, &mTargetTextureSize.x, &mTargetTextureSize.y);

    std::cout << "queried: " << mTargetTextureSize.x << ' ' << mTargetTextureSize.y << std::endl;

    clear();
}

/**
 * @brief Render char `c` to `texture` in common dialogue text style.
*/
void IngameDialogueBox::BMPFont::render(char c) const {
    static auto endOfLine = [&]() {
        mGlyphOrigin.x = 0;
        mGlyphOrigin.y += mSrcSize.y + mSpacing.y;
    };

    switch (c) {
        case ' ':
            mGlyphOrigin.x += mSrcSize.y / 2 + mSpacing.x;
            return;

        case '\n':
            endOfLine();
            return;

        case '\0': return;
        default: break;
    }

    const auto data = mSrcRectsMap[c];
    if (!data.srcRect.w) return;

    SDL_Rect chrDestRect;
    chrDestRect.w = data.srcRect.w;
    chrDestRect.h = data.srcRect.h;

    if (mGlyphOrigin.x + chrDestRect.w > mTargetTextureSize.x) { if (mGlyphOrigin.y + 2 * chrDestRect.h > mTargetTextureSize.y) clear(); else endOfLine(); }

    chrDestRect.x = mGlyphOrigin.x;
    chrDestRect.y = mGlyphOrigin.y;
    mGlyphOrigin.x += data.advance + mSpacing.x;

    std::cout << c << " | " << data.srcRect.x << ' ' << data.srcRect.y << ' ' << data.srcRect.w << ' ' << data.srcRect.h << " | " << chrDestRect.x << ' ' << chrDestRect.y << ' ' << chrDestRect.w << ' ' << chrDestRect.h << " | " << data.advance << std::endl;

    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTargetTexture);
    SDL_RenderCopy(globals::renderer, mTexture, &data.srcRect, &chrDestRect);
    SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);
}

void IngameDialogueBox::BMPFont::clear() const {
    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTargetTexture);
    SDL_RenderClear(globals::renderer);
    utils::setRendererDrawColor(globals::renderer, mPreset.backgroundColor);
    SDL_RenderFillRect(globals::renderer, nullptr);
    SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);

    mGlyphOrigin = { 0, 0 };
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

/**
 * @see https://freetype.sourceforge.net/freetype2/docs/tutorial/step2.html
*/
void IngameDialogueBox::BMPFont::registerCharToMap(TTF_Font* font, char c) {
    if (TTF_GlyphIsProvided32(font, c) == 0) return;

    Data_Glyph_Storage data;

    // Query glyph width
    SDL_Surface* surface = sTextRenderMethod(font, c, mPreset.textColor, mPreset.backgroundColor);
    data.width = surface->w;
    SDL_FreeSurface(surface);

    // Query glyph advance i.e. distance between 2 adjacent origins
    TTF_GlyphMetrics32(font, c, nullptr, nullptr, nullptr, nullptr, &data.advance);

    mSrcRectsMap.insert(c, data);
    mSrcSize.x += data.width;
}

void IngameDialogueBox::BMPFont::registerCharToTexture(TTF_Font* font, char c) const {
    SDL_Surface* surface = sTextRenderMethod(font, c, mPreset.textColor, mPreset.backgroundColor);
    if (surface == nullptr) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(globals::renderer, surface);
    SDL_FreeSurface(surface);

    auto srcRect = mSrcRectsMap[c].srcRect;
    SDL_RenderCopy(globals::renderer, texture, nullptr, &srcRect);
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
    std::cout << destOffset << std::endl;
    mTextDestRect = {
        mBoxDestRect.x + destOffset,
        mBoxDestRect.y + destOffset,
        mBoxDestRect.w - 2 * destOffset,
        mBoxDestRect.h - 2 * destOffset,
    };

    if (mTextTexture != nullptr) SDL_DestroyTexture(mTextTexture);
    mTextTexture = SDL_CreateTexture(globals::renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, mTextDestRect.w, mTextDestRect.h);

    assert(mTextTexture != nullptr);
    std::cout << mTextDestRect.x << ' ' << mTextDestRect.y << ' ' << mTextDestRect.w << ' ' << mTextDestRect.h << std::endl;

    mBMPFont.setRenderTarget(mTextTexture);
    for (unsigned short int progress = 0; progress <= mCurrProgress; ++progress) mBMPFont.render(mContent[progress]);   // Retain progress
}

void IngameDialogueBox::handleKeyBoardEvent(SDL_Event const& event) {
    switch (event.key.keysym.sym) {
        case ~config::Key::kAffirmative:
            terminate();
            break;

        case ~config::Key::kNegative:
            skip();
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

void IngameDialogueBox::terminate() {
    if (mStatus != Status::kUpdateComplete) return;

    mBMPFont.clear();
    mStatus = Status::kInactive;
    globals::state = GameState::kIngamePlaying;
}

void IngameDialogueBox::skip() {
    if (mStatus != Status::kUpdateInProgress) return;

    const auto prevProgress = mCurrProgress;
    mCurrProgress = static_cast<unsigned short int>(mContent.size()) - 1;
    for (auto progress = prevProgress; progress < mCurrProgress; ++progress) mBMPFont.render(mContent[progress]);
}


template <>
const double GenericComponent<IngameDialogueBox>::kDestSizeModifier = config::components::dialogue_box::destSizeModifier;

template <>
const SDL_Point GenericComponent<IngameDialogueBox>::kDestRectRatio = config::components::dialogue_box::destRectRatio;

TTF_Font* IngameDialogueBox::sFont = nullptr;

const std::filesystem::path IngameDialogueBox::sFontPath = config::components::dialogue_box::fontPath;