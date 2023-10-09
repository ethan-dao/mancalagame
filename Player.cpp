//
//  Player.cpp
//  Proj3_CS32
//
//  Created by Ethan Dao on 5/26/23.
//

#include <stdio.h>
#include "Player.h"
#include <iostream>

unsigned long f_helper(int n, double timeLimit, JumpyTimer& timer);
unsigned long f(int n);

unsigned long f(int n)
{
    double timeLimit = 4990;  // 4.99 seconds; allow 10 ms for wrapping up
    JumpyTimer timer(1000);
    unsigned long result = f_helper(n, timeLimit, timer);
    cout << timer.actualElapsed() << " ms" << endl;
    return result;
}

unsigned long f_helper(int n, double timeLimit, JumpyTimer& timer)
{
    unsigned long callCount = 1;
    if (n == 0)
        callCount += 0;  /// do a base case computation
    else if (timeLimit <= 0)
        callCount += 0;  //  do non-recursive heuristic computation
    else
    {
        for (int branchesLeftToExplore = 10; branchesLeftToExplore > 0; branchesLeftToExplore--) {
            // Allocate remaining time limit equally among remaining braches to explore
            double thisBranchTimeLimit = timeLimit / branchesLeftToExplore;

            // Save branch start time so can compute the time the branch took
            double startTime = timer.elapsed();
            // Explore this branch
            callCount += f_helper(n-1, thisBranchTimeLimit, timer);
            // Adjust remaining time
            timeLimit -= (timer.elapsed() - startTime);
            if (timeLimit <= 0) {
                timeLimit = 0;
            }
        }
    }
    return callCount;
}

Player::Player(string name) : m_name(name) {}

string Player::name() const {
    return m_name;
}

bool Player::isInteractive() const {
    return false;
}

Player::~Player() {
    // Do nothing
}

HumanPlayer::HumanPlayer(string name) : Player(name), m_name(name) {}

string HumanPlayer::name() const {
    return m_name;
}

bool HumanPlayer::isInteractive() const {
    return true;
}

int HumanPlayer::chooseMove(const Board& b, Side s) const {
    // If no beans left
    if (b.beansInPlay(s) == 0) {
        return -1;
    }
    // Initialize the moveInput and prompt user to cin an int for their move
    int moveInput;
    cout << "Please select a hole: ";
    cin >> moveInput;
    cout << endl;
    // If move is invalid
    if (b.beans(s, moveInput) == -1 || b.beans(s, moveInput) == 0) {
        cout << "Invalid number. Please choose another hole!" << endl;
        return chooseMove(b, s);
    }
    // If move is valid
    return moveInput;
}

HumanPlayer::~HumanPlayer() {
    // Do nothing
}

BadPlayer::BadPlayer(string name) : Player(name), m_name(name) {}

string BadPlayer::name() const {
    return m_name;
}

bool BadPlayer::isInteractive() const {
    return false;
}

int BadPlayer::chooseMove(const Board& b, Side s) const {
    // A BadPlayer is a computer player that chooses an arbitrary valid move and returns that choice. "Arbitrary" can be what you like: leftmost, nearest to pot, fewest beans, random, etc.. The point of this class is to have an easy-to-implement class that at least plays legally.
    
    // If no beans left
    if (b.beansInPlay(s) == 0) {
        return -1;
    }
    // Choose the leftmost hole with beans in it
    for (int i = 1; i <= b.holes(); i++) {
        if (b.beans(s, i) != 0) {
            return i;
        }
    }
    return -1;
}

BadPlayer::~BadPlayer() {
    // Do nothing
}

SmartPlayer::SmartPlayer(string name) : Player(name), m_name(name) {}

string SmartPlayer::name() const {
    return m_name;
}

bool SmartPlayer::isInteractive() const {
    return false;
}

