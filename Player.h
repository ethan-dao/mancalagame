//
//  Player.h
//  Proj3_CS32
//
//  Created by Ethan Dao on 5/25/23.
//

#ifndef Player_h
#define Player_h
#include <string>
#include <chrono>
#include "Board.h"
using namespace std;

class Timer
{
  public:
    Timer()
    {
        start();
    }
    void start()
    {
        m_time = std::chrono::high_resolution_clock::now();
    }
    double elapsed() const
    {
        std::chrono::duration<double, std::milli> diff =
                          std::chrono::high_resolution_clock::now() - m_time;
        return diff.count();
    }
  private:
    std::chrono::high_resolution_clock::time_point m_time;
};

// Advance jumpy timer only after jumpInterval calls to elapsed
class JumpyTimer
{
  public:
    JumpyTimer(int jumpInterval)
     : m_jumpInterval(jumpInterval), m_callsMade(0)
    {
        actualElapsed();
    }
    double elapsed()
    {
        m_callsMade++;
        if (m_callsMade == m_jumpInterval)
        {
            m_lastElapsed = m_timer.elapsed();
            m_callsMade = 0;
        }
        return m_lastElapsed;
    }
    double actualElapsed()
    {
        m_lastElapsed = m_timer.elapsed();
        return m_lastElapsed;
    }
  private:
    Timer m_timer;
    int m_jumpInterval;
    int m_callsMade;
    int m_lastElapsed;
};

class Player {
public:
    Player(string name); // Create a Player with the indicated name
    string name() const; // Return the name of the player
    virtual bool isInteractive() const;
    // Return false if the player is a computer player. Return true if the player is human. Most kinds of players will be computer players.
    virtual int chooseMove(const Board& b, Side s) const = 0;
    // Every concrete class derived from this class must implement this function so that if the player were to be playing side s and had to make a move given board b, the function returns the move the player would choose. If no move is possible, return −1.
    virtual ~Player(); // Since this class is designed as a base class, it should have a virtual destructor.
private:
    string m_name;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(string name); // Create a HumanPlayer with the indicated name
    string name() const; // Return the name of the HumanPlayer
    virtual bool isInteractive() const;
    // Return false if the player is a computer player. Return true if the player is human. Most kinds of players will be computer players.
    virtual int chooseMove(const Board& b, Side s) const;
    // Every concrete class derived from this class must implement this function so that if the player were to be playing side s and had to make a move given board b, the function returns the move the player would choose. If no move is possible, return −1.
    virtual ~HumanPlayer();
private:
    string m_name;
};

class BadPlayer : public Player {
public:
    BadPlayer(string name);
    string name() const;
    virtual bool isInteractive() const;
    virtual int chooseMove(const Board& b, Side s) const;
    virtual ~BadPlayer();
private:
    string m_name;
};

class SmartPlayer : public Player {
public:
    SmartPlayer(string name);
    string name() const;
    virtual bool isInteractive() const;
    virtual int chooseMove(const Board& b, Side s, int& bestHole, int& value, JumpyTimer& timer, int depth) const;
    virtual int chooseMove(const Board& b, Side s) const;
    bool completeSow(Side s, int hole, Board& b, Side& endSide, int& endHole) const;
    int evaluate(const Board& b) const;
    virtual ~SmartPlayer();
private:
    string m_name;
};

#endif /* Player_h */
