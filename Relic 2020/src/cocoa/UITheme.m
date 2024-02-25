//
//  UITheme.m
//

#import <UIKit/UIKit.h>

#import "UITheme.h"
#import "UIUtil.h"
#import "RectUtil.h"
#import "EventWatcher.h"
#import "ImageUtilities.h"
#import "DataUtil.h"

static UITheme *SHARED_THEME = nil;

@implementation UITheme

- (void)setOutlineColor:(UIColor *)outline textColor:(UIColor *)text forType:(OutlinedTextType)type
{
    textAppearance[type].textColor = text;
    textAppearance[type].outlineColor = outline;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        imagePaths = [[NSMutableDictionary alloc] init];
        images = [[NSMutableDictionary alloc] init];
        
        UIColor *lightYellow = [UIColor colorWithRed:1.0 green:1.0 blue:0.7 alpha:1.0];
        UIColor *lightWhite = [UIColor whiteColor];
        UIColor *outlineColor = lightWhite;//lightYellow;
        
        [self setOutlineColor:outlineColor textColor:[UIColor blackColor] forType:OutlinedText_Normal];
        [self setOutlineColor:[UIColor blackColor] textColor:outlineColor forType:OutlinedText_Hilite];

        [imagePaths setObject:@"MarqueeSatBdr_1.png"                     forKey:@"TITLE"];
        [imagePaths setObject:@"MarqueeSatBdr_1.png"                     forKey:@"TITLE_1"];
        [imagePaths setObject:@"MarqueeSatBdr_2.png"                     forKey:@"TITLE_2"];
        
        [imagePaths setObject:@"paused.png"                              forKey:@"PAUSED_1"];
        
        [imagePaths setObject:@"left_button_red_gradient_small.png"      forKey:@"LEFT_BUTTON"];
        [imagePaths setObject:@"right_button_red_gradient_small.png"     forKey:@"RIGHT_BUTTON"];
        /*
        [imagePaths setObject:@"background_7.png"    forKey:@"BACKGROUND"];
        [imagePaths setObject:@"background_5.png"    forKey:@"BACKGROUND_MORPH"];
         */
        [imagePaths setObject:@"lightsbg1_.jpg"   forKey:@"BACKGROUND"];
        [imagePaths setObject:@"lightsbg2_.jpg"   forKey:@"BACKGROUND_MORPH"];
    }
    return self;
}

- (UIButton *)defaultOutlineButtonForText:(NSString *)text intoRect:(CGRect)frame expand:(CGSize)expand
{
    UIImage *normal = [[UITheme shared] outlineTextImage:text forType:OutlinedText_Normal expand:expand];
    UIImage *hilite = [[UITheme shared] outlineTextImage:text forType:OutlinedText_Hilite expand:expand];
    
    CGSize sz = MinArea(GetProportionalSizeByHeight(normal.size, frame.size.height),
                        GetProportionalSizeByWidth (normal.size, frame.size.width));
    
//  sz.width  = floor(sz.width  * scale.width);
//  sz.height = floor(sz.height * scale.height);
    
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];

 // button.backgroundColor = [UIColor cyanColor]; // debug

 // CGRect bfr = CenterRectInRect(CGRectMake(0, 0, sz.width, sz.height), frame);
 // bfr.origin.y -= 5.0; // hack??
    
    button.frame = ZeroOrigin(frame);//bfr;
    
    [button setImage:normal forState:UIControlStateNormal];
    [button setImage:hilite forState:UIControlStateHighlighted];
    
    return button;
}

- (UIImage *)outlineTextImage:(NSString *)text forType:(OutlinedTextType)type expand:(CGSize)expand
{
    OutlinedTextAppearance ota = textAppearance[type];
    UIImage *outlineText = OutlineTextImage([UITheme defaultFontWithSize:iPhoneIdiom() ? 72.0 : 36.0], text,
                                            ota.textColor, ota.outlineColor, 3.0);
    return outlineText;
}

- (UIImage *)outlineTextImage:(NSString *)text forType:(OutlinedTextType)type
{
    return [self outlineTextImage:text forType:type expand:CGSizeMake(0.0, 0.0)];
}

