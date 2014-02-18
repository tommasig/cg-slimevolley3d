#ifndef WALL_H
#define WALL_H

#include "collision.h"
//#include <iostream>

class Wall: public CAABBox {

  public:
	
	// Constructors & destructor
	Wall();
//	Wall(Wall& newWall);                                  // copy constructor
	Wall(const Vector &xPos, const Vector &xExt);
	~Wall();
	
	// Operators
	friend ostream& operator <<(ostream& os, const Wall &W);
	
	// Overriddables methods
	virtual void Render() const;
	
};

#endif // WALL_H
