#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL.h>

#include <components.hpp>
#include <meta.hpp>
#include <auxiliaries.hpp>


/* Abstract templates */

template <typename T>
class AbstractInterface : public Singleton<T> {
    public:
        INCL_SINGLETON(T)

        virtual void render() const;
        virtual void onWindowChange();

    protected:
        ~AbstractInterface();

        /**
         * @brief A temporary storage that is rendered every frame. Used to prevent multiple unnecessary calls of `SDL_RenderCopy()`.
         * @note Needs optimization to perfect relative positions of props to entities.
        */
        SDL_Texture* texture = nullptr;
};

#define INCL_ABSTRACT_INTERFACE(T) using AbstractInterface<T>::render, AbstractInterface<T>::onWindowChange, AbstractInterface<T>::texture;


/* Derived implementations */

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
        void onWindowChange() override;

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

        static void deinitialize();

        void render() const override;
        void onWindowChange() override;

        void handleMouseEvent(SDL_Event const& event);

    private:
        void renderBackground() const;
        void renderComponents() const;
};


/**
 * @brief Represent the loading screen.
*/
class LoadingInterface final : public AbstractInterface<LoadingInterface> {
    public:
        INCL_ABSTRACT_INTERFACE(LoadingInterface)

        LoadingInterface();
        ~LoadingInterface() = default;

        static void deinitialize();

        void render() const override;
        void onWindowChange() override;

        void initiateTransition(GameState const& gameState);
        void handleTransition();

    private:
        static constexpr int kIdleFramesLimit = config::interface::idleFrames;
        int currIdleFrames = 0;

        GameState nextGameState;
};


#endif