#include "game.h"
#include <math.h>

// Constructors & destructor
Game::Game() {

}

Game::Game(Ball& ball, Wall& net, Wall &floor, Player players[], CObjectList &olist, Environment *env) {

  m_oBall         = &ball;
  m_oNet          = &net;
  m_oFloor        = &floor;
  m_oPlayers      = players;
  m_oList         = &olist;
  m_oEnv          = env;
  m_iWinningScore = 10 ;
  
  // flags affecting play-mode
  skybox   = true;
  textures = true;
  pause    = false;
  scores   = false;
  winning  = false;
  osaid    = false;
  movecam  = false;
  splash   = true;
  
}

// Friend operators (non-member)
ostream& operator <<(ostream& os, const Game& game) {
//	os << "Position  = " << cam.getPos();
//	os << "Center    = " << cam.getCenter();
//	os << "Up vector = " << cam.getUpVec();
//	os << "FOV       = " << cam.getFov() << ", " ;
//	os << "zNear     = " << cam.getNearPlane() << ", ";
//	os << "zFar      = " << cam.getFarPlane();
	os << endl;
	return os;
}

// Game rules

bool Game::checkBallDown(int &whichone) {

  Vector ballpos = m_oBall->GetPosition();
  
  // the ball touches the ground
  if ( m_oList->TestCollision(m_oBall->GetLabel(), m_oFloor->GetLabel()) ) { 
  
	Vector netpos = m_oNet->GetPosition();

	// where does it fall (which part of the field) ?
	if (ballpos.x < netpos.x) {       // ball is touching the ground on player1's field
	  m_oPlayers[1].increaseScore();  // player2 scores !!
	  whichone = 1; 
	} else {                          // ball is touching the ground on player2's field
	  m_oPlayers[0].increaseScore();  // player1 scores !!
	  whichone = 0; 
	}
	return true;
  
  } else
	return false;

}

bool Game::checkPlayerWin() {

  if (m_oPlayers[0].getScore() == m_iWinningScore) {
	winner = 0;
	return true;
  }
  if (m_oPlayers[1].getScore() == m_iWinningScore) {
	winner = 1;
	return true;
  }
  
  return false;
  
}

// Utilities

// Finds the position on the floor
// where the ball is aiming to
Vector Game::findBallDown() {

  // s = s0 + v * t + 1/2 * a * t^2
  // we must find t that satisfies
  // env->field.ymin = s.y + v.y * t + 1/2 * a.y * t^2
  Vector s = m_oBall->GetPosition();
  Vector v = m_oBall->GetVelocity();
  Vector a = m_oEnv->getGAcc() / 4;
  float t, delta;
  Vector pos = Vector(0, 0, 0);
  
  // we solve the second grade equation in t
  // to find the instant t when the ball collides
  // with the floor
  delta = sqrt( v.y * v.y - 4 * (s.y - m_oEnv->field.ymin) * (a.y / 2));
  t = (- v.y - delta) / a.y;

  // we substitute t to find (x,z) position of collision
  pos.x += s.x + v.x * t;
  pos.y += m_oEnv->field.ymin;
  pos.z += s.z + v.z * t;
  
  return pos;
  
}
