//
//  DrawUtil.m
//

#import "DrawUtil.h"

UIImage *MakeRoundedRectStrokeImageWithRadiusAndGradient(CGSize size, CGFloat radius, CGFloat stroke,
                                                         ColorStyle begin, ColorStyle end, ColorStyle scolor,
                                                         unsigned flags);

static const char *HexColorKey(ColorStyle cs, char *buffer)
{
    sprintf(buffer, "%02x%02x%02x%02x",
            (unsigned)(cs.r * 255.0), (unsigned)(cs.g * 255.0),
            (unsigned)(cs.b * 255.0), (unsigned)(cs.a * 255.0));
    return buffer;
}

static NSMutableDictionary *CACHED_IMAGES = nil;

static NSString *GetRectStyleKey(CGSize size, RectStyle s)
{
    char key1[128], key2[128], key3[128];
    return [NSString stringWithFormat:@"[%s%s%s %d,%d,%0.2f,%0.2f %u]",
               HexColorKey(s.begin,        key1),
               HexColorKey(s.end,          key2),
               HexColorKey(s.stroke_color, key3),
               (int)size.width, (int)size.height,
               s.radius, s.stroke, s.flags];
}

static UIImage *GetCachedRectImage(CGSize size, RectStyle style)
{
    if (CACHED_IMAGES)
        return (UIImage *)[CACHED_IMAGES objectForKey:GetRectStyleKey(size, style)];
    CACHED_IMAGES = [[NSMutableDictionary alloc] init];
    return nil;
}

static void SetCachedRectImage(CGSize size, RectStyle style, UIImage *image)
{
    if (!CACHED_IMAGES)
        CACHED_IMAGES = [[NSMutableDictionary alloc] init];
    return [CACHED_IMAGES setObject:image forKey:GetRectStyleKey(size, style)];
}

///

ColorStyle MakeColorStyle(CGFloat r, CGFloat g, CGFloat b, CGFloat a)
{
    ColorStyle cs; cs.r = r; cs.g = g; cs.b = b; cs.a = a;
    return cs;
}

ColorStyle ColorStyleFromUIColor(UIColor *color)
{
    ColorStyle cs;
    [color getRed:&cs.r green:&cs.g blue:&cs.b alpha:&cs.a];
    return cs;
}

static CGFloat Blend(CGFloat f1, CGFloat f2, CGFloat t)
{
    return (f2 - f1) * t + f1;
}

ColorStyle BlendColorStyle(ColorStyle c1, ColorStyle c2, CGFloat t)
{
    return MakeColorStyle(Blend(c1.r, c2.r, t), Blend(c1.g, c2.g, t),
                          Blend(c1.b, c2.b, t), Blend(c1.a, c2.a, t));
}

RectStyle BlendRectStyle(RectStyle r1, RectStyle r2, CGFloat t)
{
    RectStyle rs;
    
    rs.begin        = BlendColorStyle(r1.begin,        r2.begin,        t);
    rs.end          = BlendColorStyle(r1.end,          r2.end,          t);
    rs.stroke_color = BlendColorStyle(r1.stroke_color, r2.stroke_color, t);
    
    rs.radius       = Blend(r1.radius, r2.radius, t);
    rs.stroke       = Blend(r1.stroke, r2.stroke, t);
    
    return rs;
}

RectStyle MakeRectStyleWithFlags(CGFloat radius, CGFloat stroke,
                                 ColorStyle begin, ColorStyle end,
                                 ColorStyle stroke_color, unsigned flags)
{
    RectStyle style;
    style.radius       = radius;
    style.stroke       = stroke;
    style.begin        = begin;
    style.end          = end;
    style.stroke_color = stroke_color;
    style.flags        = flags;
    return style;
}

RectStyle MakeRectStyle(CGFloat radius, CGFloat stroke,
                        ColorStyle begin, ColorStyle end,
                        ColorStyle stroke_color)
{
    return MakeRectStyleWithFlags(radius, stroke,
                                  begin, end, stroke_color, Rounded_All);
}

UIImage *GetRectImageWithStyle(CGSize size, RectStyle rs, BOOL cache)
{
    UIImage *image = GetCachedRectImage(size, rs);
    if (!image)
    {
        image = MakeRoundedRectStrokeImageWithRadiusAndGradient(size,
                                                                rs.radius, rs.stroke,
                                                                rs.begin, rs.end, rs.stroke_color, rs.flags);
        if (cache)
        {
            if (!image)
                NSLog(@"FAILED TO CREATE IMAGE FOR KEY %@", GetRectStyleKey(size, rs));
            else
                SetCachedRectImage(size, rs, image);
        }
    }
    return image;
}

