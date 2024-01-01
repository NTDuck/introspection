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


/* Abstract templates */

/**
 * @brief Represent a "div" i.e. a text container.
*/
template <typename T>
class GenericTextArea : public Multiton<T> {
    public:
        INCL_MULTITON(T)

        virtual ~GenericTextArea();

        static void initialize();
        static void deinitialize();

        virtual void render() const;
        virtual void onWindowChange();

        void editContent(std::string const& nextContent);

    protected:
        GenericTextArea(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset);

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

#define INCL_GENERIC_TEXT_AREA(T) using GenericTextArea<T>::initialize, GenericTextArea<T>::deinitialize, GenericTextArea<T>::render, GenericTextArea<T>::onWindowChange, GenericTextArea<T>::editContent, GenericTextArea<T>::loadOuterTexture, GenericTextArea<T>::loadInnerTexture, GenericTextArea<T>::destSize, GenericTextArea<T>::kDestSizeMultiplier, GenericTextArea<T>::font, GenericTextArea<T>::fontPath, GenericTextArea<T>::outerTexture, GenericTextArea<T>::innerTexture, GenericTextArea<T>::content, GenericTextArea<T>::kCenter, GenericTextArea<T>::kPreset, GenericTextArea<T>::outerDestRect, GenericTextArea<T>::innerDestRect;

namespace std {
    template <typename T>
    struct hash<GenericTextArea<T>> {
        std::size_t operator()(GenericTextArea<T> const*& instance) const;
    };

    template <typename T>
    struct equal_to<GenericTextArea<T>> {
        bool operator()(GenericTextArea<T> const*& first, GenericTextArea<T> const*& second) const;
    };
}


/**
 * @brief Represent a text area with no background.
*/
template <typename T>
class GenericTitle : public GenericTextArea<T> {
    public:
        INCL_GENERIC_TEXT_AREA(T)

        GenericTitle(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset);
        ~GenericTitle() = default;

        void render() const override;
};

#define INCL_GENERIC_TITLE(T) using GenericTitle<T>::render;


/**
 * @brief Represent a mouse-interactible `TextArea`.
*/
template <typename T>
class GenericButton : public GenericTextArea<T> {
    public:
        INCL_GENERIC_TEXT_AREA(T)

        GenericButton(GameState* destState, std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset, TextAreaPreset const& presetOnMouseOver);
        ~GenericButton() = default;

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

#define INCL_GENERIC_BUTTON(T) using GenericButton<T>::render, GenericButton<T>::onWindowChange, GenericButton<T>::handleMouseEvent, GenericButton<T>::onClick, GenericButton<T>::kDestState, GenericButton<T>::outerTextureOnMouseOver, GenericButton<T>::innerTextureOnMouseOver, GenericButton<T>::kPresetOnMouseOver;


/* Derived implementations */

class MenuAvatar final : public Singleton<MenuAvatar> {
    public:
        INCL_SINGLETON(MenuAvatar)

        MenuAvatar(tile::EntitiesTilesetData& tilesetData, const double destRectModifier);
        ~MenuAvatar() = default;

        void render() const;
        void onWindowChange();

    private:
        SDL_Texture*& texture;
        const SDL_Rect srcRect;
        SDL_Rect destRect;
        const double destRectModifier;
};


class MenuAnimatedBackground final : public Singleton<MenuAnimatedBackground> {
    public:
        INCL_SINGLETON(MenuAnimatedBackground)

        MenuAnimatedBackground(SDL_Texture*& texture);
        ~MenuAnimatedBackground() = default;

        void updateAnimation();
        void render() const;
        void onWindowChange();

    private:
        static constexpr double animationUpdateRateLimit = 1;

        SDL_Texture*& texture;
        SDL_Point srcSize;
        SDL_Point& destSize = globals::windowSize;
        std::pair<SDL_Rect, SDL_Rect> srcRects, destRects;

        double currAnimationUpdateCount;
        const double kAnimationUpdateRate = config::animated_background::animationUpdateRate;
};


class MenuButton final : public GenericButton<MenuButton> {
    public:
        INCL_GENERIC_BUTTON(MenuButton)

        MenuButton(GameState* destState, std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset, TextAreaPreset const& presetOnMouseOver);
        ~MenuButton() = default;
};


class MenuTitle final : public GenericTitle<MenuTitle> {
    public:
        INCL_GENERIC_TITLE(MenuTitle)

        MenuTitle(std::string const& content, SDL_FPoint const& center, TextAreaPreset const& preset);
        ~MenuTitle() = default;
};


#endif