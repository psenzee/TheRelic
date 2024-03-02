//
//  UITheme.h
//  Fascination
//

#import <UIKit/UIKit.h>

@class SimpleCall;

BOOL iPadIdiom(void);
BOOL iPhoneIdiom(void);

typedef struct
{
    UIColor *textColor;
    UIColor *outlineColor;
    
} OutlinedTextAppearance;

typedef enum
{
    OutlinedText_Normal = 0,
    OutlinedText_Hilite,
    
    OutlinedText_MAX
    
} OutlinedTextType;

@interface UITheme : NSObject
{
    OutlinedTextAppearance  textAppearance[OutlinedText_MAX];
    NSMutableDictionary    *imagePaths;
    NSMutableDictionary    *images;
}

- (id)init;

- (UIImage  *)imageForKey:(NSString *)key;
- (void)      setImage:(UIImage *)image forKey:(NSString *)key;

- (UIImage  *)outlineTextImage:(NSString *)text forType:(OutlinedTextType)type;
- (UIImage  *)outlineTextImage:(NSString *)text forType:(OutlinedTextType)type expand:(CGSize)expand;
- (UIButton *)defaultOutlineButtonForText:(NSString *)text intoRect:(CGRect)frame imageOffset:(CGPoint)offset imageScale:(CGSize)scale;

+ (void      )alignCenter:(UILabel *)label;
+ (CGFloat   )defaultLabelFontSize;
+ (UIFont   *)defaultFontWithSize:(CGFloat)size;
+ (UIColor  *)defaultTextColor;
+ (UIColor  *)defaultDarkBackgroundColor;
+ (UIButton *)defaultButton:(NSString *)name withFrame:(CGRect)frame;
+ (UILabel  *)defaultLabel:(NSString *)text withFrame:(CGRect)frame;
+ (UILabel  *)defaultLabelWithFrame:(CGRect)frame;
+ (UILabel  *)defaultTitleLabel:(NSString *)text withFrame:(CGRect)frame;

+ (CGFloat   )defaultToastFontSize;

+ (void      )registerUIUpdate:(SimpleCall *)call;
+ (void      )registerIntermittentUpdate:(SimpleCall *)call withInterval:(NSTimeInterval)interval;

+ (UITheme  *)shared;

@end
