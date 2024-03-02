//
//  FascinationController.m
//

#import "FascinationController.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "BackgroundView.h"
#import "Transitioner.h"
#import "TitleView.h"
#import "RectUtil.h"
#import "ImageUtilities.h"
#import "FlyingButtonView.h"
#import "AnimationView.h"
#import "MoveView.h"
#import "Mover.h"
#import "UITheme.h"
#import <stdlib.h>
#import "BaseView.h"
#import "FascMenuView.h"
#import "FascGameView.h"
#import "FascPauseView.h"
#import "FascSignInView.h"
#import "FascRegisterView.h"
#import "DebugSettingsView.h"
#import "GlobalSetters.h"
#import "SoundManager.h"
#import "GamePlay.h"
#import "ToastView.h"
#import "SpinnerView.h"
#import "events_c.h"
#import "CloudServices.h"
#import "ClientState.h"
#import "ModalViews.h"
#import "LoginView.h"
#import "UIUtil.h"

#import "DataUtil.h"

@interface Toast : NSObject

@property (nonatomic, retain) NSString       *text;
@property (nonatomic)         NSTimeInterval  duration;

- (id)initWithText:(NSString *)text duration:(NSTimeInterval)duration;
- (void)show;

@end

@implementation Toast

- (id)initWithText:(NSString *)text duration:(NSTimeInterval)duration
{
    self = [super init];
    if (self)
    {
        self.text = text;
        self.duration = duration;
    }
    return self;
}

- (void)show
{
    if ([FascinationController shared])
        [[FascinationController shared] showToast:self.text duration:self.duration];
    [self release];
}

@end

static FascinationController *SHARED = nil;

const double LIGHTING_INCREMENT = 0.05;
const double DEPTH_INCREMENT    = 0.1;

extern void SetDefaultLightingType(int type);

void UI_SetPauseEnabled(int enabled)
{
    if ([FascinationController shared])
        [[FascinationController shared] setPauseEnabled:enabled];
}

void UI_ReturnToLobby()
{
    if ([FascinationController shared])
        [[FascinationController shared] onButton:@"SIGN_IN_AND_PLAY"];
}

void UI_ShowWinner(int enabled)
{
    if ([FascinationController shared])
        [[FascinationController shared].gameView showWinner:enabled];
}

void UI_ShowRoll(int enabled)
{
    if ([FascinationController shared])
        [[FascinationController shared].gameView showRoll:enabled];
}

void UI_ShowReady(int enabled)
{
    if ([FascinationController shared])
        [[FascinationController shared].gameView showReady:enabled];
}

void UI_ShowToastCStr(const char *text, double duration, double delay)
{
    if ([FascinationController shared] && text && *text)
    {
        NSString *str = [NSString stringWithFormat:@"%s", text];
        if (delay == 0.0)
            [[FascinationController shared] showToast:str duration:duration];
        else
        {
            Toast *toast = [[Toast alloc] initWithText:str duration:duration];
            PerformOnMainThreadAfterDelay(toast, @selector(show), nil, delay);
        }
    }
}

@implementation FascinationController

+ (FascinationController *)shared
{
    return SHARED;
}

- (void)setPauseEnabled:(BOOL)enabled
{
    if (self.gameView)
        [self.gameView setPauseEnabled:enabled];
}

- (void)showToast:(NSString *)str duration:(NSTimeInterval)duration
{
    [self.toastView showText:str duration:duration];
}

- (id)init
{
    self = [super init];
    if (self)
    {
        SHARED = self;
        SetFontScale_iPhone(1.25);
        self.updateInterval = 0.2;
        [NSURLCache setSharedURLCache:[[NSURLCache alloc] initWithMemoryCapacity:0 diskCapacity:0 diskPath:nil]];
    }
    return self;
}

- (void)_transitionToGameScreen
{
    [self.transitioner showScreen:self.gameView];
}

- (void)_startGame
{
    Events_Fire("SCHEDULER", "DO_START_GAME", "NO_INFO");
    PerformOnMainThreadAfterDelay(self, @selector(_transitionToGameScreen), nil, 0.25);
}

