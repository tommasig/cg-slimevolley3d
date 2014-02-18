#ifndef GAME_H
#define GAME_H

#include "collision.h"
#include "ball.h"
#include "wall.h"
#include "player.h"
#include "env.h"
#include "SDL.h"

class Game {
  
  public:

	bool skybox;
	bool textures;
	bool pause;
	bool scores;
	bool winning;
	int  winner;
	bool osaid;    // on-screen aids
	bool movecam;  // camera is movable
	bool splash;   // showing splash screen
	
	// Constructors & destructor
	Game();
	Game(Ball& ball, Wall& net, Wall& floor, Player players[], CObjectList &olist, Environment *env);
	
	// Friend operators (non-member)
	friend ostream& operator <<(ostream& os, const Game& game);
	
	// Getter
	//Vector getPos() const;
	
	// Setter
	//Uint32 (*SDL_NewTimerCallback) (Uint32 interval, void *param);
	
	// Game rules
	bool   checkBallDown(int &whichone);
	bool   checkPlayerWin();
	
	// Utilities
	Vector findBallDown();
	
  private:
  
	Ball        *m_oBall;
	Wall        *m_oNet;
	Wall        *m_oFloor;
	Player      *m_oPlayers;
	CObjectList *m_oList;
	Environment *m_oEnv;
	int          m_iWinningScore;
	
};

#endif // GAME_H
