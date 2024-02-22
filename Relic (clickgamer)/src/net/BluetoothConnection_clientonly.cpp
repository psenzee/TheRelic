#include "BluetoothConnection_clientonly.h"
#include "luautil/LuaInterpreter.h"
#include "luautil/LuaCall.h"

#include <stdio.h> // printf
#include <string.h> // memset

#include "time/Timer.h"

typedef void (*OnCancelCallback)(void *user);
typedef void (*OnPickerConnectCallback)(void *user);

extern "C" void StartBluetoothPicker();
extern "C" void SendBluetoothData(const void *data, int length, bool reliable);
extern "C" void SetBluetoothOnReceiveCallback(Connection::OnReceiveCallback callback, void *user);
extern "C" void SetBluetoothOnCancelCallback(OnCancelCallback callback, void *user);
extern "C" void SetBluetoothOnPickerConnectCallback(OnPickerConnectCallback callback, void *user);

namespace Connection
{

BluetoothClientConnection::BluetoothClientConnection(BluetoothClientConnector *connector) 
    :  mId(0), mClientConnector(connector), mReceiveCallback(0), mReceiveUser(0), 
       mDisconnectCallback(0), mDisconnectUser(0)
{
    SetBluetoothOnReceiveCallback(OnBluetoothReceiveCallback, this);
}

BluetoothClientConnection::~BluetoothClientConnection()
{
    Close();
}

bool BluetoothClientConnection::Close()
{
    // $TODO
    return false;
}

enum { TOTAL_TIME = 5 };

static void ReportSentBandwidth(int bytes)
{
    static unsigned total = 0, lastTime = GetCurrentTimeMs();
    total += bytes;
    if (GetCurrentTimeMs() > lastTime + TOTAL_TIME * 1000)
    {
        lastTime = GetCurrentTimeMs();
        printf("Data rate (bytes sent) last %d seconds = %.2fkbps\n", TOTAL_TIME, total * 8 / static_cast<float>(TOTAL_TIME * 1024));
        total = 0;
    }
}

static void ReportReceivedBandwidth(int bytes)
{
    static unsigned total = 0, lastTime = GetCurrentTimeMs();
    total += bytes;
    if (GetCurrentTimeMs() > lastTime + TOTAL_TIME * 1000)
    {
        lastTime = GetCurrentTimeMs();
        printf("Data rate (bytes received) last %d seconds = %.2fkbps\n", TOTAL_TIME, total * 8 / static_cast<float>(TOTAL_TIME * 1024));
        total = 0;
    }
}

bool BluetoothClientConnection::Send(const char *data, int length, bool reliable, bool sequenced)
{
    SendBluetoothData(data, length, reliable);
    ReportSentBandwidth(length);
    return true;
}

void BluetoothClientConnection::OnBluetoothReceiveCallback(int connectionid, const void *data, int length, void *user)
{
	BluetoothClientConnection *connection = static_cast<BluetoothClientConnection *>(user);
	if (connection && connection->mReceiveCallback)
	{
        ReportReceivedBandwidth(length);		
		connection->mReceiveCallback(connectionid, data, length, connection);
	}
}	

void BluetoothClientConnection::Process()
{
    // OnConnect
    /*
    mClientConnector->ConnectInternal(this);
    */

    // OnReceive
    /*
    ReportReceivedBandwidth(dataLength);
    if (mReceiveCallback)
        mReceiveCallback(mId, data, dataLength, mReceiveUser);
    */

    // OnDisconnect
    /*
    if (mDisconnectCallback)
        mDisconnectCallback(mId, 0, mDisconnectUser);
    */
}

void BluetoothClientConnector::ConnectInternal(BluetoothClientConnection *connection)
{
    if (mConnectCallback)
        mConnectCallback(connection, mConnectCallbackUser);
}

void BluetoothClientConnector::SetOnConnect(OnConnectCallback callback, void *user)
{
    mConnectCallback = callback;
    mConnectCallbackUser = user;
}

void BluetoothClientConnector::SetOnAbortConnect(OnAbortConnectCallback callback, void *user)
{
    mAbortConnectCallback = callback;
    mAbortConnectCallbackUser = user;
}

void BluetoothClientConnector::Process()
{/*
    BluetoothPeerConnection *connection = 0;	
	switch (mState)
{
	case CONNECTING:
		connection = CreatePeerConnection();
		if (mConnectCallback)
			mConnectCallback(connection, mConnectCallbackUser);			
		mState = CONNECTED;
    	break;
	case PICKER_CANCELLED:
		connection = GetPeerConnection();
		if (connection)
			connection->DisconnectInternal();			
		mState = UNCONNECTED;
		break;
	}*/	
}

bool BluetoothClientConnector::Connect()
{
    BluetoothClientConnection *connection = new BluetoothClientConnection(this);
    connection->SetId(ConnectionIdAssigner::GetNewId());
/* $TODO connect bt
    if (!connection->Connect(mAddress.c_str(), mPort))
        return false;
*/
    StartBluetoothPicker();
// $TODO, Queue up this callback for Process
//  if (mConnectCallback)
//      mConnectCallback(connection, mConnectCallbackUser);
    return true;
}

void BluetoothServerConnector::ConnectInternal(BluetoothClientConnection *connection)
{
    if (mConnectCallback)
        mConnectCallback(connection, mConnectCallbackUser);
}

void BluetoothServerConnector::SetOnConnect(OnConnectCallback callback, void *user)
{
    mConnectCallback = callback;
    mConnectCallbackUser = user;
}

void BluetoothServerConnector::SetOnAbortConnect(OnAbortConnectCallback callback, void *user)
{
    mAbortConnectCallback = callback;
    mAbortConnectCallbackUser = user;
}

void BluetoothServerConnector::Process()
{/*
    BluetoothPeerConnection *connection = 0;	
	switch (mState)
{
	case CONNECTING:
		connection = CreatePeerConnection();
		if (mConnectCallback)
			mConnectCallback(connection, mConnectCallbackUser);			
		mState = CONNECTED;
    	break;
	case PICKER_CANCELLED:
		connection = GetPeerConnection();
		if (connection)
			connection->DisconnectInternal();			
		mState = UNCONNECTED;
		break;
	}*/	
}

void BluetoothServerConnector::Start()
{
    BluetoothClientConnection *connection = new BluetoothClientConnection(0/*this*/);
    connection->SetId(ConnectionIdAssigner::GetNewId());
/* $TODO connect bt
    if (!connection->Connect(mAddress.c_str(), mPort))
        return false;
*/
    StartBluetoothPicker();
// $TODO, Queue up this callback for Process
//  if (mConnectCallback)
//      mConnectCallback(connection, mConnectCallbackUser);
}

void BluetoothClientConnection::SetOnReceive(OnReceiveCallback callback, void *user)
{ 
    mReceiveCallback = callback;
    mReceiveUser = user;
}

void BluetoothClientConnection::SetOnDisconnect(OnDisconnectCallback callback, void *user)
{ 
    mDisconnectCallback = callback;
    mDisconnectUser = user;
}

}