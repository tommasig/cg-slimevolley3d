#include "ball.h"

// Constructors and destructor

Ball::Ball() {

}

Ball::Ball(const Vector& xPos, float radius, float mass) : CSphere(xPos, radius, mass) {
  
  m_fMaxSpeed = 8.0f;

}

Ball::~Ball() {

}

// Friend operators (non-member)
ostream& operator <<(ostream& os, const Ball& b) {
	os << "Label     = " << b.GetLabel() << endl;
	os << "Position  = " << b.GetPosition();
	os << "Velocity  = " << b.GetVelocity();
	os << "Max Speed = " << b.getMaxSpeed() << endl;
	os << "Mass      = " << b.GetMass() << endl;
	os << "Radius    = " << b.GetRadius() << endl;
	return os;
}

// Getter
float Ball::getMaxSpeed() const {
  return m_fMaxSpeed;
}

// Setter
void Ball::setMaxSpeed(float speed) {
  m_fMaxSpeed = speed;
}

// Overriddable methods

void Ball::Render() const {
    
    if (isVisible()) {
	  //glPushMatrix();
	  glTranslatef(m_xPos.x, m_xPos.y, m_xPos.z);
	  glutSolidSphere(m_fRad, 15, 15);
	  //glPopMatrix();
    }

}

/*
//-------------------------------------------------
// process the collision response on two objects
//-------------------------------------------------
    bool Ball::ProcessCollision(CObject& xObj, const Vector& xThisPoint, const Vector& xObjPoint)
    {
		Vector N = xObjPoint - xThisPoint;			// normal of plane of collision
		Vector vel = xObj.GetVelocity();
		Vector pos = xObj.GetPosition();

		//-------------------------------------------------
		// calculate the amount of collison response for both objects
		//-------------------------------------------------
		float fRatio1, fRatio2;
		if (!CalculateMassRatio(xObj, fRatio1, fRatio2, true))
			return false;
		
		m_xPos += N * fRatio1;					// move the Objects away from each other
  		pos -= N * fRatio2;
		xObj.SetPosition(pos);

  		Vector xVel = m_xVel - vel;		// Calcualte the relative velocity

		float nv = N * xVel;           				// Calcualte the impact velocity

		if (nv > 0.0f)								// spheres moving away from each other, so don't reflect
			return false;

       	float n2 = N * N;							// the normal of collision length squared

		if (n2 < 0.00001f)							// to small, can't be of any use
			return false;

		CalculateMassRatio(xObj, fRatio1, fRatio2, false);
		
		float fElasticity = 1.0f;	// coefficient of elqsticity
		float fFriction   = 0.1f;	// coefficient of friction
		
		//----------------------------------------------
		// Collision response. Calculate the two velocity components
		//----------------------------------------------
		Vector Vn = N * (nv / n2);		// relative velocity alon the normal of collision
		Vector Vt = xVel - Vn;			// tangencial velocity (along the collision plane)

		//----------------------------------------------
		// apply response
		// V = -Vn . (1.0f + CoR) + Vt . CoF
		//----------------------------------------------
		m_xVel -= ((1.0f + fElasticity) * fRatio1) * Vn + Vt * fFriction;	// reflect the first sphere
		vel += ((1.0f + fElasticity) * fRatio2) * Vn + Vt * fFriction;	// reflect the second sphere
		xObj.SetVelocity(vel);

		return true;
    }
*/

void Ball::Update(float dt, const Vector& xAcc)
{
	if (IsStatic())	// only massive objects can be moved
		return;
		
	// ball is not affected by the same gravity 
	Vector acc = xAcc / 4;
	m_xPos += (acc * dt * dt) + (m_xVel * dt);
	m_xVel += (acc * dt);
	
	// clamp of the ball velocity
	if (m_xVel.GetLength() > m_fMaxSpeed) {
	  m_xVel.Normalise();
	  m_xVel *= m_fMaxSpeed;
	}
}

void Ball::drawShadow(GLfloat r) {
    int npoints = 12;
    int j;
    #ifndef _WIN32
    	GLfloat v[npoints][3];
    #else
	GLfloat v[12][3];
    #endif
    GLfloat alfa, delta = 0.01;
    GLfloat nl[] = {0.0, 1.0, 0.0};

    if (isVisible()) {
	  alfa = 2*M_PI/npoints;
	  // punti sulla circonferenza di raggio r con centro nell'origine
      for (j=0; j<npoints; j++) {
        v[j][0] = r*cos(j*alfa);
        v[j][1] = delta;
        v[j][2] = -r*sin(j*alfa);
	  }

	  glBegin( GL_POLYGON );
      for (j=0; j<=npoints; j++){
    	if (j == npoints) {
      	    glNormal3fv( nl );
    	    glVertex3fv( v[0] );
    	} else {
      	    glNormal3fv( nl );
      	    glVertex3fv( v[j] );
    	}
      }
	  glEnd();
    }
}

void Ball::drawSignal() {
  GLfloat width; 

	if (isVisible()) {
	  glGetFloatv( GL_LINE_WIDTH, &width );
	  glLineWidth( 2.5f );
	  glBegin( GL_LINES );
		glVertex3f(-m_fRad /2, m_fRad /2, 0);
		glVertex3f(m_fRad /2, -m_fRad /2, 0);
		
		glVertex3f(-m_fRad /2, -m_fRad /2, 0);
		glVertex3f(m_fRad /2, m_fRad /2, 0);
	  glEnd();
	  glLineWidth( width );
	
	}

}

void Ball::drawFloorSignal() {

  GLfloat width; 
  float rext = m_fRad * m_xPos.y / 2;
  float rint = rext / 2;
  GLfloat vext[8][3];
  GLfloat vint[8][3];
  int npoints = 8;
  int j;
  float alfa;

	if (isVisible()) {
	  alfa = 2*M_PI/npoints;
	  // punti sulla circonferenza di raggio r con centro nell'origine
      for (j=0; j<npoints; j++) {
        vext[j][0] = rext*cos(j*alfa);
        vext[j][1] = -rext*sin(j*alfa);
        vext[j][2] = 0;
		
		vint[j][0] = rint*cos(j*alfa);
        vint[j][1] = -rint*sin(j*alfa);
        vint[j][2] = 0;
	  }
	
	  
	  glGetFloatv( GL_LINE_WIDTH, &width );
	  glLineWidth( 2.5f );
	  glBegin( GL_LINES );
		
		for (j=0; j<npoints; j++) {
		  glVertex3fv( vext[j] );
		  glVertex3fv( vint[j] );
		}
		
	  glEnd();
	  glLineWidth( width );
	
	}

}
