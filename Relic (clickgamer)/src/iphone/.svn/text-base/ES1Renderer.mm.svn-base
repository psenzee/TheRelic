//
//  ES1Renderer.m
//  The Relic2
//
//  Created by Paul Senzee on 10/27/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "ES1Renderer.h"

#import "AppController.h"
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

// CONSTANTS
#define kScale	           			(1.0f / 128.0f)
#define kAccelerometerFrequency		100.0 // Hz
#define kRenderingFrequency			120.0 // Hz
#define kFilteringFactor			0.1

FrameRateTracker  renderfps("render", 40), updatefps("update", 40), totalfps("game", 40), totalx("total", 40);

GameState        *state;

Fader             accelerometerFader(0.0f), accStartTimer(0.0f);

extern float __accel[3];

@implementation ES1Renderer

// Create an ES 1.1 context
- (id) init
{
	if (self = [super init])
	{
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context])
		{
            [self release];
            return nil;
        }
		
		// Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
		glGenFramebuffersOES(1, &defaultFramebuffer);
		glGenRenderbuffersOES(1, &colorRenderbuffer);
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);		
		glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
		glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);		
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
		
		// For this sample, we also need a depth buffer, so we'll create and attach one via another renderbuffer.
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);	
	
	/*
	 
	// $NOTE ACTUAL SETUP
	 */	
	UpdateAppTime();
	
	glViewport(0, 0, backingWidth, backingHeight);
	printf("viewport %d %d\n", backingWidth, backingHeight);
	
//	InitializeAudio();
	
    backingWidth = 320; backingHeight = 480;
	state = new GameState(core::Size(backingWidth, backingHeight));	

    //state->Start();
	/*		        	
    // I think this fixes the occasional stuck accelerometer issue
    // at least - I haven't seen it since
//	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];	
//	[[UIAccelerometer sharedAccelerometer] setDelegate:self]; // because something goes wrong here..		
	*/
		
	}		
	return self;
}

- (void) render
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
    // I think this fixes the occasional stuck accelerometer issue
    // at least - I haven't seen it since
//	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];	
//	[[UIAccelerometer sharedAccelerometer] setDelegate:self]; // because something goes wrong here..	

	profiler->StartFrame("frame");		
	
	totalx.EndFrame();
	totalx.StartFrame();
	totalfps.StartFrame();
	UpdateAppTime();
	
	if (!state)
		return;
	
	
	// This application only creates a single context which is already set current at this point.
	// This call is redundant, but needed if dealing with multiple contexts.
    [EAGLContext setCurrentContext:context];
    
	// This application only creates a single default framebuffer which is already bound at this point.
	// This call is redundant, but needed if dealing with multiple framebuffers.
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);	
	
	static int accState = 0;
	
	if (accState == 0)
		printf("accState = 0\n");			
	
	if (accState == 0)
	{
		accStartTimer.Fade(1.0f, 5000);
		accState = 1;
        printf("accState = 1\n");		
	}
	
    state->GetCamera()->SetTilt(Vector2(0.f/*200.0f*/, 400.0f));		
	
    GLfloat length = 0.0f;//sqrtf(accel[0] * accel[0] + accel[1] * accel[1]/* + accel[2] * accel[2]*/);	
	if (accState == 1 && accStartTimer.GetValue() >= 1.0f)
	{
		accelerometerFader.Fade(1.0f, 5000);
		accState = 2;		
        printf("accState = 2\n");
	}
	else if (accState >= 2)
	{
		float factor = accelerometerFader.GetValue() * 2.0f;
//		Vector2 move(1.0f, 1.0f); // worry about this later
#ifdef IPHONE_PORTRAIT
		Vector2 move = Vector2(-__accel[0] * factor, __accel[1] * factor);		
#else
		Vector2 move = Vector2(__accel[1] * factor, __accel[0] * factor);
#endif
		state->GetInput().move.SetVector(move);
//		state->camera->SetTilt(state->input->move.GetVector() * _g_tiltFactor);
		//		state->actors.SetForce(state->input->move.GetVector() / _g_inputSensitivity);
	}
	
	updatefps.StartFrame();
	state->Update(GameTime(updatefps.TotalLastFrameTime()));
	updatefps.EndFrame();	
	
    renderfps.StartFrame();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(1.f, -1.f, 1.f);
	glTranslatef(0.f, 1.f, 0.f);	
    state->Draw(GameTime(renderfps.TotalLastFrameTime()));
    renderfps.EndFrame();
	
	unsigned totaltime = totalfps.TotalLastFrameTime();
	totalfps.EndFrame();
	
	profiler->SetTime("total",     totaltime);
	profiler->SetTime("total_fps", 1000 / totaltime);	
	profiler->EndFrame();

	/*
    // Replace the implementation of this method to do your own custom drawing
    
    static const GLfloat squareVertices[] = {
        -0.5f,  -0.33f,
         0.5f,  -0.33f,
        -0.5f,   0.33f,
         0.5f,   0.33f,
    };
	
    static const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
    
	static float transY = 0.0f;
	 */

    /*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glTranslatef(0.0f, (GLfloat)(sinf(transY)/2.0f), 0.0f);
	transY += 0.075f;
	
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
	 */	 
	// This application only creates a single color renderbuffer which is already bound at this point.
	// This call is redundant, but needed if dealing with multiple renderbuffers.
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer
{	
	// Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	// For this sample, we also need a depth buffer, so we'll create and attach one via another renderbuffer.
	glGenRenderbuffersOES(1, &depthRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);		
	
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    
    return YES;
}

- (void) dealloc
{
	// Tear down GL
	if (defaultFramebuffer)
	{
		glDeleteFramebuffersOES(1, &defaultFramebuffer);
		defaultFramebuffer = 0;
	}

	if (colorRenderbuffer)
	{
		glDeleteRenderbuffersOES(1, &colorRenderbuffer);
		colorRenderbuffer = 0;
	}
	
	if(depthRenderbuffer)
	{
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}	
	
	// Tear down context
	if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	
	[context release];
	context = nil;
	
	[super dealloc];
}

@end
