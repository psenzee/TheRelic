//
//  CrystalLogoMoviePlayer.m
//  Crystal
//
//	Purpose:
//
//	The logo movie view controller handles creating an initial video/image logo at the very start of an application.
//
//  Created by Duane Bradbury on 21/07/2010.
//  Copyright Chillingo Ltd 2010. All rights reserved.

// TODO:
//
//	* Add alternative image support - ability to use an image if the video fails to load
//	* Add background image support - background colour is deprecated
//	* Determine how many options should be available to developers
//	* Additional observer methods to intecept events
//	* Support movie queue
//	* Support for auto-flipping the movie


#import "CrystalLogoMoviePlayer.h"


///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark CrystalLogoMoviePlayerTouchOverlay private declaration
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Crystal logo movie player touch overlay
 * Provides touch event handling for the Crystal logo movie player view controller
 */
@interface CrystalLogoMoviePlayerTouchOverlay : UIView
{
	CrystalLogoMoviePlayer *_logoMoviePlayer;
}

@property (retain) CrystalLogoMoviePlayer *_logoMoviePlayer;


@end


///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark CrystalLogoMoviePlayer private methods
///////////////////////////////////////////////////////////////////////////////////////////

@interface CrystalLogoMoviePlayer (Private)

/**
 * @brief Dismisses the logo movie
 */
- (void)dismiss;


/**
 * @brief Forced dismissal of the logo movie
 */
- (void)dismissLogoFinish;


/**
 * @brief Test to see if the right platform and OS version is being used for the newer movie player interface methods
 */
- (BOOL) canUseNewMoviePlayerInterface;


/**
 * @brief Adds the touch overlay to the movie player view to intercept user interaction
 */
- (void) addTouchOverlayToView:(UIView*)moviePlayerView;

@end



///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark CrystalLogoMoviePlayer implementation
///////////////////////////////////////////////////////////////////////////////////////////

@implementation CrystalLogoMoviePlayer

@synthesize _logoImage;
@synthesize _alternativeImage;
@synthesize _isFinishing;
@synthesize _animationDelay;
@synthesize _logoMovie;

@synthesize _movieName;

@synthesize _touchOverlay;
@synthesize _observer;





///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark Logo movie player initialisation and destruction
///////////////////////////////////////////////////////////////////////////////////////////
//- (id)initWithMovieName: (NSString*)movieName alternativeImage:(UIImage *)alternativeImage orientation:(UIInterfaceOrientation)orientation observer:(id<CrystalLogoMoviePlayerObserver>)observer;

- (id)initWithMovieName: (NSString*)movieName orientation:(UIInterfaceOrientation)orientation observer:(id<CrystalLogoMoviePlayerObserver>)observer;
{	
	// Default to screensize to iPhone portrait size
	CGRect screenSize = CGRectMake(0,0,320,480);
	
	BOOL doRotate = NO;

	if(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		if(UIInterfaceOrientationIsLandscape(orientation))
		{
			screenSize = CGRectMake(0,0,1024,768);
			doRotate = YES;
		}
		else 
		{
			screenSize = CGRectMake(0,0,768,1024);
		}		
	}
	else 
	{
		if(UIInterfaceOrientationIsLandscape(orientation))
		{
			screenSize = CGRectMake(0,0,480,320);
			doRotate = YES;
		}
	}
	
	
	if (self = [super initWithFrame:screenSize]) {
		
		self._movieName = movieName;
		self._logoMovie = nil;
		
		//self._alternativeImage = alternativeImage;
		self._alternativeImage = nil;
		
		self._animationDelay = 0.5;
		self._isFinishing = NO;
		
		self._touchOverlay = nil;
		self._observer = observer;
		
		
		if(doRotate == YES)
		{
			CGSize viewSize = screenSize.size;
			CGPoint centrePoint = CGPointMake(viewSize.height/2, viewSize.width/2);

			[self setCenter:centrePoint];
			[self setTransform:CGAffineTransformMakeRotation(M_PI / 2)]; 
		}			
	}
	return self;
}

- (void)dealloc 
{
	//Cleanup in case the view is removed prematurely
	self._movieName = nil;
	self._touchOverlay = nil;
	self._logoMovie = nil;
	self._logoImage = nil;
	self._alternativeImage = nil;
	self._observer = nil;

	// Remove the logo movie player from the notification system
	[[NSNotificationCenter defaultCenter] removeObserver:self];

    [super dealloc];
}


///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark Logo movie player control
///////////////////////////////////////////////////////////////////////////////////////////


/**
 * @brief Start the logo movie playback.
 * If the video fails to load and play for any reason the alternative image will be displayed for a period
 * If no alternative image is provided then the logo movie player will notify the observer (if specified) that
 * the logo movie has finished.
 */
