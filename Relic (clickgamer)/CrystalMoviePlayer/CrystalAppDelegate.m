//
//  CrystalAppDelegate.m
//  Crystal
//
//	Sample app delegate for using the Crystal Logo Movie Player
//
//  IMPORTANT NOTE:	May not work as a standalone project.
//					This sample should be used as a guide only for where to place the equivalent code in a developers project.
//
//  Created by Duane Bradbury on 22/07/2010.
//  Copyright Chillingo Ltd 2010. All rights reserved.


#import "CrystalAppDelegate.h"

@implementation CrystalAppDelegate

@synthesize window;

@synthesize _crystalLogoMoviePlayer;


- (void) startApp
{
	NSLog(@"Start app");
	
	// Actual application start code goes here.
	// If using unity start unity here...
	
	
}

- (void) setGameToBackground
{
	
}

- (void) setGameToForeground
{
	
}

- (void) startCrystal
{
    // Any game-related cleanup
    [self setGameToBackground];
		
	// Open the Crystal UI
	[CrystalSession activateCrystalUI];
}
	
{
    // Save the state of your app, just as you would have in applicationWillTerminate.
	
    // You may also want to set a variable to indicate that the app has been put in the
    // background.  I check this variable in my OpenGL loop to prevent rendering when
    // in this state.  A single OpenGL call to a backgrounded app will cause it to crash.
    g_bInBackground = true;
}

	
- (void) applicationDidFinishLaunching:(UIApplication *)application 
{
	// May need to setup the window here...

	// Start the logo movie player 
	[self startLogoMovieplayer];
}


- (BOOL) application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	// May need to setup the window here...
	
	// Start the logo movie player 
	[self startLogoMovieplayer];
	
	return YES;
}


- (void) startLogoMovieplayer
{
	NSLog(@"Starting logo movie player");
		
	// Start the logo movie player	
	
	// Check the current orientation and select the most appropriate video
	// Default to the portrait version
	NSString* movieName = nil;

	// Test forcing the orientation to portrait
	//UIInterfaceOrientation orientation = UIInterfaceOrientationPortrait;
	
	// Test forcing the orientation to landscape
	//UIInterfaceOrientation orientation = UIInterfaceOrientationLandscapeLeft; 
	
	// Test getting the orientation from the interface
	//UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation]; 


	// Check the orientation and select the appropriate movie to play.  The movie should be setup in the 
	// right orientation to coincide with this.
	// NOTE: All movies played should be in the m4v format currently.
	if(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		// iPad movie
		if(UIInterfaceOrientationIsLandscape(orientation))
			movieName =  @"Chillingo_Stinger_iPad_Landscape_h264";
		else
			movieName =  @"Chillingo_Stinger_iPad_Portrait_h264";
	}
	else 
	{
		// iPhone movie
		if(UIInterfaceOrientationIsLandscape(orientation))
			movieName = @"Chillingo_Stinger_iPhone_Landscape_h264";
		else
		{
			// Note: On the hardware prior to iOS 4 the movie always gets rotated to be in landscape so we need to use a different movie
			// On iOS 4.0 or the simulator the non-rotated version works fine.
			NSArray* versionComponents = [[UIDevice currentDevice].systemVersion componentsSeparatedByString:@"."];
			NSString* majorVersionNumberString = (NSString*)[versionComponents objectAtIndex:0];

			int majorVersionNumber = [majorVersionNumberString intValue];

			if(majorVersionNumber >= 4)
			{
				movieName = @"Chillingo_Stinger_iPhone_Portrait_h264";
			}
			else
			{
				movieName = @"Chillingo_Stinger_iPhone_Portrait_Rotated_h264";
			}
		}		
	}

	// Now we have a movie and orientation we can start the movie and wait for it to notify us when it has finished
	self._CrystalLogoMoviePlayer = [[[CrystalLogoMoviePlayer alloc] initWithMovieName:movieName orientation:orientation observer:self] autorelease];
	
	// Test no observer
	//self._CrystalLogoMoviePlayer = [[[CrystalLogoMoviePlayer alloc] initWithMovieName:@"Chillingo_Stinger_iPhone_Landscape_h264" observer:nil] autorelease];
	
	// If the logo movie player view controller is valid start playback
	if(self._CrystalLogoMoviePlayer != nil)
	{		
		[window addSubview:self._CrystalLogoMoviePlayer];
		[window makeKeyAndVisible];

		NSLog(@"Start logo movie player");

		[self._CrystalLogoMoviePlayer start];
	}
	else 
	{
		// Failed to create logo movie player so simply start the application	
		[window makeKeyAndVisible];

		[self startApp];
	}
	
}


- (void)applicationWillTerminate:(UIApplication *)application
{
	NSLog(@"application will terminate");	
	
	// Cleanup...
	
	self.window = nil;
	self._CrystalLogoMoviePlayer = nil;
}


- (void)dealloc 
{
	// Cleanup
    self._viewController = nil;
	self.window =nil];
	self._CrystalLogoMoviePlayer = nil;
	
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
	NSLog(@"Log movie finished");
	
	// Notification from the movie player that the movie has finished	
	// We can now release the logo movie player view controller
	self._crystalLogoMoviePlayer = nil;
	
	// Start the main application
	// If you have multiple movies to display in sequence create a  
	// new logo movie player and increment a counter until all movies have played
	// NOTE: The logo movie player may support movie queues at some point in the future
	[self startApp];
}


@end
