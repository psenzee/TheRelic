//
//  AlertWithOptions.h
//  Dial
//
//  Created by Paul Senzee on 4/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SimpleCall;

@interface AlertOption : NSObject
{
};

@property (nonatomic, retain) NSString   *name;
@property (nonatomic, retain) SimpleCall *call;

- (id)init;
- (id)initWithName:(NSString *)name call:(SimpleCall *)call;

+ (AlertOption *)optionWithName:(NSString *)name call:(SimpleCall *)call;

@end


void AlertWithOptions(NSString *title, NSString *message, AlertOption *option1, ...);
void AlertWithOptionArray(NSString *title, NSString *message, NSMutableArray *options);
void DismissAllAlerts(void);