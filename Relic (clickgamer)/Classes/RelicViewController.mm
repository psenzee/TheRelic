//
//  RelicViewController.m
//  Relic
//
//  Created by Paul Senzee on 1/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIAlertView.h>

#import "RelicViewController.h"
#import "EAGLView.h"
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
#import "CrystalSession.h"

// Uniform index.
enum {
    UNIFORM_TRANSLATE,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum {
    ATTRIB_VERTEX,
    ATTRIB_COLOR,
    NUM_ATTRIBUTES
};

extern bool g_bInBackground;
extern id   g_App;
id          g_ViewController = nil;


extern "C" void UnloadAll();

extern "C" bool IsHiResDevice();

extern "C" bool GetPlatformIsiPad();

extern "C" void SetAlertDismissedCallback(void (*fn)(void *), void *context);

extern "C" void SetAnimationFrameInterval(int value)
{
	[g_ViewController setAnimationFrameInterval:value];
}

extern "C" void MemoryFailureCloseAll(void *)
{
	UnloadAll();
	exit(-1);	
}

extern "C" void Alert(const char *title, const char *text)
{
	UIAlertView *alertView = nil; 
	@try
	{ 	
		alertView = [[UIAlertView alloc] initWithTitle:[NSString stringWithUTF8String:title]  
					    message:[NSString stringWithUTF8String:text] 
						delegate:g_App cancelButtonTitle:@"OK" otherButtonTitles:nil]; 
	  
		[alertView show]; 
	} 
	@finally 
	{ 
		if (alertView) { [alertView release]; } 
	}	
}

extern "C" void MemoryFail()
{
	SetAlertDismissedCallback(MemoryFailureCloseAll, 0);		
	Alert("The Relic: Out of Memory", 
		  "Your device does not have sufficient available memory and"
		  " The Relic will now close.  Please close other applications and restart or reboot.");
    // Release any cached data, images, etc. that aren't in use.
}

extern "C" void SendMemoryWarning()
{
	MemoryFail();
    //[g_ViewController didReceiveMemoryWarning];
}

FrameRateTracker renderfps("render", 40), updatefps("update", 40), totalfps("game", 40), totalx("total", 40);

extern "C" void UnloadAll();

@interface RelicViewController ()
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end

@implementation RelicViewController

@synthesize animating, context, displayLink;

- (void)awakeFromNib
{
    EAGLContext *aContext = NULL;//[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
//	EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	
    if (!aContext)
    {
        aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        g_ViewController = self;
	}
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
	self.context = aContext;
	[aContext release];
	
    [(EAGLView *)self.view setContext:context];
    [(EAGLView *)self.view setFramebuffer];
    
    if ([context API] == kEAGLRenderingAPIOpenGLES2)
        [self loadShaders];
    
    animating = FALSE;
    //animationFrameInterval = 1;
	animationFrameInterval = 2;		

    self.displayLink = nil;
}

- (void)dealloc
{
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
    
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
	[super viewDidUnload];
	
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }

    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	static bool shouldRotate = false;
	static bool isKnown      = false;
	
	if (!isKnown)
	{
		shouldRotate = GetPlatformIsiPad() || IsHiResDevice();
		isKnown = true;
	}
	
	// Support for landscape orientations only
	return /*shouldRotate &&*/ ((interfaceOrientation == UIInterfaceOrientationLandscapeRight) || (interfaceOrientation == UIInterfaceOrientationLandscapeLeft));
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration

{  
	[CrystalSession willRotateToInterfaceOrientation:toInterfaceOrientation duration:duration];
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation

{
	[CrystalSession didRotateFromInterfaceOrientation:fromInterfaceOrientation];
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
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

- (void)startAnimation
{
    if (!animating)
    {
        CADisplayLink *aDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
    }
}

- (void)drawFrame
{
	if (g_bInBackground)
        return;	
	
    [(EAGLView *)self.view setFramebuffer];
	
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
    
    state->GetCamera()->SetTilt(Vector2(0.f/*200.0f*/, 400.0f));		
    //state->GetCamera()->SetTilt(Vector2(200.0f, 400.0f));	
	
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
    
    [(EAGLView *)self.view presentFramebuffer];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source)
    {
        NSLog(@"Failed to load vertex shader");
        return FALSE;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        return FALSE;
    }
    
    return TRUE;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create shader program.
    program = glCreateProgram();
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname])
    {
        NSLog(@"Failed to compile vertex shader");
        return FALSE;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname])
    {
        NSLog(@"Failed to compile fragment shader");
        return FALSE;
    }
    
    // Attach vertex shader to program.
    glAttachShader(program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(program, ATTRIB_COLOR, "color");
    
    // Link program.
    if (![self linkProgram:program])
    {
        NSLog(@"Failed to link program: %d", program);
        
        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program)
        {
            glDeleteProgram(program);
            program = 0;
        }
        
        return FALSE;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation(program, "translate");
    
    // Release vertex and fragment shaders.
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    return TRUE;
}

@end
