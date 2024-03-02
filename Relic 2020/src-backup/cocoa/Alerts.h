//
//  Alerts.h
//

#import <Foundation/Foundation.h>

@class SimpleCall;
@class AlertOption;

AlertOption *AlertOpt(NSString *name, SimpleCall *call);

void AlertWithOptions(NSString *title, NSString *message, AlertOption *option1, ...);
void AlertWithOptionArray(NSString *title, NSString *message, NSMutableArray *options);

void AlertOptions0 (NSString *title, NSString *message);

void AlertOptions1 (NSString *title, NSString *message,
                    NSString *opt, SimpleCall *call);

void AlertOptions2 (NSString *title, NSString *message,
                    NSString *name1, SimpleCall *call1,
                    NSString *name2, SimpleCall *call2);

void AlertOptions3 (NSString *title, NSString *message,
                    NSString *name1, SimpleCall *call1,
                    NSString *name2, SimpleCall *call2,
                    NSString *name3, SimpleCall *call3);

void AlertDismissAll(void);