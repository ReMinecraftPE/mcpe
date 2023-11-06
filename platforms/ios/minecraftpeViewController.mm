//
//  minecraftpeViewController.m
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
//

#import "minecraftpeViewController.h"

#import <QuartzCore/QuartzCore.h>

#import "AppPlatform_iOS.h"

#include <string>
#include <vector>

#include "client/app/App.hpp"
#include "client/app/NinecraftApp.hpp"
#include "AppContext.hpp"
#include "client/player/input/Multitouch.hpp"

#include "client/gui/screens/ProgressScreen.hpp"

#include "EAGLView.h"

extern bool g_bIsMenuBackgroundAvailable;

NSThread *G_drawFrameThread = nullptr;

@interface minecraftpeViewController () {
    GLuint _program;
    
    float _rotation;
    
    GLuint _vertexArray;
    GLuint _vertexBuffer;
    
    App *_app;
    UITouch *_touchMap[12];
    //BaseDialogController *_dialog;
    int _dialogResultStatus;
    std::vector<std::string> _dialogResultStrings;
    AppContext *_context;
    AppPlatform_iOS *_platform;
    float viewScale;
}
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, retain) CADisplayLink *displayLink;
//@property (strong, nonatomic) IASKAppSettingsViewController *appSettingsViewController;

- (void)initView;
- (int)_startTrackingTouch:(UITouch *)touch;
- (int)_stopTrackingTouch:(UITouch *)touch;
- (int)_getIndexForTouch:(UITouch *)touch;

@end

@implementation minecraftpeViewController

@synthesize animating, context, displayLink;

- (int)width
{
    CGRect bounds;
    UIScreen *screen = [UIScreen mainScreen];
    if (screen)
    {
        bounds = screen.bounds;
    }
    else
    {
        bounds.origin.x = 0, bounds.origin.y = 0;
        bounds.size.width = 0, bounds.size.height = 0;
    }
    if (bounds.size.width > bounds.size.height)
        bounds.size.height = bounds.size.width;
    return bounds.size.width * self->viewScale;
}

- (int)height
{
    CGRect bounds;
    UIScreen *screen = [UIScreen mainScreen];
    if (screen)
    {
        bounds = screen.bounds;
    }
    else
    {
        bounds.origin.x = 0, bounds.origin.y = 0;
        bounds.size.width = 0, bounds.size.height = 0;
    }
    if (bounds.size.width > bounds.size.height)
        bounds.size.height = bounds.size.width;
    return bounds.size.height * self->viewScale;
}

- (void)updateDrawSize
{
    // NOTE: Swapping width & height because of device orientation
    // I guess when the device is sideways, the view doesn't rotate to be upright?
    Minecraft::width = self.height;
    Minecraft::height = self.width;
    self->_app->sizeUpdate(self.height, self.width);
}

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    // Moved to viewDidLoad - wasn't working here & supposedly makes no difference
}

- (void)initView
{
    self->viewScale = self.view.contentScaleFactor;
    App *app = self->_app;
    
    app->m_pPlatform = self->_context->platform;
    app->init();
    
    /*var1 = app->field_10;
    app->var3 = *self->_context;
    if ( var1 )
    {
        ((void (__fastcall *)(App *))var0[15])(app);
    }
    else
    {
        ((void (__fastcall *)(App *))var0[14])(app);
        app->field_10 = 1;
    }*/
    
    [self updateDrawSize];
    // Update draw size when device orientation changes (this accounts for typical view resizes)
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateDrawSize) name:UIDeviceOrientationDidChangeNotification object:nil];
    
    /*Minecraft *mc = (Minecraft *)app;
    mc->selectLevel("TestWorld", "Test", (int)"iOS");
    mc->hostMultiplayer();
    mc->setScreen(new ProgressScreen);*/
}

