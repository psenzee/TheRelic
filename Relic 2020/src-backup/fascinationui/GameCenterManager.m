//
//  GameCenterManager.m
//

#import "GameCenterManager.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "BackgroundView.h"
#import "RectUtil.h"
#import "ImageUtilities.h"
#import "AnimationView.h"
#import "MoveView.h"
#import "Mover.h"
#import "UITheme.h"
#import <stdlib.h>
#import <GameKit/GameKit.h>

static GameCenterManager *SHARED = nil;

@implementation GameCenterManager

- (id)init
{
    self = [super init];
    if (self)
    {
        _gameCenterEnabled = NO;
        _leaderboardIdentifier = nil;

    }
    return self;
}

- (void)setViewController:(UIViewController *)controller
{
    _controller = controller;
}

-(void)authenticateLocalPlayer
{
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    
    localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *error)
    {
        if (viewController != nil)
        {
            [_controller presentViewController:viewController animated:YES completion:nil];
        }
        else
        {
            if ([GKLocalPlayer localPlayer].authenticated)
            {
                _gameCenterEnabled = YES;
                
                // Get the default leaderboard identifier.
                [[GKLocalPlayer localPlayer]
                    loadDefaultLeaderboardIdentifierWithCompletionHandler:
                         ^(NSString *leaderboardIdentifier, NSError *error)
                {
                    if (error != nil)
                        NSLog(@"%@", [error localizedDescription]);
                    else
                        _leaderboardIdentifier = leaderboardIdentifier;
                }];
            }
            else
            {
                _gameCenterEnabled = NO;
            }
        }
    };
}

+ (GameCenterManager *)shared
{
    if (!SHARED)
        SHARED = [[GameCenterManager alloc] init];
    return SHARED;
}

@end
