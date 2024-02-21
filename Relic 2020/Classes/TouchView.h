//
//  TouchView.h
//
//  Created by Paul Senzee on 8/30/14.
//

#import <UIKit/UIKit.h>

@class EAGLView;

@interface TouchView : UIView
{
}

@property(nonatomic, getter=isExclusiveTouch) BOOL exclusiveTouch;
@property(nonatomic, getter=isMultipleTouchEnabled) BOOL multipleTouchEnabled;

- (id)initWithCoder:(NSCoder*)coder;
- (id)initWithFrame:(CGRect)frame;

@end
