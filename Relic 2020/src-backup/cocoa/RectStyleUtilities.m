//
//  RectStyleUtilities.m
//  Dial
//
//  Created by Paul Senzee on 4/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "RectStyleUtilities.h"

float DEFAULT_RADIUS = 5.f;

UIColor *LightGrayColor(void)
{
    return [UIColor colorWithRed:0.8 green:0.8 blue:0.8 alpha:1.0];
}

UIColor *DarkerGrayColor(void)
{
    return [UIColor colorWithRed:0.4 green:0.4 blue:0.4 alpha:1.0];
}

NSString *GetRectStyleKey(CGSize size, RectStyle style);
UIImage  *GetCachedRectImage(CGSize size, RectStyle style);
void      SetCachedRectImage(CGSize size, RectStyle style, UIImage *image);

ColorStyle MakeColorStyle(float r, float g, float b, float a)
{
    ColorStyle cs;
    cs.r = r;
    cs.g = g;
    cs.b = b;
    cs.a = a;
    return cs;
}

float InterpolateFloat(float f1, float f2, float i)
{
    return (f2 - f1) * i + f1;
}

ColorStyle InterpolateColorStyle(ColorStyle c1, ColorStyle c2, float i)
{
    return MakeColorStyle(InterpolateFloat(c1.r, c2.r, i),
                          InterpolateFloat(c1.g, c2.g, i),
                          InterpolateFloat(c1.b, c2.b, i),
                          InterpolateFloat(c1.a, c2.a, i));
}

RectStyle InterpolateRectStyle(RectStyle r1, RectStyle r2, float i)
{
    RectStyle rs;
    
    rs.r1 = InterpolateFloat(r1.r1, r2.r1, i);
    rs.g1 = InterpolateFloat(r1.g1, r2.g1, i);
    rs.b1 = InterpolateFloat(r1.b1, r2.b1, i);
    rs.a1 = InterpolateFloat(r1.a1, r2.a1, i);
    
    rs.r2 = InterpolateFloat(r1.r2, r2.r2, i);
    rs.g2 = InterpolateFloat(r1.g2, r2.g2, i);
    rs.b2 = InterpolateFloat(r1.b2, r2.b2, i);
    rs.a2 = InterpolateFloat(r1.a2, r2.a2, i);
    
    rs.rs = InterpolateFloat(r1.rs, r2.rs, i);
    rs.gs = InterpolateFloat(r1.gs, r2.gs, i);
    rs.bs = InterpolateFloat(r1.bs, r2.bs, i);
    rs.as = InterpolateFloat(r1.as, r2.as, i);
    
    rs.radius = InterpolateFloat(r1.radius, r2.radius, i);
    rs.stroke = InterpolateFloat(r1.stroke, r2.stroke, i);
    
    return rs;
}

static NSMutableDictionary *CACHED_IMAGES = nil;

NSString *GetRectStyleKey(CGSize size, RectStyle style)
{
    return [NSString stringWithFormat:@"%d#%d-"
                                       "%.2f#%.2f-"
                                       "%.2f#%.2f#%.2f#%.2f-"
                                       "%.2f#%.2f#%.2f#%.2f-"
                                       "%.2f#%.2f#%.2f#%.2f-",
                               (int)size.width, (int)size.height,
                                style.radius, style.stroke,
                                style.rs, style.gs, style.bs, style.as,
                                style.r1, style.g1, style.b1, style.a1,
                                style.r2, style.g2, style.b2, style.a2];
}

UIImage *GetCachedRectImage(CGSize size, RectStyle style)
{
    if (!CACHED_IMAGES)
    {
        CACHED_IMAGES = [[NSMutableDictionary alloc] init];
        return nil;
    }
    return (UIImage *)[CACHED_IMAGES objectForKey:GetRectStyleKey(size, style)];
}

void SetCachedRectImage(CGSize size, RectStyle style, UIImage *image)
{
    if (!CACHED_IMAGES)
        CACHED_IMAGES = [[NSMutableDictionary alloc] init];
    return [CACHED_IMAGES setObject:image forKey:GetRectStyleKey(size, style)];
}

