//
//  AppContext.hpp
//  NBCraft
//
//  Created by Brent on 10/13/23.
//  Copyright (c) 2023 NBCraft. All rights reserved.
//

#pragma once

#include <string>

#include "client/app/AppPlatform.hpp"

class AppContext
{
public:
    AppContext()
    {
        platform = nullptr;
    }
    
    AppPlatform *platform;
    bool doRender;
};
