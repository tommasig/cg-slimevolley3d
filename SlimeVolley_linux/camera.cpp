#include "camera.h"

// Constructors & destructor
Camera::Camera() {

}

Camera::Camera(const Vector& xPos, const Vector& xCenter, float fov, const Vector& upVec, float zNear, float zFar) {
  m_xPos = xPos;
  m_xCenter = xCenter;
  m_upVec = upVec;
  m_fov = fov;
  m_zNear = zNear;
  m_zFar = zFar;
}

// Friend operators (non-member)
ostream& operator <<(ostream& os, const Camera& cam) {
	os << "Position  = " << cam.getPos();
	os << "Center    = " << cam.getCenter();
	os << "Up vector = " << cam.getUpVec();
	os << "FOV       = " << cam.getFov() << ", " ;
	os << "zNear     = " << cam.getNearPlane() << ", ";
	os << "zFar      = " << cam.getFarPlane();
	os << endl;
	return os;
}


// Getter
Vector Camera::getPos() const {
  return m_xPos;
}
	
Vector Camera::getCenter() const {
  return m_xCenter;
}
	
Vector Camera::getUpVec() const {
  return m_upVec;
}
	
float Camera::getFov() const {
  return m_fov;
}

float Camera::getNearPlane() const {
  return m_zNear;
}

float Camera::getFarPlane() const {
  return m_zFar;
}
	
// Setter
void Camera::setPos(const Vector& xPos) {
  m_xPos = xPos;
}
	
void Camera::setCenter(const Vector& xCenter) {
  m_xCenter = xCenter;
}
	
void Camera::setUpVec(const Vector& upVec) {
  m_upVec = upVec;
}
	
void Camera::setFov(float fov) {
  m_fov = fov;
}

void Camera::setPlanes(float zNear, float zFar) {
  m_zNear = zNear;
  m_zFar = zFar;
}

///////////////////////////////// MOVE CAMERA BY MOUSE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    This allows us to look around using the mouse, like in most first person games.
/////   
/////
///////////////////////////////// MOVE CAMERA BY MOUSE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Camera::MoveCameraByMouse(int swidth, int sheight)
{
    int mousePos_x,mousePos_y;
    int middleX = swidth  >> 1;                // This is a binary shift to get half the width
    int middleY = sheight >> 1;                // This is a binary shift to get half the height
    float deltaY  = 0.0f;                            // This is the direction for looking up or down
    float rotateY = 0.0f;                            // This will be the value we need to rotate around the Y axis (Left and Right)
    
    // Get the mouse's current X,Y position
    SDL_GetMouseState(&mousePos_x,&mousePos_y);

    // If our cursor is still in the middle, we never moved... so don't update the screen
    if( (mousePos_x == middleX) && (mousePos_y == middleY) ) return;

    // Set the mouse position to the middle of our window
    SDL_WarpMouse(middleX, middleY);

    // Get the direction the mouse moved in, but bring the number down to a reasonable amount
    rotateY = (float)( (middleX - mousePos_x) ) / 1000;        
    deltaY  = (float)( (middleY - mousePos_y) ) / 1000;

    // Multiply the direction vVector for Y by an acceleration (The higher the faster is goes).
    m_xCenter.y += deltaY * 8;

    // Check if the distance of our view exceeds 10 from our position, if so, stop it. (UP)
    if( ( m_xCenter.y - m_xPos.y ) >  10)  m_xCenter.y = m_xPos.y + 10;

    // Check if the distance of our view exceeds -10 from our position, if so, stop it. (DOWN)
    if( ( m_xCenter.y - m_xPos.y ) < -10)  m_xCenter.y = m_xPos.y - 10;

    // Here we rotate the view along the X avis depending on the direction (Left of Right)
    RotateView(0, -rotateY, 0);
}

///////////////////////////////// ROTATE VIEW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////    This rotates the view around the position
/////
///////////////////////////////// ROTATE VIEW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Camera::RotateView(float X, float Y, float Z)
{
    Vector vVector;                            // Vector for the position/view.

    // Get our view vVector (The direciton we are facing)
    vVector.x = m_xCenter.x - m_xPos.x;        // This gets the direction of the X    
    vVector.y = m_xCenter.y - m_xPos.y;        // This gets the direction of the Y
    vVector.z = m_xCenter.z - m_xPos.z;        // This gets the direction of the Z

    // Rotate the view along the desired axis
    if(X) {
        // Rotate the view vVector up or down, then add it to our position
        m_xCenter.z = (float)(m_xPos.z + sin(X)*vVector.y + cos(X)*vVector.z);
        m_xCenter.y = (float)(m_xPos.y + cos(X)*vVector.y - sin(X)*vVector.z);
    }
    if(Y) {
        // Rotate the view vVector right or left, then add it to our position
        m_xCenter.z = (float)(m_xPos.z + sin(Y)*vVector.x + cos(Y)*vVector.z);
        m_xCenter.x = (float)(m_xPos.x + cos(Y)*vVector.x - sin(Y)*vVector.z);
    }
    if(Z) {
        // Rotate the view vVector diagnally right or diagnally down, then add it to our position
        m_xCenter.x = (float)(m_xPos.x + sin(Z)*vVector.y + cos(Z)*vVector.x);        
        m_xCenter.y = (float)(m_xPos.y + cos(Z)*vVector.y - sin(Z)*vVector.x);
    }
}

