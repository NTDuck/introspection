#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include <functional>
#include <unordered_map>

#include <SDL.h>
#include <SDL_ttf.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


/**
 * @brief Represent the in-game interface.
*/
class IngameInterface final : public AbstractInterface<IngameInterface> {
    friend AbstractInterface<IngameInterface>;
    public:
        using AbstractInterface<IngameInterface>::render, AbstractInterface<IngameInterface>::texture;

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
        using Multiton<T>::instantiate, Multiton<T>::instances;
        virtual ~GenericTextArea();

        static void initialize();
        static void deinitialize();

        void render() const;
        void onWindowChange();

        void editContent(std::string const& nextContent);

    protected:
        GenericTextArea(std::string const& content, SDL_FPoint const& center);

        void loadOuterTexture();
        void loadInnerTexture();

        static const int size = 32;   // might get reassigned in `onWindowChange()`?
        static TTF_Font* font;

        SDL_Texture* outerTexture = nullptr;   // outer box
        SDL_Texture* innerTexture = nullptr;   // text content

        std::string content;
        const SDL_FPoint kCenter;   // ratio
        SDL_Rect innerDestRect;
        SDL_Rect outerDestRect;
};

namespace std {
    template <typename T>
    struct hash<GenericTextArea<T>> {
        std::size_t operator()(GenericTextArea<T> const*& instance) const {
            return std::hash<float>{}(instance->kCenter.x) ^ (std::hash<float>{}(instance->kCenter.y) << 1);
        }
    };

    template <typename T>
    struct equal_to<GenericTextArea<T>> {
        bool operator()(GenericTextArea<T> const*& first, GenericTextArea<T> const*& second) const {
            return (first == nullptr && second == nullptr) || (first && second && first->kCenter == second->kCenter);
        }
    };
}

/**
 * @brief Represent a mouse-interactible `TextArea`.
*/
class GenericButton : public GenericTextArea<GenericButton> {
    friend GenericTextArea<GenericButton>;
    public:
        using Multiton<GenericButton>::instances, Multiton<GenericButton>::instantiate, Multiton<GenericButton>::callOnEach;
        using GenericTextArea<GenericButton>::size, GenericTextArea<GenericButton>::font, GenericTextArea<GenericButton>::outerTexture, GenericTextArea<GenericButton>::innerTexture, GenericTextArea<GenericButton>::content, GenericTextArea<GenericButton>::kCenter, GenericTextArea<GenericButton>::innerDestRect, GenericTextArea<GenericButton>::outerDestRect;

        GenericButton(std::string const& content, SDL_FPoint const& center);
    // onMouseOut, onMouseOver, onClick
};

/**
 * @brief Represent the menu interface.
*/
class MenuInterface final : public AbstractInterface<MenuInterface> {
    friend AbstractInterface<MenuInterface>;
    public:
        using AbstractInterface<MenuInterface>::texture;

        MenuInterface();
        ~MenuInterface() = default;

        static void initialize();
        static void deinitialize();

        void render() const override;
        void onWindowChange() override;

    private:
        void renderBackground() const;
        void renderComponents() const;
};



#endif