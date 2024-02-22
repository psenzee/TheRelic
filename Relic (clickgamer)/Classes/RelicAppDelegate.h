//
//  RelicAppDelegate.h
//  Relic
//
//  Created by Paul Senzee on 1/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CrystalSession.h"
#import "CrystalLogoMoviePlayer.h"

@class RelicViewController;

@interface RelicAppDelegate : NSObject <UIApplicationDelegate, 
										CrystalSessionDelegate, 
										CrystalLogoMoviePlayerObserver,
                                        UIAlertViewDelegate> 
{
    UIWindow                *window;
    RelicViewController     *viewController;
	CrystalLogoMoviePlayer  *logoMoviePlayer;	
	UIImageView             *splashView;
}

@property (nonatomic, retain) IBOutlet UIWindow               *window;
@property (nonatomic, retain) IBOutlet RelicViewController    *viewController;
@property (nonatomic, retain)          CrystalLogoMoviePlayer *logoMoviePlayer;
@property (nonatomic, retain)          UIImageView            *splashView;

@end

