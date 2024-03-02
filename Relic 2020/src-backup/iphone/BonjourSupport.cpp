#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <assert.h>

#include "luautil/LuaInterpreter.h"
#include "luautil/LuaCall.h"

#include <CFNetwork/CFNetServices.h>

CFStreamError          gError;
volatile CFNetServiceBrowserRef gServiceBrowserRef = NULL;
volatile CFNetServiceRef        gServiceBeingResolved = NULL;
volatile CFSocketRef            gListeningSocket = NULL;

static void MyResolveService(CFNetServiceRef resolve);
static void MyStopBrowsingForServices();
void MyCancelResolve();

void RegisterCallback(CFNetServiceRef theService, CFStreamError *error, void *info)
{
	if (!error || !error->error)
	{	
	    printf("REGISTER\n");
	}
}

enum BonjourConnectionState { BROWSING_NONE, BROWSING_INPROGRESS, BROWSING_FOUND, BROWSING_COMPLETE, BROWSING_CALLBACK_COMPLETE };

char           gBonjourAddress[128] = "";
unsigned short gBonjourPort = 0;
volatile bool  gBonjourDoneBrowsing = false;
volatile bool  gBonjourIsComplete = false;
volatile BonjourConnectionState gBonjourConnectionState = BROWSING_NONE;

extern "C" void UpdateBonjourConnections()
{
	if (gBonjourConnectionState == BROWSING_FOUND)
	{
	//	sleep(1);		
        MyCancelResolve();
		MyStopBrowsingForServices();
	//	sleep(1);		
		gBonjourConnectionState = BROWSING_COMPLETE;
	}
	else if (gBonjourConnectionState == BROWSING_COMPLETE)
	{
		LuaValue values[] = {  LuaValue::_String(gBonjourAddress), LuaValue::Int(gBonjourPort) };
		LuaCall(LuaInterpreter::GetInstance()->GetState(), "OnReceiveAddress", values, 2);
        gBonjourConnectionState = BROWSING_CALLBACK_COMPLETE;
	}
}

