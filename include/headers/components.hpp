#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <type_traits>

#include <SDL.h>
#include <SDL_ttf.h>

#include <components.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief Represent a "div" i.e. a text container.
*/
template <typename T>
class TextArea : public Multiton<T> {
    public:
        INCL_MULTITON(T)

        virtual ~TextArea();

        static void initialize();
        static void deinitialize();

        virtual void render() const;
        virtual void onWindowChange();

        void editContent(std::string const& nextContent);

    protected:
        TextArea(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset);

        static void loadFont();
        void loadOuterTexture(SDL_Texture*& texture, TextAreaPreset const& preset);
        void loadInnerTexture(SDL_Texture*& texture, TextAreaPreset const& preset);

        static int destSize;
        static const double kDestSizeMultiplier;

        static TTF_Font* font;
        static const std::filesystem::path fontPath;

        SDL_Texture* outerTexture = nullptr;   // outer box
        SDL_Texture* innerTexture = nullptr;   // text content

        SDL_Rect outerDestRect;
        SDL_Rect innerDestRect;

        std::string content;

        /**
         * Determine the position of the text area relative to the window.
        */
        const SDL_FPoint kCenter;   // ratio

        const TextAreaPreset kPreset;

    private:
        static constexpr SDL_Point kOuterDestRectRatio = { 10, 2 };
};

#define INCL_GENERIC_TEXT_AREA(T) using TextArea<T>::initialize, TextArea<T>::deinitialize, TextArea<T>::render, TextArea<T>::onWindowChange, TextArea<T>::editContent, TextArea<T>::loadOuterTexture, TextArea<T>::loadInnerTexture, TextArea<T>::destSize, TextArea<T>::kDestSizeMultiplier, TextArea<T>::font, TextArea<T>::fontPath, TextArea<T>::outerTexture, TextArea<T>::innerTexture, TextArea<T>::content, TextArea<T>::kCenter, TextArea<T>::kPreset, TextArea<T>::outerDestRect, TextArea<T>::innerDestRect;

namespace std {
    template <typename T>
    struct hash<TextArea<T>> {
        std::size_t operator()(TextArea<T> const*& instance) const;
    };

    template <typename T>
    struct equal_to<TextArea<T>> {
        bool operator()(TextArea<T> const*& first, TextArea<T> const*& second) const;
    };
}


/**
 * @brief Represent the big, chunky game title in the menu.
*/
class Title : public TextArea<Title>, public Singleton<Title> {
    public:
        INCL_GENERIC_TEXT_AREA(Title)
        INCL_SINGLETON(Title)

        Title(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset);
        ~Title() = default;

        static void deinitialize();
        void render() const override;
};


/**
 * @brief Represent a mouse-interactible `TextArea`.
*/
class Button : public TextArea<Button> {
    public:
        INCL_GENERIC_TEXT_AREA(Button)

        Button(GameState* destState, std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset, TextAreaPreset const& presetOnMouseOver);
        ~Button() = default;

        void render() const override;
        void onWindowChange() override;
        void handleMouseEvent(SDL_Event const& event);

    protected:
        virtual void onClick();

        const GameState* kDestState = nullptr;

        SDL_Texture* outerTextureOnMouseOver = nullptr;
        SDL_Texture* innerTextureOnMouseOver = nullptr;

        const TextAreaPreset kPresetOnMouseOver;

    private:
        bool isMouseOut = true;
};


class Avatar final : public Singleton<Avatar> {
    public:
        INCL_SINGLETON(Avatar)

        Avatar(tile::EntitiesTilesetData& tilesetData, const double destRectModifier);
        ~Avatar() = default;

        void render() const;
        void onWindowChange();

    private:
        SDL_Texture*& texture;
        const SDL_Rect srcRect;
        SDL_Rect destRect;
        const double destRectModifier;
};


class AnimatedBackground final : public Singleton<AnimatedBackground> {
    public:
        INCL_SINGLETON(AnimatedBackground)

        AnimatedBackground(SDL_Texture*& texture, SDL_Point const& srcSize);
        ~AnimatedBackground() = default;

        void updateAnimation();
        void render() const;
        void onWindowChange(SDL_Point const& nextSrcSize);

    private:
        static constexpr double animationUpdateRateLimit = 1;

        SDL_Texture*& texture;
        SDL_Point srcSize;

        double currAnimationUpdateCount = 0;
        const double kAnimationUpdateRate = config::animated_background::animationUpdateRate;
};


#endif