//
//  FascMenuView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SimpleCall;

@class BackgroundView;
@class AnimationView;
@class MoveView;

@interface FascMenuView : UIView
{
}

@property (nonatomic, retain) BackgroundView   *background;
@property (nonatomic, retain) MoveView         *title;

@property (nonatomic, retain) NSMutableArray   *buttons;

@property (nonatomic, retain) SimpleCall       *onButtonPressed;

- (id)initWithFrame:(CGRect)frame;

@end
