//
//  AnimationView.m
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AnimationView.h"
#import "RectUtil.h"
#import "SimpleCall.h"
#import "UITheme.h"

@implementation AnimationView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        time = 0.0;
        last = -1;
        self.speed = 0.1;
        self.backgroundColor = [UIColor clearColor];
        self.autoresizesSubviews = YES;
        frames = [[NSMutableArray alloc] init];
        [UITheme registerUIUpdate:[SimpleCall callWithSelector:@selector(update) forTarget:self]];
    }
    return self;
}

- (void)addFrame:(UIView *)view
{
    if (view)
    {
        [self addSubview:view];
        view.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
        [frames addObject:view];
    }
}

- (void)addImage:(UIImage *)image
{
    if (image)
    {
        UIImageView *view = [[UIImageView alloc] initWithImage:image];
        view.frame = ZeroOrigin(self.frame);
        view.backgroundColor = [UIColor clearColor];
        [self addFrame:view];
    }
}

- (void)addImagesWithBaseKey:(NSString *)key
{
    UIImage *image = nil;
    for (int i = 1;; i++)
    {
        image = [[UITheme shared] imageForKey:[NSString stringWithFormat:@"%@_%d", key, i]];
        if (!image)
            break;
        [self addImage:image];
    }
}

- (void)update
{
    time += 0.033;
    NSUInteger count = frames.count;
    if (!self.hidden && count != 0)
    {
        NSUInteger index = (int)(time / self.speed) % count;
        if (index != last)
            for (NSUInteger i = 0; i < count; i++)
                ((UIView *)[frames objectAtIndex:i]).hidden = (i != index);
        last = index;
    }
}

@end
