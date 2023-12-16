#include <meta.hpp>

#include <game.hpp>
#include <interface.hpp>
#include <entities.hpp>


template <class T>
T* Singleton<T>::instance = nullptr;

template class Singleton<IngameInterface>;
template class Singleton<Game>;
template class Singleton<Player>;