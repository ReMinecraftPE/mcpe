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

- (void)startAnimation;
- (void)stopAnimation;

- (void)setAudioEnabled:(BOOL)audioEnabled;

- (int)width;
- (int)height;

@end
