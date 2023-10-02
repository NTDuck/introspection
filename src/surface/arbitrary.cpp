#include <surface/arbitrary.h>


ArbitraryBackgroundImage::ArbitraryBackgroundImage() {
    surface = SDL_LoadBMP(path);
}

ArbitraryBackgroundImage::~ArbitraryBackgroundImage() {
    if (surface != nullptr) SDL_FreeSurface(surface);
}