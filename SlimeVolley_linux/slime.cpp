#include "slime.h"
#include <math.h>

// Constructors & destructor
Slime::Slime() {

}

Slime::Slime(const Vector& xPos, float radius, float mass) : CSemisphere(xPos, Vector(0, 1, 0), radius, mass) {

  hdir = H_CENTER;
  vdir = V_CENTER;
  jdir = J_DOWN;
  
  jumping = false;
  
  speed = 10.0f;
  
}

Slime::~Slime() {

}

// Getter
float Slime::getSpeed() {
  return speed;
}

// Setter
void Slime::hImpulse(horizontal_direction h) {

  hdir = h;
  
}

void Slime::vImpulse(vertical_direction v) {

  vdir = v;
  
}

void Slime::jImpulse(jump_direction j) {

  jdir = j;
  
}

void Slime::setSpeed(float new_speed) {
  speed = new_speed;
}


// Friend operators (non-member)
ostream& operator <<(ostream& os, const Slime& s) {
	os << "Label     = " << s.GetLabel() << endl;
	os << "Position  = " << s.GetPosition();
	os << "Velocity  = " << s.GetVelocity();
	os << "Up Vector = " << s.GetUpVec();
	os << "Mass      = " << s.GetMass() << endl;
	os << "Radius    = " << s.GetRadius() << endl;
	return os;
}

