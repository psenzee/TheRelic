//
//  Services.m
//

#import "Services.h"

#import "Json.h"
#import "Alerts.h"
#import "DataUtil.h"
#import <stdlib.h>
#import <Foundation/Foundation.h>

#if 0

/*
NSString *GenerateUUID(void)
{
    NSString *uuidString = nil;
    CFUUIDRef uuid = CFUUIDCreate(NULL);
    if (uuid)
    {
        uuidString = (NSString *)CFUUIDCreateString(NULL, uuid);
        CFRelease(uuid);
    }
    return uuidString;
}

NSString *GetUserUniqueUDID(void)
{
    NSString *key = @"USERUNIQUEUUID";
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *uuid = [defaults objectForKey:key];
    if (uuid == nil)
    {
        uuid = GenerateUUID();
        [defaults setObject:uuid forKey:key];
        [defaults synchronize];
    }
    return uuid;
}

NSString *GetUserDefaultValue(NSString *key)
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    return [defaults objectForKey:key];
}

id SetUserDefaultValue(NSString *key, id object)
{
    if (!key) return nil;

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if (object)
    {
        [defaults setObject:object forKey:key];
        [defaults synchronize];
    }
    else
    {
        if (GetUserDefaultValue(key) != nil)
            [defaults removeObjectForKey:key];
    }
    return object;
}

BOOL Equal(id a, id b)
{
    return (a == b || (a != nil && b != nil && [a isEqual:b]));
}

id SetUserDefaultValueIfDiffers(NSString *key, id object)
{
    if (!Equal(GetUserDefaultValue(key), object))
        SetUserDefaultValue(key, object);
    return object;
}

id LoadUserDefaultValueIfNil(NSString *key, id currentValue)
{
    if (!currentValue)
        return GetUserDefaultValue(key);
    SetUserDefaultValueIfDiffers(key, currentValue);
    return currentValue;
}

id GetFirstValueInObjectTree(id obj, NSString *key)
{
    if ([obj isKindOfClass:[NSArray class]])
    {
        for (id object in (NSArray *)obj)
        {
            id value = GetFirstValueInObjectTree(object, key);
            if (value)
                return value;
        }
        return nil;
    }
    else if ([obj isKindOfClass:[NSDictionary class]])
    {
        NSDictionary *dict = (NSDictionary *)obj;
        if ([[dict allKeys] containsObject:key])
            return [dict objectForKey:key];
        for (NSString *key in [dict allKeys])
        {
            id object = [dict objectForKey:key];
            id value = GetFirstValueInObjectTree(object, key);
            if (value)
                return value;
        }
    }
    return nil;
}*/

static Services *SHARED = nil;

#define ServiceURL @"https://playfascination.com/hk/hk_db.php"

@interface Services ()

- (id)init;

@end

@implementation Services

- (id)init
{
    self = [super init];
    if (self)
    {
    }
    return self;
}

- (void)clear
{
    self.session  = SetUserDefaultValue(@"FASC_SESSION",  nil);
    self.email    = SetUserDefaultValue(@"FASC_EMAIL",    nil);
    self.login    = SetUserDefaultValue(@"FASC_LOGIN",    nil);
    self.username = SetUserDefaultValue(@"FASC_USERNAME", nil);
}

- (void)update
{
    self.session  = LoadUserDefaultValueIfNil(@"FASC_SESSION",  self.session);
    self.email    = LoadUserDefaultValueIfNil(@"FASC_EMAIL",    self.email);
    self.login    = LoadUserDefaultValueIfNil(@"FASC_LOGIN",    self.login);
    self.username = LoadUserDefaultValueIfNil(@"FASC_USERNAME", self.username);
}

- (BOOL)request:(NSString *)json error:(NSError **)error
{
    self.errorCode = nil;
    self.errorMessage = nil;
    
    NSLog(@"REQUEST: %@", json);
    json = [json stringByAddingPercentEscapesUsingEncoding:NSASCIIStringEncoding];
    NSURL *url = [NSURL URLWithString:
                  [NSString stringWithFormat:@"%@?r=%@", ServiceURL, json]];
    NSString *response = [NSString stringWithContentsOfURL:url
                                                  encoding:NSUTF8StringEncoding
                                                     error:error];
    
    bool success = false;
    id   values  = nil;
    
    if (response)
    {
        NSLog(@"RESPONSE: %@", response);
        values = ParseJSON(response);
        NSNumber *succeeded = FirstValueForKey(values, @"succeeded");
        if (succeeded && [succeeded boolValue])
            success = true;
        if (!success)
            [self setErrorCode:FirstValueForKey(values, @"code")
                       message:FirstValueForKey(values, @"message")];
        self.session = FirstValueForKey(values, @"session");
        self.response = values;
    }
    else
    {
        NSLog(@"Error = %@", *error);
    }
    return success;
}

- (void)logError
{
    if (self.errorCode || self.errorMessage)
        NSLog(@"Services - error code (%@): %@", self.errorCode, self.errorMessage);
}

- (void)setErrorCode:(NSString *)code message:(NSString *)message
{
    self.errorCode = code;
    self.errorMessage = message;
}

- (BOOL)loginSynchronousWithPassword:(NSString *)password
{
    if (!password)
    {
        [self setErrorCode:@"CLIENT_PASSWORD_NIL" message:@"Password is required."];
        return NO;
    }
    
    [self update];
    
    if (!self.login)
    {
        [self setErrorCode:@"CLIENT_LOGIN_NIL" message:@"Login username or email is required."];
        return NO;
    }
    
    NSString *request = [NSString stringWithFormat:
                         
                         @"{\"action\":\"login\","
                           "\"login\":\"%@\","
                           "\"password\":\"%@\","
                           "\"device\":\"%@\"}",
    
                           self.login, password, GetUserUniqueUUID()];
    
    NSError *error = nil;
    BOOL success = [self request:request error:&error];
    
    if (!success)
    {
        AlertWithOptions(@"Unable to Log In", self.errorMessage,
                         [AlertOption optionWithName:@"Dismiss" call:nil], nil);
        [self clear];
    }
    
    [self update];
    
    return (success && self.session != nil);
}

- (BOOL)logoutSynchronous
{
    [self update];
    
    if (!self.session)
        return YES;
    
    NSString *request = [NSString stringWithFormat:
                         
                         @"{\"action\":\"logout\","
                           "\"session\":\"%@\","
                           "\"device\":\"%@\"}",
                         
                         self.session, GetUserUniqueUDID()];
    
    NSError *error = nil;
    BOOL success = [self request:request error:&error];
    if (success)
        [self clear];
    return success;
}

+ (Services *)shared
{
    if (!SHARED)
        SHARED = [[Services alloc] init];
    return SHARED;
}

@end

#endif
