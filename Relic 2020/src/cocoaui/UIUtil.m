//
//  UIUtil.m
//

#import <UIKit/UIKit.h>
#import "UIUtil.h"

static CGSize  REFERENCE_SIZE = { 768.0, 1024.0 };
static BOOL    IS_LANDSCAPE = NO;
static CGFloat FONT_SCALE_IPHONE = 1.3;
static CGFloat FONT_SCALE_IPAD   = 1.0;

extern bool IsHiResDevice();

static CGSize OrientedSize(CGSize sz, BOOL landscape)
{
    CGFloat min = MIN(sz.width, sz.height);
    CGFloat max = MAX(sz.width, sz.height);
    return landscape ? CGSizeMake(max, min) : CGSizeMake(min, max);
}

static CGSize OrientedRatio(void)
{
    CGSize refSz = GetOrientedReferenceSize();
    CGSize screenSz = GetOrientedScreenSize();
    return CGSizeMake(screenSz.width / refSz.width,
                      screenSz.height / refSz.height);
}

static CGFloat GetFontScale(void)
{
    return iPadIdiom() ? FONT_SCALE_IPAD : FONT_SCALE_IPHONE;
}

BOOL iPadIdiom(void)
{
    return [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad;
}

BOOL iPhoneIdiom(void)
{
    return [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone;
}

BOOL IsLandscape(void)
{
    UIInterfaceOrientation iorient = [[UIApplication sharedApplication] statusBarOrientation];
    return UIInterfaceOrientationIsLandscape(iorient);
}

void SetIsLandscape(BOOL value)
{
    IS_LANDSCAPE = value;
}

void SetReferenceSize(CGSize size)
{
    REFERENCE_SIZE = size;
}

CGSize GetReferenceSize(void)
{
    return REFERENCE_SIZE;
}

CGFloat GetScreenContentScale(void)
{
    return [[UIScreen mainScreen] scale];
}

void PrintCGSize(CGSize size, const char *caption, bool newline)
{
    if (caption != NULL) {
        printf("%s: ", caption);
    }
    printf("(w:%.2f h:%.2f)", size.width, size.height);
    if (newline) {
        printf("\n");
    }
}

void PrintCGPoint(CGPoint point, const char *caption, bool newline)
{
    if (caption != NULL) {
        printf("%s: ", caption);
    }
    printf("(x:%.2f y:%.2f)", point.x, point.y);
    if (newline) {
        printf("\n");
    }
}

CGSize CGSizeSwap(CGSize sz)
{
    return CGSizeMake(sz.height, sz.width);
}

CGSize GetOrientedReferenceSize(void)
{
    return OrientedSize(REFERENCE_SIZE, IsLandscape());
}

CGSize GetScreenSize(void)
{
    return [UIScreen mainScreen].bounds.size;
}

CGSize GetOrientedScreenSize(void)
{
    return OrientedSize(GetScreenSize(), IsLandscape());
}

void PrintScreenSizes(void)
{
    printf("\n--------------------------------\n");
    printf(                                 "IsLandscape              : %s\n", IsLandscape() ? "yes" : "no");
    printf(                                 "IsHiResDevice            : %s\n", IsHiResDevice() ? "yes" : "no");
    printf(                                 "iPhoneIdiom              : %s\n", iPhoneIdiom() ? "yes" : "no");
    printf(                                 "iPadIdiom                : %s\n", iPadIdiom() ? "yes" : "no");
    PrintCGSize(GetScreenSize(),            "GetScreenSize            ", true);
    PrintCGSize(GetOrientedScreenSize(),    "GetOrientedScreenSize    ", true);
    PrintCGSize(GetReferenceSize(),         "GetReferenceSize         ", true);
    PrintCGSize(GetOrientedReferenceSize(), "GetOrientedReferenceSize ", true);
    PrintCGSize(OrientedRatio(),            "OrientedRatio            ", true);
    printf(                                 "ScreenContentScale       : %.2f\n", GetScreenContentScale());
    printf("--------------------------------\n");
}

void SetFontScale_iPhone(CGFloat value)
{
    FONT_SCALE_IPHONE = value;
}

void SetFontScale_iPad(CGFloat value)
{
    FONT_SCALE_IPAD = value;
}

CGFloat RealWidth(CGFloat value)
{
    CGSize ratio = OrientedRatio();
    return ratio.width * value;
}

CGFloat RealHeight(CGFloat value)
{
    CGSize ratio = OrientedRatio();
    return ratio.height * value;
}

CGSize RealSize(CGSize sz)
{
    CGSize ratio = OrientedRatio();
    return CGSizeMake(ratio.width * sz.width, ratio.height * sz.height);
}

CGPoint RealPoint(CGPoint p)
{
    CGSize ratio = OrientedRatio();
    return CGPointMake(ratio.width * p.x, ratio.height * p.y);
}

CGRect RealRect(CGRect r)
{
    CGRect rr = { RealPoint(r.origin), RealSize(r.size) };
    return rr;
}

CGFloat RealFontSize(CGFloat points)
{
    // relative to height, because that's the more stable dimension for fonts
    return RealHeight(points) * GetFontScale();
}

void UIView_SetHiddenWithCompletion_Fade(UIView *v, BOOL hide, NSTimeInterval duration, void (^completion)(BOOL))
{
    if (hide && !v.hidden)
    {
        [UIView animateWithDuration:duration animations:^{ v.alpha = 0.0; }
                         completion:^(BOOL complete){ v.hidden = YES; if (completion) completion(complete); }];
    }
    else if (!hide && v.hidden)
    {
        v.alpha = 0.0;
        v.hidden = NO;
        [UIView animateWithDuration:duration animations:^{ v.alpha = 1.0; }
                         completion:completion];
    }
}

void UIView_SetHidden_Fade(UIView *v, BOOL hide, NSTimeInterval duration)
{
    if (hide && !v.hidden)
    {
        [UIView animateWithDuration:duration animations:^{ v.alpha = 0.0; }
                         completion:^(BOOL complete){ v.hidden = YES; }];
    }
    else if (!hide && v.hidden)
    {
        v.alpha = 0.0;
        v.hidden = NO;
        [UIView animateWithDuration:duration animations:^{ v.alpha = 1.0; }
                         completion:nil];
    }
}

NSAttributedString *PlainString(NSString *text, UIColor *color)
{
    NSDictionary *attributes = @{NSForegroundColorAttributeName: color};
    return [[NSAttributedString alloc] initWithString:text
                                           attributes:attributes];
}

NSAttributedString *UnderlinedString(NSString *text, UIColor *color)
{
    NSDictionary *attributes = @{NSUnderlineStyleAttributeName: @(NSUnderlineStyleSingle),
                                         NSForegroundColorAttributeName: color};
    return [[NSAttributedString alloc] initWithString:text
                                           attributes:attributes];
}
