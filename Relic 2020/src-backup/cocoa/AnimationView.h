//
//  AnimationView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface AnimationView : UIView
{
    NSMutableArray  *frames;
    NSUInteger       last;
    CGFloat          time;
}

@property (nonatomic) CGFloat speed;

- (id)initWithFrame:(CGRect)frame;
- (void)addFrame:(UIView *)view;
- (void)addImage:(UIImage *)image;
- (void)addImagesWithBaseKey:(NSString *)key;

- (void)update;

@end
