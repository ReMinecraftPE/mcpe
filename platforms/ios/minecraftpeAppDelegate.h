//
//  minecraftpeAppDelegate.h
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVAudioSession.h>

@class minecraftpeViewController;

@interface minecraftpeAppDelegate : NSObject <UIApplicationDelegate, AVAudioSessionDelegate> {
    UIWindow *window;
    minecraftpeViewController *viewController;
    AVAudioSession *audioSession;
    NSString *audioSessionSoundCategory;
}
@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) minecraftpeViewController *viewController;

- (void)setAudioEnabled:(BOOL)audioEnabled;

@end
