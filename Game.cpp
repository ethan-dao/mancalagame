//
//  Game.cpp
//  Proj3_CS32
//
//  Created by Ethan Dao on 5/26/23.
//

#include <stdio.h>
#include "Game.h"
#include <iostream>


Game::Game(const Board& b, Player* south, Player* north) : m_board(b), m_southPlayer(south), m_northPlayer(north), m_currentSide(SOUTH)
{
    // Construct a Game to be played with the indicated players on a copy of the board b. The player on the south side always moves first.
}

void Game::display() const {
    // Display name of the north player
    cout << "// " << m_northPlayer -> name() << endl;
    // Display north side (with extra spaces)
    cout << "// " << "   ";
    for (int i = 1; i <= m_board.holes(); i++) {
        cout << m_board.beans(NORTH, i) << "  ";
    }
    cout << endl;
    // Display pots (first and last)
    cout << "// " << m_board.beans(NORTH, 0);
    for (int i = 0; i <= m_board.holes(); i++) {
        cout << "   ";
    }
    cout << m_board.beans(SOUTH, 0) << endl;
    // Display south side (like north side)
    cout << "// " << "   ";
    for (int i = 1; i <= m_board.holes(); i++) {
        cout << m_board.beans(SOUTH, i) << "  ";
    }
    cout << endl;
    // Display name of the south player
    cout << "// " << m_southPlayer -> name() << endl << endl << endl;
}

void Game::status(bool& over, bool& hasWinner, Side& winner) const {
    // If the game is over (i.e., the move member function has been called and returned false), set over to true; otherwise, set over to false and do not change anything else. If the game is over, set hasWinner to true if the game has a winner, or false if it resulted in a tie. If hasWinner is set to false, leave winner unchanged; otherwise, set it to the winning side.
    if (m_board.beansInPlay(NORTH) == 0 || m_board.beansInPlay(SOUTH) == 0) {
        over = true;
        if (m_board.beans(SOUTH, 0) > m_board.beans(NORTH, 0)) { // If south's pot has more beans than north's
            hasWinner = true;
            winner = SOUTH;
        } else if (m_board.beans(SOUTH, 0) < m_board.beans(NORTH, 0)) { // If north's pot has more beans than south's
            hasWinner = true;
            winner = NORTH;
        } else { // If north and south's pot are equal
            hasWinner = false;
        }
    } else { // Game is still in play
        over = false;
    }
}

bool Game::move(Side s) {
    // Attempt to make a complete move for the player playing side s. "Complete" means that the player sows the seeds from a hole and takes any additional turns required or completes a capture. Whenever the player gets an additional turn, you should display the board so someone looking at the screen can follow what's happening. If the move can be completed, return true; if not, because the move is not yet completed but side s has no holes with beans to pick up and sow, sweep any beans in s's opponent's holes into that opponent's pot and return false.
    
    // If false (move not completed but side s has no soles with beans to pick up and sow, sweep into pot and return false, i.e. game has ended
    if (m_board.beansInPlay(NORTH) == 0) {
        for (int i = 1; i <= m_board.holes(); i++) { // Use moveToPot function to clear beans and move to pot
            m_board.moveToPot(SOUTH, i, SOUTH);
        }
        cout << m_northPlayer -> name() << "has no beans left. Ending game!" << endl << endl;
//        display();
        return false;
    }
    if (m_board.beansInPlay(SOUTH) == 0) {
        for (int i = 1; i <= m_board.holes(); i++) {
            m_board.moveToPot(NORTH, i, NORTH);
        }
        cout << m_southPlayer -> name() << "has no beans left. Ending game!" << endl << endl;
//        display();
        return false;
    }
    
    // If true...
    // Initialize values
    Side endSide = NORTH; // Initial arbitrary value for endSide and endHole
    int endHole = 1;
    
    // If player is north
    if (s == NORTH) {
        int hole = m_northPlayer -> chooseMove(m_board, NORTH);
        cout << m_northPlayer -> name() << " chose hole " << to_string(hole) << "." << endl;
        if (m_board.sow(NORTH, hole, endSide, endHole)) { // If sow is true, then check for more turns/capture
            if (endHole == 0) { // If landing in own pot...
                if (m_board.beansInPlay(NORTH) > 0) { // If game is still valid (move = true)
                    cout << m_northPlayer -> name() << " got another turn." << endl;
                    display(); // Display board if player got another turn
                    return move(s);
                }
            } else if (endSide == NORTH) { // Check for capture 1
                if (m_board.beans(endSide, endHole) == 1 && m_board.beans(SOUTH, endHole) > 0) { // Capture check 2
                    m_board.moveToPot(SOUTH, endHole, NORTH);
                    m_board.moveToPot(NORTH, endHole, NORTH);
                    cout << m_northPlayer -> name() << " captured some beans!" << endl;
                }
            }
        }
    } else { // s = SOUTH
        int hole = m_southPlayer -> chooseMove(m_board, SOUTH);
        cout << m_southPlayer -> name() << " chose hole " << to_string(hole) << "." << endl;
        if (m_board.sow(SOUTH, hole, endSide, endHole)) { // If sow is true, then check for more turns/capture
            if (endHole == 0) { // If landing in own pot
                if (m_board.beansInPlay(SOUTH) > 0) { // If game is still valid (move = true)
                    cout << m_southPlayer -> name() << " got another turn." << endl;
                    display(); // Display board if player got another turn
                    return move(s);
                }
            } else if (endSide == SOUTH) { // Check for capture 1
                if (m_board.beans(endSide, endHole) == 1 && m_board.beans(NORTH, endHole) > 0) { // Capture check 2
                    m_board.moveToPot(SOUTH, endHole, SOUTH);
                    m_board.moveToPot(NORTH, endHole, SOUTH);
                    cout << m_southPlayer -> name() << " captured some beans!" << endl;
                }
            }
        }
    }
    
    // We need to check again for false at the end in case the game ends after the move so another move doesn't have to be called in order to end the game
    if (m_board.beansInPlay(NORTH) == 0) {
        for (int i = 1; i <= m_board.holes(); i++) { // Use moveToPot function to clear beans and move to pot
            m_board.moveToPot(SOUTH, i, SOUTH);
        }
        cout << m_northPlayer -> name() << " has no beans left. Ending game!" << endl << endl;
//        display();
        return false;
    }
    if (m_board.beansInPlay(SOUTH) == 0) {
        for (int i = 1; i <= m_board.holes(); i++) {
            m_board.moveToPot(NORTH, i, NORTH);
        }
        cout << m_southPlayer -> name() << " has no beans left. Ending game!" << endl << endl;
//        display();
        return false;
    }

    // Move was successful
    display(); // Display the board after every successful turn
    return true;
}

