//
//  FascGameView.mm
//

#import "FascGameView.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "RectUtil.h"
#import "ImageUtilities.h"
#import "UITheme.h"
#import "UIUtil.h"
#import <stdlib.h>

#import "RectAnimator.h"

@interface SignImageView : UIView
{
    CGRect imageViewFrame_;
}

@property (nonatomic, retain) UIImageView *imageView;

- (id)initWithFrame:(CGRect)frame imageName:(NSString *)imageName imageScale:(CGFloat)scale;

@end

@implementation SignImageView

- (UIView *)initWithFrame:(CGRect)frame imageName:(NSString *)imageName imageScale:(CGFloat)scale
{
    self = [super initWithFrame:frame];
    if (self)
    {
        CGRect fr = self.bounds;
        self.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.25];
        UIImage *image = [UIImage imageNamed:imageName];
        CGSize sz = GetProportionalSizeByWidth(image.size, fr.size.width);
        imageViewFrame_ = ScaleRect(CenterInRect(sz, fr), scale);
        UIImageView *iv = [[UIImageView alloc] initWithFrame:imageViewFrame_];
        iv.image = image;
        iv.layer.shadowOpacity = 0.5;
        iv.layer.shadowOffset = CGSizeZero;
        iv.layer.shadowRadius = RealWidth(50.0);
        iv.layer.shadowColor = [UIColor blackColor].CGColor;
        [self addSubview:iv];
        self.imageView = iv;
        [iv release];
    }
    return self;
}

- (void)show:(BOOL)show
{
    CGRect offscreen = imageViewFrame_;
    offscreen.origin.y -= self.bounds.size.height;
    if (show && self.hidden)
    {
        self.imageView.frame = offscreen;
        [RectAnimator bounceView:self.imageView to:imageViewFrame_ duration:1.0 bounciness:0.25 reverse:NO];
    }
    else if (!show && !self.hidden)
    {
        self.imageView.frame = imageViewFrame_;
 //     [RectAnimator bounceView:self.imageView to:offscreen duration:2.0 bounciness:0.25 reverse:YES];
    }
    UIView_SetHidden_Fade(self, !show, 0.4);
}

@end

@interface FascGameView ()

@property (nonatomic, retain) SignImageView *winner;
@property (nonatomic, retain) SignImageView *roll;
@property (nonatomic, retain) SignImageView *ready;

@end

extern void GamePlay_InstantWin();

@implementation FascGameView

- (void)buttonPressedWithID:(NSString *)str
{
    [SimpleCall call:self.onButtonPressed withObject:str];
}

- (void)addKey:(NSString *)key toButton:(UIButton *)button
{
    SimpleCall *c = [Closure close:@selector(buttonPressedWithID:) forTarget:self withObject:key];
    [button addTarget:c action:@selector(call) forControlEvents:UIControlEventTouchUpInside];
}

- (void)setPauseEnabled:(BOOL)enabled
{
    self.pause.hidden = !enabled;
}

- (void)instantWin
{
    GamePlay_InstantWin();
}

- (SignImageView *)addImageIndicator:(NSString *)imageName scale:(CGFloat)scale
{
    SignImageView *sv = [[SignImageView alloc] initWithFrame:self.bounds imageName:imageName imageScale:scale];
    [sv show:NO];
    [self addSubview:sv];
    return sv;
}

- (void)showWinner:(BOOL)show
{
    [self.winner show:show];
}

- (void)showReady:(BOOL)show
{
    [self.ready show:show];
}

- (void)showRoll:(BOOL)show
{
    [self.roll show:show];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.pause = [UIButton buttonWithType:UIButtonTypeCustom];

        UIImage *star = [UIImage imageNamed:@"starbutton"];
        [self.pause setImage:star forState:UIControlStateNormal];
        CGRect fr = CGRectMake(5.0, 5.0, star.size.width * 0.5, star.size.height * 0.5);
        self.pause.frame = fr;
        [self addKey:@"PAUSE_MENU" toButton:self.pause];
        [self addSubview:self.pause];
        
        self.winner = [self addImageIndicator:@"winner2" scale:0.9];
        self.roll   = [self addImageIndicator:@"roll" scale:0.55];
        self.ready  = [self addImageIndicator:@"ready" scale:0.75];
        
        UIButton *instantWinButton = [UIButton buttonWithType:UIButtonTypeCustom];
        instantWinButton.frame = RectInsideBottom(self.bounds, 50.0);
        [instantWinButton setTitle:@"[debug: instant win]" forState:UIControlStateNormal];
        [instantWinButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [instantWinButton setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
        instantWinButton.titleLabel.font = [UITheme defaultFontWithSize:20.0];
        [self addSubview:instantWinButton];
        [instantWinButton addTarget:self action:@selector(instantWin) forControlEvents:UIControlEventTouchUpInside];
     //   instantWinButton.hidden = YES;
    }
    return self;
}

@end