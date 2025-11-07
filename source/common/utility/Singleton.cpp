#include "Singleton.hpp"

using namespace mce;

template <typename T>
T* Singleton<T>::instance = nullptr;