//
//  Board.cpp
//  Proj3_CS32
//
//  Created by Ethan Dao on 5/25/23.
//
#include <stdio.h>
#include "Board.h"

Board::Board(int nHoles, int nInitialBeansPerHole) {
    // Construct a Board with the indicated number of holes per side (not counting the pot) and initial number of beans per hole. If nHoles is not positive, act as if it were 1; if nInitialBeansPerHole is negative, act as if it were 0.
    if (nHoles < 0) {
        nHoles = 1;
    }
    if (nInitialBeansPerHole < 0) {
        nInitialBeansPerHole = 0;
    }
    
    // Initializing number of beans
    m_numberOfHoles = nHoles;
    m_initialNumberOfBeans = nInitialBeansPerHole;
    
    // Initialize our north and south side vectors (+ pot)
    north.push_back(0);
    south.push_back(0);
    for (size_t i = 0; i < m_numberOfHoles; i++) {
        north.push_back(m_initialNumberOfBeans);
        south.push_back(m_initialNumberOfBeans);
    }
}

Board::Board(const Board& other) {
    // Copy over the vectors
    north = other.north;
    south = other.south;
    m_numberOfHoles = other.m_numberOfHoles;
    m_initialNumberOfBeans = other.m_initialNumberOfBeans;
}

Board& Board::operator=(const Board& other) {
    // Assign if not self-assignment
    if (this != &other) {
        north = other.north;
        south = other.south;
        m_numberOfHoles = other.m_numberOfHoles;
        m_initialNumberOfBeans = other.m_initialNumberOfBeans;
    }
    // If self-assignment, return itself
    if (this == &other) {
        return *this;
    }
    return *this;
}

int Board::holes() const {
    // Return the number of holes on a side (not counting the pot).
    return m_numberOfHoles;
}

int Board::beans(Side s, int hole) const {
    // Return the number of beans in the indicated hole or pot, or −1 if the hole number is invalid.
    if (hole < 0 || hole > m_numberOfHoles) {
        return -1;
    }
    // If side is the north side...
    if (s == NORTH) {
        return north[hole];
    }
    // If side is the south side...
    if (s == SOUTH) {
        return south[hole];
    }
    return -1; // Invalid if hole is not NORTH or SOUTH
}

int Board::beansInPlay(Side s) const {
    // Return the total number of beans in all the holes on the indicated side, not counting the beans in the pot.
    int total = 0;
    // If side is the north side...
    if (s == NORTH) {
        for (size_t i = 1; i < north.size(); i++) {
            total += north[i];
        }
    }
    // If side is the south side...
    if (s == SOUTH) {
        for (size_t i = 1; i < south.size(); i++) {
            total += south[i];
        }
    }
    return total;
}

int Board::totalBeans() const {
    // Return the total number of beans in the game, including any in the pots.
    return beansInPlay(NORTH) + beansInPlay(SOUTH) + beans(NORTH, 0) + beans(SOUTH, 0);
}

bool Board::sow(Side s, int hole, Side& endSide, int& endHole) {
    // If the hole indicated by (s,hole) is empty or invalid or a pot, this function returns false without changing anything. Otherwise, it will return true after sowing the beans: the beans are removed from hole (s,hole) and sown counterclockwise, including s's pot if encountered, but skipping s's opponent's pot. The function sets the parameters endSide and endHole to the side and hole where the last bean was placed. (This function does not make captures or multiple turns; different Kalah variants have different rules about these issues, so dealing with them should not be the responsibility of the Board class.)

    // Return false if invalid case
    if (hole <= 0 || hole > m_numberOfHoles || beans(s, hole) == 0) {
        return false;
    }

    // Create new variable beanCount to store bean count, set number of beans in hole to 0
    int beanCount = beans(s, hole);
    if (s == NORTH) {
        north[hole] = 0;
    } else if (s == SOUTH) {
        south[hole] = 0;
    }

    // Create new variables to store the current side and hole
    Side currentSide = s;
    int currentHole = hole;
    int switchCounter = 0;

    if (s == SOUTH) {
        southSowHelper(currentSide, currentHole, beanCount, switchCounter);
    } else {// s = NORTH
        northSowHelper(currentSide, currentHole, beanCount, switchCounter);
    }


    // Set endSide and endHole to the correct values
    endSide = currentSide;
    endHole = currentHole;

    return true;
}


