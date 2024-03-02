//
//  Mover.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef struct
{
    CGFloat time;
    CGFloat time_offset;
    CGFloat range;
    CGFloat speed;
    
} MoverState;

typedef CGAffineTransform (*Mover_fn_t)(MoverState *state);

CGAffineTransform Mover_Bounce(MoverState *state);
CGAffineTransform Mover_Swirl(MoverState *state);
CGAffineTransform Mover_Spin(MoverState *state);
CGAffineTransform Mover_Oscillate(MoverState *state);

@interface Mover : NSObject
{
}

@property (nonatomic) Mover_fn_t        function;
@property (nonatomic) MoverState        state;
@property (nonatomic) CGAffineTransform transform;

- (id)initWithFunction:(Mover_fn_t)function;

- (BOOL)isActive;

- (void)setSpeed:(CGFloat)speed;
- (void)setRange:(CGFloat)range;

- (void)advance:(uint64_t)milliseconds;

+ (Mover *)bouncer;
+ (Mover *)swirler;
+ (Mover *)spinner;
+ (Mover *)oscillator;

@end