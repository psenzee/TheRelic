//
//  RelicAppDelegate.m
//  Relic
//
//  Created by Paul Senzee on 1/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "RelicAppDelegate.h"
#import "RelicViewController.h"
#import "UIUtil.h"

bool   g_bInBackground               = false;

void  *g_alertDismissedContext       = 0;
void (*g_alertDismissed)(void *)     = 0;

id     g_App                         = nil;

int    g_iOSMajorVersion             = -1;
int    g_iOSMinorVersion1            = -1;
int    g_iOSMinorVersion2            = -1;

extern void GetVersion();
extern int  GetiOSMajorVersion();
extern int  GetiOSMinorVersion1();
extern int  GetiOSMinorVersion2();
extern bool IsVersionGTE(int major, int minor1, int minor2);
extern bool ShouldShowLogoSplash();

extern bool        WriteUserTextFile(const char *filename, const char *data);
extern bool        GetPlatformIsiPad(void);
extern const char *ReadUserTextFile(const char *filename);
extern void        Autosave(void);
extern void        UnloadAll(void);
extern bool        IsHiResDevice(void);

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

void PostAchievement(const char *achievementId, const char *description)
{
	NSString *nsAchievementId = [[NSString alloc] initWithUTF8String:achievementId];
	NSString *nsDescription   = [[NSString alloc] initWithUTF8String:description];
	//[CrystalSession postAchievement:nsAchievementId wasObtained:YES withDescription:nsDescription alwaysPopup:NO];
    [nsAchievementId release];
	[nsDescription release];
}

void PostLeaderboard(const char *leaderboardId, double value)
{
	NSString *nsLeaderboardId = [[NSString alloc] initWithUTF8String:leaderboardId];
	//[CrystalSession postLeaderboardResult:value forLeaderboardId:nsLeaderboardId lowestValFirst:NO];
    [nsLeaderboardId release];
}

extern void SetupGame(void);

@implementation RelicAppDelegate

@synthesize window;
@synthesize viewController;

- (void)showSplash
{
	[self performSelector:@selector(removeSplash) withObject:nil afterDelay:2.0];
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
	
    [self.window setRootViewController:self.viewController];
    [self.window addSubview:self.viewController.view];
    [self.window makeKeyAndVisible];
	
    SetupGame();
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{	
	g_App = self;
	
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
	
    [self startApp];
	
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

- (void) dealloc
{
    [viewController release];
    [window release];
	
    self.viewController = nil;
	self.window = nil;
    
    [super dealloc];
}

@end
