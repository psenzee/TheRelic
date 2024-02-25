//
//  ToastView.m
//

#import "ToastView.h"
#import "RectUtil.h"
#import "DrawUtil.h"
#import "SysUtil.h"
#import "UITheme.h"
#import "UIUtil.h"

@interface ToastView ()

@property (nonatomic, retain) UIImageView             *background;
@property (nonatomic, retain) UILabel                 *label;

@end

@implementation ToastView

- (id)initWithFrame:(CGRect)frame backgroundColor:(UIColor *)color
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.35];
        
        CGFloat sizefactor = iPhoneIdiom() ? 1.5 : 1.0;
        frame = CenterInRect(RealSize(CGSizeMake(300.0 * sizefactor, 140.0 * sizefactor)), self.bounds);
        
        frame.origin.y -= RealHeight(150.0 * sizefactor);
        
        ColorStyle cs = ColorStyle_Gray(0.0, 0.7);
        if (color)
            cs = ColorStyleFromUIColor(color);
        
        RectStyle rs = MakeRectStyle(RealWidth(20.0), 0.0, cs, cs, cs);
        UIImage *roundrect = GetRectImageWithStyle(frame.size, rs, YES);
        UIImageView *background = [[UIImageView alloc] initWithImage:roundrect];
        background.frame = frame;
        [roundrect release];
        
        [self addSubview:background];
        self.background = background;
        [background release];
        
        UILabel *text = [[UILabel alloc] initWithFrame:ExpandRect(frame, RealWidth(-30.0), 0.0)];
/*        UILabel *text = [[UILabel alloc] initWithFrame:CenterInRect(
                                           CGSizeMake(180.0, 120.0), frame)];*/
        self.label = text;
        text.numberOfLines = 0;
        [self addSubview:text];
        [text release];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    return [self initWithFrame:frame backgroundColor:nil];
}

- (void)setText:(NSString *)text
{
    self.label.text = text;
    self.label.font = [UITheme defaultFontWithSize:[UITheme defaultToastFontSize]];
    self.label.textColor = [UIColor whiteColor];
    [UITheme alignCenter:self.label];
}

- (void)showText:(NSString *)text duration:(NSTimeInterval)duration
{
    [self setText:text];
    UIView_SetHidden_Fade(self, NO, 0.4);
    PerformOnMainThreadAfterDelay(self, @selector(hide), nil, duration);
}

- (void)showText:(NSString *)text
{
    [self showText:text duration:3.0];
}

- (void)hide
{
    UIView_SetHiddenWithCompletion_Fade(self, YES, 0.4, ^(BOOL complete) { [self setText:@""]; });
}

@end
