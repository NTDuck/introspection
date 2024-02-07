// #include <entities.hpp>

// #include <SDL.h>

// #include <meta.hpp>
// #include <auxiliaries.hpp>


// template <typename T>
// GenericAerialProjectile<T>::GenericAerialProjectile(SDL_Point const& destCoords, SDL_Point const& direction) : AbstractAnimatedDynamicEntity<T>(destCoords) {
//     // A part of overriden `onMoveStart()` is executed here once instead
//     mCurrVelocity = direction;
//     pNextVelocity = &mCurrVelocity;
//     if (mCurrVelocity.x) mFlip = (mCurrVelocity.x + 1) >> 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

//     resetAnimation(AnimationType::kAttack);   // Default animation type for projectile entities
//     onWindowChange();   // Must be manually called here to populate `destRect` from provided `destCoords`

//     initiateMove();
// }

// /**
//  * @warning Memory deallocation issues.
// */
// template <typename T>
// void GenericAerialProjectile<T>::onLevelChangeAll() {
//     // Multiton<T>::deinitialize();   // Projectile entities should not persist beyond the scope of a level
//     instances.clear();
// }

// /**
//  * @note Serves as the entry point.
// */
// template <typename T>
// void GenericAerialProjectile<T>::initiateLinearAttack(SDL_Point const& destCoords, SDL_Point const& direction) {
//     instantiate(destCoords, direction);
// }

// template <typename T>
// void GenericAerialProjectile<T>::updateAnimation() {
//     AbstractAnimatedDynamicEntity<T>::updateAnimation();

//     mAngle += 0.25;
//     if (mAngle == 360) mAngle = 0;
// }

// template <typename T>
// void GenericAerialProjectile<T>::onMoveStart(EntityStatusFlag flag) {
//     // ...
// }

// /**
//  * @note Should handle `EntityStatusFlag::kInvalidated` only.
// */
// template <typename T>
// void GenericAerialProjectile<T>::onMoveEnd(EntityStatusFlag flag) {
//     if (flag != EntityStatusFlag::kInvalidated) return;   // Probably should switch to `AnimationType::kDeath`

//     auto instance = instances.find(dynamic_cast<T*>(this));
//     if (instance == instances.end()) return;
//     instances.erase(instance);
// }


// template class GenericAerialProjectile<HauntedBookcaseProjectile>;