- (void)start
{
	// Test for appropriate hardware and OS combinations to ensure we can use the new Movie player interface
	BOOL canUseNewMPMethods = [self canUseNewMoviePlayerInterface];

	// TODO: Check this works on devices
	if(canUseNewMPMethods == YES)
	{	
		// Register for the new MPMediaPlaybackIsPreparedToPlayDidChangeNotification notification
		// OS 3.2 (iPad) & 4.0 (iPhone/iPod touch) onwards
		[[NSNotificationCenter defaultCenter] addObserver:self 
												 selector:@selector(moviePlayerLoadStateChanged:) 
													 name:MPMediaPlaybackIsPreparedToPlayDidChangeNotification 
												   object:nil];
	}
	else
	{
		// Register to receive a notification that the movie is now in memory and ready to play
		[[NSNotificationCenter defaultCenter] addObserver:self 
												 selector:@selector(moviePreloadDidFinish:) 
													 name:MPMoviePlayerContentPreloadDidFinishNotification 
												   object:nil];
	}
	
	
	// Register to receive a notification when the movie has finished playing. 
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(moviePlayBackDidFinish:) 
												 name:MPMoviePlayerPlaybackDidFinishNotification 
											   object:nil];
	
	// Register to receive a notification when the movie scaling mode has changed. 
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(movieScalingModeDidChange:) 
												 name:MPMoviePlayerScalingModeDidChangeNotification 
											   object:nil];
	
	// Start the logo movie
	// If the movie fails at any point, drop into using an alternative image instead
	
	NSURL *movieURL = NULL;			
	NSString *moviePath = [[NSBundle mainBundle] pathForResource:_movieName ofType:@"m4v"];
	
	BOOL isMovieActive = NO;
	
	if (moviePath)
	{
		movieURL = [NSURL fileURLWithPath:moviePath];
		
		// Initialize a movie player object with the specified URL
		self._logoMovie = [[[MPMoviePlayerController alloc] initWithContentURL:movieURL] autorelease];
	
		if(self._logoMovie != nil)
		{
			if(canUseNewMPMethods == YES)
			{
				if([self._logoMovie respondsToSelector:@selector(setMovieControlMode:)])
				{
					[self._logoMovie setMovieControlMode:MPMovieControlModeHidden];		
				}
				else if([self._logoMovie respondsToSelector:@selector(setControlStyle:)])
				{
					// OS 3.2 (iPad) & 4.0 (iPhone/iPod touch) onwards
					[self._logoMovie setControlStyle:MPMovieControlStyleNone];
				}
				
				// Set fullscreen
				if ([self._logoMovie respondsToSelector:@selector(setFullscreen:)])
				{
					[self._logoMovie setFullscreen: YES animated:NO];
				}
			}
			else 
			{
				[self._logoMovie setScalingMode:MPMovieScalingModeAspectFit];

				// Only available in OS 2.0 - 3.2
				if([self._logoMovie respondsToSelector:@selector(setMovieControlMode:)])
					[self._logoMovie setMovieControlMode:MPMovieControlModeHidden];			
				
				if([self._logoMovie respondsToSelector:@selector(setBackgroundColor:)])
					[self._logoMovie setBackgroundColor:[UIColor blackColor]];
				
			}

					
			isMovieActive = YES;
		}
	}
	
	
	// Failed to create logo movie? Use background image  
	if(isMovieActive == NO)
	{
		if(self._alternativeImage != nil)
		{
			// Show the alternative logo screen image
			self._logoImage = [[[UIImageView alloc] initWithImage:self._alternativeImage] autorelease];
			
			//GR: addSubView implies ownership - need to removeFromSuperview
			[self addSubview:self._logoImage];
		}
		else 
		{
			// No video and no alternative image
			// Send out logo screen finished notification
			[self moviePlayBackDidFinish:nil];
		}

	}
}

///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark Movie player events
///////////////////////////////////////////////////////////////////////////////////////////


/**
 * @brief Notification called when the movie finished preloading.
 * NOTE: Used on OS 2.0 to OS 3.1.3 iPhone only
 */
- (void) moviePreloadDidFinish:(NSNotification*)notification
{
	
	// Locate the movie player window
	UIWindow *moviePlayerWindow = [[UIApplication sharedApplication] keyWindow];
	
	if(moviePlayerWindow != nil)
	{
		[self addTouchOverlayToView: moviePlayerWindow ];
	}

	// Start movie playback
	[self._logoMovie play];
}



/**
 * @brief Notification called when the movie finished preloading.
 * NOTE: Used on OS 3.2 iPad and OS 4.0 iPhone onwards
 */
- (void) moviePlayerLoadStateChanged:(NSNotification*)notification
{
	
	if ([self._logoMovie loadState] != MPMovieLoadStateUnknown)
	{
		CGRect viewSize = [self bounds];
		[[self._logoMovie view] setFrame:viewSize];


		// Add the movie player to the main window
		[self addSubview:[self._logoMovie view]];

		[self addTouchOverlayToView:[self._logoMovie view]];
		
		
		// Start movie playback
		[self._logoMovie play];
	}
}


/**
 * @brief Adds the touch overlay to the movie player view to intercept user interaction
 */
