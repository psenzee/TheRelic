//
//  ModalViews.h
//

#import <UIKit/UIKit.h>
#import "SpinnerView.h"

void HideSpinner(void);
void ShowSpinner(NSString *text);
void ShowSpinnerWithCancel(NSString *text, CancelBlock onCancel);
void UpdateSpinner(NSString *text);
void UpdateSpinnerWithCancel(NSString *text, CancelBlock onCancel);

void HideToast(void);
void ShowToast(NSString *text);

void HideErrorToast(void);
void ShowErrorToast(NSString *text);

void ShowLogin(NSString *text);
void HideLogin(void);
void SetDefaultLogin(NSString *text);

@class SpinnerView;
@class ToastView;
@class LoginView;

typedef enum { ModalViewStyle_BlackOnWhite, ModalViewStyle_WhiteOnBlack } ModalViewStyle;

@interface ModalViews : NSObject

@property (nonatomic, retain) SpinnerView *spinner;
@property (nonatomic, retain) ToastView   *toast;
@property (nonatomic, retain) ToastView   *errorToast;
@property (nonatomic, retain) LoginView   *login;

- (void)showToast:(NSString *)text;
- (void)hideToast;

- (void)showErrorToast:(NSString *)text;
- (void)hideErrorToast;

- (void)showSpinner:(NSString *)text withCancel:(CancelBlock)cancel;
- (void)updateSpinner:(NSString *)text withCancel:(CancelBlock)cancel;
- (void)hideSpinner;

- (void)updateSpinner:(NSString *)text;

- (void)showLogin:(NSString *)text;
- (void)hideLogin;

- (void)setDefaultLogin:(NSString *)text;

// returns NO if already created
+ (BOOL)createOnView:(UIView *)view style:(ModalViewStyle)style;

+ (ModalViews *)shared;

@end


