#ifndef META_H
#define META_H

#include <functional>
#include <unordered_set>
#include <type_traits>

#include <SDL.h>

#include <auxiliaries.hpp>


/**
 * @brief Flow 1: Abstract Hierarchy
 * ┌────────────────────┐
 * │ PolymorphicBase<T> │
 * └┬───────────────────┘
 *  │
 *  │ ┌──────────────┐
 *  ├─► Singleton<T> ├───────────────────────────────────────────┐
 *  │ └┬─────────────┘                                           │
 *  │  │                                                         │
 *  │  │ ┌──────┐                   ┌─────────────────┐          │
 *  │  ├─► Game │                 ┌─► IngameInterface │          │
 *  │  │ └──────┘                 │ └─────────────────┘          │
 *  │  │                          │                              │
 *  │  │ ┌──────────────────────┐ │ ┌───────────────┐            │
 *  │  ├─► AbstractInterface<T> ├─┼─► MenuInterface │            │
 *  │  │ └──────────────────────┘ │ └───────────────┘            │
 *  │  │                          │                              │
 *  │  │ ┌────────────┐           │ ┌──────────────────┐         │
 *  │  ├─► MenuAvatar │           └─► LoadingInterface │         │
 *  │  │ └────────────┘             └──────────────────┘         │
 *  │  │                                                         │
 *  │  │ ┌────────────────────────┐                              │
 *  │  └─► MenuAnimatedBackground │                              │
 *  │    └────────────────────────┘                              │
 *  │                                                            │
 *  │ ┌─────────────┐                                            │
 *  └─► Multiton<T> │                                            │
 *    └┬────────────┘                                            │
 *     │                                                         │
 *     │ ┌───────────────────┐                                   │
 *     ├─► AbstractEntity<T> │                                   │
 *     │ └┬──────────────────┘                                   │
 *     │  │                                                      │
 *     │  │ ┌───────────────────────────┐ ┌────────────┐         │
 *     │  └─► AbstractAnimatedEntity<T> ├─► Teleporter │         │
 *     │    └┬──────────────────────────┘ └────────────┘         │
 *     │     │                                                   │
 *     │     │ ┌──────────────────────────────────┐   ┌────────┐ │
 *     │     └─► AbstractAnimatedDynamicEntity<T> ├─┬─► Player ◄─┤
 *     │       └──────────────────────────────────┘ │ └────────┘ │
 *     │                                            │            │
 *     │ ┌─────────────────────┐                    │ ┌───────┐  │
 *     └─► GenericComponent<T> │                    └─► Slime │  │
 *       └┬────────────────────┘                      └───────┘  │
 *        │                                                      │
 *        │ ┌─────────────────────────┐                          │
 *        ├─► GenericTextComponent<T> ├───────┐                  │
 *        │ └┬────────────────────────┘       │                  │
 *        │  │                                │                  │
 *        │  │ ┌───────────┐                  │                  │
 *        │  ├─► MenuTitle ◄──────────────────┼──────────────────┤
 *        │  │ └───────────┘                  │                  │
 *        │  │                                │                  │
 *        │  │ ┌────────────────┐             │                  │
 *        │  └─► LoadingMessage ◄─────────────┼──────────────────┤
 *        │    └────────────────┘             │                  │
 *        │                                   │                  │
 *        │ ┌────────────────────────┐        │                  │
 *        └─► GenericBoxComponent<T> ├────────┤                  │
 *          └┬───────────────────────┘        │                  │
 *           │                                │                  │
 *           │ ┌────────────────────────────┐ │                  │
 *           │ │ GenericTextBoxComponent<T> ◄─┘                  │
 *           │ └┬───────────────────────────┘                    │
 *           │  │                                                │
 *           │  │ ┌───────────────────────────┐ ┌────────────┐   │
 *           │  └─► GenericButtonComponent<T> ├─► MenuButton │   │
 *           │    └───────────────────────────┘ └────────────┘   │
 *           │                                                   │
 *           │ ┌────────────────────────────────┐                │
 *           └─► GenericProgressBarComponent<T> │                │
 *             └┬───────────────────────────────┘                │
 *              │                                                │
 *              │ ┌────────────────────┐                         │
 *              └─► LoadingProgressBar ◄─────────────────────────┘
 *                └────────────────────┘
*/


/**
 * @brief An abstract base with certain operators deleted. Required for implementation of derived classes `Singleton<T>` and `Multiton<T>`.
*/
template <typename T>
class PolymorphicBase {
    public:
        PolymorphicBase(PolymorphicBase const&) = delete;
        PolymorphicBase& operator=(PolymorphicBase const&) = delete;
        PolymorphicBase(PolymorphicBase&&) = delete;
        PolymorphicBase& operator=(PolymorphicBase&&) = delete;

