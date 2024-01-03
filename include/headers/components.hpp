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
 * @brief Represent an abstract component. Does nothing by default.
 * @note Recommended implementation: beware of virtual inheritance! Since this is a virtual base class and this constructor requires passing parameters, all derived classes are required to explicitly call this constructor.
 * @see https://stackoverflow.com/questions/1193138/virtual-base-class-data-members/1193516#1193516
*/
template <typename T>
class GenericComponent : virtual public Multiton<T> {
    public:
        INCL_MULTITON(T)
        
        virtual ~GenericComponent() = default;
        virtual void onWindowChange();   // Not pure virtual
    
    protected:
        GenericComponent(SDL_FPoint const& center, ComponentPreset const& preset);

        static int destSize;
        static const double kDestSizeModifier;
        static constexpr SDL_Point kDestRectRatio = { 10, 2 };

        const SDL_FPoint kCenter;
        const ComponentPreset kPreset;
};

#define INCL_GENERIC_COMPONENT(T) using GenericComponent<T>::onWindowChange, GenericComponent<T>::destSize, GenericComponent<T>::kDestSizeModifier, GenericComponent<T>::kDestRectRatio, GenericComponent<T>::kCenter, GenericComponent<T>::kPreset;

namespace std {
    template <typename T>
    struct hash<GenericComponent<T>> {
        std::size_t operator()(GenericComponent<T> const*& instance) const;
    };

    template <typename T>
    struct equal_to<GenericComponent<T>> {
        bool operator()(GenericComponent<T> const*& first, GenericComponent<T> const*& second) const;
    };
}


/**
 * @brief Represent a generic box component with `OMORI`-styled line.
*/
template <typename T>
class GenericBoxComponent : virtual public GenericComponent<T> {
    public:
        INCL_MULTITON(T)
        INCL_GENERIC_COMPONENT(T)

        virtual ~GenericBoxComponent();

        static void deinitialize();

        virtual void render() const;
        void onWindowChange() override;

    protected:
        GenericBoxComponent(SDL_FPoint const& center, ComponentPreset const& preset);

        void loadBoxTexture(SDL_Texture*& texture, ComponentPreset const& preset);

        SDL_Texture* boxTexture = nullptr;
        SDL_Rect boxDestRect;
};

#define INCL_GENERIC_BOX_COMPONENT(T) using GenericBoxComponent<T>::deinitialize, GenericBoxComponent<T>::render, GenericBoxComponent<T>::onWindowChange, GenericBoxComponent<T>::loadBoxTexture, GenericBoxComponent<T>::boxTexture, GenericBoxComponent<T>::boxDestRect;


/**
 * @brief Represent a generic text component.
*/
template <typename T>
class GenericTextComponent : virtual public GenericComponent<T> {
    public:
        INCL_MULTITON(T)
        INCL_GENERIC_COMPONENT(T)

        virtual ~GenericTextComponent();

        static void deinitialize();

        virtual void render() const;
        void onWindowChange() override;

        void editContent(std::string const& nextContent);

    protected:  
        GenericTextComponent(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content);

        void loadTextTexture(SDL_Texture*& texture, ComponentPreset const& preset);

        static TTF_Font* font;
        static const std::filesystem::path fontPath;

        SDL_Texture* textTexture = nullptr;
        SDL_Rect textDestRect;

        std::string content;
};

#define INCL_GENERIC_TEXT_COMPONENT(T) using GenericTextComponent<T>::deinitialize, GenericTextComponent<T>::render, GenericTextComponent<T>::onWindowChange, GenericTextComponent<T>::editContent, GenericTextComponent<T>::loadTextTexture, GenericTextComponent<T>::font, GenericTextComponent<T>::fontPath, GenericTextComponent<T>::textTexture, GenericTextComponent<T>::textDestRect, GenericTextComponent<T>::content;


/**
 * @brief Represent a text-inside-a-box component. Essentially the combination of `GenericTextComponent<T>` and `GenericBoxComponent<T>`.
*/
template <typename T>
class GenericTextBoxComponent : public GenericTextComponent<T>, public GenericBoxComponent<T> {
    public:
        INCL_MULTITON(T)
        INCL_GENERIC_COMPONENT(T)
        INCL_GENERIC_TEXT_COMPONENT(T)
        INCL_GENERIC_BOX_COMPONENT(T)

        virtual ~GenericTextBoxComponent() = default;

        static void deinitialize();

        void render() const override;
        void onWindowChange() override;
        
    protected:
        GenericTextBoxComponent(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content);
};

#define INCL_GENERIC_TEXTBOX_COMPONENT(T) using GenericTextBoxComponent<T>::deinitialize, GenericTextBoxComponent<T>::render, GenericTextBoxComponent<T>::onWindowChange;


/**
 * @brief Represent a generic button component. Essentially an interactible `GenericTextBoxComponent<T>`.
*/
template <typename T>
class GenericButtonComponent : public GenericTextBoxComponent<T> {
    public:
        INCL_MULTITON(T)
        INCL_GENERIC_COMPONENT(T)
        INCL_GENERIC_TEXT_COMPONENT(T)
        INCL_GENERIC_BOX_COMPONENT(T)
        INCL_GENERIC_TEXTBOX_COMPONENT(T)

        virtual ~GenericButtonComponent() = default;

        void render() const;   // `override` ?
        void onWindowChange() override;

        void handleMouseEvent(SDL_Event const& event);

    protected:
        GenericButtonComponent(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState);

        virtual void onClick();

        const ComponentPreset kOnMouseOverPreset;
        SDL_Texture* onMouseOverTextTexture = nullptr;
        SDL_Texture* onMouseOverBoxTexture = nullptr;

        const GameState* kDestState = nullptr;

    private:
        bool isMouseOut = true;
};

#define INCL_GENERIC_BUTTON_COMPONENT(T) using GenericButtonComponent<T>::render, GenericButtonComponent<T>::onWindowChange, GenericButtonComponent<T>::handleMouseEvent, GenericButtonComponent<T>::onClick, GenericButtonComponent<T>::kOnMouseOverPreset, GenericButtonComponent<T>::onMouseOverTextTexture, GenericButtonComponent<T>::onMouseOverBoxTexture, GenericButtonComponent<T>::kDestState;


/* Derived implementations */

/**
 * @brief Represent the avatar visible on the menu.
*/
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


/**
 * @brief Represent the animated background on the menu, based on the most recently visited in-game level.
*/
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


/**
 * @brief Represent the button components on the menu.
*/
class MenuButton final : public GenericButtonComponent<MenuButton> {
    friend GenericButtonComponent<MenuButton>;
    public:
        INCL_GENERIC_BUTTON_COMPONENT(MenuButton)

        MenuButton(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState);
        ~MenuButton() = default;
};


/**
 * @brief Represent the big bug not-so-chunky title on the menu.
*/
class MenuTitle final : public GenericTextComponent<MenuTitle> {
    friend GenericTextComponent<MenuTitle>;
    public:
        INCL_GENERIC_TEXT_COMPONENT(MenuTitle)

        MenuTitle(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content);
        ~MenuTitle() = default;
};


#endif