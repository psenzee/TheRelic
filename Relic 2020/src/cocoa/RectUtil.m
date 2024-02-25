//
//  RectUtil.m
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import "RectUtil.h"

BOOL FloatsApproxEqual(CGFloat a, CGFloat b)
{
    return fabs(a - b) < 0.00001;
}

CGSize ScaleSize(CGSize size, CGFloat scale)
{
    return CGSizeMake(size.width * scale, size.height * scale);
}

CGFloat SizeArea(CGSize sz)
{
    return sz.width * sz.height;
}

CGSize MinArea(CGSize a, CGSize b)
{
    return SizeArea(a) < SizeArea(b) ? a : b;
}

CGSize MaxArea(CGSize a, CGSize b)
{
    return SizeArea(a) > SizeArea(b) ? a : b;
}

Border MakeBorder(CGFloat left, CGFloat right, CGFloat top, CGFloat bottom)
{
    Border b;
    b.left = left; b.right = right;
    b.top = top; b.bottom = bottom;
    return b;
}

Border MakeEqualBorder(CGFloat value)
{
    return MakeBorder(value, value, value, value);
}

Border GrowBorder(Border a, Border b)
{
    return MakeBorder(a.left - b.left, a.right + b.right, a.top - b.top, a.bottom + b.bottom);
}

CGRect GrowRectWithBorderAndMultiplier(CGRect rect, Border border, CGFloat multiplier)
{
    rect.origin.x    -= border.left * multiplier;
    rect.origin.y    -= border.top  * multiplier;
    rect.size.width  += (border.left + border.right)  * multiplier;
    rect.size.height += (border.top  + border.bottom) * multiplier;
    return rect;
}

CGRect GrowRectWithBorder(CGRect rect, Border border)
{
    return GrowRectWithBorderAndMultiplier(rect, border, 1.0);
}

CGRect ScaleRect(CGRect rect, CGFloat scale)
{
    return ScaleRectXY(rect, scale, scale);
}

CGRect ScaleRectXY(CGRect rect, CGFloat width, CGFloat height)
{
    CGFloat w = rect.size.width  * width;
    CGFloat h = rect.size.height * height;
    return CGRectMake(rect.origin.x - (w - rect.size.width)  * 0.5,
                      rect.origin.y - (h - rect.size.height) * 0.5,
                      rect.size.width * width, rect.size.height * height);
}

CGRect GrowRect(CGRect rect, CGFloat by)
{
    return GrowRectWithBorder(rect, MakeEqualBorder(by));
}

CGRect ShrinkRectWithBorder(CGRect rect, Border border)
{
    return GrowRectWithBorderAndMultiplier(rect, border, -1.0);
}

CGRect GrowRectByPixels(CGRect rect, CGFloat horizontal, CGFloat vertical)
{
    return GrowRectWithBorder(rect,
               MakeBorder(horizontal * 0.5, horizontal * 0.5, vertical * 0.5, vertical * 0.5));
}

CGRect GrowRectByRatio(CGRect rect, CGFloat horizontal, CGFloat vertical)
{
    return GrowRectByPixels(rect, rect.size.width * horizontal, rect.size.height * vertical);
}

CGRect RectWithHeight(CGRect rect, CGFloat height)
{
    rect.size.height = height;
    return rect;
}

CGRect RectWithWidth(CGRect rect, CGFloat width)
{
    rect.size.width = width;
    return rect;
}

CGRect RectWithY(CGRect rect, CGFloat y)
{
    rect.origin.y = y;
    return rect;
}

CGRect RectWithX(CGRect rect, CGFloat x)
{
    rect.origin.x = x;
    return rect;
}

CGRect RectLeftOf(CGRect rect, CGFloat width)
{
    return RectWithWidth(
               RectWithX(rect, rect.origin.x - width),
                   width);
}

CGRect RectRightOf(CGRect rect, CGFloat width)
{
    return RectWithWidth(
               RectWithX(rect, rect.origin.x + rect.size.width),
                   width);
}

CGRect RectAbove(CGRect rect, CGFloat height)
{
    return RectWithHeight(
                RectWithY(rect, rect.origin.y - height),
                         height);
    
}

CGRect RectBelow(CGRect rect, CGFloat height)
{
    return RectWithHeight(
               RectWithY(rect, rect.origin.y + rect.size.height),
                         height);
}

CGRect RectInsideLeft(CGRect rect, CGFloat width)
{
    return RectWithWidth(rect, width);
}

CGRect RectInsideRight(CGRect rect, CGFloat width)
{
    return RectWithWidth(
               RectWithX(rect, rect.origin.x + rect.size.width - width),
                         width);
}

CGRect RectInsideTop(CGRect rect, CGFloat height)
{
    return RectWithHeight(rect, height);
    
}

CGRect RectInsideBottom(CGRect rect, CGFloat height)
{
    return RectWithHeight(
                RectWithY(rect, rect.origin.y + rect.size.height - height),
                          height);
}

CGRect RectWith(CGRect r, RectValue type, CGFloat value)
{
    switch (type)
    {
    case RectX:      r.origin.x    = value; break;
    case RectY:      r.origin.y    = value; break;
    case RectWidth:  r.size.width  = value; break;
    case RectHeight: r.size.height = value; break;
    default: break;
    }
    return r;
}

CGRect RectInsideAt(CGRect r, RectEdge edge, CGFloat size)
{
    switch (edge)
    {
    case RectTop:     return RectInsideTop(r, size);
    case RectLeft:    return RectInsideLeft(r, size);
    case RectBottom:  return RectInsideBottom(r, size);
    case RectRight:   return RectInsideRight(r, size);
    default: return r;
    }
}

