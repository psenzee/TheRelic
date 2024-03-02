//
//  DebugSettingsView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SimpleCall;
@class Transitioner;

@interface DebugSettingsView : UIView
{
    NSString            *currentCategory;
    NSMutableArray      *categories;
    NSMutableDictionary *views;
    Transitioner        *transitioner;
}

@property (nonatomic, retain) SimpleCall *onReturnPressed;

- (id)initWithFrame:(CGRect)frame;

@end
