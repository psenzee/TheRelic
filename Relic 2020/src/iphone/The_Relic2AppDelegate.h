#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import <MessageUI/MessageUI.h>
#import <MessageUI/MFMailComposeViewController.h>

#import "CrystalSession.h"
#import "CrystalLogoMoviePlayer.h"

typedef enum {
	NETWORK_ACK,					// no packet
	NETWORK_COINTOSS,				// decide who is going to be the server
	NETWORK_MOVE_EVENT,				// send position
	NETWORK_FIRE_EVENT,				// send fire
	NETWORK_HEARTBEAT				// send of entire state at regular intervals
} packetCodes;

typedef struct {
	CGPoint		tankPreviousPosition;
	CGPoint		tankPosition;
	CGPoint		tankMissilePosition;
	CGPoint		tankDestination;
	
	float		tankRotation;
	float		tankDirection;
	float		tankMissileDirection;
	int			tankMissile;
} InfoPacket;

@class EAGLView;

@interface The_Relic2AppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate, 
                                             GKPeerPickerControllerDelegate, GKSessionDelegate, 
                                             UIAlertViewDelegate,
                                             MFMailComposeViewControllerDelegate, 
                                             CrystalSessionDelegate, 
                                             CrystalLogoMoviePlayerObserver>
{
    UIWindow *window;
    EAGLView *glView;
    
    UIAccelerationValue accel[3];    
	
	NSInteger	     gameState;
	NSInteger	     peerStatus;
	InfoPacket	     infoStats[2];	
	
	// networking
	GKSession		*gameSession;
	int			     gameUniqueID;
	int				 gamePacketNumber;
	NSString		*gamePeerId;
	NSDate			*lastHeartbeatDate;
	
	UIAlertView		*connectionAlert;	

	CrystalLogoMoviePlayer*	_crystalLogoMoviePlayer;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

@property (nonatomic, retain) CrystalLogoMoviePlayer* _crystalLogoMoviePlayer;

@property (nonatomic) NSInteger		gameState;
@property (nonatomic) NSInteger		peerStatus;

@property (nonatomic, retain) GKSession	  *gameSession;
@property (nonatomic, copy)	  NSString	  *gamePeerId;
@property (nonatomic, retain) NSDate	  *lastHeartbeatDate;
@property (nonatomic, retain) UIAlertView *connectionAlert;

- (void)invalidateSession:(GKSession *)session;
- (void)btUpdate;
- (void)startLogoMovieplayer;

@end

