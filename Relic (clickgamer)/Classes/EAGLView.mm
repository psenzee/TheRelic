//
//  EAGLView.mm
//  Relic
//
//  Created by Paul Senzee on 1/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "EAGLView.h"

//#import "AppController.h"
#import "core/global.h"
#import "XmlReadContext.h"
#import "GLUtils.h"
#import "OverheadCamera.h"
#import "core/size.h"
#import "LocalGameServices.h"
#import "DeviceTexture.h"
#import "Map.h"
#import "GameState.h"
#import "GameClock.h"
#import "FrameRateTracker.h"
#import "GameInput.h"
#import "Material.h"
#import "TextPopup.h"
#import "time/Timer.h"
#import "Profiler.h"

#import <stdio.h>
#include <string>

#include <sys/types.h> 
#include <sys/sysctl.h> 

// CONSTANTS
#define kScale	           			(1.0f / 128.0f)
#define kAccelerometerFrequency		100.0 // Hz
#define kRenderingFrequency			120.0 // Hz
#define kFilteringFactor			0.1

GameState        *state;

Fader             accelerometerFader(0.0f), accStartTimer(0.0f);

extern float __accel[3];

int gBackingWidth = 0, gBackingHeight = 0;

float gTouchRatioX = 1.0f, gTouchRatioY = 1.0f; 
float gTouchOffsetX = 0.0f, gTouchOffsetY = 0.0f;

//////////////////////////////////////////////////////////////////

float _g_inputSensitivity = 100.0f;
float _g_tiltFactor = 2.f;

extern "C" bool GetPlatformIsiPad();

extern "C" void UnloadAll()
{
    state->UnloadAll();	
}

inline core::Point GetAdjustedPoint(int x, int y)
{
//	return core::Point((int)(x * gTouchRatioX + gTouchOffsetX), (int)(y * gTouchRatioY + gTouchOffsetY));
	return core::Point((int)(((GetPlatformIsiPad() ? 768 : 320) - y) * gTouchRatioX + gTouchOffsetX), (int)((x) * gTouchRatioY + gTouchOffsetY));
}

extern "C" void SetScreenTouchBegan(int index, int x, int y)
{
    if (state)
        state->GetInput().ScreenTouchBegan(index, GetAdjustedPoint(x, y));
}

extern "C" void SetScreenTouchMoved(int index, int x, int y)
{
    if (state)
        state->GetInput().ScreenTouchMoved(index, GetAdjustedPoint(x, y));
}

extern "C" void SetScreenTouchStationary(int index, int x, int y)
{
    if (state)
        state->GetInput().ScreenTouchStationary(index, GetAdjustedPoint(x, y));
}

extern "C" void SetScreenTouchEnded(int index, int x, int y)
{
    if (state)
        state->GetInput().ScreenTouchEnded(index, GetAdjustedPoint(x, y));
}

extern "C" void GoToUrl(const char *url)
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

const char *iPhoneReadTextFile(const char *filename)
{
    NSString *path = [[NSBundle mainBundle] resourcePath];
    printf("\nReading file at path: %s\n", [path UTF8String]);
    NSString *fileText = [NSString stringWithContentsOfFile:[path stringByAppendingFormat:@"/%s", filename] encoding:NSUTF8StringEncoding error:nil];
    const char *text = [fileText UTF8String];
  //[path release];
    return text;
}

const char *iPhoneTranslatePath(const char *filename)
{
    static char translatedPath[1024];
    NSString *path = [[NSBundle mainBundle] resourcePath];
    NSString *file = [path stringByAppendingFormat:@"/%s", filename];
    strcpy(translatedPath, [file UTF8String]);
    return translatedPath;
}

extern "C" const char *getGlobalReadOnlyPath()
{
    static char data[1024];	
    NSString *path = [[NSBundle mainBundle] resourcePath];
    strcpy(data, [path UTF8String]);	
//  [path release];
    return data;
}

