#define WIN32
#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <Windows.h>
#undef _WINSOCK2API_

#include <tchar.h>
#include <mmsystem.h>
#include <strsafe.h>
#include <string>
#include <sstream>
#include "graphics\Util.h"
#include "network\NetClient.h"
#include "game/TileBoard.h"
#include "game/Character.h"
#include <d3dx9.h>


TileBoard* board;
NetClient cli;
Player* thisPlayer;


extern HRESULT InitD3D( HWND hWnd );
extern HRESULT InitGeometry();
extern VOID Render();

// output debug string method
void MyOutputDebugString( LPCTSTR sFormat, ... )
{
	va_list argptr;      
	va_start( argptr, sFormat ); 
	TCHAR buffer[ 3000 ];
	HRESULT hr = StringCbVPrintf( buffer, sizeof( buffer ), sFormat, argptr );	
	if ( STRSAFE_E_INSUFFICIENT_BUFFER == hr || S_OK == hr )
		OutputDebugString( buffer );
	else
		OutputDebugString( _T("StringCbVPrintf error.") );
}

// send packet
void SendPacket (const char* s) {
	if (!cli.SendData(s, strlen(s))) {
		MyOutputDebugString("error sending packet.\n");
		return;
	}
	cli.Flush();
	if (thisPlayer!=NULL)
		MyOutputDebugString("%d - ",thisPlayer->getId());
	MyOutputDebugString("sent packet of data %s.\n",s);
}

