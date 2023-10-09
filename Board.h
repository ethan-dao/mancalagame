//
//  Board.h
//  Proj3_CS32
//
//  Created by Ethan Dao on 5/25/23.
//

#ifndef Board_h
#define Board_h
#include <vector>
using namespace std;
#include "Side.h"

class Board {
public:
    Board(int nHoles, int nInitialBeansPerHole);
    Board(const Board& other); // Copy constructor, vector already defines a destructor and assignment operator
    Board& operator=(const Board& other); // Assignment operator
    int holes() const;
    int beans(Side s, int hole) const;
    int beansInPlay(Side s) const;
    int totalBeans() const;
    bool sow(Side s, int hole, Side& endSide, int& endHole);
    void southSowHelper(Side& currentSide, int& currentHole, int& beanCount, int& switchCounter);
    void northSowHelper(Side& currentSide, int& currentHole, int& beanCount, int& switchCounter);
//    void southSowHelper(Side& currentSide, int& currentHole, int& beanCount);
//    void northSowHelper(Side& currentSide, int& currentHole, int& beanCount);
    bool moveToPot(Side s, int hole, Side potOwner);
    bool setBeans(Side s, int hole, int beans);
private:
    int m_numberOfHoles;
    int m_initialNumberOfBeans;
    vector<int> north;
    vector<int> south;
};

#endif /* Board_h */