CGRect RectOutsideOf(CGRect r, RectEdge edge, CGFloat size)
{
    switch (edge)
    {
    case RectTop:     return RectAbove(r, size);
    case RectLeft:    return RectLeftOf(r, size);
    case RectBottom:  return RectBelow(r, size);
    case RectRight:   return RectRightOf(r, size);
    default: return r;
    }
}

CGRect RectHalf(CGRect r, RectEdge edge)
{
    switch (edge)
    {
    case RectTop:     return RectWithHeight(r, r.size.height * 0.5);
    case RectLeft:    return RectWithWidth(r, r.size.width * 0.5);
    case RectBottom:  return RectWithY(RectWithHeight(r, r.size.height * 0.5), r.origin.y + r.size.height * 0.5);
    case RectRight:   return RectWithX(RectWithWidth(r, r.size.width * 0.5), r.origin.x + r.size.width * 0.5);
    default: return r;
    }
}
 
CGRect ZeroOrigin(CGRect r)
{
    r.origin.x = r.origin.y = 0.0;
    return r;
}

CGRect MakeSquareRect(CGRect r)
{
    r.size.width = r.size.height = MIN(r.size.width, r.size.height);
    return r;
}

CGRect SquareAndCenterRect(CGRect r)
{
    CGFloat min = r.size.width < r.size.height ? r.size.width : r.size.height;
    return CGRectMake(floor(r.origin.x + (r.size.width  - min) * 0.5),
                      floor(r.origin.y + (r.size.height - min) * 0.5), min, min);
}

CGSize GetProportionalSizeByHeight(CGSize size, CGFloat height)
{
    if (height == 0.0)
        return size;
    CGFloat ratio = height / size.height;
    return CGSizeMake(ratio * size.width, height);
}

CGSize GetProportionalSizeByWidth(CGSize size, CGFloat width)
{
    if (width == 0.0)
        return size;
    CGFloat ratio = width / size.width;
    return CGSizeMake(width, ratio * size.height);
}

CGRect CenterInRect(CGSize s, CGRect within)
{
    return CenterRectInRectFromIn(CGRectMake(0.0, 0.0, s.width, s.height), within);
}

CGRect CenterRectInRect(CGRect r, CGRect within)
{
    CGFloat dwidth  = within.size.width  - r.size.width;
    CGFloat dheight = within.size.height - r.size.height;
    r.origin.x += floor(dwidth  * 0.5);
    r.origin.y += floor(dheight * 0.5);
    return r;
}

CGRect CenterRectInRectHorizontal(CGRect r, CGRect within)
{
    CGFloat dwidth = within.size.width  - r.size.width;
    r.origin.x += floor(dwidth  * 0.5);
    return r;
}

CGRect CenterRectInRectVertical(CGRect r, CGRect within)
{
    CGFloat dheight = within.size.height - r.size.height;
    r.origin.y += floor(dheight * 0.5);
    return r;
}

CGRect CenterRectInRectFromIn(CGRect r, CGRect within)
{
    CGFloat dwidth  = within.size.width  - r.size.width;
    CGFloat dheight = within.size.height - r.size.height;
    r.origin.x += floor(dwidth  * 0.5 + within.origin.x);
    r.origin.y += floor(dheight * 0.5 + within.origin.y);
    return r;
}

CGRect CenterRectInRectHorizontalFromIn(CGRect r, CGRect within)
{
    CGFloat dwidth  = within.size.width  - r.size.width;
    r.origin.x += floor(dwidth  * 0.5 + within.origin.x);
    return r;
}

CGRect CenterRectInRectVerticalFromIn(CGRect r, CGRect within)
{
    CGFloat dheight = within.size.height - r.size.height;
    r.origin.y += floor(dheight * 0.5 + within.origin.y);
    return r;
}

CGRect ExpandRect(CGRect r, CGFloat byx, CGFloat byy)
{
    r.origin.x    -= floor(byx * 0.5);
    r.origin.y    -= floor(byy * 0.5);
    r.size.width  += byx;
    r.size.height += byy;
    return r;
}

CGRect MoveRect(CGRect r, CGFloat x, CGFloat y)
{
    return CGRectMake(r.origin.x + x, r.origin.y + y, r.size.width, r.size.height);
}

CGRect ExpandRectRatio(CGRect r, CGFloat byx, CGFloat byy)
{
    byx *= r.size.width;
    byy *= r.size.height;
    
    r.origin.x    -= byx * 0.5;
    r.origin.y    -= byy * 0.5;
    r.size.width  += byx;
    r.size.height += byy;
    return r;
}

CGSize GridBoxSize(CGSize size, CGFloat columns, CGFloat rows)
{
    return CGSizeMake(size.width / columns, size.height / rows);
}

CGRect GridBox(CGRect r, CGFloat columns, CGFloat rows, CGFloat column, CGFloat row)
{
    CGSize size = GridBoxSize(r.size, columns, rows);
    return CGRectMake(r.origin.x + size.width * column, r.origin.y + size.height * row, size.width, size.height);
}

BOOL AreRectsEqual(CGRect a, CGRect b)
{
	return FloatsApproxEqual(a.origin.x, b.origin.x) &&
    FloatsApproxEqual(a.origin.y, b.origin.y) &&
    FloatsApproxEqual(a.size.width, b.size.width) &&
    FloatsApproxEqual(a.size.height, b.size.height);
}
