//
//  SpinnerView.h
//

#import <UIKit/UIKit.h>

typedef void (^CancelBlock)(void);

@interface SpinnerView : UIView

- (id)initWithFrame:(CGRect)frame;

- (void)show;
- (void)hide;

- (void)setText:(NSString *)text;
- (void)showText:(NSString *)text;
- (void)setOnCancel:(CancelBlock)cancel;

+ (void)showSpinner;
+ (void)hideSpinner;

+ (void)setRootView:(UIView *)view;

@end

