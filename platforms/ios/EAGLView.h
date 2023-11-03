//
//  EAGLView.h
//  GLBase
//
//  Created by Hideki Kozima on 10/12/24.
//  Copyright 2010 宮城大学 事業構想学部. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{
@private
    EAGLContext *context;
    
    // The pixel dimensions of the CAEAGLLayer.
    GLint framebufferWidth;
    GLint framebufferHeight;
    
    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
    GLuint defaultFramebuffer, colorRenderbuffer;
	GLuint depthRenderbuffer;
    
    float viewScale;
}

@property (nonatomic, retain) EAGLContext *context;

- (void)setFramebuffer;
- (BOOL)presentFramebuffer;

@end
