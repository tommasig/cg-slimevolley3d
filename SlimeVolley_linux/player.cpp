
#include "player.h"
using namespace std;//::string;

// Constructors & destructor
Player::Player() {
  slime = Slime();
  
  initKeys();
  
}

Player::Player(const string& name, const Vector& xPos, keys_t newkeys[K_NUMBER], float radius /*= 1.0*/, float mass /*= 10.0*/, int score /* = 0*/) {
  m_name = name;
  m_score = score;
  m_gameswon = 0;
  slime = Slime(xPos, radius, mass);
  
  initKeys();
  bindKeys(newkeys);
  
}

Player::~Player() {
  
}

// Friend operators (non-member)
ostream& operator <<(ostream& os, const Player& p) {
	os << "Name  = " << p.getName() << endl;
	os << "Score = " << p.getScore() << endl;
	os << "Games = " << p.getGamesWon() << endl;
	os << p.slime;
	return os;
}


// Getter

string Player::getName() const {
  return m_name;
}

int Player::getScore() const {
  return m_score;
}

int Player::getGamesWon() const {
  return m_gameswon;
}

Key Player::getKey(player_keys key) {
  return keys[key];
}

Key* Player::getKeys() {
  return keys;
}

// Setter

void Player::setName(const string& name) {
  m_name = name;
}

void Player::zeroScore() {
  m_score = 0;
}

void Player::increaseScore(int step /* = 1*/) {
  m_score += step;
}

void Player::decreaseScore(int step /* = 1*/) {
  m_score -= step;
}

void Player::winGame() {
  m_gameswon++;
}

void Player::bindKeys(keys_t newkeys[K_NUMBER]) {
  
  for (int i=0; i<K_NUMBER; i++) 
	keys[i].code = newkeys[i];
  
}

void Player::initKeys() {

  for (int i=0; i<K_NUMBER; i++) {
	keys[i].code = 0;
	keys[i].state = K_STATE_UP;
  }

}

void Player::switchKey(player_keys key) {
  if (keys[key].state == K_STATE_UP)
	keys[key].state = K_STATE_DOWN;
  else
	keys[key].state = K_STATE_UP;
}