RectStyle MakeRectStyle(float radius, float stroke,
                        float r1, float g1, float b1, float a1,
                        float r2, float g2, float b2, float a2,
                        float rs, float gs, float bs, float as)
{
    RectStyle style;
    style.radius = radius;
    style.stroke = stroke;
    style.r1 = r1; style.g1 = g1; style.b1 = b1; style.a1 = a1;
    style.r2 = r2; style.g2 = g2; style.b2 = b2; style.a2 = a2;
    style.rs = rs; style.gs = gs; style.bs = bs; style.as = as;
    return style;
}

RectStyle MakeRectStyleWithColorStyles(float radius, float stroke,
                        ColorStyle c1, ColorStyle c2, ColorStyle cs)
{
    RectStyle style;
    style.radius = radius;
    style.stroke = stroke;
    style.r1 = c1.r; style.g1 = c1.g; style.b1 = c1.b; style.a1 = c1.a;
    style.r2 = c2.r; style.g2 = c2.g; style.b2 = c2.b; style.a2 = c2.a;
    style.rs = cs.r; style.gs = cs.g; style.bs = cs.b; style.as = cs.a;
    return style;
}

UIImage *GetRectImageWithStyle(CGSize size, RectStyle rs)
{
    UIImage *image = GetCachedRectImage(size, rs);
    if (!image)
    {
        image = MakeRoundedRectStrokeImageWithRadiusAndGradient(size,
                                                                rs.radius, rs.stroke,
                                                                rs.r1, rs.g1, rs.b1, rs.a1,
                                                                rs.r2, rs.g2, rs.b2, rs.a2,
                                                                rs.rs, rs.gs, rs.bs, rs.as);
        SetCachedRectImage(size, rs, image);
    }
    return image;
}

RectStyle  RoundedRect_Invisible()
{
    return MakeRectStyle(DEFAULT_RADIUS, 0.f,
                         0.f, 0.f, 0.f, 0.0f,
                         0.f, 0.f, 0.f, 0.0f,
                         0.f, 0.f, 0.f, 0.0f);
}

RectStyle  RoundedRect_BlackStrokeOnly()
{
    return MakeRectStyle(DEFAULT_RADIUS, 0.5,
                         0.f, 0.f, 0.f, 0.0f,
                         0.f, 0.f, 0.f, 0.0f,
                         0.f, 0.f, 0.f, 1.0f);
}

RectStyle  RoundedRect_WhiteStrokeOnly()
{
    return MakeRectStyle(DEFAULT_RADIUS, 0.5,
                         0.f, 0.f, 0.f, 0.0f,
                         0.f, 0.f, 0.f, 0.0f,
                         1.f, 1.f, 1.f, 1.0f);
}

RectStyle  RoundedRect_Black()
{
    return MakeRectStyle(DEFAULT_RADIUS, 0.f,
                         0.f, 0.f, 0.f, 1.0f,
                         0.f, 0.f, 0.f, 1.0f,
                         0.f, 0.f, 0.f, 0.0f);
}

RectStyle  RoundedRect_White()
{
    return MakeRectStyle(DEFAULT_RADIUS, 0.f,
                         1.f, 1.f, 1.f, 1.0f,
                         1.f, 1.f, 1.f, 1.0f,
                         1.f, 1.f, 1.f, 0.0f);
}

RectStyle  RoundedRect_GreenStyle()
{
    return MakeRectStyle(DEFAULT_RADIUS, 1.0,
                         0.2f, 1.0f, 0.2f, 1.f,
                         0.0f, 0.5f, 0.0f, 1.f,
                         0.0f, 0.0f, 0.0f, 1.f);
}

RectStyle  RoundedRect_RedStyle()
{
    return MakeRectStyle(DEFAULT_RADIUS, 1.0,
                         1.0f, 0.2f, 0.2f, 1.f,
                         0.5f, 0.0f, 0.0f, 1.f,
                         0.0f, 0.0f, 0.0f, 1.f);
}


