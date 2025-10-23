#include <vector>
#include <random>
#include "TicTacToe.h"
#include "../Application.h"

// -----------------------------------------------------------------------------
// TicTacToe.cpp
// -----------------------------------------------------------------------------
// This file is intentionally *full of comments* and gentle TODOs that guide you
// through wiring up a complete Tic‑Tac‑Toe implementation using the game engine’s
// Bit / BitHolder grid system.
//
// Rules recap:
//  - Two players place X / O on a 3x3 grid.
//  - Players take turns; you can only place into an empty square.
//  - First player to get three-in-a-row (row, column, or diagonal) wins.
//  - If all 9 squares are filled and nobody wins, it’s a draw.
//
// Notes about the provided engine types you'll use here:
//  - Bit              : a visual piece (sprite) that belongs to a Player
//  - BitHolder        : a square on the board that can hold at most one Bit
//  - Player           : the engine’s player object (you can ask who owns a Bit)
//  - Game options     : let the mouse know the grid is 3x3 (rowX, rowY)
//  - Helpers you’ll see used: setNumberOfPlayers, getPlayerAt, startGame, etc.
//
// I’ve already fully implemented PieceForPlayer() for you. Please leave that as‑is.
// The rest of the routines are written as “comment-first” TODOs for you to complete.
// -----------------------------------------------------------------------------

const int AI_PLAYER   = 1;      // index of the AI player (O)
const int HUMAN_PLAYER= 0;      // index of the human player (X)

TicTacToe::TicTacToe()
{
}

TicTacToe::~TicTacToe()
{
}

// -----------------------------------------------------------------------------
// make an X or an O
// -----------------------------------------------------------------------------
// DO NOT CHANGE: This returns a new Bit with the right texture and owner
Bit* TicTacToe::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == 1 ? "x.png" : "o.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

//
// setup the game board, this is called once at the start of the game
//
void TicTacToe::setUpBoard()
{

    setNumberOfPlayers(2);
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;

    // lay out 3×3 board squares 
    const ImVec2 origin(100.0f, 100.0f);
    const float  cell = 128.0f;

    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 3; ++col)
            _grid[row][col].initHolder(
                ImVec2(origin.x + col * cell, origin.y + row * cell),
                "square.png",
                col, row); 

    // start turn system
    startGame();
}


//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool TicTacToe::actionForEmptyHolder(BitHolder *holder)
{
    // already over? don’t allow more moves
    if (checkForWinner() != nullptr || checkForDraw())
        return false;

    if (!holder) return false;
    if (holder->bit() != nullptr) return false; // must be empty

    // current player is zero-based (0 or 1)
    const int current = getCurrentPlayer()->playerNumber();

    // create and place the piece
    Bit* piece = PieceForPlayer(current);        
    piece->setPosition(holder->getPosition());
    holder->setBit(piece);

    // evaluate win/draw and end the turn
    ClassGame::EndOfTurn();

    return true;
}


bool TicTacToe::canBitMoveFrom(Bit *bit, BitHolder *src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void TicTacToe::stopGame()
{
    //clear every square (engine cleans up the Bit)
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            _grid[y][x].destroyBit();
}


//
// helper function for the winner check
//
Player* TicTacToe::ownerAt(int index) const
{
    int y = index / 3;
    int x = index % 3;
    Bit* b = _grid[y][x].bit();
    return b ? b->getOwner() : nullptr;
}


Player* TicTacToe::checkForWinner()
{
    // 8 winning triples (indices 0..8, row-major)
    static const int W[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},  // rows
        {0,3,6},{1,4,7},{2,5,8},  // cols
        {0,4,8},{2,4,6}           // diags
    };

    for (const auto& t : W) {
        Player* a = ownerAt(t[0]); if (!a) continue;
        Player* b = ownerAt(t[1]);
        Player* c = ownerAt(t[2]);
        if (a == b && b == c) return a;
    }
    return nullptr;
}


bool TicTacToe::checkForDraw()
{
    if (checkForWinner() != nullptr) return false;
    for (int i = 0; i < 9; ++i)
        if (ownerAt(i) == nullptr) return false;
    return true;
}


//
// state strings
//
std::string TicTacToe::initialStateString()
{
    return "000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string TicTacToe::stateString() const
{
    // 9 chars, left→right, top→bottom.
    // '0' = empty, players are zero-based, so store ('1' or '2') as (owner+1).
    std::string s;
    s.reserve(9);
    for (int i = 0; i < 9; ++i) {
        int y = i / 3, x = i % 3;
        if (Bit* bit = _grid[y][x].bit()) {
            int pn = bit->getOwner()->playerNumber(); // 0 or 1
            s.push_back(char('0' + (pn + 1)));
        } else {
            s.push_back('0');
        }
    }
    return s;
}


//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void TicTacToe::setStateString(const std::string &s)
{
    // clear existing bits
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            _grid[y][x].destroyBit();

    // rebuild from the 9-char string
    for (int i = 0; i < 9 && i < (int)s.size(); ++i) {
        char ch = s[i];
        int y = i / 3, x = i % 3;
        if (ch == '0') continue;

        int oneBased = ch - '0';   // 1 or 2
        int zeroBased = oneBased - 1; // 0 or 1 

        Bit* piece = PieceForPlayer(zeroBased);
        piece->setPosition(_grid[y][x].getPosition());
        _grid[y][x].setBit(piece);
    }
}



//
// this is the function that will be called by the AI
//
void TicTacToe::updateAI() 
{
    // we will implement the AI in the next assignment!
}

