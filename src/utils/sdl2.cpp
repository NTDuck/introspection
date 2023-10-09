#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include <auxiliaries/structs.h>
#include <utils/sdl2.h>


namespace utils {
    /**
     * @brief Creates an array of SDL_Rect representing frames from the given spritesheet.
     *
     * @param data a `TextureWrapperInitData` instance that encapsulates data required for `TextureWrapper` initialization.
     * @return a `std::vector` containing `data.spriteCounts` `SDL_Rect` elements.
     * @note `data.state` can be set to `NULL`.
     * 
     * @see <wrappers/texture.h> TextureWrapper.spriteMapping (std::unordered_map)
     * @see <auxiliaries/structs.h> TextureWrapperInitData (struct)
     * 
     * @todo `static_cast` to proper types.
     */
    std::vector<SDL_Rect> createRects(TextureWrapperInitData data) {
        std::vector<SDL_Rect> rects;
        Dat2hu spriteSize = {data.sheetSize._x / data.sheetDivs._x, data.sheetSize._y / data.sheetDivs._y};

        // `startFrame` is index therefore inverted
        for (int i = 0; i < data.spriteCounts; i++) {
            SDL_Rect rect = {spriteSize._y * data.spriteStart._y, spriteSize._x * data.spriteStart._x, spriteSize._x, spriteSize._y};
            rects.push_back(rect);

            data.spriteStart._y++;
            if (data.spriteStart._y < data.sheetDivs._y) continue;

            data.spriteStart._y = 0;
            data.spriteStart._x++;
        }

        return rects;
    }

    /**
     * @brief Check if a `SpriteState` is "permanent" or "temporary".
     * 
     * @param state a `SpriteState` value.
     * @return a boolean denoting whether the passed `SpriteState` is "permanent" or "temporary".
     * @see <auxiliaries/defs.h> SpriteState (enum)
    */
    bool isPermanent(SpriteState state) {
        return static_cast<int>(state) < 5;
    }
}