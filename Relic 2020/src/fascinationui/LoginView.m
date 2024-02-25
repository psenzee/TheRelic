//
//  LoginView.m
//

#import "LoginView.h"
#import "RectUtil.h"
#import "SysUtil.h"
#import "UITheme.h"
#import "UIUtil.h"
#import "DrawUtil.h"
#import "SimpleCall.h"

static void _DismissKeyboardForView(UIView *view)
{
    if (!view)
        return;
    for (UIView *v in view.subviews)
        if (v) _DismissKeyboardForView(v);
    [view resignFirstResponder];
}

@implementation LoginCredentials

- (BOOL)isMinimallyValid
{
    return self.login && self.login.length > 0 &&
           self.password && self.password.length > 0;
}

@end

@interface LoginView ()

@property (nonatomic, retain) UIView                  *background;
@property (nonatomic, retain) UILabel                 *label;

@property (nonatomic, retain) UITextField             *field_login;
@property (nonatomic, retain) UITextField             *field_password;

@property (nonatomic, retain) UIButton                *button_login;
@property (nonatomic, retain) UIButton                *button_cancel;

// UITextFieldDelegate: for closing out the keyboard when done with text fields -
- (void)textFieldDidEndEditing:    (UITextField *)textField;
- (BOOL)textFieldShouldEndEditing: (UITextField *)textField;
- (void)textFieldDidBeginEditing:  (UITextField *)textField;
- (BOOL)textFieldShouldReturn:     (UITextField *)textField;

@end

@implementation LoginView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        CGFloat fontSize = 18.0;
    
        frame.origin.x = frame.origin.y = 0.0;
        
        CGFloat width = GetScreenSize().width * (iPhoneIdiom() ? 0.85 : 0.60);
        
        CGFloat lineHeight = iPhoneIdiom() ? RealHeight(90.0) : RealHeight(70.0);
        
        frame = CenterInRect(CGSizeMake(width, lineHeight * 4.0), frame);
        
        if (iPhoneIdiom())
            frame.origin.y -= lineHeight * 3.0;
        
        UIView *background = [[UIImageView alloc] initWithFrame:frame];
        [self addSubview:background];
        self.background = background;
        [background release];
        self.background.backgroundColor = [UIColor whiteColor];
        self.background.layer.cornerRadius = 10.0;

        CGRect top = RectInsideAt(frame, RectTop, lineHeight);
        UILabel *text = [[UILabel alloc] initWithFrame:CenterInRect(CGSizeMake(frame.size.width, RealHeight(50.0)), top)];
        self.label = text;
        self.label.font = [UITheme defaultFontWithSize:fontSize];
        self.label.textColor = [UIColor blackColor];
        [self addSubview:text];
        [text release];

        CGRect fieldfr = ExpandRect(
                            RectInsideAt(frame, RectTop, lineHeight), -RealWidth(60.0), 0.0);
        fieldfr.origin.y += lineHeight;

        self.field_login = [[UITextField alloc] initWithFrame:fieldfr];
        self.field_login.font = [UITheme defaultFontWithSize:fontSize];
        self.field_login.backgroundColor = [UIColor clearColor];
        self.field_login.textColor  = [UIColor blackColor];
        self.field_login.placeholder = @"user@email.com";
        self.field_login.autocapitalizationType = UITextAutocapitalizationTypeNone;
        self.field_login.autocorrectionType = UITextAutocorrectionTypeNo;
        self.field_login.keyboardType = UIKeyboardTypeEmailAddress;
        [self addSubview:self.field_login];

        CGRect barfr = RectInsideAt(frame, RectTop, 1.0);
        barfr.origin.y += lineHeight * 2.0;
        
