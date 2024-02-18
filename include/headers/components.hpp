#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>
#include <functional>
#include <filesystem>
#include <map>
#include <unordered_map>
#include <type_traits>

#include <SDL.h>
#include <SDL_ttf.h>

#include <meta.hpp>
#include <auxiliaries.hpp>


/* Abstract templates */

/**
 * @brief Represent an abstract component. Does nothing by default.
 * @note Recommended implementation: beware of virtual inheritance! Since this is a virtual base class and this constructor requires passing parameters, all derived classes are required to explicitly call this constructor.
 * @see https://stackoverflow.com/questions/1193138/virtual-base-class-data-members/1193516#1193516
*/
template <typename T>
class GenericComponent : public Multiton<T> {
    public:
        INCL_MULTITON(T)
        
        virtual ~GenericComponent() = default;
        virtual void onWindowChange();   // Not pure virtual
    
    protected:
        GenericComponent(SDL_FPoint const& center, ComponentPreset const& preset);

        static int sDestSize;
        static const double kDestSizeModifier;
        static const SDL_Point kDestRectRatio;

        const SDL_FPoint kCenter;
        const ComponentPreset kPreset;
};

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

#define INCL_GENERIC_COMPONENT(T) using GenericComponent<T>::onWindowChange, GenericComponent<T>::sDestSize, GenericComponent<T>::kDestSizeModifier, GenericComponent<T>::kDestRectRatio, GenericComponent<T>::kCenter, GenericComponent<T>::kPreset;


template <typename T>
class GenericBoxComponent : virtual public GenericComponent<T> {
    public:
        INCL_MULTITON(T)
        INCL_GENERIC_COMPONENT(T)

        virtual ~GenericBoxComponent();

        virtual void render() const;
        void onWindowChange() override;

    protected:
        GenericBoxComponent(SDL_FPoint const& center, ComponentPreset const& preset);

        static void shrinkRect(SDL_Rect& rect, const float ratio);
        void loadBoxTexture(SDL_Texture*& texture, ComponentPreset const& preset);

        SDL_Texture* mBoxTexture = nullptr;
        SDL_Rect mBoxDestRect;
};

#define INCL_GENERIC_BOX_COMPONENT(T) using GenericBoxComponent<T>::render, GenericBoxComponent<T>::onWindowChange, GenericBoxComponent<T>::shrinkRect, GenericBoxComponent<T>::loadBoxTexture, GenericBoxComponent<T>::mBoxTexture, GenericBoxComponent<T>::mBoxDestRect;


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

        static TTF_Font* sFont;
        static const std::filesystem::path sFontPath;

        SDL_Texture* mTextTexture = nullptr;
        SDL_Rect mTextDestRect;

        std::string mContent;
};

#define INCL_GENERIC_TEXT_COMPONENT(T) using GenericTextComponent<T>::deinitialize, GenericTextComponent<T>::render, GenericTextComponent<T>::onWindowChange, GenericTextComponent<T>::editContent, GenericTextComponent<T>::loadTextTexture, GenericTextComponent<T>::sFont, GenericTextComponent<T>::sFontPath, GenericTextComponent<T>::mTextTexture, GenericTextComponent<T>::mTextDestRect, GenericTextComponent<T>::mContent;


/**
 * @brief A text-inside-a-box component, essentially the combination of `GenericTextComponent<T>` and `GenericBoxComponent<T>`.
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

        static void initialize();
        static void deinitialize();

        void render() const override;
        void onWindowChange() override;

        void handleMouseEvent(SDL_Event const& event);
        static void handleCursor();

    protected:
        GenericButtonComponent(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState);

        virtual void onClick();

        const ComponentPreset kOnMouseOverPreset;
        SDL_Texture* mTextTextureOnMouseOver = nullptr;
        SDL_Texture* mBoxTextureOnMouseOver = nullptr;

        const GameState* kTargetGameState = nullptr;

    private:
        static SDL_Cursor* sOnMouseOutCursor;
        static SDL_Cursor* sOnMouseOverCursor;

        static bool sPrevAllMouseOutState;
        static bool sCurrAllMouseOutState;
        bool mIsMouseOut = true;
};

#define INCL_GENERIC_BUTTON_COMPONENT(T) using GenericButtonComponent<T>::initialize, GenericButtonComponent<T>::deinitialize, GenericButtonComponent<T>::render, GenericButtonComponent<T>::onWindowChange, GenericButtonComponent<T>::handleMouseEvent, GenericButtonComponent<T>::handleCursor, GenericButtonComponent<T>::onClick, GenericButtonComponent<T>::kOnMouseOverPreset, GenericButtonComponent<T>::mTextTextureOnMouseOver, GenericButtonComponent<T>::mBoxTextureOnMouseOver, GenericButtonComponent<T>::kTargetGameState;


template <typename T>
class GenericProgressBarComponent : public GenericBoxComponent<T> {
    public:
        INCL_MULTITON(T)
        INCL_GENERIC_COMPONENT(T)
        INCL_GENERIC_BOX_COMPONENT(T)

        virtual ~GenericProgressBarComponent() = default;

        void render() const override;
        void onWindowChange() override;
        void updateAnimation();
        void resetProgress();

        bool isActivated = false;
        bool isFinished = false;

    protected:
        GenericProgressBarComponent(SDL_FPoint const& center, ComponentPreset const& preset);

    private:
        static const double kProgressUpdateRateLimit;
        static const double kProgressUpdateRate;
        
        double mCurrProgress = 0;   // Exponential decay
        double mDecoyProgress = 0;   // Linear growth

        /**
         * The "partially constant" `SDL_Rect` to generate `mProgressDestRects`.
        */
        SDL_Rect mShrinkedBoxDestRect;

        /**
         * These `SDL_Rect`, when combined, occupy the same space as `mShrinkedBoxDestRect`.
        */
        std::pair<SDL_Rect, SDL_Rect> mProgressDestRects;
};

