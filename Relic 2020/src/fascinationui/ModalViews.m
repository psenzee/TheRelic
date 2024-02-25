//
//  ModalViews.m
//

#import "ModalViews.h"
#import "SpinnerView.h"
#import "ToastView.h"
#import "LoginView.h"
#import "SysUtil.h"

static ModalViews *SHARED_INSTANCE = nil;

void HideSpinner()
{
    [[ModalViews shared] hideSpinner];
}

void ShowSpinnerWithCancel(NSString *text, CancelBlock onCancel)
{
    [[ModalViews shared] showSpinner:text withCancel:onCancel];
}

void ShowSpinner(NSString *text)
{
    ShowSpinnerWithCancel(text, nil);
}

void UpdateSpinnerWithCancel(NSString *text, CancelBlock onCancel)
{
    [[ModalViews shared] updateSpinner:text withCancel:onCancel];
}

void UpdateSpinner(NSString *text)
{
    UpdateSpinnerWithCancel(text, nil);
}

void HideToast()
{
    [[ModalViews shared] hideToast];
}

void ShowToast(NSString *text)
{
    [[ModalViews shared] showToast:text];
}

void HideErrorToast()
{
    [[ModalViews shared] hideErrorToast];
}

void ShowErrorToast(NSString *text)
{
    [[ModalViews shared] showErrorToast:text];
}

void ShowLogin(NSString *text)
{
    [[ModalViews shared] showLogin:text];
}

void SetDefaultLogin(NSString *text)
{
    [[ModalViews shared] setDefaultLogin:text];
}

void HideLogin()
{
    [[ModalViews shared] hideLogin];
}

@interface ModalViews ()

@property (nonatomic, retain) UIView *root;

- (id)initWithView:(UIView *)view;

@end

@implementation ModalViews

- (id)initWithView:(UIView *)view
{
    self = [super init];
    if (self)
    {
        UIColor *errColor = [UIColor colorWithRed:1.0 green:0.0 blue:0.0 alpha:1.0];
        
        self.toast      = [[ToastView alloc]   initWithFrame:view.bounds];
        self.errorToast = [[ToastView alloc]   initWithFrame:view.bounds
                                                  backgroundColor:errColor];
        self.spinner    = [[SpinnerView alloc] initWithFrame:view.bounds];
        self.login      = [[LoginView alloc]   initWithFrame:view.bounds];
        
        self.root = view;
        [view addSubview:self.toast];
        [view addSubview:self.errorToast];
        [view addSubview:self.spinner];
        [view addSubview:self.login];
        
        self.toast.hidden = YES;
        self.errorToast.hidden = YES;
        self.spinner.hidden = YES;
        self.login.hidden = YES;
    }
    return self;
}

- (void)_showToast:(NSString *)text
{
    [self.root bringSubviewToFront:self.toast];
    [self.toast showText:text];
}

- (void)showToast:(NSString *)text
{
    PerformOnMainThread(self, @selector(_showToast:), text, NO);
}

- (void)hideToast
{
    PerformOnMainThread(self.toast, @selector(hide), nil, NO);
}

- (void)_showErrorToast:(NSString *)text
{
    [self.root bringSubviewToFront:self.errorToast];
    [self.errorToast showText:text];
}

- (void)showErrorToast:(NSString *)text
{
    PerformOnMainThread(self, @selector(_showErrorToast:), text, NO);
}

- (void)hideErrorToast
{
    PerformOnMainThread(self.errorToast, @selector(hide), nil, NO);
}

- (void)_showSpinner:(NSString *)text
{
    [self.root bringSubviewToFront:self.spinner];
    [self.spinner showText:text];
}

- (void)_updateSpinner:(NSString *)text
{
    [self.root bringSubviewToFront:self.spinner];
    [self.spinner setText:text];
}

- (void)showSpinner:(NSString *)text withCancel:(CancelBlock)cancel
{
    [self.spinner setOnCancel:cancel];
    PerformOnMainThread(self, @selector(_showSpinner:), text, NO);
}

- (void)updateSpinner:(NSString *)text withCancel:(CancelBlock)cancel
{
    [self.spinner setOnCancel:cancel];
    PerformOnMainThread(self, @selector(_updateSpinner:), text, NO);
}

- (void)hideSpinner
{
    PerformOnMainThread(self.spinner, @selector(hide), nil, NO);
}

- (void)_showLogin:(NSString *)text
{
    [self.root bringSubviewToFront:self.login];
    [self.login showText:text];
}

- (void)_setDefaultLogin:(NSString *)text
{
    [self.login setDefaultLogin:text];
}

- (void)showLogin:(NSString *)text
{
    PerformOnMainThread(self, @selector(_showLogin:), text, NO);
}

- (void)setDefaultLogin:(NSString *)text
{
    PerformOnMainThread(self, @selector(_setDefaultLogin:), text, NO);
}

- (void)hideLogin
{
    PerformOnMainThread(self.login, @selector(hide), nil, NO);
}

+ (BOOL)createOnView:(UIView *)view style:(ModalViewStyle)style
{
    // not doing anything with style yet
    if (SHARED_INSTANCE)
        return NO;
    ModalViews *mv = [[ModalViews alloc] initWithView:view];
    SHARED_INSTANCE = mv;
    return YES;
}

+ (ModalViews *)shared
{
    return SHARED_INSTANCE;
}

@end
