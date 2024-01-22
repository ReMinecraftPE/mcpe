//
//  PlatformDefinitions_iOS.h
//  Minecraft
//
//  Created by Brent on 12/19/23.
//
//

#import <Foundation/Foundation.h>

#ifndef NS_DEPRECATED_IOS
    #define NS_DEPRECATED_IOS(_iosIntro, _iosDep) NS_DEPRECATED_IPHONE(_iosIntro, _iosDep)
#endif