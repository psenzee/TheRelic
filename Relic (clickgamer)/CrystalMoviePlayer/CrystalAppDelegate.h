//
//  CrystalAppDelegate.h
//  Crystal
//
//	Sample app delegate for using the Crystal Logo Movie Player
//
//  IMPORTANT NOTE:	May not work as a standalone project.
//					This sample should be used as a guide only for where to place the equivalent code in a developers project.
//
//  Created by Duane Bradbury on 22/07/2010.
//  Copyright Chillingo Ltd 2010. All rights reserved.


#import <UIKit/UIKit.h>

@interface CrystalAppDelegate : NSObject <UIApplicationDelegate, CrystalLogoMoviePlayerObserver> 
{
	UIWindow* window;
	
	CrystalLogoMoviePlayer*	_crystalLogoMoviePlayer;
}


@property (nonatomic, retain) IBOutlet UIWindow* window;
@property (nonatomic, retain) CrystalLogoMoviePlayer* _crystalLogoMoviePlayer;


@end