void Game::play() {
    // Play the game. Display the progress of the game in a manner of your choosing, provided that someone looking at the screen can follow what's happening. If neither player is interactive, then to keep the display from quickly scrolling through the whole game, it would be reasonable periodically to prompt the viewer to press ENTER to continue and not proceed until ENTER is pressed. (The ignore function for input streams is useful here.) Announce the winner at the end of the game. You can apportion to your liking the responsibility for displaying the board between this function and the move function.
    cout << "Welcome to Kalah! Today, we have " << m_northPlayer -> name() << " vs. " << m_southPlayer -> name() << ". It is " << m_southPlayer -> name() << "'s turn first. Here is the starting board: " << endl << endl;
    display();
    // Declare variables
    bool over = false;
    bool hasWinner = false;
    int counter = 0;
    Side winner = SOUTH; // Idk if I should do this or just leave winner uninitialized?
    
    if (m_southPlayer == nullptr || m_northPlayer == nullptr) {
        return;
    }
    
    // Play the game; if neither player is interactive, prompt the viewer to press enter to continue whenever counter if counter % 3 = 0
    while (!over) { // while move() == true
        // Increment counter
        counter++;
        move(m_currentSide); // Moves pieces as necessary and displays the board after (move displays board)
        // Change the current side so that the other player gets a turn
        if (m_currentSide == SOUTH) {
            m_currentSide = NORTH;
        } else { // m_currentSide = NORTH
            m_currentSide = SOUTH;
        }
        
        // If both players aren't interactive and counter % 4 = 0, then prompt an enter to continue
        if (m_southPlayer -> isInteractive() == false && m_northPlayer -> isInteractive() == false) {
            if (counter % 4 == 0 && counter != 0) {
                // Prompt the viewer to press enter to continue
                cout << "Press enter to continue viewing game: " << endl;
                cin.ignore();
            }
        }

        // Check game status
        status(over, hasWinner, winner);
    }
    // Game has ended...in this case, move() should return false and all the beans should be in their respective pots
    display();
    // Announce the winner
    if (hasWinner) {
        if (winner == SOUTH) {
            cout << "The winner is " << m_southPlayer->name() << "!" << endl << endl;
        } else { // winner = NORTH
            cout << "The winner is " << m_northPlayer->name() << "!" << endl << endl;
        }
    } else { // Game has no winner
        cout << "It's a tie!" << endl << endl;
    }
}

int Game::beans(Side s, int hole) const {
    // Return the number of beans in the indicated hole or pot of the game's board, or −1 if the hole number is invalid. This function exists so that we and you can more easily test your program.
    // If hole is less than 0 (pot) or more than the number of holes, it is invalid.
    if (hole > m_board.holes() || hole < 0) {
        return -1;
    }
    // If valid, return the number of beans in the indicated hole or pot of the game's board.
    return m_board.beans(s, hole);
}