- (void) addTouchOverlayToView:(UIView*)moviePlayerView
{
	if(self._touchOverlay == nil)
	{
		CGRect viewSize = [self bounds];
		// Create a new UIView to accept touch events and pass them back to this class.
		self._touchOverlay = [[[CrystalLogoMoviePlayerTouchOverlay alloc] initWithFrame:viewSize] autorelease];
		
		[self._touchOverlay setOpaque:false];
		self._touchOverlay._logoMoviePlayer = self;
		
		// Add our overlay view to the movie player's subviews so it is 
		// displayed above it.
		[moviePlayerView addSubview:self._touchOverlay];
	}
}



/**
 * @brief Notification called when the movie finished playing.
 */
- (void) moviePlayBackDidFinish:(NSNotification*)notification
{
	[self._touchOverlay removeFromSuperview];		
	self._touchOverlay = nil;
	
	self._logoMovie = nil;
		
	/*
	// Open the next logo screen
	self._logoImage = [[UIImageView alloc] initWithImage:self.alternativeImage];
	[self addSubview:self._logoImage];
	*/
	
	
	// Tell the observer that the logo screen movie has finished
	if(self._observer != nil)
	{
		// TODO: Test to see if the method is implemented?
		//if([self._observer respondsToSelector:@selector(logoMovieDidFinish:)])
			[self._observer logoMovieDidFinish];
	}
	
	// Remove the logo movie player from the notification system
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	
	// Remove the logo movie player from it's superview
	[self removeFromSuperview];
}


/**
 * @brief Notification called when the movie scaling mode has changed.
 */
- (void) movieScalingModeDidChange:(NSNotification*)notification
{
}


///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark Logo movie dismissal
///////////////////////////////////////////////////////////////////////////////////////////


/**
 * @brief Dismisses the splash screen
 */
- (void)dismiss 
{

	if (self._isFinishing)
	{
		[self dismissLogoFinish];
	} 
	else
	{
		CABasicAnimation *animLogo = [CABasicAnimation animationWithKeyPath:@"opacity"];
		animLogo.duration = self._animationDelay;
		animLogo.removedOnCompletion = NO;
		animLogo.fillMode = kCAFillModeForwards;
		animLogo.toValue = [NSNumber numberWithFloat:0];
		animLogo.delegate = self;
		[self.layer addAnimation:animLogo forKey:@"animateOpacity"];
	}
	self._isFinishing = YES;
}


/**
 * @brief Notification called when the animation stops.
 */
- (void)animationDidStop:(CAAnimation *)theAnimation finished:(BOOL)flag 
{

	[self dismissLogoFinish];
}


/**
 * @brief Forced dismissal of the logo movie
 */
- (void)dismissLogoFinish 
{

	if(self._logoImage != nil)
	{
		[self._logoImage removeFromSuperview];
		self._logoImage = nil;
				
		self._alternativeImage = nil;
	}
	
	[self removeFromSuperview];
}


///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark User interaction
///////////////////////////////////////////////////////////////////////////////////////////


/**
 * @brief Stops the logo movie as soon as a user begins interaction
 */
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	// On a touch event, stop the movie.
	// This will kick off the moviePlayBackDidFinish callback.
	[self._logoMovie stop];
}



///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark Helper methods
///////////////////////////////////////////////////////////////////////////////////////////

- (BOOL) canUseNewMoviePlayerInterface
{	
	NSArray* versionComponents = [[UIDevice currentDevice].systemVersion componentsSeparatedByString:@"."];
	NSString* majorVersionNumberString = (NSString*)[versionComponents objectAtIndex:0];
	NSString* minorVersionNumberString = @"0";
	
	if([versionComponents count] > 1)
		minorVersionNumberString = (NSString*)[versionComponents objectAtIndex:1];
	
	int majorVersionNumber = [majorVersionNumberString intValue];
	int minorVersionNumber = [minorVersionNumberString intValue];
	
	//GR: Maybe we should pull this lot out into a helper method as we're using it twice?
	if(majorVersionNumber >= 4)
	{
		// Everything on OS 4.0 or above can use the new movie player interfaces
		return YES;
	}
	else if(majorVersionNumber >= 3) 
	{
		// OS 3.X
		
		// Check to see if we are on the iPad
		if(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
		{
			return YES;
		}
	 
		// Failsafe check for the minor version being 2 or above just in case the
		// UI_USER_INTERFACE_IDIOM check fails
		if(minorVersionNumber >= 2)
		{
			return YES;
		}
	}

	// Using older OS/hardware combination
	return NO;	
}



@end





///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark CrystalLogoMoviePlayerTouchOverlay implementation
///////////////////////////////////////////////////////////////////////////////////////////


@implementation CrystalLogoMoviePlayerTouchOverlay

@synthesize _logoMoviePlayer;


- (void)dealloc 
{
	self._logoMoviePlayer = nil;
	
	[super dealloc];
}

/**
 * @brief Stops the logo movie as soon as a user begins interaction
 */
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	// forward the touch event to the Crystal logo movie player view controller
	if(self._logoMoviePlayer != nil)
	{
		[self._logoMoviePlayer touchesBegan:touches withEvent:event];
	}
}

@end

