//
//  MoveView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

@class Mover;

@interface MoveView : UIView
{
    NSMutableArray *movers;
    uint64_t        time;
}

- (id)initWithFrame:(CGRect)frame;

- (void)addMover:(Mover *)mover;
- (void)addMover:(Mover *)mover withSpeed:(CGFloat)speed withRange:(CGFloat)range;

@end
