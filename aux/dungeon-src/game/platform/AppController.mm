/*

File: AppController.m
Abstract: UIApplication's delegate class and the central controller of the
application.

Version: 2.0

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
("Apple") in consideration of your agreement to the following terms, and your
use, installation, modification or redistribution of this Apple software
constitutes acceptance of these terms.  If you do not agree with these terms,
please do not use, install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and subject
to these terms, Apple grants you a personal, non-exclusive license, under
Apple's copyrights in this original Apple software (the "Apple Software"), to
use, reproduce, modify and redistribute the Apple Software, with or without
modifications, in source and/or binary forms; provided that if you redistribute
the Apple Software in its entirety and without modifications, you must retain
this notice and the following text and disclaimers in all such redistributions
of the Apple Software.
Neither the name, trademarks, service marks or logos of Apple Inc. may be used
to endorse or promote products derived from the Apple Software without specific
prior written permission from Apple.  Except as expressly stated in this notice,
no other rights or licenses, express or implied, are granted by Apple herein,
including but not limited to any patent rights that may be infringed by your
derivative works or by other works in which the Apple Software may be
incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2008 Apple Inc. All Rights Reserved.

*/

#import "AppController.h"
#import "core/global.h"
#import "XmlReadContext.h"
#import "GLUtils.h"
#import "OverheadCamera.h"
#import "core/size.h"
#import "LocalGameServices.h"
#import "DeviceTexture.h"
#import "ContentCache.h"
#import "MapGenerator.h"
#import "Map.h"
#import "GameState.h"
#import "GameClock.h"
#import "FrameRateTracker.h"
#import "GameInput.h"
#import "Player.h"
#import "BallCharacter.h"
#import "Material.h"
#import "TextPopup.h"
#import "Timer.h"
#import "Profiler.h"

#import <AudioToolbox/AudioServices.h>

#import <math.h>

// CONSTANTS
#define kScale	           			(1.0f / 128.0f)
#define kAccelerometerFrequency		100.0 // Hz
#define kRenderingFrequency			120.0 // Hz
#define kFilteringFactor			0.1

// MACROS
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)

#define OLD_DRAW 0

inline float clamp(float v, float min, float max)
{
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

float             _g_inputSensitivity = 100.0f,
                  _g_tiltFactor       = 1.0f;

GameState        *state = 0;

Fader             accelerometerFader(0.0f), accStartTimer(0.0f);

FrameRateTracker  renderfps("render", 40), updatefps("update", 40), totalfps("game", 40), totalx("total", 40);

const char *tags[] =
{ 
  "update", "game", "total", "total_fps",
  "balls", "level_draw", "dl_render",
  "messages_hud", "fx_explosion_narration",
  "ui_draw", "gs_draw", 0
};

Profiler profiler(globalTranslateReadWritePath("profile.csv"), tags, 40);

extern "C" void SetScreenTouch(int x, int y)
{
	if (state)
        state->input->iPhoneTouch(core::Point(x, y));
}

// CLASS IMPLEMENTATION
@implementation AppController

- (void)drawView:(GLGravityView*)view;
{
    // I think this fixes the occasional stuck accelerometer issue
    // at least - I haven't seen it since
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];	
    [[UIAccelerometer sharedAccelerometer] setDelegate:self]; // because something goes wrong here..	
	
	profiler.StartFrame("frame");		
	
	totalx.EndFrame();
	totalx.StartFrame();
	totalfps.StartFrame();
	UpdateAppTime();

	if (!state)
		return;	
	
	static int accState = 0;

	if (accState == 0)
        printf("accState = 0\n");	
	
	if (accState == 0)
	{
		accStartTimer.Fade(1.0f, 5000);
		accState = 1;
        printf("accState = 1\n");		
	}
	
    GLfloat length = sqrtf(accel[0] * accel[0] + accel[1] * accel[1]/* + accel[2] * accel[2]*/);	
	if (accState == 1 && accStartTimer.GetValue() >= 1.0f)
	{
		accelerometerFader.Fade(1.0f, 5000);
		accState = 2;		
        printf("accState = 2\n");
	}
	else if (accState >= 2 && length >= 0.1f)
	{
		float factor = accelerometerFader.GetValue() * 200.0f;
		Vector2 move = Vector2(accel[1] * factor, accel[0] * factor);
		state->input->move.SetVector(move);
		state->camera->SetTilt(state->input->move.GetVector() * _g_tiltFactor);
		state->actors.SetForce(state->input->move.GetVector() / _g_inputSensitivity);
		if (move.x == 0.0f && move.y == 0.0f)
			printf("move == 0.0f!! ERROR?\n");
	}
	else
	{
#ifdef _DEVELOPMENT_
//		printf("bad state? length = %.4f accState = %d\n", length, accState);
#endif		
	}

	updatefps.StartFrame();
	state->Update(GameTime(updatefps.TotalLastFrameTime()));
	updatefps.EndFrame();	
	
    renderfps.StartFrame();
	glClearColor(0.1f, 0.1f, 0.5f, 0.0f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    state->Draw(GameTime(renderfps.TotalLastFrameTime()));
    renderfps.EndFrame();
	
	unsigned totaltime = totalfps.TotalLastFrameTime();
	totalfps.EndFrame();
	
	profiler.SetTime("total",     totaltime);
	profiler.SetTime("total_fps", 1000 / totaltime);	
	profiler.EndFrame();		
}

class XmlReadContext;

void InitializeAudio()
{
	OSStatus result = AudioSessionInitialize(NULL, NULL, NULL, 0);
	if (result)
		printf("Error initializing audio session! %d\n", result);
	else
	{
		UInt32 category = kAudioSessionCategory_AmbientSound;
		result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
		if (result)
			printf("Error setting audio session category! %d\n", result);
		else
		{
			result = AudioSessionSetActive(true);
			if (result)
				printf("Error setting audio session active! %d\n", result);
		}
	}	
}

extern "C" void appStoreLink()
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://phobos.apple.com/WebObjects/MZStore.woa/wa/viewSoftware?id=305576697&mt=8"]];
}

