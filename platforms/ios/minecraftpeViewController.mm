//
//  minecraftpeViewController.m
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 ReMinecraftPE. All rights reserved.
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
#include "ShowKeyboardView.h"

extern bool g_bIsMenuBackgroundAvailable;

NSThread *G_drawFrameThread = nil;

@interface minecraftpeViewController () {
    GLuint _program;
    
    float _rotation;
    
    GLuint _vertexArray;
    GLuint _vertexBuffer;
    
    Minecraft *_app;
    AppContext *_context;
    AppPlatform_iOS *_platform;
    UITouch *_touchMap[12];
    int _dialogResultStatus;
    std::vector<std::string> _dialogResultStrings;
    ShowKeyboardView *_keyboardView;
    //ThreadSafeQueue<std::function<void ()> > mMainQueue;
    //ImagePickingCallback *mPickingCallback
    float viewScale;
    //GameControllerHandler_iOS *_gameControllerHandler;
    //UIPopoverController *_imagePickerPopoverController;
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
@synthesize platform = _platform;

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
    Minecraft::width = self.height; // drawWidth
    Minecraft::height = self.width; // drawHeight
	Minecraft::setRenderScaleMultiplier(self->viewScale);
    self->_app->sizeUpdate(self.height / self->viewScale, self.width / self->viewScale); // windowWidth, windowHeight
    NSLog(@"Updated draw size to %d, %d\n", self.height, self.width);
}

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    // Moved to viewDidLoad - wasn't working here & supposedly makes no difference
}

- (void)drawFrame
{
    if (!suspended)
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
    
        // In 0.12.2 we loop through self->mMainQueue & call some functions, then run _app->update() after each. Not sure what it's really doing though...
        _app->update();
    
        BOOL success = [(EAGLView *)self.view presentFramebuffer];
        if (!success)
        {
            NSLog(@"Failed to present renderbuffer object %x\n", glCheckFramebufferStatus(GL_RENDERBUFFER));
        }
    }
}

- (void)viewDidLoad
{
    //_keyboardView = [[ShowKeyboardView alloc] init];
    [super viewDidLoad];
    
    // Setup logging
    Logger::setSingleton(new Logger);
    
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
    
    AppPlatform_iOS *platform = new AppPlatform_iOS(self);
    self->_platform = platform;
    
    AppContext *ctx = new AppContext();
    ctx->platform = platform;
    self->_context = ctx;
    self->viewScale = 1.0;
    self->suspended = NO;
    
    NSString *dir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, 1u, YES) objectAtIndex:0];
    NinecraftApp *app = new NinecraftApp();
    app->m_externalStorageDir = [dir UTF8String];
    self->_app = app;
    
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
    
#if !__has_feature(objc_arc)
    if (_app) delete _app;
    if (_platform) delete _platform;
    // This is what was done, but should we really be doing this?
    if (_touchMap) delete[] (UITouch **)_touchMap;
    
    [self.context release];
    
    [super dealloc];
#endif
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

- (void)initView
{
    _keyboardView = [[ShowKeyboardView alloc] initWithMinecraft:self->_app];
    
    if ([self.view respondsToSelector:@selector(contentScaleFactor)])
    {
        self->viewScale = [self.view contentScaleFactor];
    }
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
     mc->selectLevel("TestWorld", "Test", (int)"iOS");*/
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
        NSLog(@"Set animationFrameInterval to %d\n", animationFrameInterval);
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (![self isViewLoaded])
        NSLog(@"Warning! Tried to startAnimation before view was loaded!\n");
    
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
    if (![self isViewLoaded])
        NSLog(@"Warning! Tried to stopAnimation before view was loaded!\n");
    
    if (animating)
    {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
        NSLog(@"stop-animation: %@\n", [NSThread currentThread]);
        
        // HACK: applicationWillTerminate simply does not serve its purpose
        _app->saveOptions();
    }
}

- (void)setAudioEnabled:(BOOL)audioEnabled
{
    if (audioEnabled)
        _platform->getSoundSystem()->startEngine();
    else
        _platform->getSoundSystem()->stopEngine();
}

- (void)showKeyboard
{
    for (UIView *view in self.view.subviews)
    {
        if ([view isKindOfClass:[ShowKeyboardView class]])
        {
            [((ShowKeyboardView*)view) showKeyboard];
        }
    }
    [self.view insertSubview:_keyboardView atIndex:0];
    [_keyboardView showKeyboard];
}

- (void)hideKeyboard
{
    for (UIView *view in self.view.subviews)
    {
        if ([view isKindOfClass:[ShowKeyboardView class]])
        {
            [((ShowKeyboardView*)view) hideKeyboard];
            [((ShowKeyboardView*)view) removeFromSuperview];
        }
    }
}

- (void)handleCharInput:(char)c
{
    _app->handleCharInput(c);
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
        if (!_touchMap[i])
        {
            _touchMap[i] = touch;
            return i;
        }
    }
    
    return -1;
}

- (int)_stopTrackingTouch:(UITouch *)touch
{
    for (int i = 0; i < 12; i++)
    {
        if (_touchMap[i] == touch)
        {
            _touchMap[i] = nil;
            return i;
        }
    }
    
    return -1;
}


- (int)_getIndexForTouch:(UITouch *)touch
{
    for (int i = 0; i < 12; i++)
    {
        if (_touchMap[i] == touch)
            return i;
    }
    
    return -1;
}

@end