int SmartPlayer::chooseMove(const Board& b, Side s, int& bestHole, int& value, JumpyTimer& timer, int depth) const {
    // Make an opponentSide
    Side opponentSide; // Change the opponent's side
    if (s == NORTH) {
        opponentSide = SOUTH;
    } else { // s = SOUTH
        opponentSide = NORTH;
    }
    // If no move for player exists (game over, no beans in smartPlayer's side)
    if (b.beansInPlay(s) == 0) {
        bestHole = -1;
        value = evaluate(b);
        return -1;
    }
    // If the criterion says we shouldn't search below this node (timer > 5 seconds)
    if (timer.elapsed() >= 5000) {
        bestHole = -1;
        value = evaluate(b);
        return -1;
    }
    if (depth > 5) {
        bestHole = -1;
        value = evaluate(b);
        return -1;
    }
    // For every hole the player can choose...
    // Initialize the value (initial value) so it can be compared to v2; value will be whatever i is to be compared
    int initialValue = 1;
    for (int i = 1; i <= b.holes(); i++) {
        if (b.beans(s, i) != 0) {
            // Make the move h
            Board boardCopy(b);
            Side endSide;
            int endHole;
            int h2 = -1; // Arbitrary value
            int v2 = -9999; // Arbitrary value
            int h3 = -1; // Arbitrary value
            if (completeSow(s, i, boardCopy, endSide, endHole)) {
                // If completeSow returns true, choose move for opponent using recursion
                chooseMove(boardCopy, opponentSide, h2, v2, timer, depth + 1);
            } else { // If completeSow returns false, it's the same player's turn; don't increment depth
                chooseMove(boardCopy, s, h3, v2, timer, depth);
            }
            // Undo the move
            boardCopy = b;
            // Set the first value so that there is something to be compared to initially (first move)
            if (i == initialValue) {
                bestHole = i;
                value = v2;
            }
            // If the second value (v2) > best value seen so far, bestHole = h and value = v2
            if (s == SOUTH) {
                if (v2 > value && b.beans(s, i) != 0) {
                    bestHole = i;
                    value = v2;
                }
            } else { // s = NORTH
                if (v2 < value && b.beans(s, i) != 0) {
                    bestHole = i;
                    value = v2;
                }
            }
            // Check if timer ran out; if so, then break and stop checking
            if (timer.elapsed() >= 5000) {
                break;
            }
        } else { // b.beans(s, i) == 0
            // We also need to increment initialValue (value that is being compared) when an invalid hole is placed
            initialValue++;
        }
    }
    // Return bestHole (should be the most optimal move)
    return bestHole;
}

bool SmartPlayer::completeSow(Side s, int hole, Board& b, Side& endSide, int& endHole) const {
    // If you can sow...
    if (b.sow(s, hole, endSide, endHole)) { // Makes sure sow is valid 
        if (endHole == 0) { // If another turn is needed, return false
            return false;
        } else if (endSide == s) { // Account for captures
            Side opponentSide; // Change the opponent's side
            if (s == NORTH) {
                opponentSide = SOUTH;
            } else {
                opponentSide = NORTH;
            }
            if (b.beans(endSide, endHole) == 1 && b.beans(opponentSide, endHole) > 0) { // Move beans to pot
                b.moveToPot(NORTH, endHole, s);
                b.moveToPot(SOUTH, endHole, s);
            }
        }
        return true;
    }
    return false; // If another turn is needed
}



// Overloading the chooseMove function
int SmartPlayer::chooseMove(const Board& b, Side s) const {
    int bestHole = -1;
    int value = -9999;
    int depth = 0;
    JumpyTimer timer(1000); // What do we start jumpyTimer with?
    // Should return chooseMove with arbitrary bestHole and value parameters
    return chooseMove(b, s, bestHole, value, timer, depth);
}

int SmartPlayer::evaluate(const Board& b) const {
    // If end of game and one side wins, set it to that value
    // Bigger number is better for the south side, smaller number is better for the north side
    if (b.beansInPlay(SOUTH) == 0 || b.beansInPlay(NORTH) == 0) {
        if (b.beans(SOUTH, 0) + b.beansInPlay(SOUTH) > b.beans(NORTH, 0) + b.beansInPlay(NORTH)) { // If user won
            return 9999;
        } else if (b.beans(SOUTH, 0) + b.beansInPlay(SOUTH) < b.beans(NORTH, 0) + b.beansInPlay(NORTH)) { // If opponent won
            return -9999;
        } else {
            return 0;
        }
    }
    return (b.beans(SOUTH, 0) + b.beansInPlay(SOUTH)) - (b.beans(NORTH, 0) + b.beansInPlay(NORTH));
}


SmartPlayer::~SmartPlayer() {
    // Do nothing
}


