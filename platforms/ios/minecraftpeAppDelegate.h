//
//  minecraftpeAppDelegate.h
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 ReMinecraftPE. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVAudioSession.h>

#import "PlatformDefinitions_iOS.h"

@class minecraftpeViewController;

@interface minecraftpeAppDelegate : NSObject <UIApplicationDelegate, AVAudioSessionDelegate> {
    UIWindow *window;
    minecraftpeViewController *viewController;
    AVAudioSession *audioSession;
    NSString *audioSessionSoundCategory;
    UIBackgroundTaskIdentifier bgTask NS_AVAILABLE_IOS(4_0);
}
@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) minecraftpeViewController *viewController;

- (void)setAudioEnabled:(BOOL)audioEnabled;

@end
