#ifndef BALL_H
#define BALL_H

#include "collision.h"

class Ball: public CSphere {
    
  public:
    
	// Constructors and destructor
	Ball();
	Ball(const Vector& xPos, float radius, float mass);
	~Ball();
	
	// Friend operators (non-member)
	friend ostream& operator <<(ostream& os, const Ball& b); 

	// Getter
	float getMaxSpeed() const;
	
	// Setter
	void setMaxSpeed(float speed);

	// Overriddable methods
	virtual void Render() const;
	//virtual bool ProcessCollision(CObject& xObj, const Vector& xThisPoint, const Vector& xObjPoint);
	virtual void Update(float dt, const Vector& xAcc);
	
	// Rendering details
	void drawShadow(GLfloat r);
	void drawSignal();
	void drawFloorSignal();

  private:
  
	float m_fMaxSpeed;

};

#endif // BALL_H
