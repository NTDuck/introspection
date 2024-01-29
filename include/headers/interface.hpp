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
 * @brief Handle the in-game map.
*/
class IngameMapHandler final : public AbstractInterface<IngameMapHandler> {
    public:
        INCL_ABSTRACT_INTERFACE(IngameMapHandler)

        IngameMapHandler(const level::LevelName levelName);
        ~IngameMapHandler();

        static void initialize();
        
        void render() const override;
        void onLevelChange();
        void onWindowChange() override;
        void handleKeyBoardEvent(SDL_Event const& event);

        inline level::LevelName getLevel() { return levelName; }
        void changeLevel(const level::LevelName levelName);

        bool isOnGrayscale = false;

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

        /**
         * A grayscaled version of `texture`.
        */
        SDL_Texture* grayscaleTexture = nullptr;
};


/**
 * @brief Handle in-game camera angles i.e. perspectives.
*/
class IngameViewHandler final : public AbstractInterface<IngameViewHandler> {
    public:
        INCL_ABSTRACT_INTERFACE(IngameViewHandler)

        IngameViewHandler(std::function<void()> const& callable, SDL_Rect& destRect, const IngameViewMode viewMode = config::interface::defaultViewMode);
        ~IngameViewHandler() = default;

        void render() const override;
        void onWindowChange() override;
        void handleKeyBoardEvent(SDL_Event const& event);

        void switchViewMode(const IngameViewMode newViewMode);

    private:
        /**
         * A function with `void` return type that calls all `render()` method on all dependencies. Dependencies are other entities and the map, and not UI components, for example.
        */
        const std::function<void()> callable;

        IngameViewMode viewMode;

        double tileCountWidth;
        static constexpr double tileCountHeight = config::interface::tileCountHeight;

        SDL_Rect& destRect;   // Read-only
        mutable SDL_Rect srcRect;   // Prevent warning: `note: the first difference of corresponding definitions is field 'destRect'`
};


/**
 * @brief Represent the in-game interface.
*/
class IngameInterface final : public Singleton<IngameInterface> {
    public:
        INCL_SINGLETON(IngameInterface);

        IngameInterface();
        ~IngameInterface() = default;

        static void initialize();
        static void deinitialize();

        void render() const;
        void onLevelChange() const;
        void onWindowChange() const;

        void handleKeyBoardEvent(SDL_Event const& event) const;
        void handleMouseEvent(SDL_Event const& event) const;
        void handleCustomEvent(SDL_Event const& event) const;
        void pushEvent() const;

        void handleEntities() const;

    private:
        void handleEntitiesMovement() const;
        void handleEntitiesInteraction() const;
        void handleEntitiesSFX() const;

        template <typename Active, typename Passive>
        void onEntityCollision(Active& active, Passive& passive) const;
        template <typename Active, typename Passive>
        void onEntityAnimation(AnimationType animationType, Active& active, Passive& passive) const;
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
        void updateAnimation();

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

        inline static void initialize() {}
        static void deinitialize();

        void render() const override;
        void onWindowChange() override;

        void updateAnimation();
        void initiateTransition(GameState const& gameState);
        void handleTransition();

    private:
        void renderBackground() const;
        void renderComponents() const;

        static constexpr int kIdleFramesLimit = config::interface::idleFrames;
        int currIdleFrames = 0;

        GameState nextGameState;
};


/**
 * @brief Represent whatever shows up when the player should be defeated.
*/
class GameOverInterface final : public AbstractInterface<GameOverInterface> {
    public:
        INCL_ABSTRACT_INTERFACE(GameOverInterface)

        GameOverInterface();
        ~GameOverInterface() = default;
        
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