void MyResolveCallback(CFNetServiceRef theService, CFStreamError *error, void *info)
{
	if (gBonjourConnectionState != BROWSING_INPROGRESS)
		return;
	bool done = false;
	if (!error || !error->error)
	{
	    printf("RESOLVE\n");
	    // i think this means we're done
	    CFArrayRef addresses = CFNetServiceGetAddressing(gServiceBeingResolved);
	    long count = CFArrayGetCount(addresses);
	    for (long i = 0; i < count; i++)
        {
            sockaddr ipaddress;
            sockaddr_in *ipin = (sockaddr_in *)&ipaddress;
            CFDataRef data = (CFDataRef)CFArrayGetValueAtIndex(addresses, i);
            if (!data)
                continue;
		    CFDataGetBytes(data, CFRangeMake(0, sizeof(sockaddr) - 1), (unsigned char *)&ipaddress);
		    unsigned int   ip   = *(uint32_t *)&(ipin->sin_addr);
            gBonjourPort = *(uint32_t *)&(ipin->sin_port);
	        ip = ntohl(ip); gBonjourPort = ntohs(gBonjourPort);
		    printf("%u.%u.%u.%u:%u\n", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff, gBonjourPort);
			snprintf(gBonjourAddress, 16, "%u.%u.%u.%u", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
		    done = true;
	    }
    }
	if (done)
	{
//		MyStopBrowsingForServices();
//        MyCancelResolve();
//		sleep(1);		
//		MyStopBrowsingForServices();
		gBonjourConnectionState = BROWSING_FOUND;
	}
}

void MyBrowseCallBack(CFNetServiceBrowserRef browser, CFOptionFlags flags, CFTypeRef domainOrService, CFStreamError *error, void *info)
{
	printf("This is the BROWSE callback, don't know what it's supposed to do.. %d %p : ", error->error, info);
	CFShow(domainOrService);
    MyResolveService((CFNetServiceRef)domainOrService);
	printf("\n");
// ..	
}

void StartBonjour(CFNetServiceRef netService)
{
	CFStreamError error;
	CFNetServiceClientContext clientContext = { 0, NULL, NULL, NULL, NULL };
	
	CFNetServiceSetClient(netService, RegisterCallback, &clientContext);
	CFNetServiceScheduleWithRunLoop(netService, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
	//CFNetServiceRegisterWithOptions(netService, 0, NULL);
	if (CFNetServiceRegisterWithOptions(netService, 0, &error) == false)
//	if (false)
	{
//		CFNetServiceUnscheduleFromRunLoop(netService, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
		CFNetServiceSetClient(netService, NULL, NULL);
		CFRelease(netService);
		fprintf(stderr, "could not register Bonjour service");
	}
}

static Boolean MyStartBrowsingForServices(CFStringRef domain, CFStringRef type)
{
	CFNetServiceClientContext clientContext = { 0, NULL, NULL, NULL, NULL };
	CFStreamError error;
	Boolean result;
	
	assert(type != NULL);
	
	gServiceBrowserRef = CFNetServiceBrowserCreate(kCFAllocatorDefault, MyBrowseCallBack, &clientContext);
	assert(gServiceBrowserRef != NULL);
	gBonjourConnectionState = BROWSING_INPROGRESS;
	CFNetServiceBrowserScheduleWithRunLoop(gServiceBrowserRef, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
	
	result = CFNetServiceBrowserSearchForServices(gServiceBrowserRef, domain, type, &error);
	if (result == false)
	{
		
		// Something went wrong so lets clean up.
		CFNetServiceBrowserUnscheduleFromRunLoop(gServiceBrowserRef, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);         
		CFRelease(gServiceBrowserRef);
		gServiceBrowserRef = NULL;
		
		fprintf(stderr, "CFNetServiceBrowserSearchForServices returned (domain = %ld (%d), error = %d)\n", (long)error.domain, (int)kCFStreamErrorDomainNetServices, error.error);
	}

	return result;
}

static void MyResolveService(CFStringRef name, CFStringRef type, CFStringRef domain)
{
    CFNetServiceClientContext context = { 0, NULL, NULL, NULL, NULL };
    CFTimeInterval duration = 0; // use infinite timeout
    CFStreamError error;
	
    gServiceBeingResolved = CFNetServiceCreate(kCFAllocatorDefault, domain, type, name, 0);
    assert(gServiceBeingResolved != NULL);
	
    CFNetServiceSetClient(gServiceBeingResolved, MyResolveCallback, &context);
    CFNetServiceScheduleWithRunLoop(gServiceBeingResolved, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
	
    if (CFNetServiceResolveWithTimeout(gServiceBeingResolved, duration, &error) == false) {
		
		// Something went wrong so lets clean up.
		CFNetServiceUnscheduleFromRunLoop(gServiceBeingResolved, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
		CFNetServiceSetClient(gServiceBeingResolved, NULL, NULL);
		CFRelease(gServiceBeingResolved);
		gServiceBeingResolved = NULL;
		
		fprintf(stderr, "CFNetServiceResolve returned (domain = %d, error = %ld)\n", (int)error.domain, (long)error.error);
	}
}

static void MyResolveService(CFNetServiceRef resolve)
{
    CFNetServiceClientContext context = { 0, NULL, NULL, NULL, NULL };
    CFTimeInterval duration = 0; // use infinite timeout
    CFStreamError error;
	
    gServiceBeingResolved = resolve;
    assert(gServiceBeingResolved != NULL);
	
    CFNetServiceSetClient(gServiceBeingResolved, MyResolveCallback, &context);
    CFNetServiceScheduleWithRunLoop(gServiceBeingResolved, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
	
    if (CFNetServiceResolveWithTimeout(gServiceBeingResolved, duration, &error) == false) {
		
		// Something went wrong so lets clean up.
		CFNetServiceUnscheduleFromRunLoop(gServiceBeingResolved, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
		CFNetServiceSetClient(gServiceBeingResolved, NULL, NULL);
		CFRelease(gServiceBeingResolved);
		gServiceBeingResolved = NULL;
		
		fprintf(stderr, "CFNetServiceResolve returned (domain = %d, error = %ld)\n", (int)error.domain, (long)error.error);
	}
}

void MyCancelResolve()
{
	if (gServiceBeingResolved)
	{
        assert(gServiceBeingResolved != NULL);
        CFNetServiceUnscheduleFromRunLoop(gServiceBeingResolved, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
     //   CFNetServiceSetClient(gServiceBeingResolved, NULL, NULL);
     //   CFNetServiceCancel(gServiceBeingResolved);
	 //   CFRelease(gServiceBeingResolved);
        gServiceBeingResolved = NULL;
	}
}

static void MyStopBrowsingForServices()
{
	if (gServiceBrowserRef)
	{
        assert(gServiceBrowserRef != NULL);
   //     CFNetServiceBrowserStopSearch(gServiceBrowserRef, &gError);
        CFNetServiceBrowserUnscheduleFromRunLoop(gServiceBrowserRef, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
    //    CFNetServiceBrowserInvalidate(gServiceBrowserRef);
   //     CFRelease(gServiceBrowserRef);
        gServiceBrowserRef = NULL;
	}
}

extern "C" void StartBonjour()
{
    CFNetServiceRef netService = CFNetServiceCreate(kCFAllocatorDefault, CFSTR(""), CFSTR("_relicv1._udp."), CFSTR("The Relic (v1.0)"), 0xc849 /* 18888 */);
	if (!netService)
	{
		printf("Unable to register bonjour service!\n");
	}
	else
	{
		printf("Start bonjour\n");
		StartBonjour(netService);
	}
}

extern "C" void StartWiFiPicker()
{
    printf("Start browsing\n");	
    MyStartBrowsingForServices(CFSTR(""), CFSTR("_relicv1._udp."));	
}
