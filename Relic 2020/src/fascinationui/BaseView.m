//
//  BaseView.m
//

#import "BaseView.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "BackgroundView.h"
#import "RectUtil.h"
#import "ImageUtilities.h"
#import "AnimationView.h"
#import "MoveView.h"
#import "Mover.h"
#import "UITheme.h"
#import "UIUtil.h"
#import <stdlib.h>

@implementation BaseView

- (BOOL)hasBackground
{
    return YES;
}

- (void)addBackground
{
    if ([self hasBackground])
    {
        self.background = [[BackgroundView alloc] initWithFrame:self.frame];
        [self addSubview:self.background];
    }
}

- (void)addTitle
{
    // title
    UIImage *title = [[UITheme shared] imageForKey:@"TITLE_1"];
    CGSize sz = GetProportionalSizeByWidth(title.size, self.frame.size.width);
    self.title = [[MoveView alloc] initWithFrame:RectWithY(RectInsideAt(self.frame, RectTop, sz.height), 25.0)];
    AnimationView *av = [[AnimationView alloc] initWithFrame:ZeroOrigin(self.title.frame)];
    [av addImagesWithBaseKey:@"TITLE"];
    [self.title addSubview:av];
    [self.title addMover:[Mover bouncer]    withSpeed:0.001 withRange:0.100];
    [self.title addMover:[Mover oscillator] withSpeed:3.000 withRange:0.025];
    [self addSubview:self.title];
}

- (void)doHide
{
    UIView_SetHidden_Fade(self, YES, 0.4);
}

- (void)doShow
{
    UIView_SetHidden_Fade(self, NO, 0.4);
}

- (void)showTitle
{
    UIView_SetHidden_Fade(self.title, NO, 0.4);
}

- (void)hideTitle
{
    UIView_SetHidden_Fade(self.title, YES, 0.4);
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        [self addBackground];
        [self addTitle];
        self.title.hidden = YES;
    }
    return self;
}

@end