- (void)logout
{
    [[CloudServices shared] request_logout];
    [self.signInView setLoginId:@""];
}

- (void)showGame
{
    SetIsRendering(YES);
    SetIsUpdating(YES);
    // we need to wait until we start updating or our events get messed up
    PerformOnMainThreadAfterDelay(self, @selector(_startGame), nil, 0.25);
}

- (void)hideGame
{
//  SoundManager_SetAmbientVolume(0.0);
    SoundManager_SetThemeVolume(1.0);
    SetIsRendering(NO);
    SetIsUpdating(NO);
}

- (void)onButton:(NSString *)identifier
{
    if ([identifier isEqualToString:@"SIGN_IN_AND_PLAY"])
    {
        HideSpinner();
        type = LIGHT_ZERO;
        [self.baseView doShow];
        [self.baseView hideTitle];
        [self.transitioner showScreen:self.signInView];
    }
    else if ([identifier isEqualToString:@"PLAY"])
    {
        type = LIGHT_FULL;
        [self.baseView doHide];

        [self showGame];
    }
    else if ([identifier isEqualToString:@"PLAY_AS_GUEST"])
    {
        type = LIGHT_FULL;
        [self.baseView doHide];
        [self.transitioner showScreen:self.gameView];
        [self showGame];
    }
    else if ([identifier isEqualToString:@"RESUME_PLAY"])
    {
        type = LIGHT_FULL;
        [self.baseView doHide];
        [self.transitioner showScreen:self.gameView];
    }
    else if ([identifier isEqualToString:@"MAIN_MENU"])
    {
        type = LIGHT_ZERO;
        [self.baseView doShow];
        [self.baseView showTitle];
        [self.transitioner showScreen:self.menuView];
        [self hideGame];
        HideLogin();
    }
    else if ([identifier isEqualToString:@"REGISTER_TO_PLAY"])
    {
        type = LIGHT_ZERO;
        [self.baseView doShow];
        [self.baseView hideTitle];
        [self.transitioner showScreen:self.registerView];
        [self hideGame];
        HideLogin();
    }
    else if ([identifier isEqualToString:@"PAUSE_MENU"])
    {
        type = LIGHT_HALF;
        [self.baseView doHide];
        [self.transitioner showScreen:self.pauseView];
    }
    else if ([identifier isEqualToString:@"DEV_SETTINGS"])
    {
        type = LIGHT_FULL;
        [self.baseView doHide];
        [self.transitioner showScreen:self.debugView];
    }
}

- (void)showMainScreen
{
    [[CloudServices shared] request_loadRooms];
    [self.baseView showTitle];
    [self.transitioner showScreen:self.menuView];
    [UITheme registerUIUpdate:[SimpleCall callWithSelector:@selector(update) forTarget:self]];
}

- (void)startApp
{
//  SoundManager_PlayTheme("bensound-retrosoul.mp3", 1.0);
    SoundManager_PlayTheme("rolling-sounds.mp3", 0.5);
//  SoundManager_PlaySoundAtOrigin("bell", 1.0);
    PerformOnMainThreadAfterDelay(self, @selector(showMainScreen), nil, 0.5);
}

