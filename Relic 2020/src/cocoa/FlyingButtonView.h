//
//  FlyingButtonView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface FlyingButtonView : UIView
{
    UIImageView     *view;
    CGPoint          origin;
    CGFloat          theta;
    CGFloat          radius;
    CGFloat          increment;
}

@property (nonatomic, retain) UIButton *button;

- (id)initWithFrame:(CGRect)frame backgroundImageKey:(NSString *)key text:(NSString *)text;

- (void)setIncrement:(CGFloat)value;

@end
