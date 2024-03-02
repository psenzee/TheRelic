//
//  Transitioner.mm
//  TonesPOC
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import "Transitioner.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "RectUtil.h"
#import "ImageUtilities.h"
#import "FlyingButtonView.h"
#import "AnimationView.h"
#import "MoveView.h"
#import "Mover.h"
#import "UITheme.h"
#import <stdlib.h>
#import "FascMenuView.h"

#define SLIDE_DURATION 0.4
#define DELAY          0.05

@implementation Transitioner

- (void)slide:(UIView *)view offset:(double)x
{
    [UIView animateWithDuration:SLIDE_DURATION animations:^{
    
        CGRect f = view.frame;
        f.origin.x = x;
        view.frame = f;
    
    } completion:NULL];
}

- (void)slideIn:(UIView *)view
{
    [self slide:view offset:0.0];
}

- (void)slideOut:(UIView *)view direction:(double)direction
{
    [self slide:view offset:direction * view.frame.size.width];
}

- (void)slideOutN:(UIView *)view
{
    [self slideOut:view direction:-1.0];
}

- (void)slideOutP:(UIView *)view
{
    [self slideOut:view direction:1.0];
}

- (void)slideScreensR:(UIView *)first and:(UIView *)second
{
    if (first)
    {
        first.hidden = NO;
        first.frame = RectWithX(first.frame, 0.0);
        PerformOnMainThreadAfterDelay(self, @selector(slideOutP:), first, DELAY);
    }
    if (second)
    {
        second.hidden = NO;
        second.frame = RectWithX(second.frame, -second.frame.size.width);
        PerformOnMainThreadAfterDelay(self, @selector(slideIn:),  second, DELAY);
    }
}

- (void)slideScreensL:(UIView *)first and:(UIView *)second
{
    if (first)
    {
        first.hidden = NO;
        first.frame = RectWithX(first.frame, 0.0);
        PerformOnMainThreadAfterDelay(self, @selector(slideOutN:), first, DELAY);
    }
    if (second)
    {
        second.hidden = NO;
        second.frame = RectWithX(second.frame, second.frame.size.width);
        PerformOnMainThreadAfterDelay(self, @selector(slideIn:),  second, DELAY);
    }
}

- (int)indexOfView:(UIView *)view
{
    if (!view)
        return -1;
    for (int i = 0, sz = self.screens.count; i < sz; i++)
        if ([self.screens objectAtIndex:i] == view)
            return i;
    return -1;
}

- (UIView *)viewForScreen:(int)index
{
    return (UIView *)[self.screens objectAtIndex:index];
}

- (void)hideView:(UIView *)v
{
    v.hidden = YES;
}

- (void)showScreen:(UIView *)v
{
    if (current == v)
        return;
    
    int to = [self indexOfView:v];
    if (to == -1)
        return;
    int from = [self indexOfView:current];
    
    if (to < from)
        [self slideScreensR:current and:v];
    else
        [self slideScreensL:current and:v];
    
    UIView *previous = current;
    current = v;

    PerformOnMainThreadAfterDelay(self, @selector(hideView:), previous, SLIDE_DURATION - DELAY);
}

- (id)init
{
    self = [super init];
    if (self)
    {
        self.screens = [[NSMutableArray alloc] init];
        current = nil;
    }
    return self;
}

- (void)registerScreen:(UIView *)view
{
    [self.screens addObject:view];
}

@end
