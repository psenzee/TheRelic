//
//  MoveView.m
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MoveView.h"
#import "RectUtil.h"
#import "SimpleCall.h"
#import "UITheme.h"
#import "GetCurrentTimeMs.h"
#import "Mover.h"

@implementation MoveView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor clearColor];
        self.autoresizesSubviews = YES;
        time = 0;
        movers = [[NSMutableArray alloc] init];
        [UITheme registerUIUpdate:[SimpleCall callWithSelector:@selector(update) forTarget:self]];
    }
    return self;
}

- (void)addMover:(Mover *)mover
{
    [movers addObject:mover];
}

- (void)addMover:(Mover *)mover withSpeed:(CGFloat)speed withRange:(CGFloat)range
{
    [mover setSpeed:speed];
    [mover setRange:range];
    [self addMover:mover];
}

- (uint64_t)timeSinceLastCall
{
    if (!time)
        time = GetCurrentTimeMs();
    uint64_t newt  = GetCurrentTimeMs();
    uint64_t delta = newt - time;
    time = newt;
    return delta;
}

- (void)update
{
    uint64_t timeChange = [self timeSinceLastCall];
    CGAffineTransform t = CGAffineTransformIdentity;
    for (Mover *mover in movers)
    {
        [mover advance:timeChange];
        t = CGAffineTransformConcat(t, mover.transform);
    }
    self.transform = t;
}

@end
