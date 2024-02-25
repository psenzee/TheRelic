//
//  FascinationController.h
//  TonesPOC
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "CloudServices.h"

@class BaseView;
@class FascMenuView;
@class FascGameView;
@class FascPauseView;
@class FascSignInView;
@class FascRegisterView;
@class DebugSettingsView;
@class SpinnerView;
@class ToastView;
@class Transitioner;

void UI_SetPauseEnabled(int enabled);
void UI_ShowWinner(int enabled);
void UI_ReturnToLobby();

typedef enum { LIGHT_ZERO, LIGHT_HALF, LIGHT_FULL } LightingType;

@interface FascinationController : NSObject <CloudServicesDelegate>
{
    double lighting;
    double direction;
    
    LightingType type;
}

@property (nonatomic, retain) Transitioner      *transitioner;
@property (nonatomic, retain) BaseView          *baseView;
@property (nonatomic, retain) ToastView         *toastView;
@property (nonatomic, retain) SpinnerView       *spinnerView;

@property (nonatomic, retain) FascMenuView      *menuView;
@property (nonatomic, retain) FascGameView      *gameView;
@property (nonatomic, retain) FascPauseView     *pauseView;
@property (nonatomic, retain) FascSignInView    *signInView;
@property (nonatomic, retain) FascRegisterView  *registerView;
@property (nonatomic, retain) DebugSettingsView *debugView;

@property (nonatomic, retain) NSString          *connectState;
@property (nonatomic)         NSTimeInterval     updateInterval;

- (id)init;
- (void)initViews:(UIView *)parent;
- (void)setPauseEnabled:(BOOL)enabled;

- (void)showToast:(NSString *)str duration:(NSTimeInterval)duration;
- (void)showWinner:(BOOL)show;
- (void)showRoll:(BOOL)show;
- (void)showReady:(BOOL)show;

- (void)onButton:(NSString *)identifier;

+ (FascinationController *)shared;

@end
