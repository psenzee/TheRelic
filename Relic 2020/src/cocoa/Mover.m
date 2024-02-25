#import <UIKit/UIKit.h>
#import "Mover.h"

CGAffineTransform Mover_Bounce(MoverState *state)
{
    CGFloat time = state->time + state->time_offset;
    CGFloat scale = sin(time) * state->range + 1.0;
    return CGAffineTransformMakeScale(scale, scale);
}

CGAffineTransform Mover_Swirl(MoverState *state)
{
    CGFloat time = state->time + state->time_offset;
    CGFloat radius = state->range, ts = time * state->speed;
    return CGAffineTransformMakeTranslation(cos(ts) * radius, sin(ts) * radius);
}

CGAffineTransform Mover_Spin(MoverState *state)
{
    CGFloat time = state->time + state->time_offset;
    return CGAffineTransformMakeRotation(time * state->speed * state->range);
}

CGAffineTransform Mover_Oscillate(MoverState *state)
{
    CGFloat time = state->time + state->time_offset;
    return CGAffineTransformMakeRotation(sin(time * state->speed) * state->range);
}

static BOOL IsActiveState(const MoverState state)
{
    return state.speed != 0.0 && state.range != 0.0;
}

static MoverState MakeMoverState()
{
    MoverState state;
    memset(&state, 0, sizeof(MoverState));
    state.range = 1.0;
    return state;
}

@implementation Mover

- (id)initWithFunction:(Mover_fn_t)function
{
    self = [super init];
    if (self)
    {
        self.function = function;
        self.state = MakeMoverState();
        self.transform = CGAffineTransformIdentity;
    }
    return self;
}

- (BOOL)isActive
{
    return IsActiveState(self.state);
}

- (void)setSpeed:(CGFloat)speed
{
    MoverState state = self.state;
    state.speed = speed;
    self.state = state;
}

- (void)setRange:(CGFloat)range
{
    MoverState state = self.state;
    state.range = range;
    self.state = state;
}

- (void)setTime:(CGFloat)time
{
    MoverState state = self.state;
    state.time = time;
    self.state = state;
}

- (void)addTime:(CGFloat)time
{
    [self setTime:(self.state.time) + time];
}

- (void)update
{
    if (!self.function || !IsActiveState(self.state))
    {
        self.transform = CGAffineTransformIdentity;
    }
    else
    {
        MoverState state = self.state;
        self.transform = (self.function)(&state);
    }
}

- (void)advance:(uint64_t)milliseconds
{
    [self addTime:(milliseconds / 1000.0)];
    [self update];
}

+ (Mover *)bouncer
{
    return [[Mover alloc] initWithFunction:Mover_Bounce];
}

+ (Mover *)swirler
{
    return [[Mover alloc] initWithFunction:Mover_Swirl];
}

+ (Mover *)spinner
{
    return [[Mover alloc] initWithFunction:Mover_Spin];
}

+ (Mover *)oscillator
{
    return [[Mover alloc] initWithFunction:Mover_Oscillate];
}

@end