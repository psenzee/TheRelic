//
//  ToastView.h
//

#import <UIKit/UIKit.h>

@interface ToastView : UIView

- (id)initWithFrame:(CGRect)frame;
- (id)initWithFrame:(CGRect)frame backgroundColor:(UIColor *)color;

- (void)showText:(NSString *)text;
- (void)showText:(NSString *)text duration:(NSTimeInterval)duration;
- (void)hide;

@end