- (void)setImage:(UIImage *)image forKey:(NSString *)key
{
    if (!image)
    {
        NSLog(@"UITheme setImage:forKey: - trying to set a null image for key: %@", key);
        return;
    }
    [images setObject:image forKey:key];
}

- (NSString *)imageNameForKey:(NSString *)key
{
    return Dict_ObjectForKeyWithDefault(imagePaths, key, nil);
}

+ (UITheme *)shared
{
    if (!SHARED_THEME)
        SHARED_THEME = [[UITheme alloc] init];
    return SHARED_THEME;
}

+ (CGFloat)defaultToastFontSize
{
    return 20.0;
}

+ (BOOL)isLandscape
{
    return NO;
}

- (UIImage *)imageForKey:(NSString *)key
{
    UIImage *image = Dict_ObjectForKeyWithDefault(images, key, nil);
    if (image)
        return image;
    NSString *name = [self imageNameForKey:key];
    image = [UIImage imageNamed:name];
    if (image)
        [self setImage:image forKey:key];
    return image;
}
        
+ (void)alignCenter:(UILabel *)label
{
    // this is deprecated, so this exists to contain
    //   all the deprecation warnings in one place
    label.textAlignment = NSTextAlignmentCenter;
}

+ (UIFont *)defaultFontWithSize:(CGFloat)size
{
    return [UIFont fontWithName:@"Bernard MT Condensed" size:RealFontSize(size)];
}

+ (UIColor *)defaultTextColor
{
    return [UIColor whiteColor];
}

+ (UIColor *)defaultHighlightTextColor
{
    return [UIColor grayColor];
}

+ (UIColor *)defaultDarkBackgroundColor
{
    return [UIColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:0.35];
}

+ (UIButton *)defaultButton:(NSString *)name withFrame:(CGRect)frame
{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.frame = frame;
    button.titleLabel.font = [self defaultFontWithSize:24.0];
    [button setTitle:name forState:UIControlStateNormal];
    [button setTitleColor:[self defaultTextColor] forState:UIControlStateNormal];
    [button setTitleColor:[self defaultHighlightTextColor] forState:UIControlStateHighlighted];
    [button setBackgroundColor:[self defaultDarkBackgroundColor]];
    return button;
}

+ (CGFloat)defaultLabelFontSize
{
    return 24.0;
}

+ (UILabel *)defaultLabel:(NSString *)text withFrame:(CGRect)frame
{
    UILabel *label = [[UILabel alloc] initWithFrame:frame];
    label.backgroundColor = [UIColor clearColor];
    label.text = text;
    label.font = [self defaultFontWithSize:[self defaultLabelFontSize]];
    label.textColor = [self defaultTextColor];
    [self alignCenter:label];
    return label;
}

+ (UILabel *)defaultLabelWithFrame:(CGRect)frame
{
    return [self defaultLabel:@"" withFrame:frame];
}

+ (UILabel *)defaultTitleLabel:(NSString *)text withFrame:(CGRect)frame
{
    UILabel *label = [[UILabel alloc] initWithFrame:frame];
    label.backgroundColor = [UIColor clearColor];
    label.text = text;
    label.font = [self defaultFontWithSize:36.0];
    label.textColor = [self defaultTextColor];
    [self alignCenter:label];
    label.numberOfLines = 1;
    label.adjustsFontSizeToFitWidth = YES;
    return label;
}

+ (void)registerUIUpdate:(SimpleCall *)call
{
    [[EventWatcher shared] startRecurringEventWithName:@"UI_UPDATE" forKeys:@"UI_UPDATE" atTimeInterval:0.033 mainThread:YES];
    [[EventWatcher shared] addEventForKeys:@"UI_UPDATE" withCall:call];
}

+ (void)registerIntermittentUpdate:(SimpleCall *)call withInterval:(NSTimeInterval)interval
{
    NSString *key = [NSString stringWithFormat:@"INTERM_%.2f", interval];
    [[EventWatcher shared] startRecurringEventWithName:key forKeys:key atTimeInterval:interval mainThread:YES];
    [[EventWatcher shared] addEventForKeys:key withCall:call];
}

@end