RectStyle  RoundedRect_OrangeStyle()
{
    return MakeRectStyle(DEFAULT_RADIUS, 0.5,
                         1.0f, 0.50f, 0.2f, 1.f,
                         0.5f, 0.25f, 0.0f, 1.f,
                         0.0f, 0.00f, 0.0f, 1.f);
}

RectStyle  RoundedRect_Gray1Style()
{
    return MakeRectStyle(DEFAULT_RADIUS, 1.0,
                         0.2f, 0.2f, 0.2f, 1.f,
                         0.5f, 0.5f, 0.5f, 1.f,
                         0.0f, 0.0f, 0.0f, 1.f);
}

RectStyle  RoundedRect_Gray2Style()
{
    return MakeRectStyle(DEFAULT_RADIUS, 1.0,
                         0.8f, 0.8f, 0.8f, 1.f,
                         0.4f, 0.4f, 0.4f, 1.f,
                         0.0f, 0.0f, 0.0f, 1.f);
}

RectStyle  RoundedRect_DarkGlassStyle()
{
    return MakeRectStyle(DEFAULT_RADIUS, 0.f,
                         0.f, 0.f, 0.f, 0.2f,
                         0.f, 0.f, 0.f, 0.2f,
                         0.f, 0.f, 0.f, 0.0f);
    
}

RectStyle  RoundedRect_GrayToGray(float topGray, float bottomGray, float borderGray)
{
    return MakeRectStyle(DEFAULT_RADIUS, 1.0,
                         topGray, topGray, topGray, 1.f,
                         bottomGray, bottomGray, bottomGray, 1.f,
                         borderGray, borderGray, borderGray, 1.0f);
    
}

////////

CGPathRef CreateRoundedRectPath(CGRect RECT, CGFloat cornerRadius)
{
	CGMutablePathRef path;
	path = CGPathCreateMutable();
	
	double maxRad = MAX(CGRectGetHeight(RECT) / 2., CGRectGetWidth(RECT) / 2.);
	
	if (cornerRadius > maxRad) cornerRadius = maxRad;
	
	CGPoint	bl, tl, tr, br;
	
	bl = tl = tr = br = RECT.origin;
	tl.y += RECT.size.height;
	tr.y += RECT.size.height;
	tr.x += RECT.size.width;
	br.x += RECT.size.width;
	
	CGPathMoveToPoint(path, NULL, bl.x + cornerRadius, bl.y);
	CGPathAddArcToPoint(path, NULL, bl.x, bl.y, bl.x, bl.y + cornerRadius, cornerRadius);
	CGPathAddLineToPoint(path, NULL, tl.x, tl.y - cornerRadius);
	CGPathAddArcToPoint(path, NULL, tl.x, tl.y, tl.x + cornerRadius, tl.y, cornerRadius);
	CGPathAddLineToPoint(path, NULL, tr.x - cornerRadius, tr.y);
	CGPathAddArcToPoint(path, NULL, tr.x, tr.y, tr.x, tr.y - cornerRadius, cornerRadius);
	CGPathAddLineToPoint(path, NULL, br.x, br.y + cornerRadius);
	CGPathAddArcToPoint(path, NULL, br.x, br.y, br.x - cornerRadius, br.y, cornerRadius);
	
	CGPathCloseSubpath(path);
	
	CGPathRef ret;
	ret = CGPathCreateCopy(path);
	CGPathRelease(path);
	return ret;
}

