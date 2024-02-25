//
//  LoginView.h
//

#import <UIKit/UIKit.h>

@class SimpleCall;

@interface LoginCredentials : NSObject

@property (nonatomic, retain) NSString *login;
@property (nonatomic, retain) NSString *password;

- (BOOL)isMinimallyValid;

@end

@interface LoginView : UIView

@property (nonatomic, retain) SimpleCall *onLogin;
@property (nonatomic, retain) SimpleCall *onRegister;
@property (nonatomic, retain) SimpleCall *onCancel;
@property (nonatomic, retain) SimpleCall *onSkip;

- (id)initWithFrame:(CGRect)frame;

- (void)showText:(NSString *)text;
- (void)hide;

- (void)setDefaultLogin:(NSString *)text;

@end