// Overriddable methods
void Slime::Render() const {
    int m = 10, n = 16;
    float r = m_fRad;
    int npoints = n*m+1;
    #ifndef _WIN32
    	GLfloat v[npoints][3];
    	GLfloat nl[npoints][3];
    #else
    	GLfloat v[161][3];
    	GLfloat nl[161][3];
    #endif
    GLfloat alfa, beta;
    int i, j;
	GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	// scaling factor, to make the texture cover the whole object (no tiling)
	GLfloat factor = 1.0f / (2*r);
	
	GLfloat sgenparams[4];
	GLfloat tgenparams[4];
	// Define s and t planes... 
	// ... for slime1
	if (GetLabel().compare("slime1") == 0) {
	  sgenparams[0] = factor;
	  sgenparams[1] = 0;
	  sgenparams[2] = 0;
	  sgenparams[3] = 1.0 ;
	  
	  tgenparams[0] = 0;
	  tgenparams[1] = 0;
	  tgenparams[2] = factor;
	  tgenparams[3] = 1.0 ;

	} else
	// ... for slime2
	if (GetLabel().compare("slime2") == 0) {
	  sgenparams[0] = -factor;
	  sgenparams[1] = 0;
	  sgenparams[2] = 0;
	  sgenparams[3] = 1.0 ;
	  
	  tgenparams[0] = 0;
	  tgenparams[1] = 0;
	  tgenparams[2] = factor;
	  tgenparams[3] = 1.0 ;
	// ... for a default slime
	} else {
	  sgenparams[0] = 1.0;
	  sgenparams[1] = 0;
	  sgenparams[2] = 0;
	  sgenparams[3] = 1.0 ;
	  
	  tgenparams[0] = 0;
	  tgenparams[1] = 0;
	  tgenparams[2] = 1.0;
	  tgenparams[3] = 1.0 ;
	}
	
	// enabling automatic texture generation
	glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );
	
	if (hasTexture() && isTextureEnabled()) {
	  glEnable( GL_TEXTURE_2D );
	  glBindTexture( GL_TEXTURE_2D, m_aTexture[0]);
	}
			
	// Specify texture coordinate generation planes 
	glTexGenfv( GL_S, GL_OBJECT_PLANE, sgenparams );
	glTexGenfv( GL_T, GL_OBJECT_PLANE, tgenparams );

    // vertex list construction
    alfa = 2*M_PI/n;
    beta = M_PI_2/m;
    for (i=0; i<m; i++) 
        for (j=0; j<n; j++) {
	    v[i*n+j][0] = r*cos(i*beta)*cos(j*alfa) + r;
	    v[i*n+j][1] = r*sin(i*beta);
	    v[i*n+j][2] = -r*cos(i*beta)*sin(j*alfa) + r;
			
	    nl[i*n+j][0] = cos(j*alfa)*cos(i*beta);
	    nl[i*n+j][1] = sin(i*beta);
	    nl[i*n+j][2] = -sin(j*alfa)*cos(i*beta);
	}
    v[npoints-1][1] = 0.0 + r;
    v[npoints-1][2] = r;
    v[npoints-1][3] = 0.0 + r;
    
  if (isVisible()) {
	// rendering
	
	glTranslatef(m_xPos.x - r, m_xPos.y, m_xPos.z - r);
	
	for (i=0; i<m-1; i++) {
  	  glBegin( GL_QUAD_STRIP );
    	for (j=0; j<=n; j++) {
		if (j == n) {
		    glNormal3fv( nl[i*n+j] );
		    glVertex3fv( v[i*n+j] );
		    glNormal3fv( nl[i*n] );
		    glVertex3fv( v[i*n] );
		} else {
		    glNormal3fv( nl[(i+1)*n+j] );
		    glVertex3fv( v[(i+1)*n+j] );
		    glNormal3fv( nl[i*n+j] );
		    glVertex3fv( v[i*n+j] );
		}
	    }
	  glEnd();	
	}
	glBegin( GL_TRIANGLE_FAN );
	    glNormal3f( 0.0, 1.0, 0.0 );
	    glVertex3f( 0.0 + r, r, 0.0 + r );
	    for (j=0; j<=n; j++) {
    		if (j == n) {
		    glNormal3fv( nl[(m-1)*n] );
		    glVertex3fv( v[(m-1)*n] );
		} else {
		    glNormal3fv( nl[(m-1)*n+j] );
		    glVertex3fv( v[(m-1)*n+j] );
		}
	    }
	glEnd();
    
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	
	if (hasTexture() && isTextureEnabled()) {
	  glDisable( GL_TEXTURE_2D );
	}
	
	// rendering 3d eyes (not textured)
	// left eye
	glPushMatrix();
	  glMaterialfv( GL_FRONT, GL_AMBIENT, white );
	  glMaterialfv( GL_FRONT, GL_DIFFUSE, white );
	  glMaterialfv( GL_FRONT, GL_SPECULAR, white );
	  glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	  if (GetLabel().compare("slime1") == 0) {
		glTranslatef(0.8f*2*r, r/2, 0.3f*2*r);
	  } else
	  if (GetLabel().compare("slime2") == 0) {
		glTranslatef(0.2f*2*r, r/2, 0.7f*2*r);
	  }
	  glutSolidSphere(r/4,10,10);
	  
	  glMaterialfv( GL_FRONT, GL_AMBIENT, black );
	  glMaterialfv( GL_FRONT, GL_DIFFUSE, black );
	  glMaterialfv( GL_FRONT, GL_SPECULAR, black );
	  glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	  if (GetLabel().compare("slime1") == 0) {
		glTranslatef(0.75*r/4, 0.3*r/4, -0.2*r/4);
	  } else
	  if (GetLabel().compare("slime2") == 0) {
		glTranslatef(-0.75*r/4, 0.3*r/4, 0.2*r/4);
	  }
	  glutSolidSphere(r/12,10,10);
	glPopMatrix();
	
	// right eye
	glPushMatrix();
	  glMaterialfv( GL_FRONT, GL_AMBIENT, white );
	  glMaterialfv( GL_FRONT, GL_DIFFUSE, white );
	  glMaterialfv( GL_FRONT, GL_SPECULAR, white );
	  glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	  if (GetLabel().compare("slime1") == 0) {
		glTranslatef(0.8f*2*r, r/2, 0.7f*2*r);
	  } else
	  if (GetLabel().compare("slime2") == 0) {
		glTranslatef(0.2f*2*r, r/2, 0.3f*2*r);
	  }
	  glutSolidSphere(r/4,10,10);
	  
	  glMaterialfv( GL_FRONT, GL_AMBIENT, black );
	  glMaterialfv( GL_FRONT, GL_DIFFUSE, black );
	  glMaterialfv( GL_FRONT, GL_SPECULAR, black );
	  glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	  if (GetLabel().compare("slime1") == 0) {
		glTranslatef(0.75*r/4, 0.3*r/4, 0.2*r/4);
	  } else
	  if (GetLabel().compare("slime2") == 0) {
		glTranslatef(-0.75*r/4, 0.3*r/4, -0.2*r/4);
	  }
	  glutSolidSphere(r/12,10,10);
	glPopMatrix();
	
	
	
	
  } // isVisible ?
}

void Slime::Update(float dt, const Vector& xAcc) {
  
  Vector move(0, 0, 0);
  Vector jump(0, 0, 0);

  if (IsStatic())	// only massive objects can be moved
	return;

  switch (vdir) {
	case V_UP:
	  move += Vector(0, 0, -speed);
	  break;
	case V_DOWN:
	  move += Vector(0, 0, speed);
	  break;
	default:
	  break;
  }
  
  switch (hdir) {
	case H_LEFT:
	  move += Vector(-speed, 0, 0);
	  break;
	case H_RIGHT:
	  move += Vector(speed, 0, 0);
	  break;
	default:
	  break;
  }  
  
  switch (jdir) {
	case J_UP:
	  jump += Vector(0, speed, 0);
	  //jumping = true;
	  jImpulse(J_DOWN);
	  break;
	default:
	  break;
  }
  
  move.Normalise();
  Vector nmove = speed * move;
	
  m_xPos += (xAcc * dt * dt) + nmove * dt + m_xVel * dt + jump * dt;
  m_xVel += (xAcc * dt) + jump;
  
}