    protected:
        explicit PolymorphicBase() = default;
        virtual ~PolymorphicBase() = default;
};


/**
 * @brief A standard Singleton template class.
*/
template <typename T>
class Singleton : virtual public PolymorphicBase<T> {
    public:
        template <typename... Args>
        static T* instantiate(Args&&... args) {
            if (instance == nullptr) instance = new T(std::forward<Args>(args)...);
            return instance;
        }

        template <typename... Args>
        static T* instantiate(std::tuple<Args...> const& tuple) {
            if (instance == nullptr) instance = fromTuple(tuple, std::index_sequence_for<Args...>());
            return instance;
        }

        static void deinitialize() {
            delete instance;
            instance = nullptr;
        }

        template <typename Callable, typename... Args>
        static void invoke(Callable&& callable, Args&&... args) {
            if (instance == nullptr) return;
            std::invoke(std::forward<Callable>(callable), *instance, std::forward<Args>(args)...);
        }

    protected:
        static T* instance;

    private:
        template <std::size_t... Indices, typename Tuple>
        static T* fromTuple(Tuple const& tuple, std::index_sequence<Indices...>) {
            return new T(std::get<Indices>(tuple)...);
        }
};

#define INCL_SINGLETON(T) using Singleton<T>::instantiate, Singleton<T>::invoke, Singleton<T>::deinitialize, Singleton<T>::instance;


/**
 * @brief An adapted Multiton template class that governs instances via a `std::unordered_set` instead of a `std::unordered_map`.
*/
template <typename T>
class Multiton : virtual public PolymorphicBase<T> {
    public:      
        template <typename... Args>
        static T* instantiate(Args&&... args) {
            auto instance = new T(std::forward<Args>(args)...);
            instances.emplace(instance);
            return instance;
        }

        template <typename... Args>
        static T* instantiate(std::tuple<Args...> const& tuple) {
            auto instance = fromTuple(tuple, std::index_sequence_for<Args...>());
            instances.emplace(instance);
            return instance;
        }

        template <size_t N, typename... Args>
        static void instantiate(std::array<std::tuple<Args...>, N> const& container) {
            for (const auto& args : container) T::instantiate(args);
        }

        /**
         * @bug Somehow this yields weird segmentation faults. Might consider switching to smart pointers.
         * @note Segmentation faults are, weirdly, unencountered as of this commit. Perhaps due to the transitioning to `dynamic_cast`?
        */
        static void deinitialize() {
            // Somehow this yields weird segfaults. Consider switching to smart pointers?
            for (auto& instance : instances) if (instance != nullptr) delete instance;
            instances.clear();
        }

        /**
         * @brief Variadically call `method` on each instance of derived class `T` with the same parameters `args`.
         * @note Defined here to avoid lengthy explicit template instantiation.
        */
        template <typename Callable, typename... Args>
        static void invoke(Callable&& callable, Args&&... args) {
            for (auto& instance : instances) if (instance != nullptr) std::invoke(std::forward<Callable>(callable), *instance, std::forward<Args>(args)...);
        }

        static std::unordered_set<T*> instances;

    protected:
        /**
         * @see https://stackoverflow.com/questions/3747066/c-cannot-convert-from-base-a-to-derived-type-b-via-virtual-base-a
         * @remark You can't use `static_cast` in this situation because the compiler doesn't know the offset of B relative to A at compile time. The offset must be calculated at run-time based on the exact type of the most derived object. Therefore you must use `dynamic_cast`.
         * @note Compile-time `static_cast` yields "error: cannot convert from pointer to base class `Multiton<T>` to pointer to derived class `T` because the base is virtual". Therefore run-time `dynamic_cast` must be used instead.
        */
        virtual ~Multiton() override {
            instances.erase(dynamic_cast<T*>(this));   // remove from `instances`
        }

    private:
        template <std::size_t... Indices, typename Tuple>
        static T* fromTuple(Tuple const& tuple, std::index_sequence<Indices...>) {
            return new T(std::get<Indices>(tuple)...);
        }
};

#define INCL_MULTITON(T) using Multiton<T>::instantiate, Multiton<T>::deinitialize, Multiton<T>::invoke, Multiton<T>::instances;


/* Internal initialization of static members */

template <typename T>
T* Singleton<T>::instance = nullptr;

template <typename T>
std::unordered_set<T*> Multiton<T>::instances;


#endif