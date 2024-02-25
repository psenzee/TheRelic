//
//  Transitioner.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface Transitioner : NSObject
{
    UIView *current;
}

@property (nonatomic, retain) NSMutableArray *screens;

- (id)init;
- (void)registerScreen:(UIView *)view;
- (void)showScreen:(UIView *)view;

@end