//----------------------------------------------------------------
// Collision response routine: it differs from the generic one
// for the collision between slime and floor. For this to work
// the order of these objects in the CObjectList must be
// obj1 ... slime ... floor ... objn
// since the ProcessCollision member is called on an object
// against the following objects in the list order.
//----------------------------------------------------------------
bool Slime::ProcessCollision(CObject& xObj, const Vector& xThisPoint, const Vector& xObjPoint)
{
		Vector N = xObjPoint - xThisPoint;			// normal of plane of collision
		Vector vel2 = xObj.GetVelocity();
		Vector pos2 = xObj.GetPosition();
		
		//-------------------------------------------------
		// calculate the amount of collison response for both objects
		//-------------------------------------------------
		float fRatio1, fRatio2;
		if (!CalculateMassRatio(xObj, fRatio1, fRatio2, true))
			return false;
		
		m_xPos += N * fRatio1;					// move the Objects away from each other
  		pos2   -= N * fRatio2;
		xObj.SetPosition(pos2);

  		Vector xVel = m_xVel - vel2;		// Calculate the relative velocity

		float nv = N * xVel;           				// Calculate the impact velocity

		if (nv > 0.0f)								// spheres moving away from each other, so don't reflect
			return false;

       	float n2 = N * N;							// the normal of collision length squared

		if (n2 < 0.00001f)							// too small, can't be of any use
			return false;

		CalculateMassRatio(xObj, fRatio1, fRatio2, false);
		
		float fElasticity = 0.8f;	// coefficient of elasticity
		float fFriction   = 0.1f;	// coefficient of friction
		
		//----------------------------------------------
		// Collision response. Calculate the two velocity components
		//----------------------------------------------
		Vector Vn = N * (nv / n2);		// relative velocity along the normal of collision
		Vector Vt = xVel - Vn;			// tangencial velocity (along the collision plane)

		//----------------------------------------------
		// apply response
		// V = -Vn . (1.0f + CoR) + Vt . CoF
		//----------------------------------------------
		
		if (!IsStatic()) {
		  m_xVel -= ((1.0f + fElasticity) * fRatio1) * Vn + Vt * fFriction;	// reflect the first object (slime)
		  if ( (xObj.GetLabel().compare("floor") == 0) )                    // if slime is colliding with the floor, 
			m_xVel.y = 0;                                                   // then don't bounce
		
		}
		if (!xObj.IsStatic()) {
		  vel2   += ((1.0f + fElasticity) * fRatio2) * Vn + Vt * fFriction;	// reflect the second object
		  xObj.SetVelocity(vel2);
		}
		
		return true;
}

// function to load in bitmap as a GL texture 
bool Slime::LoadGLTexture(GLuint textureArray[], const char *filename, int textureID) {
	    // Status indicator 
	    bool    Status = false;
		GLubyte *image;
		GLsizei imageWidth, imageHeight, components;
		
		// Create The Texture 
		glGenTextures( 1, &textureArray[textureID] );
		
		//textureArray[textureID] = texture;
		
		glBindTexture( GL_TEXTURE_2D, textureArray[textureID] );
		
		if ( (image = (GLubyte *) read_texture(filename, &imageWidth, &imageHeight, &components)) ) {
		  // Set the status to true 
		  Status = true;
		  
		  // Set some texture parameters
		  initSlimeTexture( image, imageWidth, imageHeight );
		}
		  
		free(image);
		
		/*
	    // Create storage space for the texture 
	    SDL_Surface *TextureImage[1]; 

	    // Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit 
	    if ( ( TextureImage[0] = SDL_LoadBMP( filename ) ) ) {

		// Set the status to true 
		Status = true;

		// Create The Texture 
		glGenTextures( 1, &textureArray[textureID] );

		// Typical Texture Generation Using Data From The Bitmap 
		glBindTexture( GL_TEXTURE_2D, textureArray[textureID] );

		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage[0]->w,
			      TextureImage[0]->h, 0, GL_BGR,
			      GL_UNSIGNED_BYTE, TextureImage[0]->pixels );

		// Linear Filtering 
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		
    	    }

	    // Free up any memory we may have used
	    if ( TextureImage[0] )
		SDL_FreeSurface( TextureImage[0] );
		*/
		
	    return Status;
}


void Slime::drawShadow(GLfloat r) {
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

void Slime::drawSignal() {

  GLfloat alfa, delta = 0.01;
  int npoints = 12;
  int j;
  #ifndef _WIN32
  	GLfloat v[npoints][3];
  #else
  	GLfloat v[12][3];
  #endif
  
  
  if (isVisible()) {
	alfa = M_PI/npoints;
	for (j=0; j<npoints; j++) {
	  v[j][0] = GetRadius()*cos(j*alfa);
	  v[j][1] = GetRadius()*sin(j*alfa);
	  v[j][2] = delta;
	}
	
	glBegin( GL_LINE_LOOP );
	  for (j=0; j<=npoints; j++)
		if (j == npoints) 
		  glVertex3f(-GetRadius(), 0, delta );
		else 
		  glVertex3fv( v[j] );
	glEnd();
  }
  
}

