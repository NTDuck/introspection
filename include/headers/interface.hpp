#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL.h>

#include <components.hpp>
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

        std::unordered_set<Button*>& buttons = Button::instances;
        Title* title = nullptr;
};


#endif


