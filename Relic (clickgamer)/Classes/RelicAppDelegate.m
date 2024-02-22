//
//  RelicAppDelegate.m
//  Relic
//
//  Created by Paul Senzee on 1/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "RelicAppDelegate.h"
#import "RelicViewController.h"
#import "CrystalSession.h"

bool   g_bInBackground               = false,
       g_activateCrystalAtProfile    = false,
       g_crystalIsActive             = false;

void  *g_crystalActivateContext      = 0;
void (*g_crystalActivated)(void *)   = 0;

void  *g_crystalDeactivateContext    = 0;
void (*g_crystalDeactivated)(void *) = 0;

void  *g_alertDismissedContext       = 0;
void (*g_alertDismissed)(void *)     = 0;

id     g_App                         = nil;

bool   g_doActivateCrystal           = false;

int    g_iOSMajorVersion             = -1;
int    g_iOSMinorVersion1            = -1;
int    g_iOSMinorVersion2            = -1;

extern bool        WriteUserTextFile(const char *filename, const char *data);
extern bool        GetPlatformIsiPad();
extern const char *ReadUserTextFile(const char *filename);
extern void        Autosave();
extern void        UnloadAll();

void GetVersion()
{
	NSArray *versionComponents = [[UIDevice currentDevice].systemVersion componentsSeparatedByString:@"."];
	NSString *majorVersionNumberString = (NSString*)[versionComponents objectAtIndex:0];
	g_iOSMajorVersion = [majorVersionNumberString intValue];	
	NSString *minorVersion1NumberString = (NSString*)[versionComponents objectAtIndex:1];
	g_iOSMinorVersion1 = [minorVersion1NumberString intValue];	
	NSString *minorVersion2NumberString = (NSString*)[versionComponents objectAtIndex:2];
	g_iOSMinorVersion2 = [minorVersion2NumberString intValue];		
}

int GetiOSMajorVersion()
{
	if (g_iOSMajorVersion <= 0)
	    GetVersion();
    return g_iOSMajorVersion;	
}

int GetiOSMinorVersion1()
{
	if (g_iOSMinorVersion1 <= 0)
	    GetVersion();
    return g_iOSMinorVersion1;	
}

int GetiOSMinorVersion2()
{
	if (g_iOSMinorVersion2 <= 0)
	    GetVersion();
    return g_iOSMinorVersion2;	
}

bool IsVersionGTE(int major, int minor1, int minor2)
{
    if (GetiOSMajorVersion() > major)
		return true;
	if (GetiOSMajorVersion() < major)
		return false;
    if (GetiOSMinorVersion1() > minor1)
		return true;
	if (GetiOSMinorVersion1() < minor1)
		return false;
    if (GetiOSMinorVersion2() >= minor2)
		return true;
	return false;
}

bool ShouldShowLogoSplash()
{
	return (GetPlatformIsiPad() && GetiOSMajorVersion() < 4);
}

void SetAlertDismissedCallback(void (*fn)(void *), void *context)
{
	g_alertDismissed           = fn;
	g_alertDismissedContext    = context;
}

void SetCrystalDeactivatedCallback(void (*fn)(void *), void *context)
{
	g_crystalDeactivated       = fn;
	g_crystalDeactivateContext = context;
}

void SetCrystalActivatedCallback(void (*fn)(void *), void *context)
{
	g_crystalActivated         = fn;
	g_crystalActivateContext   = context;
}

bool IsCrystalActive()
{
    return g_crystalIsActive;
}

bool ShouldCrystalActivate()
{
    return g_doActivateCrystal;	
}

void SetCrystalIsActive(bool active)
{
	if (g_crystalIsActive != active)
	{
	    g_crystalIsActive = active;
		if (active && g_crystalActivated)
			g_crystalActivated(g_crystalActivateContext);
		else if (!active && g_crystalDeactivated)
			g_crystalDeactivated(g_crystalDeactivateContext);
	}
}

