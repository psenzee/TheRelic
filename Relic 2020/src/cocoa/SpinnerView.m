//
//  SpinnerView.m
//

#import "SpinnerView.h"
#import "RectUtil.h"
#import "DrawUtil.h"
#import "SysUtil.h"
#import "UITheme.h"
#import "UIUtil.h"

static SpinnerView *SPINNER_VIEW;
static UIView      *SPINNER_ROOT_VIEW;

@interface SpinnerView ()

@property (nonatomic, copy)   CancelBlock              onCancel_;

@property (nonatomic, retain) UIView                  *background;
@property (nonatomic, retain) UIActivityIndicatorView *spinner;
@property (nonatomic, retain) UILabel                 *label;
@property (nonatomic, retain) UIButton                *cancel;

@end

@implementation SpinnerView

- (void)callCancel
{
    if (self.onCancel_)
        self.onCancel_();
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.25];
        
        CGFloat sizefactor = iPhoneIdiom() ? 1.5 : 1.0;
        frame = CenterInRect(RealSize(CGSizeMake(300.0 * sizefactor, 140.0 * sizefactor)), self.bounds);
        
        UIView *background = [[UIImageView alloc] initWithFrame:frame];
        [self addSubview:background];
        self.background = background;
        [background release];
        self.background.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.7];
        self.background.layer.cornerRadius = RealWidth(20.0);
        
        UIActivityIndicatorView *spinner =
            [[UIActivityIndicatorView alloc] 
                initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        
        CGRect spinner_frame = CenterInRect(RealSize(CGSizeMake(25.0, 25.0)), frame);
        spinner_frame.origin.y -= RealHeight(25.0);
        spinner.frame = spinner_frame;
        [self addSubview:spinner];
        [spinner startAnimating];
        self.spinner = spinner;
        [spinner release];
        
        UILabel *text = [[UILabel alloc] initWithFrame:
                           RectInsideAt(
                             ExpandRect(frame, RealWidth(-30.0), 0.0), RectBottom, RealHeight(95.0))];
        self.label = text;
        text.numberOfLines = 0;
        [self addSubview:text];
        [text release];
        
        UIButton *cancel = [UIButton buttonWithType:UIButtonTypeCustom];
        self.cancel = cancel;
        CGFloat buttonHeight = RealHeight(50.0), margin = RealHeight(3.0);
        CGRect buttonFrame = RectInsideAt(frame, RectBottom, buttonHeight);
        buttonFrame.origin.y += buttonHeight + margin;
        cancel.frame = buttonFrame;
        [cancel setTitle:@"Cancel" forState:UIControlStateNormal];
        [cancel setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [cancel setTitleColor:[UIColor grayColor]  forState:UIControlStateHighlighted];
        [self addSubview:cancel];
        cancel.hidden = YES;
        cancel.titleLabel.font = [UITheme defaultFontWithSize:[UITheme defaultToastFontSize]];
        cancel.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.7];
        cancel.layer.cornerRadius = RealWidth(20.0);
        [cancel addTarget:self action:@selector(callCancel) forControlEvents:UIControlEventTouchUpInside];
        
        self.hidden = YES;
    }
    return self;
}

- (void)setOnCancel:(CancelBlock)cancel
{
    self.onCancel_ = cancel;
    self.cancel.hidden = (cancel == nil);
}

- (void)setText:(NSString *)text
{
    self.label.text = text;
    self.label.font = [UITheme defaultFontWithSize:[UITheme defaultToastFontSize]];
    self.label.textColor = [UIColor whiteColor];
    [UITheme alignCenter:self.label];
}

- (void)showText:(NSString *)text
{
    [self setText:text];
    [self show];
}

- (void)show
{
    [self.spinner startAnimating];
    UIView_SetHidden_Fade(self, NO, 0.4);
}

- (void)clear_
{
    self.hidden = YES;
    [self setText:@""];
    [self setOnCancel:nil];
    [self.spinner stopAnimating];
}

- (void)hide
{
    UIView_SetHiddenWithCompletion_Fade(self, YES, 0.4, ^(BOOL done) { [self clear_]; });
}

+ (void)setRootView:(UIView *)view
{
    if (SPINNER_ROOT_VIEW == view)
        return;
    else if (SPINNER_ROOT_VIEW != nil)
        [SPINNER_ROOT_VIEW release];
    if (view)
        [view retain];
    SPINNER_ROOT_VIEW = view;
}

+ (void)_hideSpinner
{
    if (SPINNER_VIEW)
    {
        [SPINNER_VIEW hide];
        [SPINNER_VIEW removeFromSuperview];
        [SPINNER_VIEW release];
    }
    SPINNER_VIEW = nil;
}

+ (void)_showSpinner
{
    [SpinnerView _hideSpinner];
    UIView *rootView = SPINNER_ROOT_VIEW;
    if (!rootView)
        rootView = [UIApplication sharedApplication].keyWindow.rootViewController.view;
    SPINNER_VIEW = [[SpinnerView alloc] initWithFrame:rootView.frame];
    [rootView addSubview:SPINNER_VIEW];
    [SPINNER_VIEW show];
}

+ (void)showSpinner
{
    PerformOnMainThread([SpinnerView class], @selector(_showSpinner), nil, NO);
}

+ (void)hideSpinner
{
    PerformOnMainThread([SpinnerView class], @selector(_hideSpinner), nil, NO);
}

@end
