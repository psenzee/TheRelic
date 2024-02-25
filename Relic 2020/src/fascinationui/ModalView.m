//
//  ModalView.m
//

#import "ModalView.h"
#import "RectUtil.h"
#import "SysUtil.h"
#import "UITheme.h"
#import "UIUtil.h"
#import "DrawUtil.h"
#import "SimpleCall.h"

@interface ModalView ()

@property (nonatomic, retain) UIView  *background;
@property (nonatomic, retain) UIView  *modalContent;

@end

@implementation ModalView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.5];
        UIView *background = [[UIImageView alloc] initWithFrame:self.bounds];
        [self addSubview:background];
        self.background = background;
        [background release];
        self.background.backgroundColor = [UIColor whiteColor];
        self.background.layer.cornerRadius = 10.0;
        self.background.layer.masksToBounds = YES;
    }
    return self;
}

- (void)showWithContentView:(UIView *)content
{
    if (self.modalContent)
        [self.modalContent removeFromSuperview];
    // set up the content view here
    self.modalContent = content;
    [self.background addSubview:content];
    self.background.frame = CenterInRect(content.frame.size, self.bounds);
    content.frame = ZeroOrigin(content.frame);
    [self.background addSubview:content];
    UIView_SetHidden_Fade(self, NO, 0.4);
}
                             
- (void)_clearContent
{
    [self.modalContent removeFromSuperview];
    self.modalContent = nil;
}

- (void)dismiss
{
    UIView_SetHiddenWithCompletion_Fade(self, YES, 0.4, ^(BOOL complete){ [self _clearContent]; });
}

@end