ColorStyle ColorStyle_Gray(CGFloat grayscale, CGFloat alpha)
{
    return MakeColorStyle(grayscale, grayscale, grayscale, alpha);
}

RectStyle  RoundedRect_GrayStrokeOnly(CGFloat radius, CGFloat stroke, CGFloat grayscale)
{
    return MakeRectStyle(radius, stroke,
                         ColorStyle_Gray(grayscale, 0.0),
                         ColorStyle_Gray(grayscale, 0.0),
                         ColorStyle_Gray(grayscale, 1.0));
}

RectStyle  RoundedRect_SolidGray(CGFloat radius, CGFloat grayscale)
{
    return MakeRectStyle(radius, 0.0,
                         ColorStyle_Gray(grayscale, 1.0),
                         ColorStyle_Gray(grayscale, 1.0),
                         ColorStyle_Gray(grayscale, 0.0));
}

RectStyle  RoundedRect_GrayToGray(CGFloat radius, CGFloat topgray, CGFloat bottomgray, CGFloat bordergray)
{
    return MakeRectStyle(radius, 1.0,
                         ColorStyle_Gray(topgray,    1.0),
                         ColorStyle_Gray(bottomgray, 1.0),
                         ColorStyle_Gray(bordergray, 1.0));
}

////////

CGPathRef CreateRoundedRectPath(CGRect RECT, CGFloat cornerRadius, unsigned flags)
{
	CGMutablePathRef path;
	path = CGPathCreateMutable();
	
	double maxRad = MIN(CGRectGetHeight(RECT) / 2., CGRectGetWidth(RECT) / 2.);
	
	if (cornerRadius > maxRad) cornerRadius = maxRad;
	
	CGPoint	bl, tl, tr, br;
	
	bl = tl = tr = br = RECT.origin;
	tl.y += RECT.size.height;
	tr.y += RECT.size.height;
	tr.x += RECT.size.width;
	br.x += RECT.size.width;
	
	CGPathMoveToPoint   (path, NULL, bl.x + cornerRadius, bl.y);
    if ((flags & Rounded_TopLeft) > 0)
    	CGPathAddArcToPoint (path, NULL, bl.x, bl.y, bl.x, bl.y + cornerRadius, cornerRadius);
    else
    {
        CGPathAddLineToPoint (path, NULL, bl.x, bl.y);
        CGPathAddLineToPoint (path, NULL, bl.x, bl.y + cornerRadius);
    }
	CGPathAddLineToPoint(path, NULL, tl.x, tl.y - cornerRadius);
    if ((flags & Rounded_BottomLeft) > 0)
        CGPathAddArcToPoint (path, NULL, tl.x, tl.y, tl.x + cornerRadius, tl.y, cornerRadius);
    else
    {
        CGPathAddLineToPoint(path, NULL, tl.x, tl.y);
        CGPathAddLineToPoint(path, NULL, tl.x + cornerRadius, tl.y);
    }
	CGPathAddLineToPoint(path, NULL, tr.x - cornerRadius, tr.y);
    if ((flags & Rounded_BottomRight) > 0)
        CGPathAddArcToPoint (path, NULL, tr.x, tr.y, tr.x, tr.y - cornerRadius, cornerRadius);
    else
    {
        CGPathAddLineToPoint(path, NULL, tr.x, tr.y);
        CGPathAddLineToPoint(path, NULL, tr.x, tr.y - cornerRadius);
    }
	CGPathAddLineToPoint(path, NULL, br.x, br.y + cornerRadius);
    if ((flags & Rounded_TopRight) > 0)
        CGPathAddArcToPoint (path, NULL, br.x, br.y, br.x - cornerRadius, br.y, cornerRadius);
    else
    {
        CGPathAddLineToPoint(path, NULL, br.x, br.y);
        CGPathAddLineToPoint(path, NULL, br.x - cornerRadius, br.y);
    }
	
	CGPathCloseSubpath(path);
	
	CGPathRef ret;
	ret = CGPathCreateCopy(path);
	CGPathRelease(path);
	return ret;
}

