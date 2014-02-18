#ifndef SLIME_H
#define SLIME_H

#include "collision.h"

typedef enum {H_LEFT, H_CENTER, H_RIGHT} horizontal_direction;
typedef enum {V_UP, V_CENTER, V_DOWN} vertical_direction;
typedef enum {J_UP, J_DOWN} jump_direction;

class Slime: public CSemisphere {

  public:
	
	// Attributes
	bool jumping;
	
	// Constructors & destructor
	Slime();
	Slime(const Vector& xPos, float radius, float mass);
	~Slime();
	
	// Getter
	float getSpeed();
	
	// Setter
	void hImpulse(horizontal_direction h);
	void vImpulse(vertical_direction v);
	void jImpulse(jump_direction j);
	void setSpeed(float new_speed);
	
	// Friend operators (non-member)
	friend ostream& operator <<(ostream& os, const Slime& s); 
	
	// Overriddable methods
	virtual void Render() const;
	virtual void Update(float dt, const Vector& xAcc);
	virtual bool ProcessCollision(CObject& xObj, const Vector& xThisPoint, const Vector& xObjPoint);
	
	// Rendering details
	void drawShadow(GLfloat r);
	void drawSignal();
	
  protected:
  
	virtual bool LoadGLTexture(GLuint textureArray[], const char *filename, int textureID);
	
  private:
	
	horizontal_direction hdir;
	vertical_direction   vdir;
	jump_direction       jdir;
	float                speed;
	

};

#endif // SLIME_H
