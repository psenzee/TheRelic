//
//  DrawUtilities.h
//

#import <UIKit/UIKit.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    Rounded_None = 0,
    
    Rounded_TopLeft  = 1,
    Rounded_TopRight = 2,
    
    Rounded_BottomLeft  = 4,
    Rounded_BottomRight = 8,
    
    Rounded_Top    = Rounded_TopLeft    | Rounded_TopRight,
    Rounded_Bottom = Rounded_BottomLeft | Rounded_BottomRight,
    
    Rounded_Left   = Rounded_TopLeft    | Rounded_BottomLeft,
    Rounded_Right  = Rounded_TopRight   | Rounded_BottomRight,
    
    Rounded_All    = Rounded_Top        | Rounded_Bottom
    
} Rounded_Flags;

typedef struct ColorStyle_t
{
    CGFloat r, g, b, a;
    
} ColorStyle;

typedef struct RectStyle_t
{
    unsigned   flags;
    
    CGFloat    radius;
    CGFloat    stroke;
    
    ColorStyle begin;
    ColorStyle end;
    
    ColorStyle stroke_color;
    
} RectStyle;


ColorStyle MakeColorStyle(CGFloat r, CGFloat g, CGFloat b, CGFloat a);

ColorStyle ColorStyleFromUIColor(UIColor *color);

ColorStyle ColorStyle_Gray(CGFloat value, CGFloat alpha);

RectStyle  RoundedRect_GrayStrokeOnly(CGFloat radius, CGFloat stroke, CGFloat grayscale);
RectStyle  RoundedRect_SolidGray(CGFloat radius, CGFloat grayscale);
RectStyle  RoundedRect_GrayToGray(CGFloat radius, CGFloat topgray, CGFloat bottomgray, CGFloat bordergray);

ColorStyle BlendColorStyle(ColorStyle a, ColorStyle b, CGFloat t);
RectStyle  BlendRectStyle(RectStyle a, RectStyle b, CGFloat t);

RectStyle  MakeRectStyle(CGFloat radius, CGFloat stroke,
                         ColorStyle c1, ColorStyle c2, ColorStyle cs);

RectStyle  MakeRectStyleWithFlags(CGFloat radius, CGFloat stroke,
                                  ColorStyle c1, ColorStyle c2, ColorStyle cs,
                                  unsigned flags);

UIImage   *GetRectImageWithStyle(CGSize size, RectStyle rs, BOOL cache);
    
#ifdef __cplusplus
}
#endif


UIImage   *Colorize(UIImage *image, UIColor *color);