- (void)initViews:(UIView *)parent
{
    CGRect frame = parent.frame;
    
    lighting = -1.0;
    direction = 0.0;
    
    self.baseView = [[BaseView alloc] initWithFrame:frame];
    self.menuView = [[FascMenuView alloc] initWithFrame:frame];
    self.gameView = [[FascGameView alloc] initWithFrame:frame];
    self.signInView = [[FascSignInView alloc] initWithFrame:frame];
    self.registerView = [[FascRegisterView alloc] initWithFrame:frame];
    self.pauseView = [[FascPauseView alloc] initWithFrame:frame];
    self.debugView = [[DebugSettingsView alloc] initWithFrame:frame];

    self.menuView.hidden = YES;
    self.gameView.hidden = YES;
    self.pauseView.hidden = YES;
    self.debugView.hidden = YES;
    self.signInView.hidden = YES;
    self.registerView.hidden = YES;
    
    self.menuView.onButtonPressed   = [SimpleCall callWithSelector:@selector(onButton:) forTarget:self];
    self.gameView.onButtonPressed   = [SimpleCall callWithSelector:@selector(onButton:) forTarget:self];
    self.pauseView.onButtonPressed  = [SimpleCall callWithSelector:@selector(onButton:) forTarget:self];
    self.signInView.onButtonPressed = [SimpleCall callWithSelector:@selector(onButton:) forTarget:self];
    self.registerView.onButtonPressed = [SimpleCall callWithSelector:@selector(onButton:) forTarget:self];
    self.debugView.onReturnPressed  = [SimpleCall callWithSelector:@selector(onButton:) forTarget:self];
    
    self.transitioner = [[Transitioner alloc] init];
    
    [self.transitioner registerScreen:self.registerView];
    [self.transitioner registerScreen:self.menuView];
    [self.transitioner registerScreen:self.signInView];
    [self.transitioner registerScreen:self.gameView];
    [self.transitioner registerScreen:self.pauseView];
    [self.transitioner registerScreen:self.debugView];
    
    [parent addSubview:self.baseView];
    [parent addSubview:self.gameView];
    [parent addSubview:self.menuView];
    [parent addSubview:self.pauseView];
    [parent addSubview:self.registerView];
    [parent addSubview:self.signInView];
    [parent addSubview:self.debugView];
    
    self.toastView   = [[ToastView alloc] initWithFrame:frame];
    self.spinnerView = [[SpinnerView alloc] initWithFrame:frame];
    
    [parent addSubview:self.toastView];
    [parent addSubview:self.spinnerView];
    
    self.toastView.hidden = YES;
    self.spinnerView.hidden = YES;

    self.signInView.onLogoutPressed =
        [SimpleCall callWithSelector:@selector(logout) forTarget:self];
    self.signInView.onLoginPressed =
        [SimpleCall callWithSelector:@selector(login) forTarget:self];

    [CloudServices shared].delegate = self;
    [ModalViews createOnView:parent style:ModalViewStyle_BlackOnWhite];
    
    [ModalViews shared].login.onLogin = [SimpleCall callWithSelector:@selector(loginWithCredentials:) forTarget:self];
    
    PerformOnMainThreadAfterDelay(self, @selector(startApp), nil, 0.35);
}

- (void)cancelLogin
{
    HideSpinner();
    NSLog(@"$TODO LOGIN CANCELLED! HANDLE");
}

- (void)cancelConnect
{
    [[CloudServices shared] request_cancelWait];
    [self waitSucceeded:NO forServices:[CloudServices shared]];
    [[CloudServices shared].clientState reset];
    UI_ReturnToLobby();
}

- (void)loginWithCredentials:(LoginCredentials *)creds
{
    if (!creds.isMinimallyValid)
        ShowErrorToast(@"Please enter a valid username and password.");
    else
    {
        [CloudServices shared].login = creds.login;
        if (![[CloudServices shared] request_loginWithPassword:creds.password])
            ShowErrorToast(@"Something went wrong.  Please try again.");
        else
            ShowSpinnerWithCancel(@"Logging in..", ^() { [self cancelLogin]; });
    }
}

- (void)verifySessionSucceeded:(BOOL)success forServices:(CloudServices *)services
{
    if (success)
        [self.signInView setLoginId:[CloudServices shared].login];
}

- (void)loadRoomsSucceeded:(BOOL)success forServices:(CloudServices *)services
{
    if (success)
        [self.signInView listUpdate];
}

