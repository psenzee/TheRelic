//
//  TextPopup.m
//  GLGravity
//
//  Created by Paul Senzee on 1/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "TextPopup.h"

text_callback  text_popup_callback = 0;
void          *text_popup_context  = 0;

@implementation TextPopup

-(id)init
{
	self = [super init];
	if (self != nil)
	{	
		[self setDelegate:self];
		[self setMessage:@" "];
		[self addButtonWithTitle:@"OK"];
		
		text = [[UITextField alloc] initWithFrame:CGRectMake(20.0, 45.0, 245.0, 25.0)];
		
		text.borderStyle = UITextBorderStyleBezel;
		text.textColor = [UIColor blackColor];
		text.font = [UIFont systemFontOfSize:17.0];
		text.backgroundColor = [UIColor whiteColor];
		text.autocorrectionType = UITextAutocorrectionTypeNo;        // no auto correction support
		text.keyboardType = UIKeyboardTypeDefault;   // use the default type input method (entire keyboard)
		text.returnKeyType = UIReturnKeyDone;
		
		text.clearButtonMode = UITextFieldViewModeWhileEditing;      // has a clear 'x' button to the right
		
		[self addSubview:text];
		CGAffineTransform moveUp = CGAffineTransformMakeTranslation(0.0, 80.0);
		[self setTransform: moveUp];
	}
	return self;
}

- (void) alertView:(UIAlertView *)alert clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 0)
	{ // OK pushed
		NSString *s = [text text];
        NSLog(s);
		if (text_popup_callback)
			text_popup_callback([s UTF8String], text_popup_context);
		printf("%s\n", [s UTF8String]);
		text_popup_callback = 0;
		text_popup_context  = 0;
	}
}

// release resources when they are no longer needed.
- (void)dealloc
{
	[text release];
	[super dealloc];
}

@end

void GetTextFromKeyboard(const char *message, text_callback callback, void *context)
{
	text_popup_callback = callback; text_popup_context = context;
	TextPopup *dialog = [[[TextPopup alloc] init] retain];
	[dialog setTitle:[NSString stringWithCString:message]];
	[dialog show];
	[dialog release];
}
