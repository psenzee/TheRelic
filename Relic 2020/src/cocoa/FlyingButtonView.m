//
//  TitleView.mm
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "FlyingButtonView.h"
#import "RectUtil.h"
#import "SimpleCall.h"
#import "UITheme.h"
#import "ImageUtilities.h"
#import "UIUtil.h"

UIImage *ComposeTextAndBackground(NSString *text, UIImage *background, CGSize scale, OutlinedTextType type)
{
    UIView *tmpView = [[UIView alloc] initWithFrame:
                          CGRectMake(0.0, 0.0, background.size.width  * scale.width,
                                               background.size.height * scale.height)];
    tmpView.backgroundColor = [UIColor clearColor];
    UIImageView *bv = [[UIImageView alloc] initWithImage:background];
    bv.backgroundColor = [UIColor clearColor];
    [tmpView addSubview:bv];
    [bv release];
    bv.frame = tmpView.frame;
    UIImage *textImage = [[UITheme shared] outlineTextImage:text forType:type expand:CGSizeMake(0.0, 0.0)];
    CGRect fr = CenterRectInRect(CGRectMake(0.0, 0.0, textImage.size.width, textImage.size.height), tmpView.frame);
    UIImageView *textImageView = [[UIImageView alloc] initWithImage:textImage];
    textImageView.frame = fr;
    [textImage release];
    [tmpView addSubview:textImageView];
    [textImageView release];
    UIImage *composed = CreateImageWithView(tmpView);
    [tmpView release];
    return composed;
}

@implementation FlyingButtonView

- (id)initWithFrame:(CGRect)frame backgroundImageKey:(NSString *)key text:(NSString *)text
{
    self = [super initWithFrame:frame];
    if (self)
    {
        increment = 0.1;
        self.backgroundColor = [UIColor clearColor];
        radius = 3.0; theta = 0.0;
        
        UIImage *background = [[UITheme shared] imageForKey:key];

        CGFloat scale = 0.5;//RealWidth(0.4);
        UIImage *normal = ComposeTextAndBackground(text, background, CGSizeMake(scale, scale), OutlinedText_Normal);
        UIImage *hilite = ComposeTextAndBackground(text, background, CGSizeMake(scale, scale), OutlinedText_Hilite);

        origin = frame.origin;
        
        self.button = [UIButton buttonWithType:UIButtonTypeCustom];
        
        CGSize sz = GetProportionalSizeByHeight(normal.size, frame.size.height);
        
        self.button.frame = CGRectMake((frame.size.width  - sz.width)  * 0.5,
                                       (frame.size.height - sz.height) * 0.5, sz.width, sz.height);
        
        [self.button setImage:normal forState:UIControlStateNormal];
        [self.button setImage:hilite forState:UIControlStateHighlighted];
        [self addSubview:self.button];
 
        [UITheme registerUIUpdate:[SimpleCall callWithSelector:@selector(update) forTarget:self]];

    }
    return self;
}

- (void)setIncrement:(CGFloat)inc_
{
    increment = inc_;
}

- (void)update
{
    theta += increment;
    CGRect fr = self.frame;
    fr.origin.x = origin.x + cos(theta) * radius;
    fr.origin.y = origin.y + sin(theta) * radius;
    self.frame = fr;
}

@end
