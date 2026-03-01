//
//  NBCAppDelegate.h
//  NBCraft
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 NBCraft. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVAudioSession.h>

#import "PlatformDefinitions_iOS.h"

@class NBCViewController;

@interface NBCAppDelegate : NSObject <UIApplicationDelegate, AVAudioSessionDelegate> {
    UIWindow *window;
    NBCViewController *viewController;
    AVAudioSession *audioSession;
    NSString *audioSessionSoundCategory;
    UIBackgroundTaskIdentifier bgTask NS_AVAILABLE_IOS(4_0);
}
@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) NBCViewController *viewController;

- (void)setAudioEnabled:(BOOL)audioEnabled;

@end