void ActivateCrystaliPad()
{
	if (g_activateCrystalAtProfile)
	{
		g_activateCrystalAtProfile = false;
        [CrystalSession activateCrystalUIAtProfile];
	}
	else
	    [CrystalSession activateCrystalUI];
	SetCrystalIsActive(true);	
}

void DeactivateCrystaliPad()
{
	[CrystalSession deactivateCrystalUI];
	SetCrystalIsActive(false);	
}

void StartCrystal()
{
	[g_App startCrystal];
}

void ShowCrystalLeaderboards()
{
	[CrystalSession activateCrystalUIAtLeaderboards];
	SetCrystalIsActive(true);	
}

void ShowCrystalAchievements()
{
	[CrystalSession activateCrystalUIAtAchievements];	
	SetCrystalIsActive(true);
}

void PostAchievement(const char *achievementId, const char *description)
{
	NSString *nsAchievementId = [[NSString alloc] initWithUTF8String:achievementId];
	NSString *nsDescription   = [[NSString alloc] initWithUTF8String:description];
	[CrystalSession postAchievement:nsAchievementId wasObtained:YES withDescription:nsDescription alwaysPopup:NO];
    [nsAchievementId release];
	[nsDescription release];
}

void PostLeaderboard(const char *leaderboardId, double value)
{
	NSString *nsLeaderboardId = [[NSString alloc] initWithUTF8String:leaderboardId];
	[CrystalSession postLeaderboardResult:value forLeaderboardId:nsLeaderboardId lowestValFirst:NO];
    [nsLeaderboardId release];
}

void RemoveSplashScreen()
{
	[[NSNotificationCenter defaultCenter]
	 postNotificationName: @"RemoveSplashScreen" object: nil];	
}

extern void SetupGame();

@implementation RelicAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize logoMoviePlayer;

- (void)showSplash
{
	[self performSelector:@selector(removeSplash) withObject:nil afterDelay:2.0];
}

- (void)splashScreenFinishedWithActivateCrystal:(BOOL)activateCrystal
{
    if (activateCrystal)
	{
		g_activateCrystalAtProfile = true;
	    g_doActivateCrystal = true;
	}
	
//  [[UIDevice currentDevice] setOrientation:UIInterfaceOrientationLandscapeRight];	
	
  //[CrystalSession activateCrystalUIAtProfile];
	[self startApp];
}

- (void)startCrystalSplash
{
	if (IsVersionGTE(3, 2, 0) && !ReadUserTextFile("crystal_splash.flag"))
	{
//		[[UIDevice currentDevice] setOrientation:UIInterfaceOrientationLandscapeRight];	
		
        [CrystalSession displaySplashScreen];
		WriteUserTextFile("crystal_splash.flag", "true");
	}
	else
	{
		[self startApp];
	}
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
	[alertView release];
	if (g_alertDismissed != 0)
		g_alertDismissed(g_alertDismissedContext);
}

- (void)startApp
{
	NSLog(@"Start app");
	
	[self.window addSubview:self.viewController.view];
	
//  [[UIDevice currentDevice] setOrientation:UIInterfaceOrientationLandscapeRight];		
	
    SetupGame();
}

- (void)startCrystal
{
	NSLog(@"Start Crystal UI");
	if (g_activateCrystalAtProfile)
	    [CrystalSession activateCrystalUIAtProfile];
	else
		[CrystalSession activateCrystalUI];
	g_activateCrystalAtProfile = false;
	g_doActivateCrystal = false;		
	SetCrystalIsActive(true);	
}

