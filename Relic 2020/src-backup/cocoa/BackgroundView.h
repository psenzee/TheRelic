//
//  BackgroundView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BackgroundView : UIView
{
    UIImageView     *view;
    UIImageView     *morph;
    CGPoint          origin;
    CGFloat          theta;
    CGFloat          radius;
}

- (id)initWithFrame:(CGRect)frame;

@end