- (void)updateSucceeded:(BOOL)success forServices:(CloudServices *)services
{
    NSLog(@"** UPDATE SUCCEEDED: %d **", success);
    static int errorCount = 0;
    NSDictionary *dict = services.response;
    if ([CloudServices shared].cancellingConnect)
    {
        // we're cancelling..  do nothing (or maybe some cleanup needs to go here)
    }
    else if (!success || !dict)
    {
        GeneralError *error = [CloudServices shared].error;
        if (error.identifier && ([error.identifier isEqualToString:@"SERVER_DOWN"] ||
                                 [error.identifier isEqualToString:@"NOT_IN_LOBBY"])) // some errors we handle immediately
        {
            errorCount = 0;
            HideSpinner();
            ShowErrorToast(error.message);
            [[CloudServices shared].clientState reset];
            UI_ReturnToLobby();
        }
        else
        {
            errorCount++;
            if (errorCount > 5) // fails more than five times in a row, give up
            {
                HideSpinner();
                ShowErrorToast(error.message);
                errorCount = 0;
            }
            else // try again
            {
                ClientState *cs = services.clientState;
           //     PerformOnMainThreadAfterDelay(cs, @selector(ping), nil, self.updateInterval);
            }
        }
    }
    else
    {
        errorCount = 0;
        ClientState *cs = services.clientState;
        if ([cs waiting])
            UpdateSpinnerWithCancel(cs.statusMessage, ^(){ [self cancelConnect]; });
        else if (cs.state == ClientState_READY)
        {
            // we're done waiting
            HideSpinner();
            [self onButton:@"PLAY"]; // temporarily..
        }
        else if (cs.state == ClientState_ROLLING)
        {
            // start updating the table states
        }
  //      PerformOnMainThreadAfterDelay(cs, @selector(ping), nil, self.updateInterval);
    }
            ClientState *cs = services.clientState;
        PerformOnMainThreadAfterDelay(cs, @selector(ping), nil, self.updateInterval);
}

- (void)cancelWaitSucceeded:(BOOL)success forServices:(CloudServices *)services
{
    if (success)
    {
    }
    else
    {
    }
}

- (void)waitSucceeded:(BOOL)success forServices:(CloudServices *)services
{
    if (success)
    {
        ClientState *cs = services.clientState;
        UpdateSpinnerWithCancel(cs.statusMessage, ^(){ [self cancelConnect]; });
        [cs ping];
    }
    else
    {
        HideSpinner();
        NSString *message = @"Failed to join game.";
        if (services.error.message)
        {
            if ([services.error.identifier isEqualToString:@"LOGIN_FIRST"])
            {
                [self.signInView showLoginWhenNecessary];
                return;
            }
            else
            {
                message = [NSString stringWithFormat:@"%@ %@", message, services.error.message];
                NSLog(@"ERROR IDENTIFIER: %@", services.error.identifier);
            }
        }
        ShowErrorToast(message);
    }
}

- (void)loginSucceeded:(BOOL)success forServices:(CloudServices *)services
{
    if (success)
    {
        [[ModalViews shared] hideLogin];
        [self.signInView setLoginId:[CloudServices shared].login];
    }
    else
    {
        ShowErrorToast(services.error.message);
    }
    HideSpinner();
}

- (void)logoutSucceeded:(BOOL)success forServices:(CloudServices *)services
{
    if (!success)
    {
        ShowErrorToast(services.error.message);
    }
    HideSpinner();
}

- (void)registerSucceeded:(BOOL)success forServices:(CloudServices *)services
{
    if (!success)
    {
        ShowErrorToast(services.error.message);     
    }
    HideSpinner();
}

double CAM_DEPTH_SPEED_UPDATE = 0.02;

- (void)update
{
    switch (type)
    {
    case LIGHT_FULL:
        SetCameraDepthSpeed(-CAM_DEPTH_SPEED_UPDATE);
        lighting += LIGHTING_INCREMENT * 2.0;
        if (lighting >= 1.0)
            lighting = 1.0;
        break;
    case LIGHT_HALF:
        SetCameraDepthSpeed(CAM_DEPTH_SPEED_UPDATE);
        if (lighting > 0.2)
            lighting -= LIGHTING_INCREMENT;
        else if (lighting < -0.2)
            lighting += LIGHTING_INCREMENT;
        break;
    case LIGHT_ZERO:
        SetCameraDepthSpeed(CAM_DEPTH_SPEED_UPDATE);
        lighting -= LIGHTING_INCREMENT * 2.0;
        if (lighting <= -1.0)
            lighting = -1.0;
        break;
    }
    SetLightingLevel(lighting);
}

@end
