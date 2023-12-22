#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include <unordered_map>

#include <SDL.h>
#include <SDL_ttf.h>

#include <meta.hpp>
#include <auxiliaries/utils.hpp>
#include <auxiliaries/globals.hpp>


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
        void renderBackground();
        void renderLevelTiles();

        level::LevelName levelName;

        /**
         * @brief Maps a level's name with its corresponding relative file path.
         * @see <src/interface.cpp> Interface.loadLevel() (classmethod)
        */
        static level::LevelMapping levelMapping;
};

/**
 * @brief Represent a "div" i.e. a text container.
*/
class Division {
    public:
        struct Hasher {
            std::size_t operator()(const Division* pointer) const;
        };

        struct EqualityOperator {
            bool operator()(const Division* first, const Division* second) const;
        };
        
        Division(const std::string& content, const SDL_FPoint& center);
        virtual ~Division();

        static void initialize();
        static void deinitialize();

        void render();
        void onWindowChange();

        void editContent(const std::string& nextContent);

    protected:
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

/**
 * @brief Represent a mouse-interactible `Division`.
*/
class Button : public Division {
    // onMouseOut, onMouseOver, onClick
};

/**
 * @brief Represent the menu interface.
*/
class MenuInterface final : public AbstractInterface<MenuInterface> {
    friend AbstractInterface<MenuInterface>;
    public:
        using AbstractInterface<MenuInterface>::render, AbstractInterface<MenuInterface>::texture;

        MenuInterface();
        ~MenuInterface() = default;

        static void initialize();
        static void deinitialize();

        void render() override;
        void onWindowChange() override;

    private:
        void renderBackground();
        void renderComponents();

        std::unordered_set<Division*, Division::Hasher, Division::EqualityOperator> buttons;
};



#endif