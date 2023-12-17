#include <meta.hpp>

#include <unordered_set>

#include <game.hpp>
#include <interface.hpp>
#include <entities.hpp>


template <class T>
T* Singleton<T>::instance = nullptr;

template class Singleton<IngameInterface>;
template class Singleton<Game>;
template class Singleton<Player>;


template <class T>
std::size_t Multiton<T>::Hasher::operator()(const T* pointer) const {
    return std::hash<int>()(pointer->destCoords.x) ^ std::hash<int>()(pointer->destCoords.y);
}

template <class T>
bool Multiton<T>::EqualityOperator::operator()(const T* first, const T* second) const {
    return *first == *second;
}

/**
 * @note This static member should be made public.
*/
template <class T>
std::unordered_set<T*, typename Multiton<T>::Hasher, typename Multiton<T>::EqualityOperator> Multiton<T>::instances;

template class Multiton<Player>;
template class Multiton<Teleporter>;
template class Multiton<Slime>;