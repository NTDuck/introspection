#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include <functional>
#include <unordered_map>
#include <type_traits>

#include <SDL.h>
#include <SDL_ttf.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief Represent the in-game interface.
*/
class IngameInterface final : public AbstractInterface<IngameInterface> {
    public:
        INCL_ABSTRACT_INTERFACE(IngameInterface)

        IngameInterface(const level::LevelName levelName);
        ~IngameInterface() = default;

        static void initialize();
        
        void changeLevel(const level::LevelName levelName);
        void onLevelChange();
        void onWindowChange();

    private:
        void loadLevel();
        void renderBackground() const;
        void renderLevelTiles() const;

        level::LevelName levelName;

        /**
         * @brief Maps a level's name with its corresponding relative file path.
         * @see <src/interface.cpp> Interface.loadLevel() (classmethod)
        */
        static level::LevelMapping kLevelMapping;
};

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

        void render() const;
        void onWindowChange();

        void editContent(std::string const& nextContent);

    protected:
        GenericTextArea(std::string const& content, SDL_FPoint const& center);

        static void loadFont();
        void loadOuterTexture();
        void loadInnerTexture();

        TextAreaPreset preset = globals::config::kButtonOnMouseOutPreset;
        static int size;
        static TTF_Font* font;

        SDL_Texture* outerTexture = nullptr;   // outer box
        SDL_Texture* innerTexture = nullptr;   // text content

        SDL_Rect outerDestRect;
        SDL_Rect innerDestRect;

        std::string content;

        /**
         * Determine the position of the text area relative to the window.
        */
        const SDL_FPoint kCenter;   // ratio

    private:
        static constexpr SDL_Point kOuterDestRectRatio = { 10, 2 };
};

#define INCL_GENERIC_TEXT_AREA(T) using GenericTextArea<T>::initialize, GenericTextArea<T>::deinitialize, GenericTextArea<T>::render, GenericTextArea<T>::onWindowChange, GenericTextArea<T>::editContent, GenericTextArea<T>::loadOuterTexture, GenericTextArea<T>::loadInnerTexture, GenericTextArea<T>::preset, GenericTextArea<T>::size, GenericTextArea<T>::font, GenericTextArea<T>::outerTexture, GenericTextArea<T>::innerTexture, GenericTextArea<T>::content, GenericTextArea<T>::kCenter, GenericTextArea<T>::outerDestRect, GenericTextArea<T>::innerDestRect;

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
 * @brief Represent a mouse-interactible `TextArea`.
*/
class GenericButton : public GenericTextArea<GenericButton> {
    public:
        INCL_GENERIC_TEXT_AREA(GenericButton)

        GenericButton(GameState* destState, std::string const& content, SDL_FPoint const& center);

        void handleMouseEvent(SDL_Event const& event);

    protected:
        void onMouseOut();
        void onMouseOver();
        virtual void onClick();

        GameState* destState = nullptr;

    private:
        bool isMouseOut = true;
};


/**
 * @brief Represent the menu interface.
*/
class MenuInterface final : public AbstractInterface<MenuInterface> {
    public:
        INCL_ABSTRACT_INTERFACE(MenuInterface)

        MenuInterface();
        ~MenuInterface() = default;

        static void initialize();
        static void deinitialize();

        void render() const override;
        void onWindowChange() override;

        void handleMouseEvent(SDL_Event const& event);

  private:
        void renderBackground() const;
        void renderComponents() const;
};


#endif


