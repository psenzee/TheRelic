//
//  FascRegisterView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SimpleCall;

@class BackgroundView;
@class WebView;

@interface FascRegisterView : UIView

@property (nonatomic, retain) BackgroundView   *background;
@property (nonatomic, retain) WebView          *web;

@property (nonatomic, retain) SimpleCall       *onButtonPressed;

- (id)initWithFrame:(CGRect)frame;

@end
