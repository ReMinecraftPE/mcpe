//
//  minecraftpeAppDelegate.h
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class minecraftpeViewController;

@interface minecraftpeAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) minecraftpeViewController *viewController;

@end
