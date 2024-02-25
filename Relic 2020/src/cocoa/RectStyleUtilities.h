//
//  RectStyleUtilities.h
//  Dial
//
//  Created by Paul Senzee on 4/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef struct ColorStyle_t
{
    float r, g, b, a;
    
} ColorStyle;

typedef struct RectStyle_t
{
    float  radius;
    float  stroke;
    float  r1, g1, b1, a1;
    float  r2, g2, b2, a2;
    float  rs, gs, bs, as;    
    
} RectStyle;

UIColor   *LightGrayColor(void);
UIColor   *DarkerGrayColor(void);

ColorStyle MakeColorStyle(float r, float g, float b, float a);
float      InterpolateFloat(float f1, float f2, float i);
ColorStyle InterpolateColorStyle(ColorStyle a, ColorStyle b, float i);
RectStyle  InterpolateRectStyle(RectStyle a, RectStyle b, float i);

RectStyle  RoundedRect_Invisible(void);
RectStyle  RoundedRect_BlackStrokeOnly(void);
RectStyle  RoundedRect_WhiteStrokeOnly(void);
RectStyle  RoundedRect_Black(void);
RectStyle  RoundedRect_White(void);
RectStyle  RoundedRect_GreenStyle(void);
RectStyle  RoundedRect_RedStyle(void);
RectStyle  RoundedRect_OrangeStyle(void);
RectStyle  RoundedRect_Gray1Style(void);
RectStyle  RoundedRect_Gray2Style(void);
RectStyle  RoundedRect_DarkGlassStyle(void);
RectStyle  RoundedRect_GrayToGray(float topGray, float bottomGray, float borderGray);

RectStyle  MakeRectStyle(float radius, float stroke,
                         float r1, float g1, float b1, float a1,
                         float r2, float g2, float b2, float a2,
                         float rs, float gs, float bs, float as);
RectStyle MakeRectStyleWithColorStyles(float radius, float stroke,
                                       ColorStyle c1, ColorStyle c2, ColorStyle cs);

UIImage   *GetRectImageWithStyle(CGSize size, RectStyle rs);

CGPathRef  CreateRoundedRectPath(CGRect RECT, CGFloat cornerRadius);
UIImage   *MakeRoundedRectImage(CGSize size);
UIImage   *MakeRoundedRectImageWithColor(CGSize size, float r, float g, float b, float a);
UIImage   *MakeRoundedRectStrokeImageWithColor(CGSize size, float stroke, float r, float g, float b, float a);
UIView    *AddAsSubviewAndRelease(UIView *parent, UIView *child);

UIImage   *MakeRoundedRectInsetImageWithColor(CGSize size, float r, float g, float b, float a);
UIButton  *MakeButton(NSString *imageName, id target, SEL sel, CGRect frame);
UIButton  *MakeButtonRespectingFrame(NSString *imageName, id target, SEL sel, CGRect frame);



UIImage   *MakeRoundedRectImageWithRadiusAndColor(CGSize size, float radius, float r, float g, float b, float a);
UIImage   *MakeRoundedRectStrokeImageWithRadiusAndGradient(CGSize size, float radius, float stroke, 
                                                           float r1, float g1, float b1, float a1,
                                                           float r2, float g2, float b2, float a2,
                                                           float rs, float gs, float bs, float as);
UIButton  *MakeButtonWithGradient(CGRect frame, float radius, float stroke, 
                                 float r1, float g1, float b1, float a1,
                                 float r2, float g2, float b2, float a2,
                                 float rs, float gs, float bs, float as);

UIImage   *MakeButtonImageWithGradient(CGRect frame, float radius, float stroke,
                                       float r1, float g1, float b1, float a1,
                                       float r2, float g2, float b2, float a2,
                                       float rs, float gs, float bs, float as);

UIButton  *MakeButtonWithGradientAndText(NSString *text,
                                        CGRect frame, float radius, float stroke, 
                                        float r1, float g1, float b1, float a1,
                                        float r2, float g2, float b2, float a2,
                                        float rs, float gs, float bs, float as);

UIButton *MakeButtonWithInterpolatedGradientAndText(NSString *text,
                                        CGRect frame, float radius, float stroke,
                                        float r1, float g1, float b1, float a1,
                                        float r2, float g2, float b2, float a2,
                                        float interpolant,
                                        float rs, float gs, float bs, float as);

UIButton *MakeButtonWithImage(CGRect frame, UIImage *image);
UIButton *MakeButtonWithImageAndText(NSString *text, CGRect frame, UIImage *image);

UIButton *MakeGreenButton(NSString *text, CGRect frame);
UIButton *MakeGreenButton2(NSString *text, CGRect frame);
UIButton *MakeRedButton(NSString *text, CGRect frame);
UIButton *MakeDarkGlassButton(NSString *text, CGRect frame);
UIButton *MakeDarkGlassButton2(NSString *text, CGRect frame);
UIImage  *MakeDarkGlassButtonImage(CGRect frame);
UIButton *MakeOrangeButton(NSString *text, CGRect frame);
UIButton *MakeGrayButton1(NSString *text, CGRect frame);
UIButton *MakeGrayButton2(NSString *text, CGRect frame);
UIButton *MakeGrayOverlayButton(NSString *imageName, id target, SEL sel, CGRect frame);
