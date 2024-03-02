//
//  AlertWithOptions.m
//  Dial
//
//  Created by Paul Senzee on 4/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AlertWithOptions.h"
#import "SimpleCall.h"

NSMutableArray *OUTSTANDING_ALERTS = nil;

static void AddAlert(UIAlertView *alert)
{
    if (!alert)
        return;
    if (!OUTSTANDING_ALERTS)
        OUTSTANDING_ALERTS = [[NSMutableArray alloc] init];
    [OUTSTANDING_ALERTS addObject:alert];
}

static void RemoveAlert(UIAlertView *alert)
{
    if (!alert || !OUTSTANDING_ALERTS)
        return;
    [OUTSTANDING_ALERTS removeObject:alert];
}

static void DismissAlert(UIAlertView *alert)
{
    int cancel = alert.cancelButtonIndex;
    [alert dismissWithClickedButtonIndex:cancel animated:YES];
}

@interface AlertWithOptionsDelegate : NSObject <UIAlertViewDelegate>
{
}
@property (nonatomic, retain) NSMutableArray *options;

- (id)initWithOptions:(NSMutableArray *)options;
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;

@end

@implementation AlertWithOptionsDelegate 

- (id)initWithOptions:(NSMutableArray *)options_
{
	self = [super init];
	if (self)
        self.options = options_;
	return self;
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (self.options && buttonIndex < self.options.count)
    {
        AlertOption *option = [self.options objectAtIndex:buttonIndex];
        if (option)
            [option.call call];
        RemoveAlert(alertView);
    }
}

@end

@implementation AlertOption

@synthesize name;
@synthesize call;

- (id)init
{
    self = [super init];
    if (self)
    {
        self.name = nil;
        self.call = nil;
    }
    return self;
}

- (id)initWithName:(NSString *)_name call:(SimpleCall *)_call
{
    self = [super init];
    if (self)
    {
        self.name = _name;
        self.call = _call;
    }
    return self;
}

+ (AlertOption *)optionWithName:(NSString *)name call:(SimpleCall *)call
{
    return [[AlertOption alloc] initWithName:name call:call];
}

@end

void AlertWithOptionArray(NSString *title, NSString *message, NSMutableArray *options)
{
#ifndef __clang_analyzer__ // this turns off the analyzer (strange!)    
    AlertWithOptionsDelegate  *delegate = [[AlertWithOptionsDelegate  alloc] initWithOptions:options];

    UIAlertView *view = [[UIAlertView alloc] initWithTitle:title message:message delegate:delegate cancelButtonTitle:nil otherButtonTitles:nil];
    AddAlert(view);
    for (AlertOption *option in options)
        [view addButtonWithTitle:option.name];
    [view show];    
#endif
}

void AlertWithOptions(NSString *title, NSString *message,
                          AlertOption *option1, ...)
{
    va_list args;
    va_start(args, option1);
    NSMutableArray *options = [[NSMutableArray alloc] init];
    for (AlertOption *arg = option1; arg != nil; arg = va_arg(args, id))
        [options addObject:arg];
    AlertWithOptionArray(title, message, options);
    va_end(args);
}

void DismissAllAlerts(void)
{
    if (!OUTSTANDING_ALERTS)
        return;
    for (UIAlertView *alert in OUTSTANDING_ALERTS)
    {
        DismissAlert(alert);
        RemoveAlert(alert);
    }
}