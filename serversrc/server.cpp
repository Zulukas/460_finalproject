#include <netinet/in.h>		//needed for sockets
#include <pthread.h>		//threading capability
#include <string.h>			//bzero
#include <sys/types.h>		//needed for sockets
#include <sys/socket.h>		//needed for sockets
#include <unistd.h>			//usleep

#include <cassert>
#include <cstdlib>			//atoi
#include <string>
#include <sstream>
#include <iostream>

#include "checkers.hpp"

using namespace std;

#define BUFFER_LENGTH 1024

struct Server
{
	int socketFD;
	int portNumber;

	socklen_t socketLength;

	struct sockaddr_in address;
} server;

class Client
{
public:
	bool occupied = false;
	struct sockaddr_in address;
	int socketFD;
	pthread_t thread;
	int threadNum;

	int client_write(const string msg)
	{
		return write(socketFD, msg.c_str(), msg.length() + 1);
	}

	string client_read()
	{
		char buffer[32];

		read(socketFD, buffer, 32);

		return string(buffer);
	}
};

struct Game
{
	Client white;
	Client red;
	bool whitePlayerTurn;
	Checkers checkers;
} game;

Location strToLoc(const string locstr)
{
	if (locstr.size() != 2)
	{
		Location loc;
		loc.y = 255;
		loc.x = 255;

		return loc;
	}

	int row = (uint8_t)toupper(locstr[0]);
	int col = (uint8_t)toupper(locstr[1]);

	Location loc;

	loc.y = (row - 65);
	loc.x = (col - 49);

	if (loc.y < 0 || loc.y > 7)
		loc.y = 255;
	if (loc.x < 0 || loc.x > 7)
		loc.x = 255;

	return loc;
}

