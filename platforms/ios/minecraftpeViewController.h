//
//  minecraftpeViewController.h
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <QuartzCore/CADisplayLink.h>

class AppPlatform_iOS;

@interface minecraftpeViewController : UIViewController
{
    EAGLContext *context;
    GLuint program;
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (readonly, nonatomic) AppPlatform_iOS* platform;

- (void)startAnimation;
- (void)stopAnimation;

- (void)setAudioEnabled:(BOOL)audioEnabled;
- (void)showKeyboard;
- (void)hideKeyboard;
- (void)handleCharInput:(char)c;

- (int)width;
- (int)height;

@end