UIImage *MakeRoundedRectStrokeImageWithColor(CGSize size, float stroke, float r, float g, float b, float a)
{/*
 return MakeRoundedRectStrokeImageWithRadiusAndGradient(size, DEFAULT_RADIUS, stroke,
                                                             0.f, 0.f, 0.f, 0.f, 
                                                             0.f, 0.f, 0.f, 0.f, 
                                                             r, g, b, a);*/
    
	// Set up our overlay view that pops up when we are pinching/zooming the oscilloscope
	UIImage *img_ui = nil;
	{
        
        float halfWidth = stroke * 0.5f;
        
		// Draw the rounded rect for the bg path using this convenience function
		CGPathRef bgPath = CreateRoundedRectPath(CGRectMake(halfWidth, halfWidth, size.width - stroke, size.height - stroke), DEFAULT_RADIUS);
		
		CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
		// Create the bitmap context into which we will draw
		CGContextRef cxt = CGBitmapContextCreate(NULL, size.width, size.height, 8, 4*size.width, cs, (CGBitmapInfo)kCGImageAlphaPremultipliedFirst);
        if (!cxt)
            NSLog(@"NULL CONTEXT RETURNED");
		CGContextSetFillColorSpace(cxt, cs);
		CGContextSetStrokeColorSpace(cxt, cs);
		CGFloat fillClr[] = {r, g, b, a};
		CGContextSetFillColor(cxt, fillClr);
		CGContextSetStrokeColor(cxt, fillClr);
        
        CGContextSetLineWidth(cxt, stroke);
		// Add the rounded rect to the context...
		CGContextAddPath(cxt, bgPath);
		// ... and fill it.
        CGContextStrokePath(cxt);
		
		// Make a CGImage out of the context
		CGImageRef img_cg = CGBitmapContextCreateImage(cxt);
		// Make a UIImage out of the CGImage
		img_ui = [[UIImage alloc] initWithCGImage:img_cg];
		
		// Clean up
		CGImageRelease(img_cg);
		CGColorSpaceRelease(cs);
		CGContextRelease(cxt);
		CGPathRelease(bgPath);
	}	
	return img_ui;
}

UIImage *MakeRoundedRectInsetImageWithColor(CGSize size, float r, float g, float b, float a)
{
	UIImage *img_ui = nil;
	{
		// Draw the rounded rect for the bg path using this convenience function
		CGPathRef bgPath = CreateRoundedRectPath(CGRectMake(0, 0, size.width, size.height), DEFAULT_RADIUS);
		
		CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
		// Create the bitmap context into which we will draw
		CGContextRef cxt = CGBitmapContextCreate(NULL, size.width, size.height, 8, 4*size.width, cs, (CGBitmapInfo)kCGImageAlphaPremultipliedFirst);
        if (!cxt)
            NSLog(@"NULL CONTEXT RETURNED");
        CGContextClipToRect(cxt, CGRectMake(0, 0, size.width, size.height));

		CGContextSetFillColorSpace(cxt, cs);
		CGContextSetStrokeColorSpace(cxt, cs);
        
		CGFloat fillClr[] = {r, g, b, a};
		CGFloat strokeClr[] = {1.f, 1.f, 1.f, 1.f};
		CGContextSetFillColor(cxt, fillClr);
		CGContextSetStrokeColor(cxt, strokeClr);
        
        CGContextSetLineWidth(cxt, 5.f);
		// Add the rounded rect to the context...
		CGContextAddPath(cxt, bgPath);
		// ... and fill it.
        
        CGContextStrokePath(cxt);
        CGContextFillPath(cxt);        
		
		// Make a CGImage out of the context
		CGImageRef img_cg = CGBitmapContextCreateImage(cxt);
		// Make a UIImage out of the CGImage
		img_ui = [[UIImage alloc] initWithCGImage:img_cg];
		
		// Clean up
		CGImageRelease(img_cg);
        CGColorSpaceRelease(cs);        
		CGContextRelease(cxt);
		CGPathRelease(bgPath);
	}	
	return img_ui;
} 

UIImage *MakeRoundedRectImageWithColor(CGSize size, float r, float g, float b, float a)
{
    return MakeRoundedRectImageWithRadiusAndColor(size, DEFAULT_RADIUS, r, g, b, a);
}