// void *handleWhitePlayer(void* param)
// {
// 	game.white.client_write("WHITE");
//
// 	bool subsequentAction = false;
//
// 	for (;;)
// 	{
// 		while (!game.whitePlayerTurn)
// 		{
// 			usleep(100000); //Poll every 100ms
// 			continue;
// 		}
//
// 		game.white.client_write(game.checkers.getGridData());
// 		string reply = game.white.client_read();
//
// 		cout << "\tSERVER DEBUG: AWAITING \"RECEIVED\"\n";
// 		assert(reply == "RECEIVED");
// 		cout << "\tSERVER DEBUG:  \"RECEIVED\" CONFIRMED\n";
//
// 		game.white.client_write(game.lastMove);
//
// 		if (!subsequentAction)
// 		{
// 			game.lastMove = "";
// 		}
//
// 		//If it's the white player's turn, read then write to client
// 		string msg = game.white.client_read();
//
// 		//Parse the command
// 		stringstream ss(msg);
//
// 		string cmd;
// 		ss >> cmd;	//Grab the first word
//
// 		if (cmd == "move" || cmd == "jump")
// 		{
// 			//Parse the locations
// 			string tmp;
//
// 			ss >> tmp;
//
// 			Location src = strToLoc(tmp);
//
// 			if (src.x == 255 || src.y == 255)
// 			{
// 				game.white.client_write("ACTION_INVALID");
// 				continue;
// 			}
//
// 			ss >> tmp;
//
// 			Location dst = strToLoc(tmp);
//
// 			if (dst.x == 255 || dst.y == 255)
// 			{
// 				game.white.client_write("ACTION_INVALID");
// 				continue;
// 			}
//
// 			if (cmd == "move")
// 			{
// 				game.checkers.move(src, dst);
// 				game.whitePlayerTurn = false;
//
// 				game.white.client_write("Successful move action! Please wait for your opponent to make their move.");
//
// 				subsequentAction = false;
// 				game.lastMove += "White played moved " + locToStr(src) + " to " + locToStr(dst) + ".";
// 			}
//
// 			if (cmd == "jump")
// 			{
// 				game.checkers.jump(src, dst);
// 				game.white.client_write("Successful jump action! Please make another action.");
//
// 				subsequentAction = true;
// 				game.lastMove += "White player jumped " + locToStr(src) + " over " + locToStr(dst) + "; ";
// 			}
//
// 		}
// 		else if (cmd == "quit") //Player has requested to disconnect
// 		{
// 			game.white.client_write("DISCONNECT");
//
// 			close(game.white.socketFD);		//close the socket
// 			game.white.occupied = false;	//Open up the socket to new players
//
// 			pthread_exit(&game.white.threadNum); //Kill the thread
// 		}
// 		else //Bad command
// 		{
// 			game.white.client_write("COMMAND_INVALID");
// 		}
// 	}
// }
//
// void *handleRedPlayer(void* param)
// {
// 	game.white.client_write("RED");
//
// 	for (;;)
// 	{
// 		while (game.whitePlayerTurn)
// 		{
// 			usleep(100000); //Poll every 100ms
// 			continue;
// 		}
//
// 		game.red.client_write(game.checkers.getGridData());
//
// 		//If it's the white player's turn, read then write to client
// 		string msg = game.red.client_read();
//
// 		//Parse the command
// 		stringstream ss(msg);
//
// 		string cmd;
// 		ss >> cmd;	//Grab the first word
//
// 		if (cmd == "move" || cmd == "jump")
// 		{
// 			//Parse the locations
// 			string tmp;
//
// 			ss >> tmp;
//
// 			Location src = strToLoc(tmp);
//
// 			if (src.x == 255 || src.y == 255)
// 			{
// 				game.red.client_write("ACTION_INVALID");
// 				continue;
// 			}
//
// 			ss >> tmp;
//
// 			Location dst = strToLoc(tmp);
//
// 			if (dst.x == 255 || dst.y == 255)
// 			{
// 				game.red.client_write("ACTION_INVALID");
// 				continue;
// 			}
//
// 			if (cmd == "move")
// 				game.checkers.move(src, dst);
//
// 			if (cmd == "jump")
// 				game.checkers.jump(src, dst);
//
// 			game.whitePlayerTurn = true;
// 		}
// 		else if (cmd == "quit") //Player has requested to disconnect
// 		{
// 			game.red.client_write("DISCONNECT");
//
// 			close(game.white.socketFD);		//close the socket
// 			game.red.occupied = false;	//Open up the socket to new players
//
// 			pthread_exit(&game.red.threadNum); //Kill the thread
// 		}
// 		else //Bad command
// 		{
// 			game.red.client_write("COMMAND_INVALID");
// 		}
// 	}
// }

void init(int portNumber) throw (const string)
{
	cout << "SERVER: Initializing Server Socket...\n";
	//Initialize the socket
	server.socketFD = socket(AF_INET, SOCK_STREAM, 0);

	if (server.socketFD < 0)
	{
		throw string("ERROR: Socket failed to open.");
	}

	//Clear the server address buffer
	bzero((char*) &server.address, sizeof(server.address));

	//Set the port number of this server
	server.portNumber = portNumber;

	//Set the server address object
	server.address.sin_family = AF_INET;
	server.address.sin_addr.s_addr = INADDR_ANY;
	server.address.sin_port   = htons(server.portNumber);

	//Bind the socket
	if (bind(server.socketFD,
		     (struct sockaddr *) &server.address,
		     sizeof (server.address)) < 0)
	{
		throw string("ERROR: Socket failed to bind.");
	}

	cout << "SERVER: Socket is ready.\n";

	game.white.occupied = false;
	game.red.occupied = false;
	game.whitePlayerTurn = true;
}

void server_accept()
{
	cout << "SERVER: Listening for connections...\n";

	listen(server.socketFD, 5);

	server.socketLength = sizeof(server.address);

	//WHITE PLAYER CONNECTION//

	cout << "Waiting for white player...\n\n";

	//Wait for white
	game.white.socketFD = accept(server.socketFD,
							 (struct sockaddr*) &game.white.address,
							 &server.socketLength);

	cout << "White player connected.\n";

	game.white.client_write("WHITE");
	string reply = game.white.client_read();
	assert(reply == "RECEIVED");

	//RED PLAYER CONNECTION//

	cout << "\nWaiting for red player...\n\n";

	//Wait for red
	game.red.socketFD = accept(server.socketFD,
							 (struct sockaddr*) &game.red.address,
							 &server.socketLength);

	cout << "Red player connected.\n";

	game.red.client_write("RED");
	reply = game.red.client_read();
	assert(reply == "RECEIVED");

	//GAME CAN NOW START//
}