#define INCL_GENERIC_PROGRESS_BAR_COMPONENT(T) using GenericProgressBarComponent<T>::render, GenericProgressBarComponent<T>::onWindowChange, GenericProgressBarComponent<T>::updateAnimation, GenericProgressBarComponent<T>::resetProgress, GenericProgressBarComponent<T>::isActivated, GenericProgressBarComponent<T>::isFinished;


/* Derived implementations */

/**
 * @brief Represent the FPS displayed at all times.
*/
class FPSOverlay final : public Singleton<FPSOverlay>, public GenericTextBoxComponent<FPSOverlay> {
    public:
        INCL_GENERIC_TEXTBOX_COMPONENT(FPSOverlay)
        INCL_SINGLETON(FPSOverlay)

        FPSOverlay(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content);
        ~FPSOverlay() = default;

        static void deinitialize();

        static constexpr int kAnimationUpdateRate = config::components::fps_overlay::updateRate;
};


/**
 * @brief Represent the big but not-so-chunky title on the menu.
*/
class ExitText final : public Singleton<ExitText>, public GenericTextComponent<ExitText> {
    public:
        INCL_GENERIC_TEXT_COMPONENT(ExitText)
        INCL_SINGLETON(ExitText)

        ExitText(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content);
        ~ExitText() = default;

        static void deinitialize();
        void render() const override;
        void onWindowChange() override;
        void handleKeyBoardEvent(SDL_Event const& event);
    
    private:
        void registerProgress();

        const double kProgressUpdateRateLimit = config::components::exit_text::progressUpdateRateLimit;
        const double kProgressUpdateRate = config::components::exit_text::progressUpdateRate;

        double mCurrProgress;
};


class IngameDialogueBox : public Singleton<IngameDialogueBox>, public GenericBoxComponent<IngameDialogueBox> {
    enum class Status : unsigned char {
        kInactive,
        kUpdateInProgress,
        kUpdateComplete,
    };
    
    class BMPFont {
        struct Data_Glyph_Storage {
            int width;
            int advance;
        };

        struct Data_Glyph_Query {
            SDL_Rect srcRect;
            int advance;
        };

        class Map {
            public:
                Map() = default;
                inline ~Map() { clear(); }

                void insert(char c, Data_Glyph_Storage const& data);
                inline void clear() { mGlyphDataUMap.clear(); }
                Data_Glyph_Query operator[](char c) const;

                inline void setGlyphHeight(int height) { mGlyphHeight = height; }

            private:
                int mGlyphHeight;
                std::map<char, Data_Glyph_Storage> mGlyphDataUMap;
        };

        public:
            BMPFont(ComponentPreset const& preset);
            ~BMPFont();

            void load(TTF_Font* font);
            void clear() const;
            void render(char c) const;

            void setRenderTarget(SDL_Texture*& targetTexture);
            inline void setSpacing(SDL_Point spacing) { mSpacing = spacing; }

        private:
            std::string getChars() const; 
            void registerCharToMap(TTF_Font* font, char c);
            void registerCharToTexture(TTF_Font* font, char c) const;

            static constexpr auto sTextRenderMethod = TTF_RenderGlyph32_LCD;

            SDL_Texture* mTexture = nullptr;
            SDL_Point mSrcSize;
            Map mSrcRectsMap;

            SDL_Texture* mTargetTexture = nullptr;
            SDL_Point mTargetTextureSize;

            ComponentPreset mPreset;
            mutable SDL_Point mGlyphOrigin;
            SDL_Point mSpacing = { 0, 0 };
    };

    public:
        INCL_GENERIC_BOX_COMPONENT(IngameDialogueBox)
        INCL_SINGLETON(IngameDialogueBox)

