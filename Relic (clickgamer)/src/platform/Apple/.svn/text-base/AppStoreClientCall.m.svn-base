//
//  SoapClientCall.m
//  Hello_SOAP
//
//  Created by pucho g on 4/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AppStoreClientCall.h"


//static NSMutableData *webData;

static NSMutableString *soapResults;
static NSXMLParser *xmlParser;
static BOOL *recordResults;
static id _delegate;
static NSURLConnection *theConnection;
static UIActivityIndicatorView *activityView;	
static UIAlertView * progressAlert;

@implementation AppStoreClientCall


//@synthesize  webData,soapResults, xmlParser;

+ (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
	
	if(alertView == progressAlert && buttonIndex == alertView.cancelButtonIndex){		
		[AppStoreClientCall cancelSoapRequest];
	}
}

+ (void)alertViewCancel:(UIAlertView *)alertView{
	
	[AppStoreClientCall cancelSoapRequest];
}

+ (void)callIGPEnded:(NSString *)URL{
	[progressAlert dismissWithClickedButtonIndex:-1 animated:TRUE];
	[progressAlert release];
	
	if (URL != nil && [URL length])
	{
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:URL]];
	} else {
		UIAlertView *uiA = [[UIAlertView alloc] 
							initWithTitle: @"No internet/WIFI connection"  
							message:[NSString stringWithFormat:@"Unable to connect. Please check your network settings"]  
							delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];  
		[uiA show];  
	}
	
}


+(void)appStoreUrlServiceCall: (NSString *) fromGameCode wantedGame:(NSString *)wantedGame delegate:(id)delegate{
	
	progressAlert = [[UIAlertView alloc] initWithTitle: @"Retrieving Data..."
															 message: @"\n"
															delegate: self
												   cancelButtonTitle: @"Cancel"
												   otherButtonTitles: nil];
	
	
	if( activityView == nil ){
		activityView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
		activityView.frame = CGRectMake(139.0f-18.0f, 40.0f, 37.0f, 37.0f);
		
	}
	
	[progressAlert addSubview:activityView];
	[activityView startAnimating];
	
	[progressAlert show];
	
	
	_delegate=self;
	recordResults = FALSE;
	
	NSString *soapMessage = [NSString stringWithFormat:
							 @"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
							 "<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
							 "<soap:Body>\n"
							 "<getAppStoreURL xmlns=\"http://www.gamesmithstudios.com/gsadvert/\">\n"
							 "<from>%@</from>\n"
							 "<gameWanted>%@</gameWanted>\n"
							 "</getAppStoreURL>\n"
							 "</soap:Body>\n"
							 "</soap:Envelope>\n", fromGameCode, wantedGame];
	NSLog(soapMessage);
	
	NSURL *url = [NSURL URLWithString:@"http://www.gamesmithstudios.com/gsadvert/services.php"];
	NSMutableURLRequest *theRequest = [NSMutableURLRequest requestWithURL:url];
	NSString *msgLength = [NSString stringWithFormat:@"%d", [soapMessage length]];
	
	[theRequest addValue: @"text/xml; charset=utf-8" forHTTPHeaderField:@"Content-Type"];
	[theRequest addValue: @"http://viium.com/Hello" forHTTPHeaderField:@"SOAPAction"];
	[theRequest addValue: msgLength forHTTPHeaderField:@"Content-Length"];
	[theRequest setHTTPMethod:@"POST"];
	[theRequest setHTTPBody: [soapMessage dataUsingEncoding:NSUTF8StringEncoding]];
	
	theConnection = [[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	
	if( theConnection )
	{
		appStoreWebData = [[NSMutableData data] retain];
	}
	else
	{
		NSLog(@"theConnection is NULL");
	}
}

+(void)cancelSoapRequest{	
	[theConnection cancel];
}

+(void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	[appStoreWebData setLength: 0];
}

+(void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	[appStoreWebData appendData:data];
	NSString *str = [[NSString alloc] initWithBytes: [appStoreWebData mutableBytes] length:[appStoreWebData length] encoding:NSUTF8StringEncoding];
    printf("\n\nSOAP RESULTS: '%s'\n", [str UTF8String]);
	[str release];	
}

+(void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	NSLog(@"ERROR with theConenction");
	[_delegate callIGPEnded:@""];
	[connection release];
	[appStoreWebData release];
}

+(void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	
	NSLog(@"DONE. Received Bytes: %d", [appStoreWebData length]);
	NSString *theXML = [[NSString alloc] initWithBytes: [appStoreWebData mutableBytes] length:[appStoreWebData length] encoding:NSUTF8StringEncoding];
	NSLog(theXML);
	[theXML release];
	
	if (xmlParser)
	{
		[xmlParser release];
	}
	
	xmlParser = [[NSXMLParser alloc] initWithData: appStoreWebData];
	[xmlParser setDelegate: self];
	[xmlParser setShouldResolveExternalEntities: YES];
	[xmlParser parse];
	
	[connection release];
	[appStoreWebData release];
}



+(void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *) namespaceURI qualifiedName:(NSString *)qName
   attributes: (NSDictionary *)attributeDict
{
	if( [elementName isEqualToString:@"return"])
	{
		if (!soapResults)
		{
			soapResults = [[NSMutableString alloc] init];
		}
		recordResults = TRUE;
	}
}
+(void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string
{
	if( recordResults )
	{
		[soapResults appendString: string];
	}
}
+(void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
{
	if ([elementName isEqualToString:@"return"])
	{
		recordResults = FALSE;
		[_delegate callIGPEnded:soapResults];
		
		[soapResults release];
		soapResults = nil;
	}
}
@end
