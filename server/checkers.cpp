#include "checkers.hpp"

/******************************************************************************
* Constructor for the Checkers games.  No parameters accepted.
*   Initializes the grid to the default checkers piece pattern.
*   Starts the game with white.
******************************************************************************/
Checkers::Checkers()
{
	whitePlayerTurn = true;

	char gridstart[8][8] = {	{'w', 'e', 'w', 'e', 'w', 'e', 'w', 'e'},
								{'e', 'w', 'e', 'w', 'e', 'w', 'e', 'w'},
								{'w', 'e', 'w', 'e', 'w', 'e', 'w', 'e'},
								{'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e'},
								{'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e'},
								{'e', 'r', 'e', 'r', 'e', 'r', 'e', 'r'},
								{'r', 'e', 'r', 'e', 'r', 'e', 'r', 'e'},
								{'e', 'r', 'e', 'r', 'e', 'r', 'e', 'r'}
						   };

	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			grid[row][col] = Piece(gridstart[row][col]);
		}
	}
}

/******************************************************************************
* Make sure that the location object is within valid bounds
******************************************************************************/
bool Checkers::validateLocation(const Location& l) const
{
	if (l.x > 7 || l.x < 0)
		return false;
	if (l.y > 7 || l.y < 0)
		return false;

	return true;
}

void Checkers::jump(Location src, Location dst) throw (const string)
{
    //Validate src and dst...
	if (!validateLocation(src))
	{
		throw string("ERROR: Invalid source location");
	}
	if (!validateLocation(dst))
	{
		throw string("ERROR: Invalid destination location");
	}

    auto checkPlusMinusTwo = [](uint8_t base, uint8_t compare)
    {
        return (base + 2 == compare || base - 2 == compare);
    };
}

/******************************************************************************
* Move a piece.  Rules are as follows:
*   * Both locations must be valid      * The delta of x between must be |1|
*   * The delta of y must be |1|; dependant upon being kinged
*   * The source piece must be the current player's color
*   * The destination must be empty
*
*   Kings a piece if it reaches the opposite end it started at.
******************************************************************************/
void Checkers::move(Location src, Location dst) throw (const string)
{
	//Validate src and dst...
	if (!validateLocation(src))
	{
		throw string("ERROR: Invalid source location");
	}
	if (!validateLocation(dst))
	{
		throw string("ERROR: Invalid destination location");
	}

    //Check plus/minus one of the base with respect to compare
    auto checkPlusMinusOne = [](uint8_t base, uint8_t compare)
    {
        return ((base + 1 == compare) || (base - 1 == compare));
    };

    Piece *src_p = &grid[src.y][src.x];
    Piece *dst_p = &grid[dst.y][dst.x];

    //Check to make sure the delta X is 1 or -1
    if (!checkPlusMinusOne(src.x, dst.x))
        throw string("INVALID: Change in X value is illegal");

    //Is the piece at the destination empty?
    if (!dst_p->isEmpty())
        throw string("INVALID: Piece at destination is not empty");

	if (whitePlayerTurn)
	{
		//Is the piece at source white?
		if (!src_p->isWhite())
			throw string("INVALID: Piece at source is not white");

		//Is the source piece kinged?
		if (src_p->isKinged())
        {
            if (!checkPlusMinusOne(src.y, dst.y))
                throw string("INVALID: Change in Y value is illegal");
        }
		else if (src.y + 1 != dst.y)
			throw string("INVALID: Change in Y value is illegal");

        //If a white piece reaches y = 7, it is kinged
        if (dst.y == 7)
            src_p->setKinged();
	}
	else
	{
        //Is the piece at source red
        if (!src_p->isRed())
            throw string("INVALID: Piece at source is not red");

        //Is the source piece kinged?
        if (src_p->isKinged())
        {
            if (!checkPlusMinusOne(src.y, dst.y))
                throw string("INVALID: Change in Y value is illegal");
        }
        else if (src.y - 1 != dst.y)
            throw string("INVALID: Change in Y value is illegal");

        //If a red piece reaches y = 0, it is kinged.
        if (dst.y == 0)
            src_p->setKinged();
	}

    //If we haven't thrown, then the move is legal.
    *dst_p = *src_p;
    *src_p = Piece();

    whitePlayerTurn = !whitePlayerTurn;
};

/******************************************************************************
* Lazy way to call the ostream operator :)
******************************************************************************/
void Checkers::display() const
{
	cout << *this;
}

string Checkers::getGridData() const
{
	string data;
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			data += getPieceValue(row, col);
		}
	}

	return data;
}

/******************************************************************************
* Returns the character value of a piece.
*   Used with the ostream operator to access the grid
******************************************************************************/
char Checkers::getPieceValue(uint8_t row, uint8_t col) const
{
	char piece = '\0';

	const Piece *p = &grid[row][col];

	if (p->isWhite())
	{
		if (p->isKinged())
			piece = 'W';
		else
			piece = 'w';
	}
	else if (p->isRed())
	{
		if (p->isKinged())
			piece = 'R';
		else
			piece = 'r';
	}
	else
		piece = ' ';

	return piece;
}

/******************************************************************************
* Checkers ostream operator
******************************************************************************/
std::ostream& operator << (std::ostream& out, const Checkers& checkers)
{
	out << "     1   2   3   4   5   6   7   8\n";
	out << "   ---------------------------------\n";

    for (uint8_t row = 0; row < 8; row++)
	{

		out << " " << (char)(row + 65) << " ";

		for (uint8_t col = 0; col < 8; col++)
		{
			char piece = checkers.getPieceValue(row, col);

			out << "| " << piece << " ";
		}

		out << "|" << endl << "   ---------------------------------\n";

	}

	return out;
}

string locToStr(const Location& loc)
{
	string locStr = (char)(loc.y + 65) + "" + (char)(loc.x + 49);

	return locStr;
	// return string((char)(location.y + 65) + (char)(location.x + 49));
}

std::ostream& operator << (std::ostream& out, const Location& location)
{
	out << locToStr(location);

	return out;
}
