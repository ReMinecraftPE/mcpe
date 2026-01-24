#pragma once

#include <typeinfo>
#include "compat/LegacyCPP.hpp"
#include "common/Logger.hpp"

namespace mce
{
    template <typename T>
    class Singleton
    {
    private:
        static T* instance;

    private:
        static void internalCreateInstance()
        {
            T* newInstance = new T();
            T* oldInstance = instance;

            instance = newInstance;

            if (oldInstance)
                delete oldInstance;
        }

    public:
        static bool createInstance()
        {
            if (!instance)
            {
                internalCreateInstance();
                return true;
            }

            return false;
        }

        static void deleteInstance()
        {
            if (instance)
                delete instance;
            instance = nullptr;
        }
        
        static T& getInstance()
        {
            if (instance == nullptr)
            {
                LOG_E("The singleton instance doesn't exist");
                throw std::bad_cast();
            }
            
            return *instance;
        }

    public:
        Singleton()
        {
            if (instance != nullptr)
            {
                LOG_E("You attempted to create a singleton where one already exists");
                throw std::bad_cast();
            }
        }
    };

    template <typename T>
    T* Singleton<T>::instance = nullptr;
}