void play_game()
{
	string reply = "";
	string lastMove = "";

	bool invalidCommand = false;

	//Lambda expression to handle a commonly recurring idea
	//Could have been done with a function I suppose, but lambdas are cooler :-)
	auto send_n_receive = [](string messageToSend)
	{
		cout << "SENDING: \"" << messageToSend << "\"\n";

		//Send the message
		(game.whitePlayerTurn) ?
			(game.white.client_write(messageToSend))
			:
			(game.red.client_write(messageToSend));

		//Receive the response
		string reply =  (game.whitePlayerTurn) ?
			(game.white.client_read())
			:
			(game.red.client_read());

		cout << "RECEIVED: \"" << reply << "\" OF LENGTH: " << reply.length() << endl;

		return reply;
	};

	for (;;)
	{
		if (!invalidCommand)
		{
			//Send the board and await acknowledgement...
			reply = send_n_receive(game.checkers.getGridData());

			assert (reply == "DATA RECEIVED");

			//Send the last action(s) and await acknowledgement
			reply = send_n_receive(lastMove);

			assert (reply == "LAST MOVE RECEIVED");
		}

		//Parse action from player...
		reply = send_n_receive("READY");

		stringstream ss(reply);
		string action = reply;

		string part;
		ss >> part;
		string cmd = part;

		if (part != "move" || part != "jump" || part != "quit")
		{
			reply = send_n_receive("INVALID ACTION");
			assert(reply == "ACKNOWLEDGED");

			invalidCommand = true;

			continue;
		}

		if (part == "quit")
		{
			//SPECIAL CASE TO HANDLE LATER
			continue;
		}

		ss >> part; //get src loc

		Location src = strToLoc(part);

		if (src.x > 7 || src.y > 7) //the values of loc can only be unsigned!
		{
			reply = send_n_receive("INVALID ACTION");
			assert(reply == "ACKNOWLEDGED");

			invalidCommand = true;

			continue;
		}

		ss >> part; //get src loc

		Location dst = strToLoc(part);

		if (dst.x > 7 || dst.y > 7) //the values of loc can only be unsigned!
		{
			reply = send_n_receive("INVALID ACTION");
			assert(reply == "ACKNOWLEDGED");

			invalidCommand = true;

			continue;
		}

		if (cmd == "move")
		{
			try
			{
				//Move will throw an exception if invalid
				game.checkers.move(src, dst);
				game.whitePlayerTurn = !game.whitePlayerTurn;
				lastMove += reply + ".";
			}
			catch (const string ex)
			{
				send_n_receive("INVALID ACTION");
				assert(reply == "ACKNOWLEDGED");
			}
		}

		if (cmd == "jump")
		{
			try
			{
				//Jump will throw an exception if invalid
				game.checkers.jump(src,dst);
				lastMove += reply + "; ";
			}
			catch (const string ex)
			{
				send_n_receive("INVALID ACTION");
				assert(reply == "ACKNOWLEDGED");
			}
		}

	}
}

int main(int argc, char* argv[])
{
	int portNumber = 5000;
	if (argc == 2)
	{
		int newPortNumber = 0;

		newPortNumber = atoi(argv[1]);

		if (newPortNumber != 0)
		{
			portNumber = newPortNumber;
		}
	}

	cout << "SERVER: opening on port " << portNumber << endl;

	try
	{
		init(portNumber);
		server_accept();
		play_game();
	}
	catch (const string ex)
	{
		cout << ex << endl;
	}

	cout << "SERVER: shutting down.\n";

	close(server.socketFD);
}
