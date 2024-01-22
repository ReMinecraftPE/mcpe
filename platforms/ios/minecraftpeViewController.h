//
//  minecraftpeViewController.h
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 ReMinecraftPE. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/CADisplayLink.h>

class AppPlatform_iOS;

@interface minecraftpeViewController : UIViewController
{
    EAGLContext *context;
    GLuint program;
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
    //BaseDialogController *dialog;
    
    @public
    bool suspended;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (readonly, nonatomic) AppPlatform_iOS* platform;
//@property (readonly, nonatomic) BaseDialogController *dialog;

- (void)startAnimation;
- (void)stopAnimation;

- (void)setAudioEnabled:(BOOL)audioEnabled;
- (void)showKeyboard;
- (void)hideKeyboard;
- (void)handleCharInput:(char)c;

- (int)width;
- (int)height;

@end
