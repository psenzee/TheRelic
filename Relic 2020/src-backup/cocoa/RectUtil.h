//
//  RectUtil.h
//  TonesPOC
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    CGFloat left;
    CGFloat right;
    CGFloat top;
    CGFloat bottom;
    
} Border;

typedef enum { RectTop, RectLeft, RectBottom, RectRight } RectEdge;
typedef enum { RectX, RectY, RectWidth, RectHeight }      RectValue;

BOOL    FloatsApproxEqual(CGFloat a, CGFloat b);
BOOL    AreRectsEqual(CGRect a, CGRect b);
    
CGSize  ScaleSize(CGSize size, CGFloat scale);
CGRect  ScaleRect(CGRect rect, CGFloat scale);
CGRect  ScaleRectXY(CGRect rect, CGFloat width, CGFloat height);
    
CGFloat SizeArea(CGSize sz);
CGSize  MinArea(CGSize a, CGSize b);
CGSize  MaxArea(CGSize a, CGSize b);
    
Border MakeBorder(CGFloat left, CGFloat right, CGFloat top, CGFloat bottom);
Border MakeEqualBorder(CGFloat value);

Border GrowBorder(Border a, Border b);
CGRect GrowRectWithBorderAndMultiplier(CGRect rect, Border border, CGFloat multiplier);
CGRect GrowRectByPixels(CGRect rect, CGFloat horizontal, CGFloat vertical);
CGRect GrowRectByRatio(CGRect rect, CGFloat horizontal, CGFloat vertical);
CGRect GrowRectWithBorder(CGRect rect, Border border);
CGRect ShrinkRectWithBorder(CGRect rect, Border border);
CGRect GrowRect(CGRect rect, CGFloat by);

   
CGRect RectWith(CGRect r, RectValue type, CGFloat value);
CGRect RectInsideAt(CGRect r, RectEdge edge, CGFloat size);
CGRect RectOutsideOf(CGRect r, RectEdge edge, CGFloat size);
CGRect RectHalf(CGRect r, RectEdge edge);
    
CGRect ZeroOrigin(CGRect r);
CGRect MakeSquareRect(CGRect r);
CGRect SquareAndCenterRect(CGRect r);
CGSize GetProportionalSizeByHeight(CGSize size, CGFloat height);
CGSize GetProportionalSizeByWidth(CGSize size, CGFloat width);
    
CGRect CenterInRect(CGSize s, CGRect within);
CGRect CenterRectInRect(CGRect r, CGRect within);
CGRect CenterRectInRectHorizontal(CGRect r, CGRect within);
CGRect CenterRectInRectVertical(CGRect r, CGRect within);
CGRect CenterRectInRectFromIn(CGRect r, CGRect within);
CGRect CenterRectInRectHorizontalFromIn(CGRect r, CGRect within);
CGRect CenterRectInRectVerticalFromIn(CGRect r, CGRect within);
CGRect ExpandRect(CGRect r, CGFloat byx, CGFloat byy);
    
CGRect MoveRect(CGRect r, CGFloat x, CGFloat y);
CGRect ExpandRectRatio(CGRect r, CGFloat byx, CGFloat byy);

CGSize GridBoxSize(CGSize size, CGFloat columns, CGFloat rows);
CGRect GridBox(CGRect r, CGFloat columns, CGFloat rows, CGFloat column, CGFloat row);
    
// deprecate?
    
CGRect RectWithWidth(CGRect rect, CGFloat width);
CGRect RectWithHeight(CGRect rect, CGFloat height);
CGRect RectWithX(CGRect rect, CGFloat x);
CGRect RectWithY(CGRect rect, CGFloat y);
    
CGRect RectLeftOf(CGRect rect, CGFloat width);
CGRect RectRightOf(CGRect rect, CGFloat width);
CGRect RectAbove(CGRect rect, CGFloat height);
CGRect RectBelow(CGRect rect, CGFloat height);
    
CGRect RectInsideLeft(CGRect rect, CGFloat width);
CGRect RectInsideRight(CGRect rect, CGFloat width);
CGRect RectInsideTop(CGRect rect, CGFloat height);
CGRect RectInsideBottom(CGRect rect, CGFloat height);
    
#ifdef __cplusplus
}
#endif
