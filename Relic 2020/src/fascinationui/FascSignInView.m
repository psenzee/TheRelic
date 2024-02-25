//
//  FascSignInView.m
//

#import "FascSignInView.h"

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
#import "UIUtil.h"
#import <stdlib.h>
#import "ListView.h"
#import "CloudServices.h"
#import "ModalViews.h"

UIView *GetRootView()
{
    UIApplication *app = [UIApplication sharedApplication];
    UIWindow *window = app ? app.keyWindow : nil;
    UIViewController *vc = window ? window.rootViewController : nil;
    return vc ? vc.view : nil;
}

void DismissKeyboardForView(UIView *view)
{
    if (!view)
        return;
    for (UIView *v in view.subviews)
        if (v) DismissKeyboardForView(v);
    [view resignFirstResponder];
}

void DismissKeyboard()
{
    DismissKeyboardForView(GetRootView());
}

@implementation FascSignInView

- (BOOL)hasBackground
{
    return YES;
}

- (void)buttonPressedWithID:(NSString *)str
{
    [SimpleCall call:self.onButtonPressed withObject:str];
}

- (void)goMainMenu
{
    [SimpleCall call:self.onButtonPressed withObject:@"MAIN_MENU"];
}

- (void)loginPressed
{
    DismissKeyboard();
    [SimpleCall call:self.onLoginPressed];
}

- (void)logoutPressed
{
    DismissKeyboard();
    [SimpleCall call:self.onLogoutPressed];
}

- (int)listCount
{
    return self.rooms ? self.rooms.count : 0;
}

- (void)listUpdate
{
    NSMutableArray *arr = [CloudServices shared].rooms;
    if (arr && !self.rooms)
        self.rooms = arr;
}

- (NSString *)listNameAtIndex:(int)index
{
    if (!self.rooms) return @"[Loading..]";
    RoomInfo *info = (RoomInfo *)[self.rooms objectAtIndex:index];
    return info ? info.name : @"[Can't load..]";
}

- (void)startWaitForGame:(int)index
{
    RoomInfo *info = (RoomInfo *)[self.rooms objectAtIndex:index];
    ShowSpinner([NSString stringWithFormat:@"Joining %@", info.name]);
    [[CloudServices shared] request_waitForGame:info.identifier];
}

- (void)listSelectIndex:(int)index
{
    CloudServices *cs = [CloudServices shared];
    if ([cs isLoginNecessary])
        [cs showLoginWhenNecessary:^(NSString *login)
         { [self setLoginId:cs.login]; [self startWaitForGame:index]; }];
    else
        [self startWaitForGame:index];
}

- (id)listObjectAtIndex:(int)index
{
    if (!self.rooms)
        return nil;
    RoomInfo *info = (RoomInfo *)[self.rooms objectAtIndex:index];
    return info ? info.identifier : @"";
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        // buttons
        self.buttons = [[NSMutableArray alloc] init];
        
        self.tables = [[ListView alloc] initWithFrame:frame withTitle:@"Select Room"];
        self.tables.delegate = self;
        self.tables.onDone = [SimpleCall callWithSelector:@selector(goMainMenu) forTarget:self];
        
        [self addSubview:self.tables];
        
        CGRect fr = self.bounds;
        
        fr.size.height = RealHeight(40.0);

        self.login = [UIButton buttonWithType:UIButtonTypeCustom];
        fr.origin.x += fr.size.width * (4.0 / 5.0);
        fr.origin.y += RealHeight(10.0);
        fr.size.width *= (1.0 / 5.0);
        self.login.frame = fr;
        self.login.titleLabel.font = [UITheme defaultFontWithSize:16.0];
        self.login.titleLabel.textAlignment = NSTextAlignmentRight;
        [self.login setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.login setTitleColor:[UIColor redColor]   forState:UIControlStateHighlighted];
        [self addSubview:self.login];
        
        [self.login addTarget:self action:@selector(logoutPressed) forControlEvents:UIControlEventTouchUpInside];
        
         ListenForPropertyChange_Hidden(self, self, @selector(onHiddenChange));
    }
    return self;
}

- (void)showLoginWhenNecessary
{
    [[CloudServices shared] showLoginWhenNecessary:^(NSString *login) { [self setLoginId:[CloudServices shared].login]; }];
}

- (void)onHiddenChange
{
    if (!self.hidden)
    {
        [self showLoginWhenNecessary];
        [self.tables update];
    }
}

- (void)setLoginId:(NSString *)login
{
    NSString *text = @"";
    if (login != nil && login.length > 0)
        text = [NSString stringWithFormat:@"%@ [log out]", login];
    [self.login setAttributedTitle:UnderlinedString(text, [UIColor whiteColor]) forState:UIControlStateNormal];
    [self.login setAttributedTitle:PlainString(text, [UIColor grayColor]) forState:UIControlStateHighlighted];
    self.login.titleLabel.textAlignment = NSTextAlignmentRight;
    self.login.hidden = (text.length == 0);
}

@end
