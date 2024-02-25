//
//  UIUtil.h
//

#import <UIKit/UIKit.h>

#ifdef __cplusplus
extern "C" {
#endif

void    SetIsLandscape(BOOL value);         // defaults to NO
void    SetReferenceSize(CGSize size);      // defaults to iPad 768x1024
void    SetFontScale_iPhone(CGFloat value); // defaults to 1.5
void    SetFontScale_iPad(CGFloat value);   // defaults to 1.0

BOOL    iPadIdiom(void);
BOOL    iPhoneIdiom(void);
BOOL    IsLandscape(void);
    
CGSize  GetScreenSize(void);
CGSize  GetReferenceSize(void);
CGSize  GetOrientedReferenceSize(void);
CGSize  GetOrientedScreenSize(void);

CGFloat RealWidth(CGFloat value);
CGFloat RealHeight(CGFloat value);
CGFloat RealFontSize(CGFloat value);

CGSize  RealSize(CGSize value);
CGPoint RealPoint(CGPoint value);
CGRect  RealRect(CGRect value);

NSAttributedString *PlainString(NSString *text, UIColor *color);
NSAttributedString *UnderlinedString(NSString *text, UIColor *color);
    
void    UIView_SetHidden_Fade(UIView *v, BOOL hide, NSTimeInterval duration);
void    UIView_SetHiddenWithCompletion_Fade(UIView *v, BOOL hide, NSTimeInterval duration, void (^completion)(BOOL));
    
#ifdef __cplusplus
}
#endif
