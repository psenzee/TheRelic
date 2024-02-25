//
//  BaseView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SimpleCall;

@class BackgroundView;
@class AnimationView;
@class MoveView;

@interface BaseView : UIView
{
}

@property (nonatomic, retain) BackgroundView   *background;
@property (nonatomic, retain) MoveView         *title;

- (id)initWithFrame:(CGRect)frame;

- (void)doHide;
- (void)doShow;

- (void)showTitle;
- (void)hideTitle;

@end
