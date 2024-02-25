//
//  FascGameView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SimpleCall;

@interface FascGameView : UIView

@property (nonatomic, retain) SimpleCall  *onButtonPressed;
@property (nonatomic, retain) UIButton    *pause;

- (id)initWithFrame:(CGRect)frame;
- (void)setPauseEnabled:(BOOL)enabled;

- (void)showWinner:(BOOL)show;
- (void)showReady:(BOOL)show;
- (void)showRoll:(BOOL)show;

@end