- (void)drawFrame
{
    NSThread *currentThread = [NSThread currentThread];
    NSThread *thread = G_drawFrameThread;
    if (!G_drawFrameThread)
    {
        G_drawFrameThread = currentThread;
        thread = currentThread;
    }
    if (currentThread != thread)
    {
        NSLog(@"Warning! draw-frame thread changed (%@ -> %@)\n", G_drawFrameThread, thread);
        G_drawFrameThread = thread;
    }
    
    [(EAGLView *)self.view setFramebuffer];
    
    self->_app->update();
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_4_0 && __IPHONE_OS_VERSION_MIN_REQUIRED <= __IPHONE_12_0
    int attachments[3] = { 36096 };
    glDiscardFramebufferEXT(36160, 1, attachments);
#endif
    
    [(EAGLView *)self.view presentFramebuffer];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    //EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    //
    //if (!aContext)
    //{
    //    aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    //}
    
	EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
	self.context = aContext;
	
    [(EAGLView *)self.view setContext:context];
    [(EAGLView *)self.view setFramebuffer];
    
    //if ([context API] == kEAGLRenderingAPIOpenGLES2)
    //    [self loadShaders];
    
    animating = FALSE;
    animationFrameInterval = 1;
    self.displayLink = nil;
    
    g_bIsMenuBackgroundAvailable = true;
    
    AppPlatform_iOS *platform = new AppPlatform_iOS();
    self->_platform = platform;
    
    AppContext *ctx = new AppContext();
    ctx->platform = platform;
    self->_context = ctx;
    self->viewScale = 1.0;
    
    NSString *dir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, 1u, YES) objectAtIndex:0];
    App *app = new NinecraftApp();
    self->_app = app;
    ctx->doRender = [dir UTF8String];
    
    [self initView];
}

- (void)dealloc
{
    //if (program)
    //{
    //    glDeleteProgram(program);
    //    program = 0;
    //}
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
	[super viewDidUnload];
	
    //if (program)
    //{
    //    glDeleteProgram(program);
    //    program = 0;
    //}
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating)
    {
        CADisplayLink *aDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        NSLog(@"start-animation: %@\n", [NSThread currentThread]);
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
        NSLog(@"stop-animation: %@\n", [NSThread currentThread]);
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown &&
            interfaceOrientation != UIInterfaceOrientationPortrait
            );
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        int touchIndex = [self _startTrackingTouch: touch];
        if (touchIndex > -1)
        {
            CGPoint point;
            float posX, posY;
            if (touch)
            {
                point = [touch locationInView:self.view];
            }
            else
            {
                point.x = 0, point.y = 0;
                //point2.x = 0, point2.y = 0;
            }
            posX = viewScale * point.x;
            posY = viewScale * point.y;
            
            Mouse::feed(BUTTON_LEFT, true, posX, posY);
            Multitouch::feed(BUTTON_LEFT, true, posX, posY, touchIndex);
        }
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        int touchIndex = [self _getIndexForTouch: touch];
        if (touchIndex > -1)
        {
            CGPoint point;
            float posX, posY;
            if (touch)
            {
                point = [touch locationInView:self.view];
            }
            else
            {
                point.x = 0, point.y = 0;
                //point2.x = 0, point2.y = 0;
            }
            posX = viewScale * point.x;
            posY = viewScale * point.y;
            
            Mouse::feed(BUTTON_NONE, false, posX, posY);
            Multitouch::feed(BUTTON_NONE, false, posX, posY, touchIndex);
        }
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        int touchIndex = [self _stopTrackingTouch: touch];
        if (touchIndex > -1)
        {
            CGPoint point;
            float posX, posY;
            if (touch)
            {
                point = [touch locationInView:self.view];
            }
            else
            {
                point.x = 0, point.y = 0;
                //point2.x = 0, point2.y = 0;
            }
            posX = viewScale * point.x;
            posY = viewScale * point.y;
            
            Mouse::feed(BUTTON_LEFT, false, posX, posY);
            Multitouch::feed(BUTTON_LEFT, false, posX, posY, touchIndex);
        }
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self touchesEnded:touches withEvent:event];
}

- (int)_startTrackingTouch:(UITouch *)touch
{
    for (int i = 0; i < 12; i++)
    {
        UITouch __strong **touchMap = self->_touchMap;
        if (!touchMap[i])
        {
            touchMap[i] = touch;
            return i;
        }
    }
    
    return -1;
}

- (int)_stopTrackingTouch:(UITouch *)touch
{
    for (int i = 0; i < 12; i++)
    {
        UITouch __strong **touchMap = self->_touchMap;
        if (touchMap[i] == touch)
        {
            touchMap[i] = nil;
            return i;
        }
    }
    
    return -1;
}


- (int)_getIndexForTouch:(UITouch *)touch
{
    for (int i = 0; i < 12; i++)
    {
        UITouch __strong **touchMap = self->_touchMap;
        if (touchMap[i] == touch)
            return i;
    }
    
    return -1;
}

@end
