//
//  FascRegisterView.m
//

#import "FascRegisterView.h"

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
#import "WebView.h"

@implementation FascRegisterView

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

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.web = [[WebView alloc] initWithFrame:self.bounds withTitle:@"Register to Play"];
  //    self.web.delegate = self;
        self.web.onDone = [SimpleCall callWithSelector:@selector(goMainMenu) forTarget:self];
        
        [self addSubview:self.web];
       /*
        CGRect fr = self.bounds;
        
        fr.size.height = RealHeight(40.0);
        */
        
        
         ListenForPropertyChange_Hidden(self, self, @selector(onHiddenChange));
    }
    return self;
}

- (void)onHiddenChange
{
    if (!self.hidden)
        [self.web loadURLString:@"http://playfascination.com/hk/html_mobile_register.php"];
}

@end
