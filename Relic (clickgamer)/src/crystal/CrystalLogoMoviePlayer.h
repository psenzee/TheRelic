//
//  CrystalLogoMoviePlayer.h
//  Crystal
//
//	Frameworks required:
//
//			* MediaPlayer
//			* QuartzCore
//
//  Created by Duane Bradbury on 21/07/2010.
//  Copyright Chillingo Ltd 2010. All rights reserved.


#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <MediaPlayer/MediaPlayer.h>


/**
 * @brief Crystal logo movie player observer protocol for recieving notifications from the Crystal logo movie player view controller
 * You should conform to this protocol of you need to know about events from the Crystal logo movie player view controller
 */
@protocol CrystalLogoMoviePlayerObserver

@optional
- (void) logoMovieDidFinish;

@end


// Forward declaration of the crystal logo movie player touch overlay
@class CrystalLogoMoviePlayerTouchOverlay;

/**
 * @brief Crystal logo movie player view controller
 */
@interface CrystalLogoMoviePlayer : UIView
{
@private
	UIImageView *_logoImage;
	UIImage *_alternativeImage;
	
	id<CrystalLogoMoviePlayerObserver> _observer;
	
	NSTimeInterval _animationDelay;
	MPMoviePlayerController *_logoMovie;
		
	CrystalLogoMoviePlayerTouchOverlay* _touchOverlay;  // Overlay view to capture touch events
	
	BOOL _isFinishing;
}

@property (retain) UIImageView *_logoImage;
@property (retain) UIImage *_alternativeImage;
@property NSTimeInterval _animationDelay;

@property (retain) MPMoviePlayerController *_logoMovie;
@property (retain) NSString* _movieName;

@property (retain) id<CrystalLogoMoviePlayerObserver> _observer;

@property (retain) CrystalLogoMoviePlayerTouchOverlay *_touchOverlay;

@property BOOL _isFinishing;


/**
 * @brief Initialises the logo movie player view controller
 * @param movieName The name of the movie to play
 * @param orientation The orientation to sho the movie
 * @param observer An observer to receive movie player notifications
 */
- (id)initWithMovieName: (NSString*)movieName orientation:(UIInterfaceOrientation)orientation observer:(id<CrystalLogoMoviePlayerObserver>)observer;
//- (id)initWithMovieName: (NSString*)movieName alternativeImage:(UIImage *)alternativeImage orientation:(UIInterfaceOrientation)orientation observer:(id<CrystalLogoMoviePlayerObserver>)observer;

/**
 * @brief Start the logo movie playback.
 * If the video fails to load and play for any reason the alternative image will be displayed for a period
 * If no alternative image is provided then the logo movie player will notify the observer (if specified) that
 * the logo movie has finished.
 */
- (void)start;


@end


