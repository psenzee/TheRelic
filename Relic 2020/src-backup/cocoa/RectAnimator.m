//
//  RectAnimator.m
//

#import "RectAnimator.h"
#import "Updater.h"
#import "GetCurrentTimeMs.h"
#import "math_c.h"
#import "SysUtil.h"

@interface RectAnimator ()
{
    CGRect              start;
    CGRect              end;
    
    Updater            *updater;
    
    ease_callback_fn_t  ease_function;
    void               *ease_context;
    
    NSTimeInterval      duration;
    unsigned long long  startms;
}

@property (nonatomic, retain) UIView *view;

- (id)initWithView:(UIView *)view duration:(NSTimeInterval)duration easing:(ease_callback_fn_t)fn context:(void *)context;
- (void)goTo:(CGRect)frame;
- (void)update;

@end

@implementation RectAnimator

- (id)initWithView:(UIView *)view duration:(NSTimeInterval)dura easing:(ease_callback_fn_t)fn context:(void *)context
{
    self = [super init];
    if (self)
    {
        self.view = view;
        
        duration = dura;
        ease_function = fn;
        ease_context = context;
        startms = 0;
    }
    return self;
}

- (void)goTo:(CGRect)frame
{
    start = self.view.frame;
    end = frame;
    if (updater)
        [updater stop];
    startms = GetCurrentTimeMs();
    updater = [Updater updaterWithObject:self selector:@selector(update) interval:0.033 onMainThread:YES];
}

- (void)update
{
    static BOOL inprogress = NO;
    if (!updater || inprogress)
        return;
    inprogress = YES;
    
    NSTimeInterval time = (unsigned)(GetCurrentTimeMs() - startms) / 1000.0;
    if (time >= duration)
    {
        // quit here and self release
        [updater invalidate];
        PerformOnMainThreadAfterDelay(updater, @selector(stop), nil, 0.1);
        self.view.frame = end;
        inprogress = NO;
        [self release];  // destroy self
        return;
    }
    else
    {
        double t = time / duration;
        double t1 = ease_function ? ease_function(t, ease_context) : t;
        CGRect frame = CGRectMake(interpolate(t1, start.origin.x,    end.origin.x),
                                  interpolate(t1, start.origin.y,    end.origin.y),
                                  interpolate(t1, start.size.width,  end.size.width),
                                  interpolate(t1, start.size.height, end.size.height));
        self.view.frame = frame;
    }
    inprogress = NO;
}

+ (RectAnimator *)animateView:(UIView *)view to:(CGRect)frame duration:(NSTimeInterval)duration easing:(ease_callback_fn_t)fn context:(void *)context
{
    RectAnimator *ra = [[RectAnimator alloc] initWithView:view duration:duration easing:fn context:context];
    [ra goTo:frame];
    return ra;
}

+ (RectAnimator *)bounceView:(UIView *)view to:(CGRect)frame duration:(NSTimeInterval)duration
                  bounciness:(double)bouncy reverse:(BOOL)reverse
{
    static ease_context_t ease;
    ease.power = interpolate(bouncy, 1.0/5.0, 2.0);
    ease.domain = interpolate(bouncy, 3.0, 7.0);
    return [RectAnimator animateView:view to:frame duration:duration
                              easing:reverse ? ease_reverse_bounce : ease_bounce
                             context:&ease];
}

@end