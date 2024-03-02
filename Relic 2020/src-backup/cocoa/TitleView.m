//
//  TitleView.mm
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TitleView.h"
#import "RectUtil.h"
#import "SimpleCall.h"
#import "UITheme.h"

@implementation TitleView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor clearColor];
        view = [[UIImageView alloc] initWithImage:[[UITheme shared] imageForKey:@"TITLE"]];
        radius = 4.0; theta = 0.0;
        CGSize sz = GetProportionalSizeByWidth(view.image.size, frame.size.width);
        view.frame = RectWithY(CenterRectInRect(CGRectMake(0, 0, sz.width, sz.height), frame), 0.0);
        initialfr = view.frame;
        view.backgroundColor = [UIColor clearColor];
        [self addSubview:view];
        [UITheme registerUIUpdate:[SimpleCall callWithSelector:@selector(update) forTarget:self]];
    }
    return self;
}

- (void)update
{
    theta -= 0.1;
    CGFloat range = 0.025;
    CGRect fr = initialfr;
//    fr.origin.x = origin.x + cos(theta) * radius;
//    fr.origin.y = origin.y + sin(theta) * radius;
    CGFloat neww = initialfr.size.width  + (sin(theta) * initialfr.size.width  * range);
    CGFloat newh = initialfr.size.height + (sin(theta) * initialfr.size.height * range);
    fr.size.width = neww;
    fr.size.height = newh;
    fr.origin.x -= (neww - initialfr.size.width) * 0.5;
    fr.origin.y -= (newh - initialfr.size.height) * 0.5;
    view.frame = fr;
}

@end