UIImage *MakeRoundedRectStrokeImageWithRadiusAndGradient(CGSize size, CGFloat radius, CGFloat stroke,
                                                         ColorStyle begin, ColorStyle end, ColorStyle scolor,
                                                         unsigned flags)
{   
	UIImage *img_ui = nil;

    // Create a gradient from white to red
    CGFloat colors[] =
    {
        begin.r, begin.g, begin.b, begin.a,
        end.r,   end.g,   end.b,   end.a,
    };
        
    CGRect frame = CGRectMake(stroke, stroke, size.width - stroke * 2.f, size.height - stroke * 2.f);

    // Draw the rounded rect for the bg path using this convenience function
    CGPathRef bgPath = CreateRoundedRectPath(frame, radius, flags);
		
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    // Create the bitmap context into which we will draw
    
    UIGraphicsBeginImageContextWithOptions(size, NO, 0.0);
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    if (!context)
        NSLog(@"NULL CONTEXT RETURNED");
    
    CGContextClipToRect(context, CGRectMake(0, 0, size.width, size.height));
        
    CGColorSpaceRef baseSpace = CGColorSpaceCreateDeviceRGB();
    CGGradientRef gradient = CGGradientCreateWithColorComponents(baseSpace, colors, NULL, 2);
    CGColorSpaceRelease(baseSpace);
    
    baseSpace = NULL;
    
    CGContextSaveGState(context);
    
    CGContextAddPath(context, bgPath);
    CGContextClip(context);
        
    CGRect rect = CGRectMake(0.f, 0.f, size.width, size.height);
    
    CGPoint startPoint = CGPointMake(CGRectGetMidX(rect), CGRectGetMinY(rect));
    CGPoint endPoint   = CGPointMake(CGRectGetMidX(rect), CGRectGetMaxY(rect));
    
    CGContextDrawLinearGradient(context, gradient, startPoint, endPoint, 0);
    CGGradientRelease(gradient);
    gradient = NULL;
    
    CGContextRestoreGState(context);
        
	CGContextSetFillColorSpace(context, cs);
        
    if (stroke > 0.f)
    {
        CGContextSetStrokeColorSpace(context, cs);
        CGContextSetLineWidth(context, stroke);
		CGFloat fillClr[] = { scolor.r, scolor.g, scolor.b, scolor.a };
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
	CGPathRelease(bgPath);
    
    UIGraphicsEndImageContext();
        
	return img_ui;    
}
/*
// Image Processing
typedef void (*RGBA32ImageTransform_t)(void *user, UInt8 *data, int width, int height);

CGFloat Min(CGFloat a, CGFloat b) { return a < b ? a : b; }

CGImageRef RGBA32TransformImageData(CGImageRef inImage, RGBA32ImageTransform_t transform, void *user)
{
	if (!transform)
		return NULL;
    
#ifndef __clang_analyzer__ // this turns off the analyzer (strange!)
    
    // Use the generic RGB color space.
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    if (colorSpace == NULL)
    {
        fprintf(stderr, "Error allocating color space\n");
        return NULL;
    }
	
	// Get image width, height. We'll use the entire image.
    size_t w = CGImageGetWidth(inImage);
    size_t h = CGImageGetHeight(inImage);
	int    bitsPerComponent = 8;
	int    components = 4;
	size_t stride = w * components;
    
    CFDataRef immutableCopy = CGDataProviderCopyData(CGImageGetDataProvider(inImage));
	CFMutableDataRef cfdata = CFDataCreateMutableCopy(NULL, 0, immutableCopy);
    CFRelease(immutableCopy);
    
	transform(user, CFDataGetMutableBytePtr(cfdata), (int)w, (int)h);
    
	CGDataProviderRef provider = CGDataProviderCreateWithCFData(cfdata);
	CGImageRef outImage = CGImageCreate(w, h,
                                        bitsPerComponent,
                                        bitsPerComponent * components,
                                        stride,
                                        colorSpace,
                                        kCGBitmapByteOrderDefault | kCGImageAlphaLast,
                                        provider,
                                        NULL, true, kCGRenderingIntentDefault);
	
	CFRelease(cfdata);
	CGDataProviderRelease(provider);
	CGColorSpaceRelease(colorSpace);
	return outImage;
    
#endif
}

static void RGBA32ImageMultiply(void *user, UInt8 *data, int width, int height)
{
	const UInt8 *argb = (const UInt8 *)user;
	int components = 4, size = width * height * components, bitsPerComponent = 8;
    for (int i = 0; i < size; i++)
	    data[i] = (UInt8)(((uint)data[i] * (uint)argb[i % components]) >> bitsPerComponent);
}

CGImageRef MultiplyImage(CGImageRef image, const UInt8 rgba[])
{
	return RGBA32TransformImageData(image, RGBA32ImageMultiply, (void *)rgba);
}

UIImage *ColorizeFromUInt8(UIImage *image, const UInt8 rgba[])
{
	CGImageRef out = MultiplyImage(image.CGImage, rgba);
    return !out ? nil : [UIImage imageWithCGImage:out];
}

void FloatColorToUInt8(const CGFloat *in, UInt8 *out, int count)
{
	for (int i = 0; i < count; i++) out[i] = (UInt8)(in[i] * 255.f);
}

UIImage *Colorize(UIImage *image, UIColor *color)
{
	UInt8 rgba[4];
	FloatColorToUInt8(CGColorGetComponents(color.CGColor), rgba, 4);
	return ColorizeFromUInt8(image, rgba);
}*/
