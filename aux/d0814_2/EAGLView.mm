#import "EAGLView.h"

#import "ES1Renderer.h"
#import "ES2Renderer.h"

#import "core/core.h"
#import "game/GameState.h"
#import "render/OverheadCamera.h"
#import "GameClock.h"
#import "global.h"

class GameState;

@implementation EAGLView

@synthesize animating;
@dynamic animationFrameInterval;

//////////////////////////////////////////////////////////////////

float             _g_inputSensitivity = 100.0f,
_g_tiltFactor       = 2.f;

const char *iPhoneReadTextFile(const char *filename)
{
    NSString *path     = [[NSBundle mainBundle] resourcePath];
    printf("path: %s\n", [path cString]);
    NSString *fileText = [NSString stringWithContentsOfFile:[path stringByAppendingFormat:@"/%s", filename] encoding:NSUTF8StringEncoding error:nil];
    const char *text = [fileText cString];
//  [path release];
    return text;
}

const char *iPhoneTranslatePath(const char *filename)
{
    static char translatedPath[1024];
    NSString *path = [[NSBundle mainBundle] resourcePath];
    NSString *file = [path stringByAppendingFormat:@"/%s", filename];
    strcpy(translatedPath, [file cString]);
    return translatedPath;
}

extern "C" const char *getGlobalReadOnlyPath()
{
    static char data[1024];	
    NSString *path = [[NSBundle mainBundle] resourcePath];
    strcpy(data, [path cString]);	
//    [path release];
    return data;
}

extern "C" const char *getGlobalReadWritePath()
{
    static char data[1024];	
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    strcpy(data, [path cString]);	
//    [path release];
    return data;
}

const char *iPhoneTranslateReadWritePath(const char *filename)
{
    static char translatedPath[1024];
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *file = [path stringByAppendingFormat:@"/%s", filename];
    strcpy(translatedPath, [file cString]);
//    [path release];
//    [file release];	
    return translatedPath;
}

extern "C" void SetScreenTouchBegan(int index, int x, int y);
extern "C" void SetScreenTouchMoved(int index, int x, int y);
extern "C" void SetScreenTouchEnded(int index, int x, int y);
extern "C" void SetScreenTouchStationary(int index, int x, int y);

void UpdateTouch(UITouch *touch, UIView *window)
{
    CGPoint pt = [touch locationInView:window];
	switch ([touch phase])
	{
	case UITouchPhaseBegan:      SetScreenTouchBegan((intptr_t)touch, pt.x, pt.y); break;
	case UITouchPhaseMoved:      SetScreenTouchMoved((intptr_t)touch, pt.x, pt.y); break;
	case UITouchPhaseStationary: SetScreenTouchStationary((intptr_t)touch, pt.x, pt.y); break;
	case UITouchPhaseCancelled:
	case UITouchPhaseEnded:      SetScreenTouchEnded((intptr_t)touch, pt.x, pt.y); break;			
	default: break;
	}
}

void UpdateTouches(NSSet *touches, UIView *window)
{
    for (UITouch *touch in touches)
    {
        CGPoint pt = [touch locationInView:window];		
        UpdateTouch(touch, window);
    }
}

- (BOOL) isMultipleTouchEnabled
{
    return YES;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)events
{
    UpdateTouches(touches, self);	
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)events
{
    UpdateTouches(touches, self);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)events
{
    UpdateTouches(touches, self);
}

const char *_g_MainLuaFile = "server-game.lua";

bool buttons[32] =
{
false, false, false, false, false, false, false, false,
false, false, false, false, false, false, false, false,
false, false, false, false, false, false, false, false,
false, false, false, false, false, false, false, false
};

extern "C" const char *globalReadTextFile(const char *path)
{
    return iPhoneReadTextFile(path);	
}

extern "C" const char *globalTranslatePath(const char *path)
{
	return iPhoneTranslatePath(path);
}

extern "C" const char *globalTranslateReadWritePath(const char *path)
{
	return iPhoneTranslateReadWritePath(path);
}
//////////////////////////////////////////////////////////////////

// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder:(NSCoder*)coder
{    
    if ((self = [super initWithCoder:coder]))
	{
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat, nil];
		
		renderer = 0;//[[ES2Renderer alloc] init];
		
		if (!renderer)
		{
			renderer = [[ES1Renderer alloc] init];
			
			if (!renderer)
			{
				[self release];
				return nil;
			}
		}
        
		animating = FALSE;
		displayLinkSupported = FALSE;
//		animationFrameInterval = 1;
		animationFrameInterval = 2;		
		displayLink = nil;
		animationTimer = nil;
		
		// A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
		// class is used as fallback when it isn't available.
		NSString *reqSysVer = @"3.1";
		NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
		if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
			displayLinkSupported = TRUE;
    }
	
    return self;
}

extern "C" void UpdateBonjourConnections();

- (void) drawView:(id)sender
{
	/*
    // I think this fixes the occasional stuck accelerometer issue
    // at least - I haven't seen it since
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];	
	[[UIAccelerometer sharedAccelerometer] setDelegate:self]; // because something goes wrong here..	
	 */
	//glCullFace(GL_FRONT);
	UpdateBonjourConnections();
    [renderer render];
}

- (void) layoutSubviews
{
	[renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
    [self drawView:nil];
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is two on a display that refreshes 60 times a second. The default
	// frame interval setting of one will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
		if (displayLinkSupported)
		{
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
			// if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
			// not be called in system versions earlier than 3.1.

			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		}
		else
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];
		
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating)
	{
		if (displayLinkSupported)
		{
			[displayLink invalidate];
			displayLink = nil;
		}
		else
		{
			[animationTimer invalidate];
			animationTimer = nil;
		}
		
		animating = FALSE;
	}
}

- (void) dealloc
{
    [renderer release];
	
    [super dealloc];
}

@end
