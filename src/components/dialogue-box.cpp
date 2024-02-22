#include <components.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <string>

#include <SDL.h>

#include <mixer.hpp>
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
    mSrcSize.x = 0;
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
    mTargetTexture = targetTexture;
    SDL_QueryTexture(mTargetTexture, nullptr, nullptr, &mTargetTextureSize.x, &mTargetTextureSize.y);

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

        case '\0':
            clear();
            return;

        default: break;
    }

    const auto data = mSrcRectsMap[c];
    if (!data.srcRect.w) return;

    SDL_Rect chrDestRect;
    chrDestRect.w = data.srcRect.w;
    chrDestRect.h = data.srcRect.h;

    if (mGlyphOrigin.x + chrDestRect.w > mTargetTextureSize.x) { if (mGlyphOrigin.y + chrDestRect.h > mTargetTextureSize.y) clear(); else endOfLine(); }

    chrDestRect.x = mGlyphOrigin.x;
    chrDestRect.y = mGlyphOrigin.y;
    mGlyphOrigin.x += data.advance + mSpacing.x;

    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTargetTexture);
    SDL_RenderCopy(globals::renderer, mTexture, &data.srcRect, &chrDestRect);
    SDL_SetRenderTarget(globals::renderer, cachedRenderTarget);
}

void IngameDialogueBox::BMPFont::clear() const {
    auto cachedRenderTarget = SDL_GetRenderTarget(globals::renderer);
    SDL_SetRenderTarget(globals::renderer, mTargetTexture);
    utils::setRendererDrawColor(globals::renderer, mPreset.backgroundColor);
    SDL_RenderClear(globals::renderer);
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


IngameDialogueBox::IngameDialogueBox(SDL_FPoint const& center, ComponentPreset const& preset) : GenericComponent<IngameDialogueBox>(center, preset), GenericBoxComponent<IngameDialogueBox>(center, preset), mBMPFont(preset) {}

IngameDialogueBox::~IngameDialogueBox() {
    if (mTextTexture != nullptr) {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }
    if (mFont != nullptr) {
        TTF_CloseFont(mFont);
        mFont = nullptr;
    }
}

void IngameDialogueBox::render() const {
    GenericBoxComponent<IngameDialogueBox>::render();
    if (mStatus == Status::kUpdateInProgress) mBMPFont.render(mContents.front()[mCurrProgress]);
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

    if (mFont != nullptr) TTF_CloseFont(mFont);
    mFont = TTF_OpenFont(sFontPath.generic_string().c_str(), getFontSize(sDestSize));

    mBMPFont.clear();
    mBMPFont.load(mFont);
    mBMPFont.setRenderTarget(mTextTexture);
    if (mStatus == Status::kUpdateInProgress) for (unsigned short int progress = 0; progress <= mCurrProgress; ++progress) mBMPFont.render(mContents.front()[progress]);   // Retain progress
}

void IngameDialogueBox::handleKeyBoardEvent(SDL_Event const& event) {
    if (event.key.repeat != 0) return;

    switch (event.key.keysym.sym) {
        case ~config::Key::kAffirmative:
            close();
            break;

        case ~config::Key::kNegative:
            skip();
            break;
            
        default: break;
    }
}

void IngameDialogueBox::handleSFX() const {
    static Status prevStatus = mStatus;
    if (prevStatus == mStatus) return;

    switch (mStatus) {
        case Status::kUpdateInProgress:
            // SFX should suffice when a dialogue box fully loads
            Mixer::invoke(&Mixer::playSFX, Mixer::SFXName::kDialogue);
            break;

        case Status::kUpdateComplete:
            Mixer::invoke(&Mixer::stopSFX);
            break;

        default: break;
    }

    prevStatus = mStatus;
}

void IngameDialogueBox::updateProgress() {
    switch (mStatus) {
        case Status::kUpdateInProgress:
            if (mCurrProgress == static_cast<unsigned short int>(mContents.front().size()) - 1) mStatus = Status::kUpdateComplete;
            else ++mCurrProgress;
            break;

        case Status::kInactive:
            if (mDelayCounter) --mDelayCounter;
            break;

        default: break;
    }
}

void IngameDialogueBox::enqueueContent(std::string const& content) {
    if (mStatus != Status::kInactive || content.empty()) return;

    mContents.push(content);

    mCurrProgress = 0;
    mStatus = Status::kUpdateInProgress;
    globals::state = GameState::kIngameDialogue;
}

void IngameDialogueBox::enqueueContents(std::vector<std::string> const& contents) {
    if (mStatus != Status::kInactive || mDelayCounter) return;

    for (const auto& content : contents) if (!content.empty()) mContents.push(content);

    mCurrProgress = 0;
    mDelayCounter = sDelayCounterLimit;   // Reset
    mStatus = Status::kUpdateInProgress;
    globals::state = GameState::kIngameDialogue;
}

int IngameDialogueBox::getFontSize(const double destSize) {
    int size = static_cast<int>(destSize * 0.2);   // Accommodate as needed

    // The "recommended" sizes are: 12, 18, 24, 36, 48, 60, 72
    // So we're just gonna make it divisible to 6
    int lower_limit = static_cast<int>(size / 6) * 6;
    int upper_limit = lower_limit + 6;
    
    return abs(size - lower_limit) > abs(size - upper_limit) ? lower_limit : upper_limit;   // Return whichever is closer
}

void IngameDialogueBox::close() {
    if (mStatus != Status::kUpdateComplete) return;

    mBMPFont.clear();
    mContents.pop();

    if (!mContents.empty()) {
        mCurrProgress = 0;
        mStatus = Status::kUpdateInProgress;
    } else {
        mStatus = Status::kInactive;
        globals::state = GameState::kIngamePlaying;
    }
}

void IngameDialogueBox::skip() {
    if (mStatus != Status::kUpdateInProgress) return;

    const auto prevProgress = mCurrProgress;
    mCurrProgress = static_cast<unsigned short int>(mContents.front().size()) - 1;
    for (auto progress = prevProgress; progress < mCurrProgress; ++progress) mBMPFont.render(mContents.front()[progress]);
}


template <>
const double GenericComponent<IngameDialogueBox>::kDestSizeModifier = config::components::dialogue_box::destSizeModifier;

template <>
const SDL_Point GenericComponent<IngameDialogueBox>::kDestRectRatio = config::components::dialogue_box::destRectRatio;

const std::filesystem::path IngameDialogueBox::sFontPath = config::components::dialogue_box::fontPath;