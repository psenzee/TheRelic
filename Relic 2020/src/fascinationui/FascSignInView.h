//
//  FascMenuView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ListView.h"

@class SimpleCall;

@class BackgroundView;
@class AnimationView;
@class MoveView;

@interface FascSignInView : UIView <ListDelegate>

@property (nonatomic, retain) BackgroundView   *background;
@property (nonatomic, retain) MoveView         *title;
@property (nonatomic, retain) UIButton         *login;

@property (nonatomic, retain) ListView         *tables;

@property (nonatomic, retain) UIView           *playButton;

@property (nonatomic, retain) NSMutableArray   *buttons;
@property (nonatomic, retain) NSMutableArray   *rooms;

@property (nonatomic, retain) SimpleCall       *onButtonPressed;
@property (nonatomic, retain) SimpleCall       *onLoginPressed;
@property (nonatomic, retain) SimpleCall       *onLogoutPressed;

@property (nonatomic, retain) UITextField      *field_login;
@property (nonatomic, retain) UITextField      *field_password;
@property (nonatomic, retain) UIButton         *field_submit;

- (id)initWithFrame:(CGRect)frame;
- (void)setLoginId:(NSString *)login;

- (void)showLoginWhenNecessary;

@end
