//
//  TouchView.mm
//

#import "TouchView.h"

#import <UIKit/UIKit.h>
#import "EAGLView.h"
#import "UIUtil.h"
#import "core/point.h"
#import <stdlib.h>

struct TouchTransform
{
    CGSize  ratio  = { 1.0f, 1.0f };
    CGPoint offset = { 0.0f, 0.0f };
};

inline core::Point GetAdjustedPoint(int x, int y)
{
    TouchTransform tt;
    core::Point p = core::Point((int)(x * tt.ratio.width + tt.offset.x), (int)(y * tt.ratio.height + tt.offset.y));
    printf("touched %d, %d\n", p.x, p.y);
    return p;
}

extern "C" void SetScreenTouchBegan(int index, int x, int y);
extern "C" void SetScreenTouchMoved(int index, int x, int y);
extern "C" void SetScreenTouchEnded(int index, int x, int y);
extern "C" void SetScreenTouchStationary(int index, int x, int y);

static void UpdateTouch(UITouch *touch, UIView *view)
{
    CGPoint pt = [touch locationInView:view];
    PrintCGPoint(pt, "touched", true);
    //core::Point p = GetAdjustedPoint(pt.x, pt.y);
    switch ([touch phase])
    {
        case UITouchPhaseBegan:      SetScreenTouchBegan((intptr_t)touch, pt.x, pt.y); break;
        case UITouchPhaseMoved:      SetScreenTouchMoved((intptr_t)touch, pt.x, pt.y); break;
        case UITouchPhaseStationary: SetScreenTouchStationary((intptr_t)touch, pt.x, pt.y); break;
        case UITouchPhaseCancelled:
        case UITouchPhaseEnded:      SetScreenTouchEnded((intptr_t)touch, pt.x, pt.y); break;
        default: break;
    }
}

static void UpdateTouches(NSSet *touches, UIView *window)
{
    int count = 0;
    for (UITouch *touch in touches) {
        UpdateTouch(touch, window);
        count++;
    }
    if (count > 1) {
        printf("multitouch count %d", count);
    }
}

@implementation TouchView

- (id)initWithCoder:(NSCoder*)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        self.backgroundColor = [UIColor colorNamed:@"red"];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor colorNamed:@"red"];
    }
    return self;
}

- (BOOL)isMultipleTouchEnabled
{
    return YES;
}

- (BOOL)exclusiveTouch
{
    return NO;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)events
{
    UpdateTouches(touches, self);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)events
{
    UpdateTouches(touches, self);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)events
{
    UpdateTouches(touches, self);
}

@end


