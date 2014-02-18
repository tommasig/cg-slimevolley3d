#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "slime.h"
using namespace std;//::string;

// number of available controls for the player
#define K_NUMBER 5

// the 5 possible controls
//#define K_UP	0
//#define K_LEFT	1
//#define K_DOWN	2
//#define K_RIGHT	3
//#define K_JUMP	4
//#define K_ALTJUMP 5

typedef enum {K_UP, K_LEFT, K_DOWN, K_RIGHT, K_JUMP} player_keys; 

// possible states for the keys
//#define K_STATE_UP		0
//#define K_STATE_DOWN	1

typedef enum {K_STATE_UP, K_STATE_DOWN} key_state;

// SDL type for a key code
typedef unsigned short keys_t;

// structure representing a key for players
typedef struct {

  keys_t    code;
  key_state state;
  
} Key;

class Player {

  public:
	// Attributes
	Slime slime;

    // Constructors & destructor
	Player();
	Player(const string &name, const Vector& xPos, keys_t newkeys[K_NUMBER], float radius = 1.0, float mass = 10.0, int score = 0);
	~Player();
	
	// Friend operators (non-member)
	friend ostream& operator <<(ostream& os, const Player& p);
	
	// Getter
	string getName() const;
	int    getScore() const;
	int    getGamesWon() const;
	Key    getKey(player_keys key);
	Key*   getKeys();
	
	// Setter
	void setName(const string &name);
	void zeroScore();
	void increaseScore(int step = 1);
	void decreaseScore(int step = 1);
	void winGame();
	void bindKeys(keys_t newkeys[K_NUMBER]);
	void switchKey(player_keys key);
  
  private:
	// Attributes
	string m_name;
	int    m_score;
	int    m_gameswon;
	Key    keys[K_NUMBER];
	
	// Methods
	void initKeys();
	
};

#endif // PLAYER_H
