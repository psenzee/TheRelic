//
//  RectAnimator.h
//

#import <UIKit/UIKit.h>
#import "math_c.h"

@interface RectAnimator : NSObject

+ (RectAnimator *)animateView:(UIView *)view to:(CGRect)frame duration:(NSTimeInterval)duration easing:(ease_callback_fn_t)fn context:(void *)context;

+ (RectAnimator *)bounceView:(UIView *)view to:(CGRect)frame duration:(NSTimeInterval)duration
                  bounciness:(double)bouncy reverse:(BOOL)reverse;

@end