//
//  FascMenuView.mm
//

#import "FascMenuView.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "BackgroundView.h"
#import "RectUtil.h"
#import "ImageUtilities.h"
#import "FlyingButtonView.h"
#import "AnimationView.h"
#import "MoveView.h"
#import "Mover.h"
#import "UITheme.h"
#import <stdlib.h>

extern void SetDefaultLightingType(int type);

@implementation FascMenuView

- (BOOL)hasBackground
{
    return YES;
}

- (void)buttonPressedWithID:(NSString *)str
{
    [SimpleCall call:self.onButtonPressed withObject:str];
}

- (void)addBackground
{
    if ([self hasBackground])
    {
        self.background = [[BackgroundView alloc] initWithFrame:self.frame];
        [self addSubview:self.background];

//                self.backgroundColor = [UIColor colorWithRed:0.01 green:0.0 blue:0.05 alpha:1.0];
    }
}

- (void)addTitle
{
    // title
    self.title = [[MoveView alloc] initWithFrame:RectWithY(RectInsideAt(self.frame, RectTop, 135.0), 25.0)];
    AnimationView *av = [[AnimationView alloc] initWithFrame:ZeroOrigin(self.title.frame)];
    [av addImagesWithBaseKey:@"TITLE"];
    [self.title addSubview:av];
    [self.title addMover:[Mover bouncer] withSpeed:0.001 withRange:0.075];
    [self.title addMover:[Mover oscillator] withSpeed:3.0 withRange:0.025];
    [self addSubview:self.title];
}

- (void)addButtonWithTitle:(NSString *)title key:(NSString *)key
{
    CGRect frame = self.frame;
   
    double start     = frame.size.height * 0.40;
    double increment = frame.size.height / 6.0;
    double xoffset   = frame.size.width  / (iPadIdiom() ? 4.0 : 6.0);
    
    int i = self.buttons.count;
    
    bool even = (i % 2) == 0;
    double xoffsetFactor = 1.0;//0.25;//1.0
    CGRect bfr = CGRectMake((!even ? -1.0 : 1.0) * xoffset * xoffsetFactor,
                            start + i * increment, frame.size.width, increment);
    FlyingButtonView *fbv = [[FlyingButtonView alloc] initWithFrame:bfr
                                                 backgroundImageKey:!even ? @"LEFT_BUTTON" : @"RIGHT_BUTTON"
                                                               text:title];
    [fbv setIncrement:(2.0 - rand() / (double)RAND_MAX) * 0.05];
    [self.buttons addObject:fbv];
        
    SimpleCall *c = [Closure close:@selector(buttonPressedWithID:) forTarget:self withObject:key];
    [fbv.button addTarget:c action:@selector(call) forControlEvents:UIControlEventTouchUpInside];
    
    [self addSubview:fbv];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
  //      [self addBackground];
  //      [self addTitle];
    
        // buttons
        self.buttons = [[NSMutableArray alloc] init];
    
        NSString *labels[] = { @"Sign In and Play",
                               @"Register to Play",
                               @"Play as Guest", nil };
    
        NSString *keys[] = { @"SIGN_IN_AND_PLAY",
                             @"REGISTER_TO_PLAY",
                             @"PLAY_AS_GUEST", nil };
    
        for (int i = 0; labels[i]; i++)
            [self addButtonWithTitle:labels[i] key:keys[i]];
    }
    return self;
}

@end
