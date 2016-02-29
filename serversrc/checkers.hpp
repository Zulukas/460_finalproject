#ifndef CHECKERS_HPP
#define CHECKERS_HPP

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

/******************************************************************************
* Simple struct to store coordinates
******************************************************************************/
struct Location
{
	uint8_t x, y;
};

string locToStr(const Location& location);
std::ostream& operator << (std::ostream& out, const Location& location);

/******************************************************************************
* Bitmask object to contain info about an individual piece
* 	Mask values: 1 = white, 2 = red, 4 = kinged.  All 0's implies empty
******************************************************************************/
class Piece
{
	public:
		Piece() { mask = 0; }

		Piece(char type)
		{
			mask = 0;

			if (type == 'w' || type == 'W')
				mask |= 1;
			else if (type == 'r' || type == 'R')
				mask |= 2;
		}

		void setKinged()      { (mask & 3) ? (mask |= 4) : (0); }

		bool isKinged() const { return mask & 4; }
		bool isRed()    const { return mask & 2; }
		bool isWhite()  const { return mask & 1; }
		bool isEmpty()  const { return !(mask & 3); }
	private:
		uint8_t mask;
};

/******************************************************************************
* Checkers class, contains an 8x8 grid of Pieces and all member methods
* to play the game
******************************************************************************/
class Checkers
{
public:
	Checkers();

	void move(Location src, Location dst) throw (const string);
	void jump(Location src, Location dst) throw (const string);

	void display() const;
	string getGridData() const;
	char getPieceValue(uint8_t row, uint8_t col) const;

private:
	Piece grid[8][8];
	bool whitePlayerTurn;

	bool validateLocation(const Location& l) const;

	bool whiteJump() throw (const string);
	bool redJump() throw (const string);
};

/******************************************************************************
* Ostream operator to easily display the grid
******************************************************************************/
std::ostream& operator << (std::ostream& out, const Checkers& checkers);

#endif
