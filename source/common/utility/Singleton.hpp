#pragma once

#include <typeinfo>
#include "common/Logger.hpp"

namespace mce
{
    template <typename T>
    class Singleton
    {
    private:
        static T* instance;

    private:
        void internalCreateInstance()
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
}