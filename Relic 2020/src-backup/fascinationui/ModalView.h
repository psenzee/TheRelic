//
//  ModalView.h
//

#import <UIKit/UIKit.h>

@class SimpleCall;

@interface ModalView : UIView

- (id)initWithFrame:(CGRect)frame;

- (void)showWithContentView:(UIView *)contentView;
- (void)dismiss;

@end