void PacketDebug(void* packet) {
	MyOutputDebugString("received packet %s.\n",(char*)packet);
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{   
	case WM_KEYDOWN:
		{
			ostringstream output;
			switch( wParam )
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case VK_LEFT://camera control
				//output << "L\0";
				CameraMove(-5.0, 0.0, 0.0,g_camera);
				break;
			case VK_RIGHT:
				//output << "R\0";
				CameraMove(5.0, 0.0, 0.0,g_camera);
				break;
			case VK_UP:
				//output << "U\0";
				CameraMove(0.0, 0.0, 5.0,g_camera);
				break;
			case VK_DOWN:
				//output << "D\0";
				CameraMove(0.0, 0.0, -5.0,g_camera);
				break;
			case 'A':
				output << "1\0";
				break;
			case 'D':
				output << "4\0";
				break;
			case 'W':
				output << "7\0";
				break;
			case 'S':
				output << "3\0";
				break;				
			case 'R':
				// send ready packet if game hasn't started
				output << "S";
				if (!board->isStarted()) {			
					// if game hasn't started
					if (!thisPlayer->isReady())
						output << "R";
					else
						output << "N";
					output << thisPlayer->getId();
				} 
				output << "\0";
				break;
			case 'P':
				// print current board state
				MyOutputDebugString("%s\n",board->printBoard().c_str());
				output << "SX\0";
				break;
			case '1':
				// buy soldier
				output << "GB" << thisPlayer->getId() << "S\0";
				break;
			case '2':
				// buy archer
				output << "GB" << thisPlayer->getId() << "A\0";
				break;
			case '3':
				// buy paladin
				output << "GB" << thisPlayer->getId() << "P\0";
				break;
			case '4':
				// buy healer
				output << "GB" << thisPlayer->getId() << "H\0";
				break;
			case 'M':	// move character 2 to location 5,5
				output << "GM" << thisPlayer->getId() << "255\0";
				break;
			case 'K':	// move king to 4,4
				output << "GM" << thisPlayer->getId() << "144\0";
				break;
			case VK_OEM_COMMA:
				CamMoveY(1.0,g_camera);
				break;
			case VK_OEM_PERIOD:
				CamMoveY(-1.0,g_camera);
				break;
			default:
				output << "XXXXXX";
				break;
			}
		//	string s = output.str();
			if(output.str()!="") //dont send empty messages
				SendPacket (output.str().c_str());
		}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
	default:
		break;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

void HandlePacket(void* packet, int dataLength)
{
	char* data = (char*) packet;
	int i;
	Player * p;
	Character *c;
	ostringstream errorMsg;

	switch( data[0] )
	{
	case 'C': // connection packets
		i = data[1] - '0';
		if (thisPlayer == NULL) {
			if (i!='X') {
				for (int n=0; n<i; n++)
					thisPlayer = board->addPlayer();
				MyOutputDebugString ("Client initialized as player %d.\n",thisPlayer->getId());
			} else
				MyOutputDebugString ("Server board full. Unable to join game.\n");
		} else {
			if (i!='X') {
				for (int n=board->getLastPlayer(); n<i; n++) {
					p = board->addPlayer();
					MyOutputDebugString ("%d - Added player %d to board.\n",thisPlayer->getId(),p->getId());
				}
			} 
		}
		break;

	case 'S': // status packets
		i = data[2] - '0';
		if ((p = board->getPlayerById(i)) == NULL && data[1] != 'G') {
			MyOutputDebugString("%d - Error finding player %d.\n",thisPlayer->getId(),i);
			errorMsg << "Error - could not find player " << i;
			SendPacket (errorMsg.str().c_str());
			return;
		}
		switch (data[1]) {
		case 'R':	/* ready packet */
			p->isReady(true);				
			MyOutputDebugString ("%d - Player %d set as ready.\n",thisPlayer->getId(),i);
			break;
		case 'N':	/* not ready packet */
			p->isReady(false);				
			MyOutputDebugString ("%d - Player %d set as not ready.\n",thisPlayer->getId(),i);
			break;
		case 'D':	/*disconnect packet */
			PacketDebug(packet);
			if (board->isStarted()) {
				p->isDefeated(true);
				MyOutputDebugString("%d - Player %d disconnected. Signalled as lost.\n",thisPlayer->getId(),p->getId());
			} else {
				// remove player from play list
				board->removePlayer(p->getId());
				MyOutputDebugString("%d - Player %d disconnected. Removed from player list.\n",thisPlayer->getId(),p->getId());
			}
			break;
		case 'G':
			// game start packet
			if (board->startBoard())
				MyOutputDebugString ("%d - Game started.\n",thisPlayer->getId());
			else
				MyOutputDebugString ("%d - Error starting game.\n",thisPlayer->getId());
			break;
		default:
			//MyOutputDebugString ("%d - %s", thisPlayer->getId(), board->printBoard().c_str());
			break;
		}
		break;

	case 'G': // game packet	
		i = data[2] - '0';
		if ((p = board->getPlayerById(i)) == NULL) {
			MyOutputDebugString("%d - Error finding player %d.\n",thisPlayer->getId(),i);
			errorMsg << "Error - could not find player " << i;
			SendPacket (errorMsg.str().c_str());
			return;
		}
		switch (data[1]) {
		case 'B':
			;
			break;
		case 'M':
			if ((c = p->getCharacter(data[3]-'0')) != NULL) {
				board->setCharacterDest(c, data[4]-'0', data[5]-'0');
			} else {
				MyOutputDebugString ("character %d was not found!\n",data[3]-'0');
				errorMsg << "Error - character "<< data[3]-'0' << " for player " << i << " was not found";
				SendPacket (errorMsg.str().c_str());
				return;
			}
			break;
		case 'S':
			break;
		}
		break;

	case 'T':
		switch (data[1]) {	
		case 'H': // update hexbanks
			board->incHexBank();
			break;
		case 'C': // run cooldowns;
			board->minusCooldown();
			break;
		}
		break;

	case 'W': // winner detected
		MyOutputDebugString ("%d - Winner detected.\n",thisPlayer->getId());
		break;

	case 'L':	/* update character locations */
		if (!board->updateLocationsOnClient(++data)) {
			MyOutputDebugString("Character location update failed!\n");
		}
		break;

	case 'R':
		//c1.gCoord.xMove += 5;
		break;

	case 'U':
		//c1.gCoord.zMove += 5;
		break;

	case 'D':
		//c1.gCoord.zMove -= 5;
		break;

	case '1':
		//c2.gCoord.xMove -= 5;
		break;

	case '4':
		//c2.gCoord.xMove += 5;
		break;

	case '7':
		//c2.gCoord.zMove += 5;
		break;

	case '3':
		//c2.gCoord.zMove -= 5;
		break;
	}
}

bool HandleConnect()
{

	// request player ID packet
	SendPacket ("CX\0");
	// request ready player list
	SendPacket ("SRX\0");
	// Yay, we get to play!
	return true;
}

void HandleDisconnect(int reason)
{
	// Do something.
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	cli.RegisterHandlers(&HandleConnect, &HandleDisconnect, &HandlePacket);
	if (cli.Initialize() != 0)
	{
		exit (EXIT_FAILURE);
	}

	// This chunk of code makes the Master server system get used
	/*cli.FetchServerList();
	while(!cli.ReceivedServerList()) {
	cli.Poll();
	}
	server_list_item * server = cli.GetNextServer();
	if (server->address == NULL)
	server->address = "127.0.0.1";
	if (!cli.Connect(server->address, server->port))
	{
	exit (EXIT_FAILURE);
	}*/

	// Get rid of this once GUI for server selection is ready
	if (!cli.Connect("127.0.0.1", 4000))
	{
		exit (EXIT_FAILURE);
	}

	UNREFERENCED_PARAMETER( hInst );

	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		"HexSoft", NULL
	};
	RegisterClassEx( &wc );

	// Create the application's window
	HWND hWnd = CreateWindow( "HexSoft", "HexSoft",
		WS_OVERLAPPEDWINDOW, 100, 100, 640, 480,
		NULL, NULL, wc.hInstance, NULL );
	
	//gamelogic init


	// Initialize Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		// Create the scene geometry
		if( SUCCEEDED( InitGeometry() ) )
		{
			if (board == NULL)
				board = new TileBoard();
//			Player* p =board->addPlayer();
//			board->addCharacter(p,king,false);
			// Show the window
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

			// Enter the message loop
			MSG msg;
			ZeroMemory( &msg, sizeof( msg ) );
			while( msg.message != WM_QUIT )
			{
				cli.Poll();
				if(PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				{
					//RENDER
					Render();
				}

			}
		}
	}

	cli.Disconnect(1337);
	cli.Deinitialize();	
	free(board);
	free(thisPlayer);
	UnregisterClass( "D3D Tutorial", wc.hInstance );
	return 0;
}

/*int main() {
cli.RegisterHandlers(&HandleConnect, &HandleDisconnect, &HandlePacket);
if (cli.Initialize() != 0)
{
return EXIT_FAILURE;
}

cli.FetchServerList();

while(!cli.ReceivedServerList()) {
cli.Poll();
}

char * server_name = cli.GetNextServer();
while (server_name != NULL)
{
printf("%s\n", server_name);
server_name = cli.GetNextServer();
}
server_name = "127.0.0.1";

if (!cli.Connect("127.0.0.1", 4000))
{
exit (EXIT_FAILURE);
}

while(true)
cli.Poll();

cli.Disconnect(1337);
cli.Deinitialize();
return 0;
}*/