extern "C" const char *getGlobalReadWritePath()
{
    static char data[1024];	
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    strcpy(data, [path UTF8String]);	
//  [path release];
    return data;
}

const char *iPhoneTranslateReadWritePath(const char *filename)
{
    static char translatedPath[1024];
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *file = [path stringByAppendingFormat:@"/%s", filename];
    strcpy(translatedPath, [file UTF8String]);
	// [file release];	
    return translatedPath;
}

extern "C" bool WriteUserTextFile(const char *filename, const char *data)
{
	const char *path = iPhoneTranslateReadWritePath(filename);
    printf("\nWriting file at path: %s\n", path);
	FILE *file = fopen(path, "wb");
	if (file == NULL)
		return false;
	size_t sz = fwrite(data, strlen(data), 1, file);
	fclose(file);
    return sz != 0;
}

extern "C" const char *ReadUserTextFile(const char *filename)
{
	const char *path = iPhoneTranslateReadWritePath(filename);
    printf("\nReading file at path: %s\n", path);
    NSString *fileText = [NSString stringWithContentsOfFile:[NSString stringWithUTF8String:path] encoding:NSUTF8StringEncoding error:nil];
    const char *text = [fileText UTF8String];
    return text;
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
//		case UITouchPhaseStationary: SetScreenTouchStationary((intptr_t)touch, pt.x, pt.y); break;
//		case UITouchPhaseCancelled:
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

extern "C" void SetupGame()
{
	UpdateAppTime();
	
	glViewport(0, 0, gBackingWidth, gBackingHeight);
	printf("viewport %d %d\n", gBackingWidth, gBackingHeight);
	
//  int backingWidth = 320, backingHeight = 480;
    int backingWidth = 480, backingHeight = 320;
//  int backingWidth = gBackingWidth, backingHeight = gBackingHeight;
	backingHeight = (int)(gBackingWidth * ((float)backingWidth / gBackingHeight));
	state = new GameState(core::Size(backingWidth, backingHeight));
}

extern "C" void CaptureScreen()
{
	// we don't do this on the phone	
}

extern "C" const char *GetPlatformUniqueIdentifier()
{
	static char data[1024] = "";
	strcpy(data, [[[UIDevice currentDevice] uniqueIdentifier] UTF8String]);
	return data;
}

extern "C" const char *GetPlatformDeviceName()
{
	static char data[1024] = "";
	strcpy(data, [[[UIDevice currentDevice] name] UTF8String]);
	return data;
}

extern "C" const char *GetPlatformSystemName()
{
	static char data[1024] = "";
	strcpy(data, [[[UIDevice currentDevice] systemName] UTF8String]);
	// NSLog(@"systemName: %@", [[UIDevice currentDevice] systemName]);
	return data;
}

extern "C" const char *GetPlatformSystemVersion()
{
	static char data[1024] = "";
	strcpy(data, [[[UIDevice currentDevice] systemVersion] UTF8String]);
	// NSLog(@"systemVersion: %@", [[UIDevice currentDevice] systemVersion]);
	return data;
}

extern "C" const char *GetPlatformModel()
{
	static char data[1024] = "";
	strcpy(data, [[[UIDevice currentDevice] model] UTF8String]);
	// NSLog(@"model: %@", [[UIDevice currentDevice] model]);
	return data;
}

extern "C" const char *GetPlatformMachine()
{
    static char machine[1024] = "";
    size_t size = 0;  
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);  
    sysctlbyname("hw.machine", machine, &size, NULL, 0);  
    return machine;
}

extern "C" const char *GetPlatformLocalizedModel()
{
	static char data[1024];
	strcpy(data, [[[UIDevice currentDevice] localizedModel] UTF8String]);
	// NSLog(@"localizedModel: %@", [[UIDevice currentDevice] localizedModel]);
	return data;
}

