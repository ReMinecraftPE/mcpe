//
//  minecraftpeAppDelegate.m
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 ReMinecraftPE. All rights reserved.
//

#import "minecraftpeAppDelegate.h"

#import "minecraftpeViewController.h"
#import "AppPlatform_iOS.h"

NSError *G_audioSessionError = nil;

@interface minecraftpeAppDelegate ()
    - (void)initAudio;
@end

@implementation minecraftpeAppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self initAudio];
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // Override point for customization after application launch.
    self.viewController = [[minecraftpeViewController alloc] initWithNibName:@"minecraftpeViewController" bundle:nil];
    if ([self.window respondsToSelector:@selector(setRootViewController:)])
    {
        [self.window setRootViewController:self.viewController];
    }
    else
    {
        [self.window addSubview:self.viewController.view];
    }
    NSLog(@"ViewController: %p\n", (void *)self.viewController);
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    NSLog(@"resign-active: %@\n", [NSThread currentThread]);
    [self.viewController stopAnimation];
    //AppPlatform_iOS *platform = [self.viewController platform];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
    
    void (^handler)() = ^()
    {
        [application endBackgroundTask:bgTask];
        bgTask = UIBackgroundTaskInvalid;
        NSLog(@"TIME RAN OUT %p\n", (void *)pthread_self()); // This originally used std::out, not sure why
    };
    bgTask = [application beginBackgroundTaskWithExpirationHandler:handler];
    viewController->suspended = YES;
    
    AppPlatform_iOS *platform = [self.viewController platform];
    platform->_fireAppSuspended();
    
    [application endBackgroundTask:bgTask];
    bgTask = UIBackgroundTaskInvalid;
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
    viewController->suspended = NO;
    
    AppPlatform_iOS *platform = [self.viewController platform];
    platform->_fireAppResumed();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    NSLog(@"become-active: %@\n", [NSThread currentThread]);
    [self.viewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */
    [self.viewController stopAnimation];
}

- (void)beginInterruption
{
    NSLog(@"beginInterruption\n");
    [self setAudioEnabled:NO];
}

- (void)endInterruption
{
    NSLog(@"endInterruption\n");
    [self setAudioEnabled:YES];
}

- (void)initAudio
{
    self->audioSession = [AVAudioSession sharedInstance];
    
    self->audioSessionSoundCategory = AVAudioSessionCategoryAmbient;
    [audioSession setCategory:audioSessionSoundCategory error:&G_audioSessionError];
    if (G_audioSessionError)
        NSLog(@"Warning; Couldn't init audio\n");
    
    [audioSession setDelegate:self];
    
    G_audioSessionError = nil;
    [audioSession setActive:YES error:&G_audioSessionError];
    if (G_audioSessionError)
        NSLog(@"Warning; Couldn't set audio active\n");
}

- (void)setAudioEnabled:(BOOL)audioEnabled
{
    NSLog(@"set-audio-enabled: :%d %@\n", audioEnabled, [NSThread currentThread]);
    if (audioEnabled)
    {
        [audioSession setCategory:audioSessionSoundCategory error:&G_audioSessionError];
        if (G_audioSessionError)
        {
            NSLog(@"ERROR - SoundManager: Unable to set the audio session category with error: %@\n", G_audioSessionError);
            return;
        }
        [audioSession setActive:YES error:&G_audioSessionError];
        if (G_audioSessionError)
        {
            NSLog(@"ERROR - SoundManager: Unable to set the audio session state to YES with error: %@\n", G_audioSessionError);
            return;
        }
    }
    else
    {
        NSLog(@"INFO - SoundManager: OpenAL Inactive\n");
        [audioSession setActive:NO error:&G_audioSessionError];
        if (G_audioSessionError)
        {
            NSLog(@"ERROR - SoundManager: Unable to set the audio session state to NO with error: %@\n", G_audioSessionError);
            return;
        }
    }
    [self.viewController setAudioEnabled:audioEnabled];
}

- (void)dealloc
{
#if !__has_feature(objc_arc)
    [self.window release];
    [self.viewController release];
    
    [super dealloc];
#endif
}

@end
