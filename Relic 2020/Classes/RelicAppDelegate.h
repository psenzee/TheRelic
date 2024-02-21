//
//  RelicAppDelegate.h
//  Relic
//
//  Created by Paul Senzee on 1/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RelicViewController;

@interface RelicAppDelegate : NSObject <UIApplicationDelegate,
                                        UIAlertViewDelegate> 
{
    UIWindow                *window;
    RelicViewController     *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow               *window;
@property (nonatomic, retain)          RelicViewController    *viewController;

@end

