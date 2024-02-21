//
//  RelicViewController.h
//  Relic
//
//  Created by Paul Senzee on 1/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@class TouchView;
@class EAGLView;

@interface RelicViewController : UIViewController
{
    EAGLContext    *context;
    GLuint          program;
    
    BOOL            animating;
    NSInteger       animationFrameInterval;
    CADisplayLink  *displayLink;
}

@property (nonatomic, retain) EAGLView *eagl;

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;

@end