UIImage *MakeRoundedRectStrokeImageWithRadiusAndGradient(CGSize size, float radius, float stroke,
                                                   float r1, float g1, float b1, float a1,
                                                   float r2, float g2, float b2, float a2,
                                                   float rs, float gs, float bs, float as)
{   
	UIImage *img_ui = nil;

    // Create a gradient from white to red
    CGFloat colors [] =
    { 
        r2, g2, b2, a2, 
        r1, g1, b1, a1
    };
        
    CGRect frame = CGRectMake(stroke, stroke, size.width - stroke * 2.f, size.height - stroke * 2.f);

    // Draw the rounded rect for the bg path using this convenience function
    CGPathRef bgPath = CreateRoundedRectPath(frame, radius);
		
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    // Create the bitmap context into which we will draw
    CGContextRef context = CGBitmapContextCreate(NULL, size.width, size.height, 8, 4*size.width, cs, (CGBitmapInfo)kCGImageAlphaPremultipliedFirst);
    if (!context)
        NSLog(@"NULL CONTEXT RETURNED");
    CGContextClipToRect(context, CGRectMake(0, 0, size.width, size.height));
        
    CGColorSpaceRef baseSpace = CGColorSpaceCreateDeviceRGB();
    CGGradientRef gradient = CGGradientCreateWithColorComponents(baseSpace, colors, NULL, 2);
    CGColorSpaceRelease(baseSpace), baseSpace = NULL;
    
    CGContextSaveGState(context);
    
    CGContextAddPath(context, bgPath);
    CGContextClip(context);
        
    CGRect rect = CGRectMake(0.f, 0.f, size.width, size.height);
    
    CGPoint startPoint = CGPointMake(CGRectGetMidX(rect), CGRectGetMinY(rect));
    CGPoint endPoint = CGPointMake(CGRectGetMidX(rect), CGRectGetMaxY(rect));
    
    CGContextDrawLinearGradient(context, gradient, startPoint, endPoint, 0);
    CGGradientRelease(gradient), gradient = NULL;
    
    CGContextRestoreGState(context);
        
	CGContextSetFillColorSpace(context, cs);
        
    if (stroke > 0.f)
    {
        CGContextSetStrokeColorSpace(context, cs);
        CGContextSetLineWidth(context, stroke);
		CGFloat fillClr[] = {rs, gs, bs, as};
        //	CGContextSetFillColor(context, fillClr);
		CGContextSetStrokeColor(context, fillClr);
        
        CGContextAddPath(context, bgPath);
        CGContextDrawPath(context, kCGPathStroke);
    }

        
    // Make a CGImage out of the context
	CGImageRef img_cg = CGBitmapContextCreateImage(context);
	// Make a UIImage out of the CGImage
	img_ui = [[UIImage alloc] initWithCGImage:img_cg];
		
	// Clean up
	CGImageRelease(img_cg);
	CGColorSpaceRelease(cs);
	CGContextRelease(context);
	CGPathRelease(bgPath);
        
	return img_ui;    
}

UIImage *MakeRoundedRectImageWithRadiusAndColor(CGSize size, float radius, float r, float g, float b, float a)
{
	UIImage *img_ui = nil;
	{
		// Draw the rounded rect for the bg path using this convenience function
		CGPathRef bgPath = CreateRoundedRectPath(CGRectMake(0, 0, size.width, size.height), radius);
		
		CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
		// Create the bitmap context into which we will draw
		CGContextRef cxt = CGBitmapContextCreate(NULL, size.width, size.height, 8, 4*size.width, cs, (CGBitmapInfo)kCGImageAlphaPremultipliedFirst);
        if (!cxt)
            NSLog(@"NULL CONTEXT RETURNED");
		CGContextSetFillColorSpace(cxt, cs);
        CGContextClipToRect(cxt, CGRectMake(0, 0, size.width, size.height));        
        
		CGFloat fillClr[] = {r, g, b, a};
		CGContextSetFillColor(cxt, fillClr);
        
		// Add the rounded rect to the context...
		CGContextAddPath(cxt, bgPath);
		// ... and fill it.
        CGContextFillPath(cxt);
		
		// Make a CGImage out of the context
		CGImageRef img_cg = CGBitmapContextCreateImage(cxt);
		// Make a UIImage out of the CGImage
		img_ui = [[UIImage alloc] initWithCGImage:img_cg];
		
		// Clean up
		CGImageRelease(img_cg);
		CGColorSpaceRelease(cs);
		CGContextRelease(cxt);
		CGPathRelease(bgPath);
	}	
	return img_ui;
}
