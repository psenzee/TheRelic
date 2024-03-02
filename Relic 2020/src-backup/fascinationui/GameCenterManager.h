//
//  GameCenterManager.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface GameCenterManager : NSObject
{
    NSString         *_leaderboardIdentifier;
    BOOL              _gameCenterEnabled;
    UIViewController *_controller;
}

- (id)init;
- (void)authenticateLocalPlayer;

- (void)setViewController:(UIViewController *)controller;

+ (GameCenterManager *)shared;

@end
