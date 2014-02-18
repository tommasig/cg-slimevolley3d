#ifndef CAMERA_H
#define CAMERA_H

#include "collision.h"

class Camera {
  
  public:

	// Constructors & destructor
	Camera();
	Camera(const Vector& xPos, const Vector& xCenter, float fov, const Vector& upVec, float zNear, float zFar);
	
	// Friend operators (non-member)
	friend ostream& operator <<(ostream& os, const Camera& cam);
	
	// Getter
	Vector getPos() const;
	Vector getCenter() const;
	Vector getUpVec() const;
	float  getFov() const;
	float  getNearPlane() const;
	float  getFarPlane() const;
	
	// Setter
	void setPos(const Vector& xPos);
	void setCenter(const Vector& xCenter);
	void setUpVec(const Vector& upVec);
	void setFov(float fov);
	void setPlanes(float zNear, float zFar);
	
	void MoveCameraByMouse(int swidth, int sheight);

  private:
	Vector m_xPos;
	Vector m_xCenter;
	Vector m_upVec;
	float  m_fov;
	float  m_zNear;
	float  m_zFar;
	
	void RotateView(float X, float Y, float Z);
	
};

#endif // CAMERA_H