- (void)startLogoMovieplayer
{
	NSLog(@"Starting logo movie player");
	
	if (ShouldShowLogoSplash())
	{
      //[splashView release];
		[self logoMovieDidFinish];
      //[self showSplash];
	    return;
	}
	
	// Start the logo movie player	
	
	// Check the current orientation and select the most appropriate video
	// Default to the portrait version
	NSString *movieName = nil;
	
	// Test forcing the orientation to portrait
	//UIInterfaceOrientation orientation = UIInterfaceOrientationPortrait;
	
	// Test forcing the orientation to landscape
	//UIInterfaceOrientation orientation = UIInterfaceOrientationLandscapeLeft; 
	
	// Test getting the orientation from the interface
	UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation]; 
	
	
	// Check the orientation and select the appropriate movie to play.  The movie should be setup in the 
	// right orientation to coincide with this.
	// NOTE: All movies played should be in the m4v format currently.
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{/*
		// iPad movie
		if (UIInterfaceOrientationIsLandscape(orientation))
			movieName =  @"Chillingo_Stinger_iPad_Landscape_h264";
		else
			movieName =  @"Chillingo_Stinger_iPad_Portrait_h264";
		*/
		movieName = @"ClickgamerASLogoiPad";
	}
	else 
	{
		// iPhone movie
		if (UIInterfaceOrientationIsLandscape(orientation))
		//	movieName = @"Chillingo_Stinger_iPhone_Landscape_h264";
			movieName = @"ClickgamerASLogo";
		else
		{
			// Note: On the hardware prior to iOS 4 the movie always gets rotated to be in landscape so we need to use a different movie
			// On iOS 4.0 or the simulator the non-rotated version works fine.
			NSArray* versionComponents = [[UIDevice currentDevice].systemVersion componentsSeparatedByString:@"."];
			NSString* majorVersionNumberString = (NSString*)[versionComponents objectAtIndex:0];
			
			int majorVersionNumber = [majorVersionNumberString intValue];
			
			if (majorVersionNumber >= 4)
			{
				movieName = @"Chillingo_Stinger_iPhone_Portrait_h264";
			}
			else
			{
				movieName = @"Chillingo_Stinger_iPhone_Portrait_Rotated_h264";
			}
		}		
	}
	
	self.logoMoviePlayer = nil;
	
	if (IsHiResDevice() || GetPlatformIsiPad())
	{
	    // Now we have a movie and orientation we can start the movie and wait for it to notify us when it has finished
	    self.logoMoviePlayer = [[[CrystalLogoMoviePlayer alloc] initWithMovieName:movieName orientation:orientation observer:self] autorelease];
	}
	// Test no observer
	//self._CrystalLogoMoviePlayer = [[[CrystalLogoMoviePlayer alloc] initWithMovieName:@"Chillingo_Stinger_iPhone_Landscape_h264" observer:nil] autorelease];
	
	// If the logo movie player view controller is valid start playback
	if (self.logoMoviePlayer != nil)
	{		
		[window addSubview:self.logoMoviePlayer];
		[window makeKeyAndVisible];
//		[[UIDevice currentDevice] setOrientation:UIInterfaceOrientationLandscapeRight];
		
		NSLog(@"Start logo movie player");
		
		[self.logoMoviePlayer start];
	}
	else 
	{
		// Failed to create logo movie player so simply start the application	
		[window makeKeyAndVisible];
//		[[UIDevice currentDevice] setOrientation:UIInterfaceOrientationLandscapeRight];
		
		[self startCrystalSplash];
	}
}

- (void)startLogoMovie
{
	NSLog(@"Start logo movie");
	
	// Actual application start code goes here.
	// May need to setup the window here...	
	
	// Start the logo movie player 
	[self startLogoMovieplayer];
}