extern "C" bool GetPlatformIsFirstGen()
{
    const char *machine = GetPlatformMachine();
    return strcmp(machine, "iPhone1,1") == 0 || strcmp(machine, "iPod1,1") == 0;
}

extern "C" bool GetPlatformIsSecondGen()
{
    const char *machine = GetPlatformMachine();
    return strcmp(machine, "iPhone1,2") == 0 || strcmp(machine, "iPod1,2") == 0;
}

extern "C" bool GetPlatformIsiPad()
{
	static bool isKnown = false;
    static bool isiPad = false;
	if (!isKnown)
	{
        const char *machine = GetPlatformMachine();
        isiPad = strncmp(machine, "iPad", 4) == 0;
		isKnown = true;
	}
	return isiPad;
}

extern void StartCrystal();


BOOL hasRetinaDisplay(void)
{
	if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
		return [[UIScreen mainScreen] scale] == 2.0 ? YES : NO;
	
	return NO;
}

@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation EAGLView

@dynamic context;

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:.
- (id)initWithCoder:(NSCoder*)coder
{
    self = [super initWithCoder:coder];
	if (self)
    {
		if (hasRetinaDisplay())
		{
			//__isHiResDevice = true;
			[self setContentScaleFactor:2.0f];
		}			
		
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
    }
    
    return self;
}

- (BOOL)isMultipleTouchEnabled
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


- (void)dealloc
{
    [self deleteFramebuffer];    
    [context release];
    
    [super dealloc];
}

- (EAGLContext *)context
{
    return context;
}

extern "C" bool IsHiResDevice()
{
	static bool isIPad = GetPlatformIsiPad();
	static bool isRetina = hasRetinaDisplay() == YES;
	
	if (isIPad)
	{
		gTouchRatioY = 480.f / 1024.f;
//		gTouchRatioX = 320.f / 768.f;
        gTouchRatioX = 360.f / 768.f; // this is required to account for the different aspect ratio
		gTouchOffsetX = -(360.f - 320.f) * 0.5f;
		gTouchOffsetY = 0.f;
	}
	
    return isRetina || isIPad;
}

- (void)setContext:(EAGLContext *)newContext
{
    if (context != newContext)
    {
        [self deleteFramebuffer];
        
        [context release];
        context = [newContext retain];
        
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)createFramebuffer
{
    if (context && !defaultFramebuffer)
    {
        [EAGLContext setCurrentContext:context];
        
        // Create default framebuffer object.
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        // Create color render buffer and allocate backing store.
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        
		// For this sample, we also need a depth buffer, so we'll create and attach one via another renderbuffer.
		glGenRenderbuffersOES(1, &depthFramebuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthFramebuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, framebufferWidth, framebufferHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthFramebuffer);		
		
        gBackingWidth = framebufferWidth;
		gBackingHeight = framebufferHeight; 
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
}

- (void)deleteFramebuffer
{
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        if (defaultFramebuffer)
        {
            glDeleteFramebuffers(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }
        
        if (colorRenderbuffer)
        {
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }
    }
}

int  FRAMES_TO_CLEAR = 2;
int  g_CurrentClearFrame = 0;

- (void)setFramebuffer
{
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        if (!defaultFramebuffer)
            [self createFramebuffer];
        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        glViewport(0, 0, framebufferWidth, framebufferHeight);
		if (g_CurrentClearFrame < FRAMES_TO_CLEAR)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);			
		}
    }
}


- (BOOL)presentFramebuffer
{
    BOOL success = FALSE;
    
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		
		if (g_CurrentClearFrame < FRAMES_TO_CLEAR)
		{
		    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		    glClear(GL_COLOR_BUFFER_BIT);	

		}		
	    g_CurrentClearFrame++;
        success = [context presentRenderbuffer:GL_RENDERBUFFER];
    }
    
    return success;
}

- (void)layoutSubviews
{
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    [self deleteFramebuffer];
}

@end
