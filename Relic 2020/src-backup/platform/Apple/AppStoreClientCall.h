//
//  SoapClientCall.h
//  Hello_SOAP
//
//  Created by pucho g on 4/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

NSMutableData *appStoreWebData;
/*
static NSMutableString *soapResults;
static NSXMLParser *xmlParser;
static BOOL *recordResults;
static id _delegate;
static NSURLConnection *theConnection;
static UIActivityIndicatorView *activityView;    
static UIAlertView * progressAlert;
 */

@interface AppStoreClientCall : NSObject {
    
    
}

/*@property(nonatomic, retain) NSMutableData *webData;
@property(nonatomic, retain) NSMutableString *soapResults;
@property(nonatomic, retain) NSXMLParser *xmlParser;
*/
+(void)appStoreUrlServiceCall: (NSString *) fromGameCode wantedGame:(NSString *)wantedGame delegate:(id)delegate;
+(void)cancelSoapRequest;

@end