const char *iPhoneReadTextFile(const char *filename)
{
    NSString *path     = [[NSBundle mainBundle] resourcePath];
    printf("path: %s\n", [path cString]);
	NSString *fileText = [NSString stringWithContentsOfFile:[path stringByAppendingFormat:@"/%s", filename] encoding:NSUTF8StringEncoding error:nil];
	const char *text = [fileText cString];
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

const char *iPhoneTranslateReadWritePath(const char *filename)
{
	static char translatedPath[1024];
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	NSString *file = [path stringByAppendingFormat:@"/%s", filename];
	strcpy(translatedPath, [file cString]);
	return translatedPath;
}

const char *globalReadTextFile(const char *path)
{
    return iPhoneReadTextFile(path);	
}

const char *globalTranslatePath(const char *path)
{
	return iPhoneTranslatePath(path);
}

const char *globalTranslateReadWritePath(const char *path)
{
	return iPhoneTranslateReadWritePath(path);
}

-(void)setupView:(GLGravityView*)view
{
    UpdateAppTime();
	
    const GLfloat lightAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    const GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };	
    const GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 0.0f };

    CGRect rect = view.bounds;	
	
    //Configure OpenGL lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); 			
    glShadeModel(GL_SMOOTH);
	
    glViewport(0, 0, rect.size.width, rect.size.height);
	
    InitializeAudio();
	
    state = new GameState(core::Size(rect.size.width, rect.size.height));	

#ifdef LITE_VERSION
    state->Start();
#else
    state->SavedStart();
#endif
}

- (void)applicationDidFinishLaunching:(UIApplication*)application
{
	CGRect rect = [[UIScreen mainScreen] bounds];
	
	//Create a full-screen window
	window = [[UIWindow alloc] initWithFrame:rect];
    [application setStatusBarOrientation:UIInterfaceOrientationLandscapeRight];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
	
	//Create the OpenGL ES view and add it to the window
	GLGravityView *glView = [[GLGravityView alloc] initWithFrame:rect];
	[window addSubview:glView];

	glView.delegate = self;
	glView.animationInterval = 1.0 / kRenderingFrequency;
	[glView startAnimation];

	[glView release];
	
	//Show the window
	[window makeKeyAndVisible];
	
    accel[0] = accel[1] = accel[2] = 0.0f;
	
    //Configure and start accelerometer
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
    [[UIAccelerometer sharedAccelerometer] setDelegate:self];
	
    UpdateAppTime();
}

- (void)applicationWillTerminate:(UIApplication*)application
{
#ifdef LITE_VERSION
    appStoreLink();	
#endif
}

- (void)dealloc
{
	[window release];
	[super dealloc];
}

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	//Use a basic low-pass filter to only keep the gravity in the accelerometer values
	accel[0] = acceleration.x * kFilteringFactor + accel[0] * (1.0 - kFilteringFactor);
	accel[1] = acceleration.y * kFilteringFactor + accel[1] * (1.0 - kFilteringFactor);
	accel[2] = acceleration.z * kFilteringFactor + accel[2] * (1.0 - kFilteringFactor);
}

@end
