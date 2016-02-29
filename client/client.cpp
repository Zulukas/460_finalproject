#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#define BUFFER_SIZE 1024

class ClientSocket
{
public:
    ClientSocket(string serverString, int portNumber) throw (const string)
    {
        portno = portNumber;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
            throw string("ERROR opening socket");
        server = gethostbyname(serverString.c_str());
        if (server == NULL)
        {
            throw string("ERROR, no such host.");
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;

        bcopy((char *)server->h_addr,
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);

        serv_addr.sin_port = htons(portno);

        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
            throw string("ERROR connecting");
    }

    void writeToServer(const string message) throw (const string)
    {
        cout << "SENDING TO SERVER: \"" << message << "\" OF LENGTH: " << message.length() + 1 << endl;
        n = write(sockfd, message.c_str(), message.length() + 1);

        if (n < 0)
             throw string("ERROR writing to socket");

        bzero(buffer,256);
    }

    string readFromServer() throw (const string)
    {
        n = read(sockfd,buffer,255);
        if (n < 0)
             throw string("ERROR reading from socket");

        string msg = string(buffer);

        cout << "MESSAGE RECEIVED: \"" << msg << "\" OF LENGTH: " << msg.length() << endl;

        return string(buffer);
    }

private:
    int sockfd,
        portno,
        n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];
};

void displayBoard(const string data)
{
    assert(data.length() >= 64);

    cout << "     1   2   3   4   5   6   7   8\n";
	cout << "   ---------------------------------\n";

    for (int row = 0; row < 8; row++)
    {
        cout << " " << (char)(row + 65) << " ";

        for (int col = 0; col < 8; col++)
        {
            char piece = data[row * 8 + col];
            cout << "| " << piece << " ";
        }

        cout << "|" << endl << "   ---------------------------------\n";
    }
}

void displayHelp()
{
    cout << "\tCommands:\n"
         << "\t\tmove <source location> <destination location>\n"
         << "\t\tjump <source location> <destination location>\n"
         << "\t\tquit\n\n"
         << "location format: [A-H][1-9]\n\n";
}

void play(ClientSocket& socket)
{
    bool invalidAction = false;

    for (;;)
    {
        if (!invalidAction)
        {
            //Receive grid data and then acknowledge
            string data = socket.readFromServer();
            displayBoard(data);

            socket.writeToServer("DATA RECEIVED");

            //Receive last move from server and then acknowledge
            string lastMove = socket.readFromServer();

            if (lastMove != "")
            {
                cout << lastMove << endl;
            }

            socket.writeToServer("LAST MOVE RECEIVED");

        }
        else
        {
            cout << "Invalid action, please try again.\n";
            displayHelp();
        }

        string reply = socket.readFromServer();

        assert(reply == "READY");

        invalidAction = false;

        cout << "Enter your action: ";
        string action;
        getline(cin, action);

        socket.writeToServer(action);

        reply = socket.readFromServer();

        socket.writeToServer("ACKNOWLEDGED");

        if (reply == "INVALID ACTION")
        {
            invalidAction = true;
        }
    }

//     int count = 0;
//     bool INVALID_INPUT = false;
//
//     cout << ++count << endl;
//
//     for(;;)
//     {
//         cout << ++count << endl;
//         //READ (data) READ (last move) WRITE (command) READ (command reply) protocol
//         if (!INVALID_INPUT)
//         {
//             cout << ++count << endl;
//             string data = socket.readFromServer();
//             cout << ++count << endl;
//             displayBoard(data);
//
// cout << ++count << endl;
//
//             socket.writeToServer("RECEIVED");
//
//
//             string lastMove = socket.readFromServer();
//
//             if (lastMove != "")
//                 cout << lastMove << endl;
//         }
//         else
//         {
//             cout << "Invalid input, please try again.\n";
//             displayHelp();
//         }
//
//         string cmd;
//         cout << "Enter command: ";
//         getline(cin, cmd);
//
//         stringstream ss(cmd);
//
//         string cmd_part;
//
//         ss >> cmd_part;
//
//         if (cmd_part == "move" || cmd_part == "jump" || cmd_part == "quit")
//         {
//             INVALID_INPUT = false;
//             socket.writeToServer(cmd);
//         }
//         else
//         {
//             INVALID_INPUT = true;
//             continue;
//         }
//
//         string cmdReply = socket.readFromServer();
//
//         if (cmdReply == "ACTION_INVALID")
//         {
//
//         }
//         else if (cmdReply == "DISCONNECT")
//         {
//
//         }
//         else if (cmdReply == "COMMAND_INVALID")
//         {
//
//         }
//         else
//         {
//             cout << cmdReply << endl;
//         }
//     }
}

int main(int argc, char *argv[])
{
    string address = "127.0.0.1";
    int portNumber = 5000;

    if (argc >= 3)
    {
        address = argv[1];
        portNumber = atoi(argv[2]);
    }

    try
    {
        ClientSocket socket(address, portNumber);
        string color = socket.readFromServer(); //Get color

        cout << color << endl;

        if (color == "WHITE" || color == "RED")
        {
            socket.writeToServer("RECEIVED");
            play(socket);
        }
        else
        {
            cout << "Game is full, please try again later!\n";
        }
    }
    catch (const string ex)
    {
        cout << ex << endl;
    }

    return 0;
}