void Board::southSowHelper(Side& currentSide, int& currentHole, int& beanCount, int& switchCounter) {
    // Base cases
    if (beanCount == 0) {
        return;
    }

    // Update currentHole; if currentHole = 0, then that means if went over
    currentHole = (currentHole + 1) % (m_numberOfHoles + 1);

    // If current side is south but the board has already switched, we skip the opponent's pot.
    if (currentSide == SOUTH && currentHole == 0 && switchCounter % 2 != 0) {
        currentSide = NORTH;
        switchCounter++;
    }
    
    // If currentHole == 0 (you're in your own pot)
    if (currentHole == 0) {
        // Change the currentSide to the other side of the board
        if (currentSide == SOUTH) {
            south[currentHole]++;
            beanCount--;
            currentSide = NORTH;
            switchCounter++;
            northSowHelper(currentSide, currentHole, beanCount, switchCounter);
        } else { // currentSide = NORTH
            // If the side is north, that means the opponent's pot was drawn. We want to increment the hole and keep sowing the rest of the holes on that side.
            currentHole = m_numberOfHoles;
            north[currentHole]++;
            beanCount--;
            northSowHelper(currentSide, currentHole, beanCount, switchCounter);
        }
    } else { // currentHole != 0; decrement/increment and keep going
        south[currentHole]++;
        beanCount--;
        southSowHelper(currentSide, currentHole, beanCount, switchCounter);
    }
}


void Board::northSowHelper(Side& currentSide, int& currentHole, int& beanCount, int& switchCounter) {
    // Base cases
    if (beanCount == 0) {
        return;
    }

    // Update currentHole to move to the previous hole
    currentHole = (currentHole + m_numberOfHoles) % (m_numberOfHoles + 1);

    // We want to skip our opponent's pot. We set the side to south and let it fall to the else statement.
    if (currentSide == NORTH && currentHole == 0 && switchCounter % 2 != 0) {
        currentSide = SOUTH;
        switchCounter++;
    }

    // If currentHole == 0 (you're in your own pot)
    if (currentHole == 0) {
        if (currentSide == NORTH) {
            north[currentHole]++;
            beanCount--;
            currentSide = SOUTH;
            switchCounter++;
            southSowHelper(currentSide, currentHole, beanCount, switchCounter);
        } else { // currentSide = SOUTH
            // We are in our opponent's pot. We increment and then call the south version (other version) of sow
            currentHole++;
            south[currentHole]++;
            beanCount--;
            southSowHelper(currentSide, currentHole, beanCount, switchCounter); // idk?
        }
    } else { // currentHole != 0; decrement/increment and keep going
        north[currentHole]++;
        beanCount--;
        northSowHelper(currentSide, currentHole, beanCount, switchCounter);
    }
}


bool Board::moveToPot(Side s, int hole, Side potOwner) {
    // If the indicated hole is invalid or a pot, return false without changing anything. Otherwise, move all the beans in hole (s,hole) into the pot belonging to potOwner and return true.

    // Checking invalid cases
    if (hole <= 0 || hole > m_numberOfHoles) {
        return false;
    }
    // If pot owner is north, move beans to north pot
    if (potOwner == NORTH) {
        north[0] += beans(s, hole);
    }
    // If pot owner is south, move beans to south pot
    if (potOwner == SOUTH) {
        south[0] += beans(s, hole);
    }
    // If s is north...
    if (s == NORTH) {
        north[hole] = 0;
    }
    // If s is south...
    if (s == SOUTH) {
        south[hole] = 0;
    }
    return true;
}

bool Board::setBeans(Side s, int hole, int beans) {
    // If the indicated hole is invalid or beans is negative, this function returns false without changing anything. Otherwise, it will return true after setting the number of beans in the indicated hole or pot to the value of the third parameter. (This could change what beansInPlay and totalBeans return if they are called later.)

    // Invalid case checking
    if (hole < 0 || hole > m_numberOfHoles || beans < 0) {
        return false;
    }
    // Set number of beans in indicated hole/pot to the value of the third parameter
    if (s == NORTH) {
        north[hole] = beans;
    }
    if (s == SOUTH) {
        south[hole] = beans;
    }
    return true;
}