//        UILabel *bar = [[UILabel alloc] initWithFrame:barfr];
//        bar.backgroundColor = [UIColor grayColor];
//        [self addSubview:bar];
        
        barfr.origin.y += lineHeight;
        
        UILabel *bar = [[UILabel alloc] initWithFrame:barfr];
        bar.backgroundColor = [UIColor grayColor];
        [self addSubview:bar];
        
        fieldfr.origin.y += lineHeight;
        
        self.field_password = [[UITextField alloc] initWithFrame:fieldfr];
        self.field_password.font = [UITheme defaultFontWithSize:fontSize];
        self.field_password.backgroundColor = [UIColor clearColor];
        self.field_password.textColor  = [UIColor blackColor];
        self.field_password.placeholder = @"password";
        self.field_password.secureTextEntry = YES;
        self.field_password.autocapitalizationType = UITextAutocapitalizationTypeNone;
        self.field_password.autocorrectionType = UITextAutocorrectionTypeNo;
        self.field_password.enablesReturnKeyAutomatically = YES;
        [self addSubview:self.field_password];
        
        fieldfr.origin.y += lineHeight;
        fieldfr.size.width *= 0.5;
        
        self.button_login = [UIButton buttonWithType:UIButtonTypeCustom];
        self.button_login.frame = fieldfr;
        self.button_login.backgroundColor = [UIColor clearColor];
        self.button_login.titleLabel.font = [UITheme defaultFontWithSize:fontSize];
        [self.button_login setTitle:@"Login" forState:UIControlStateNormal];
        [self.button_login setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        [self.button_login setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
        [self addSubview:self.button_login];
        [self.button_login addTarget:self action:@selector(login) forControlEvents:UIControlEventTouchUpInside];
        
        fieldfr.origin.x += fieldfr.size.width;
        self.button_cancel = [UIButton buttonWithType:UIButtonTypeCustom];
        self.button_cancel.frame = fieldfr;
        self.button_cancel.backgroundColor = [UIColor clearColor];
        self.button_cancel.titleLabel.font = [UITheme defaultFontWithSize:fontSize];
        [self.button_cancel setTitle:@"Cancel" forState:UIControlStateNormal];
        [self.button_cancel setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        [self.button_cancel setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
        [self addSubview:self.button_cancel];
        [self.button_cancel addTarget:self action:@selector(cancel) forControlEvents:UIControlEventTouchUpInside];
        
        ListenForPropertyChange_Hidden(self, self, @selector(onHiddenChange));
    }
    return self;
}

- (BOOL)textFieldShouldEndEditing:(UITextField *)textField
{
    //    [super textFieldShouldEndEditing:textField];
    return YES;
}

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
    //    [self TPKeyboardAvoiding_scrollToActiveTextField];
    //  [super textFieldDidBeginEditing:textField];
}

-(BOOL) textFieldShouldReturn: (UITextField *) textField
{
    //    [super textFieldShouldReturn:textField];
    [textField resignFirstResponder];
    return YES;
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
    //    [super textFieldDidEndEditing:textField];
    [textField resignFirstResponder];
    NSLog(@"RESIGNING RESPONDER");
}

- (void)cancel
{
    _DismissKeyboardForView(self);
    [self hide];
}

- (void)setDefaultLogin:(NSString *)text
{
    self.field_login.text = text;
}

- (void)onHiddenChange
{
    if (!self.hidden)
        self.field_password.text = nil;
}

- (void)login
{
    LoginCredentials *creds = [[LoginCredentials alloc] init];
    creds.login = self.field_login.text;
    creds.password = self.field_password.text;
    _DismissKeyboardForView(self);
    [SimpleCall call:self.onLogin withObject:creds];
    [creds release];
    self.field_password.text = nil;
}

- (void)setText:(NSString *)text
{
    self.label.text = text;
    self.label.textColor = [UIColor blackColor];
    [UITheme alignCenter:self.label];
}

- (void)showText:(NSString *)text
{
    [self setText:text];
    UIView_SetHidden_Fade(self, NO, 0.4);
}

- (void)hide
{
    UIView_SetHidden_Fade(self, YES, 0.4);
}

@end
