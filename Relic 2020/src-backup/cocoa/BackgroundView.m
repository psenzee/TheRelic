//
//  BackgroundView.m
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BackgroundView.h"
#import "RectUtil.h"
#import "SimpleCall.h"
#import "UITheme.h"
#import "GlobalSetters.h"

@implementation BackgroundView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor blackColor];
        view = [[UIImageView alloc] initWithImage:[[UITheme shared] imageForKey:@"BACKGROUND"]];
        morph = [[UIImageView alloc] initWithImage:[[UITheme shared] imageForKey:@"BACKGROUND_MORPH"]];
        radius = 10.0; theta = 0.0;
        // this fills and crops, keeping the aspect ratio
        CGSize sz = GetProportionalSizeByWidth(view.image.size, frame.size.height);
        frame = self.bounds;
        frame.size = sz;
        view.frame = GrowRectByPixels(frame, radius * 2.0, radius * 2.0);
        origin = view.frame.origin;
        view.backgroundColor = [UIColor clearColor];
        [self addSubview:morph];
        [self addSubview:view];
        view.alpha = 0.0; morph.alpha = 0.0;
        [UITheme registerUIUpdate:[SimpleCall callWithSelector:@selector(update) forTarget:self]];
    }
    return self;
}

- (void)update
{
    theta += 0.01;
    CGRect fr = view.frame;
    fr.origin.x = origin.x + cos(theta) * radius;
    fr.origin.y = origin.y + sin(theta) * radius;
    view.frame = fr;
    morph.frame = fr;
    CGFloat alpha = (1.0 + sin(theta * 10.0)) * 0.5;
    CGFloat overall = (1.0 - cos(theta * 0.5)) * 1.0;;
    if (overall > 1.0)
        overall = 1.0;
    view.alpha = alpha * overall;//pow(alpha, 3.0);
    morph.alpha = (1.0 - alpha) * overall;//pow(1.0 - alpha, 3.0);
    if (!GetUseBackdrop())
    {
        view.alpha = 0.0;
        morph.alpha = 0.0;
    }
}

@end
