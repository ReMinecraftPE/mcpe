//
//  AppContext.hpp
//  Minecraft
//
//  Created by Brent on 10/13/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
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
    std::string doRender;
};