        IngameDialogueBox(SDL_FPoint const& center, ComponentPreset const& preset);
        ~IngameDialogueBox();

        static void initialize();
        static void deinitialize();

        void render() const override;
        void onWindowChange() override;
        void handleKeyBoardEvent(SDL_Event const& event);

        void updateProgress();
        void editContent(std::string const& content);
        
    private:
        void terminate();
        void skip();

        static TTF_Font* sFont;
        static constexpr int sFontSize = config::components::dialogue_box::fontSize;
        static const std::filesystem::path sFontPath;

        Status mStatus = Status::kInactive;

        BMPFont mBMPFont;
        SDL_Texture* mTextTexture = nullptr;
        SDL_Rect mTextDestRect;
        static constexpr inline double mTextTextureOffsetRatio = config::components::dialogue_box::destOffsetRatio;

        std::string mContent;
        unsigned short int mCurrProgress;
};


// /**
//  * @brief Represent the avatar visible on the menu.
// */
// class MenuAvatar final : public Singleton<MenuAvatar> {
//     public:
//         INCL_SINGLETON(MenuAvatar)

//         MenuAvatar(tile::EntitiesTilesetData& tilesetData, const double destRectModifier);
//         ~MenuAvatar() = default;

//         void render() const;
//         void onWindowChange();

//     private:
//         SDL_Texture*& texture;
//         const SDL_Rect srcRect;
//         SDL_Rect destRect;
//         const double destRectModifier;
// };


/**
 * @brief Represent the animated background on the menu, based on the most recently visited in-game level.
*/
class MenuParallax final : public Singleton<MenuParallax> {
    public:
        INCL_SINGLETON(MenuParallax)

        MenuParallax();
        ~MenuParallax();

        void updateAnimation();
        void render() const;
        void onWindowChange();

    private:
        const double kAnimationUpdateRateLimit = 1;
        double mCurrAnimationUpdateCount;
        const double kAnimationUpdateRate = config::components::menu_parallax::animationUpdateRate;

        SDL_Texture* mTexture;
        SDL_Point mSrcSize;
        SDL_Point& mDestSize = globals::windowSize;
        std::pair<SDL_Rect, SDL_Rect> mSrcRects, mDestRects;
};


/**
 * @brief Represent the button components on the menu.
*/
class MenuButton final : public GenericButtonComponent<MenuButton> {
    public:
        INCL_GENERIC_BUTTON_COMPONENT(MenuButton)

        MenuButton(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState);
        ~MenuButton() = default;
};


/**
 * @brief Represent the big but not-so-chunky title on the menu.
*/
class MenuTitle final : public Singleton<MenuTitle>, public GenericTextComponent<MenuTitle> {
    public:
        INCL_GENERIC_TEXT_COMPONENT(MenuTitle)
        INCL_SINGLETON(MenuTitle)

        MenuTitle(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content);
        ~MenuTitle() = default;

        static void deinitialize();
};


/**
 * @brief Represent the message on the loading screen.
*/
class LoadingMessage final : public Singleton<LoadingMessage>, public GenericTextComponent<LoadingMessage> {
    public:
        INCL_GENERIC_TEXT_COMPONENT(LoadingMessage)
        INCL_SINGLETON(LoadingMessage)

        LoadingMessage(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content);
        ~LoadingMessage() = default;

        static void deinitialize();
};


/**
 * @brief Represent the progress bar on the loading screen.
*/
class LoadingProgressBar final : public Singleton<LoadingProgressBar>, public GenericProgressBarComponent<LoadingProgressBar> {
    public:
        INCL_GENERIC_PROGRESS_BAR_COMPONENT(LoadingProgressBar)
        INCL_SINGLETON(LoadingProgressBar)

        LoadingProgressBar(SDL_FPoint const& center, ComponentPreset const& preset);
        ~LoadingProgressBar() = default;

        static void deinitialize();
};


class GameOverTitle final : public Singleton<GameOverTitle>, public GenericTextComponent<GameOverTitle> {
    public:
        INCL_GENERIC_TEXT_COMPONENT(GameOverTitle)
        INCL_SINGLETON(GameOverTitle)

        GameOverTitle(SDL_FPoint const& center, ComponentPreset const& preset, std::string const& content);
        ~GameOverTitle() = default;

        static void deinitialize();
};


class GameOverButton final : public Singleton<GameOverButton>, public GenericButtonComponent<GameOverButton> {
    public:
        INCL_GENERIC_BUTTON_COMPONENT(GameOverButton)
        INCL_SINGLETON(GameOverButton)

        GameOverButton(SDL_FPoint const& center, ComponentPreset const& onMouseOutPreset, ComponentPreset const& onMouseOverPreset, std::string const& content, GameState* destState);
        ~GameOverButton() = default;

        static void deinitialize();
};


#endif