- (void)removeSplash
{
	[splashView removeFromSuperview];
	[splashView release];
	[self logoMovieDidFinish];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{	
	g_App = self;
	
    [application setStatusBarOrientation:UIInterfaceOrientationLandscapeRight];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
	
    [self startLogoMovie];
	
    // The AppID and version parameters must be populated with the data for your application
	// from the developer dashboard
	// The theme parameter must match that of one of the *.crystaltheme resources bundled with your application
	[CrystalSession initWithAppID:@"1374239207" delegate:self version:1.0 theme:@"the_relic_03" secretKey:@"p5ms0ns0bjjtld79vl2hbs0onobt67"];
	
    /* 
    if (!GetPlatformIsiPad())
	    [CrystalSession initWithAppID:@"1374239207" delegate:self version:1.0 theme:@"the_relic_02" secretKey:@"p5ms0ns0bjjtld79vl2hbs0onobt67"];
    else
	    [CrystalSession initWithAppID:@"1374239207" delegate:self version:1.0 theme:@"iPadIndigo_006" secretKey:@"p5ms0ns0bjjtld79vl2hbs0onobt67"];
	*/
	[CrystalSession lockToOrientationList:
     [NSArray arrayWithObjects:
	  [NSNumber numberWithInt:UIDeviceOrientationLandscapeLeft],
	  [NSNumber numberWithInt:UIDeviceOrientationLandscapeRight], nil]];	
	
  //[CrystalSession lockToOrientation:UIDeviceOrientationLandscapeLeft];	
	[CrystalSession application:application didFinishLaunchingWithOptions:launchOptions];

	if (ShouldShowLogoSplash())
	{
		/*
	    if (GetPlatformIsiPad())
	        splashView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 720, 1024)];
	    else 
		    splashView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 320, 480)];
	    splashView.image = [UIImage imageNamed:@"Default-iPad.png"];
	    [window addSubview:splashView];	
        [window bringSubviewToFront:splashView];
		 */
	}
	
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	Autosave();
    [self.viewController stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [self.viewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	NSLog(@"application will terminate");
	
    [self.viewController stopAnimation];
	
	Autosave();
//	UnloadAll();
//	exit(-1);
}

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	[application setStatusBarOrientation:UIInterfaceOrientationLandscapeRight];
}

- (void)crystalUiDeactivated
{
	printf("Crystal deactivated.");
	SetCrystalIsActive(false);
}

- (void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken
{
	[CrystalSession application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
}

- (void)application:(UIApplication*)application didFailToRegisterForRemoteNotificationsWithError:(NSError*)error
{
	[CrystalSession application:application didFailToRegisterForRemoteNotificationsWithError:error];
}

- (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo
{
	[CrystalSession application:application didReceiveRemoteNotification:userInfo];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	Autosave();
//	UnloadAll();
//	exit(-1);
	
    // Save the state of your app, just as you would have in applicationWillTerminate.
	
    // You may also want to set a variable to indicate that the app has been put in the
    // background.  I check this variable in my OpenGL loop to prevent rendering when
    // in this state.  A single OpenGL call to a backgrounded app will cause it to crash.
    g_bInBackground = true;
}


- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // The app was successfully restored.  If you saved game state that is no longer
    // needed, you can delete that data now.
    g_bInBackground = false;
}

- (void)challengeStartedWithGameConfig:(NSString*)gameConfig
{
	// Start a challenge with the specified game config
	// The game config will match the config shown in the Developer Dashboard
}

- (void) dealloc
{
    [viewController release];
    [window release];
	[logoMoviePlayer release];
	
    self.viewController = nil;
	self.window = nil;
	self.logoMoviePlayer = nil;
    
    [super dealloc];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark CrystalLogoMoviePlayerObserver
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

- (void) logoMovieDidFinish
{
	// This method will be called when the movie fishes either naturally or 
	// by the user tapping the screen
	NSLog(@"Logo movie finished");
	
	// Notification from the movie player that the movie has finished	
	// We can now release the logo movie player view controller
	[logoMoviePlayer release];
	
	// Start the main application
	// If you have multiple movies to display in sequence create a  
	// new logo movie player and increment a counter until all movies have played
	// NOTE: The logo movie player may support movie queues at some point in the future
	[self startCrystalSplash];
}

@end
