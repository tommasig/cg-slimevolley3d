/*
* collision.h - Contains classes definition for our collision detection routines
*
* class Vector      : vectors definition
* class CObject     : generic collision object
* class CObjectList : list of generic objects, for collision management and rendering purposes
* class CSphere     : generic collision sphere
* class CSemisphere : generic collision semisphere
* class CAABBox     : generic collision Axis Aligned Bounding Box
*
* CObject ------> CSphere | ---> CSemisphere ---> Slime
*         \               | \
*          \              |  \
*           \             |   -> Ball
*            \            |
*             \           | 
*              -> CAABBox | ---> Wall
*/

#ifndef COLLISION_H
#define COLLISION_H

#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <string>
#include "SDL.h"
#include "texture.h"
#include "main.h"

using namespace std;


extern float frand(float range); // float random number generator
extern float sgn(float x);       // returns 1.0f if positive, -1.0f if negative 

//===========================================================================
// VECTORS
//===========================================================================
class Vector
{
public:
	float x,y,z;

public:
	inline Vector(void)
	{}

	inline Vector(float Ix,float Iy,float Iz): x(Ix), y(Iy), z(Iz) {}

	inline Vector &operator /=(const float Scalar) { *this *= (1.0f / Scalar);	return *this; }

	inline Vector &operator *=(const float Scalar) { x *= Scalar; y *= Scalar; z *= Scalar;	return *this; }

	inline Vector &operator +=(const Vector &Other) { x += Other.x;	y += Other.y;	z += Other.z; return *this; }

	inline Vector &operator -=(const Vector &Other) { x -= Other.x;	y -= Other.y;	z -= Other.z; return *this;	}

	inline Vector& operator ^=(const Vector &V) // Cross product
	{
		float Tempx	= (y * V.z) - (z * V.y);
		float Tempy	= (z * V.x) - (x * V.z);
		      z		= (x * V.y) - (y * V.x);
		      x     = Tempx;
		      y     = Tempy;

		return *this;
	}

	inline Vector operator ^ (const Vector& V) const {	Vector Temp(*this); return Temp ^= V; }

	inline Vector operator * (float  s)		   const {	Vector Temp(*this); return Temp *= s; };

	inline Vector operator / (float  s)		   const {	Vector Temp(*this); return Temp /= s; }

	inline Vector operator + (const Vector &V) const {	Vector Temp(*this); return Temp += V; }

	inline Vector operator - (const Vector &V) const {	Vector Temp(*this); return Temp -= V; }

	friend Vector operator * (float k, const Vector& V) { return V * k; } // dot product

	inline float operator * (const Vector &V) const { return (x * V.x) + (y * V.y) + (z * V.z); }

	inline Vector operator -(void) const { return Vector(-x, -y, -z); }
	
	friend ostream& operator <<(ostream& os, const Vector &V);
	
	//istream& operator >>(istream& is, Vector &V) { 
	//	is >> V.x;
	//	is >> V.y;
	//	is >> V.z;
	//	return is; 
	//}

	inline float GetLength(void) const { return (float) sqrt((*this) * (*this)); }

	float Normalise()
	{
		float Length = GetLength();

		if (Length == 0.0f)
			return 0.0f;

		(*this) *= (1.0f / Length);

		return Length;
	}

	static Vector Random(const Vector& Radius=Vector(1.0f, 1.0f, 1.0f))
	{
		return Vector(frand(Radius.x), frand(Radius.y), frand(Radius.z));
	}

	static Vector Random(float radius)
	{
		return Vector(frand(radius), frand(radius), frand(radius));
	}

	//-------------------------------------------------------------------------
	// Compute normal of a triangle. return normal length
	//-------------------------------------------------------------------------
	float ComputeNormal(const Vector& V0, const Vector& V1, const Vector& V2)
	{
		Vector E = V1; E -= V0;
		Vector F = V2; F -= V1;

		(*this)  = E ^ F;

		return (*this).Normalise();
	}

	void Render(void) const
	{
		glPointSize(3.0f);
		glBegin(GL_POINTS);
		glVertex3fv(&x);
		glEnd();
	}
	static void Render(const Vector& V0, const Vector& V1)
	{
		V0.Render();
		V1.Render();
		glBegin(GL_LINES);
		glVertex3fv(&V0.x);
		glVertex3fv(&V1.x);
		glEnd();
	}
};


//---------------------------------------------------------------------
// Base collision object class
//---------------------------------------------------------------------
class CObject
{
public:

	//---------------------------------------------------------------------
	// Constructors
	//---------------------------------------------------------------------
	CObject()
	{}
	
	CObject(const Vector& xPos, const Vector& xVel, float fMass)
	: m_xPos(xPos)
	, m_xVel(xVel)
	, m_fMass(fMass)
	, m_bVisible(true)
	, m_bTexturized(false)
	, m_bTextureEnabled(true)
	, m_sLabel("generic object")
	{
	}

	//---------------------------------------------------------------------
	// Modifiers / Selectors
	//---------------------------------------------------------------------
 	bool IsStatic(void) const { return (m_fMass < 0.0000001f); }

	const Vector& GetPosition     () const { return m_xPos; }
	const Vector& GetVelocity     () const { return m_xVel; }
	float         GetMass         () const { return m_fMass; }
	bool          isVisible       () const { return m_bVisible; }
	bool          hasTexture      () const { return m_bTexturized; }
	bool          isTextureEnabled() const { return m_bTextureEnabled; }
	string        GetLabel        () const { return m_sLabel; } 

	void SetPosition      (const Vector& xPos)       { m_xPos = xPos;   }
 	void SetVelocity      (const Vector& xVel)       { m_xVel = xVel;   }
  	void SetMass          (float         fMass)      { m_fMass = fMass; }
	void SetVisibility    (bool          visibility) { m_bVisible = visibility; }
	bool SetRGBTexture    (const string &name)       { return (m_bTexturized = LoadGLTexture(m_aTexture, name.c_str(), 0)); }
	void SetTextureEnabled(bool enable)              { m_bTextureEnabled = enable; }
	void SetLabel         (const string &label)      { m_sLabel = label; }
   
	//---------------------------------------------------------------------
	// Member functions
	//---------------------------------------------------------------------
   	virtual void Update(float dt, const Vector& xAcc)
	{
		if (IsStatic())	// only massive objects can be moved
			return;
			
		m_xPos += (xAcc * dt * dt) + (m_xVel * dt);
		m_xVel += (xAcc * dt);
	}
	
	//---------------------------------------------------------------------
	// Virtual Interface
	//---------------------------------------------------------------------
	virtual void Render(void) const = 0;
	virtual bool Intersect(const       CObject& xObject, Vector& xP0, Vector &xP1) const = 0;
	virtual bool Intersect(const class CSphere& xSphere, Vector& xP0, Vector &xP1) const = 0;
	virtual bool Intersect(const class CAABBox& xBox,    Vector& xP0, Vector &xP1) const = 0;
	virtual bool Intersect(const class CSemisphere& xSemisphere, Vector& xP0, Vector& xP1) const = 0;

	//-------------------------------------------------
	// process the collision response on two objects
	//-------------------------------------------------
    virtual bool ProcessCollision(CObject& xObj, const Vector& xThisPoint, const Vector& xObjPoint)
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

  		Vector xVel = m_xVel - vel2;		// Calcualte the relative velocity

		float nv = N * xVel;           				// Calcualte the impact velocity

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
		if (!IsStatic())
		  m_xVel -= ((1.0f + fElasticity) * fRatio1) * Vn + Vt * fFriction;	// reflect the first sphere
		if (!xObj.IsStatic()) {
		  vel2   += ((1.0f + fElasticity) * fRatio2) * Vn + Vt * fFriction;	// reflect the second sphere
		  xObj.SetVelocity(vel2);
		}
		
		return true;
    }
private:
	
protected:
	Vector m_xPos;
	Vector m_xVel;
	float  m_fMass;
	bool   m_bVisible;
	GLuint m_aTexture[1];
	bool   m_bTexturized;
	bool   m_bTextureEnabled;
	string m_sLabel;

	//-------------------------------------------------
	// calculate the amount of collison response for both objects
	// based on the ratio of mass
	// if one of the object is static (mass = 0.0f)
	// then the amount of response will be maximum on the mobile object
	// to avoid squqshing objects against static walls,
	// you can set the amount of impulse equally apart when separating objects
	//-------------------------------------------------
	bool CalculateMassRatio(CObject& xObj, float &fRatio1, float& fRatio2, bool bNormalise=false)
	{
		float m = (GetMass() + xObj.GetMass());

		if (m < 0.000001f)
			return false;
			
		else if (xObj.GetMass() < 0.0000001f)
		{
			fRatio1 = 1.0f;
			fRatio2 = 0.0f;
		}
		else if (GetMass() < 0.0000001f)
		{
			fRatio1 = 0.0f;
			fRatio2 = 1.0f;
		}
		else
		{
			if (bNormalise)
			{
				fRatio1 = 0.5f;
				fRatio2 = 1.0f - fRatio1;
			}
			else
			{
				fRatio1 = xObj.GetMass() / m;
				fRatio2 = 1.0f - fRatio1;
			}
		}

		return true;
	}

	// function to load in bitmap as a GL texture 
	virtual bool LoadGLTexture(GLuint textureArray[], const char *filename, int textureID) {
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
		  initTexture( image, imageWidth, imageHeight );
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


};


//---------------------------------------------------------------------------
// Object list management
//
// Maintain a list of objects, for collision and rendering
//---------------------------------------------------------------------------
class CObjectList
{
public:
	//---------------------------------------------------------------------------
	// constructors
 	//---------------------------------------------------------------------------
	CObjectList()
	: m_iNumObjects(0)
	{}
	
	//---------------------------------------------------------------------------
	// update all the objects
	//---------------------------------------------------------------------------
	void Update(float dt, const Vector& xAcc)
	{
			
		int i;

		//---------------------------------------------------------------------------
		// update all the objects positions
		//---------------------------------------------------------------------------
		for( i = 0; i < m_iNumObjects; i ++)
		{
			m_pxObjects[i]->Update(dt, xAcc);
		}

		//---------------------------------------------------------------------------
		// test all objects against each other and process all collisions
		//---------------------------------------------------------------------------
		for( i = 0; i < m_iNumObjects; i ++)
		{
			for (int j = i+1; j < m_iNumObjects; j ++)
			{
				//---------------------------------------------------------------------------
    			// test collision between two objects
    			//---------------------------------------------------------------------------
				if (m_pxObjects[i]->IsStatic() && m_pxObjects[j]->IsStatic())
					continue;

				//---------------------------------------------------------------------------
    			// process the collision
    			//---------------------------------------------------------------------------
     			Vector xP0;
     			Vector xP1;
     			if (m_pxObjects[i]->Intersect(*m_pxObjects[j], xP0, xP1))
     			{
     				m_pxObjects[i]->ProcessCollision(*m_pxObjects[j], xP0, xP1);
     			}
			}
		}
	  
	}

	//---------------------------------------------------------------------------
	// Test collision between two given objects
	//---------------------------------------------------------------------------
	bool TestCollision(string obj1, string obj2) {
	  
	  int i; 
	  
	  for( i = 0; i < m_iNumObjects; i ++) 
		for (int j = i+1; j < m_iNumObjects; j ++) {
		  if (    ((m_pxObjects[i]->GetLabel().compare(obj1) == 0 ) && (m_pxObjects[j]->GetLabel().compare(obj2) == 0 )) 
		       || ((m_pxObjects[j]->GetLabel().compare(obj1) == 0 ) && (m_pxObjects[i]->GetLabel().compare(obj2) == 0 )) ) {
		
			  Vector xP0;
     		  Vector xP1;
			  return (m_pxObjects[i]->Intersect(*m_pxObjects[j], xP0, xP1));
		  
		  }	
		} 
	
	}

	//---------------------------------------------------------------------------
	// render all the objects
	//---------------------------------------------------------------------------
	void Render() const
	{
		for(int i = 0; i < m_iNumObjects; i ++)
		{
			m_pxObjects[i]->Render();
		}
	}
	//---------------------------------------------------------------------------
	// Add object reference to list
	//---------------------------------------------------------------------------
	void RegisterObject(CObject* pxNewObject)
	{
		for(int i = 0; i < m_iNumObjects; i ++)
		{
			if (m_pxObjects[i] == pxNewObject)
				return;
		}

		if (m_iNumObjects >= eMaxObjects)
			return;

  		m_pxObjects[m_iNumObjects] = pxNewObject;
  		m_iNumObjects++;
	}

	//---------------------------------------------------------------------------
	// remove object reference to list
	//---------------------------------------------------------------------------
	void UnregisterObject(CObject* pxOldObject)
	{
		int i;
		for(i = 0; i < m_iNumObjects; i ++)
		{
			if (m_pxObjects[i] == pxOldObject)
				break;
		}
		if (i == m_iNumObjects)
			return;

		m_iNumObjects--;
		m_pxObjects[i] = m_pxObjects[m_iNumObjects];
	}
private:
	enum { eMaxObjects = 128 };
	CObject* m_pxObjects[eMaxObjects];
	int m_iNumObjects;
};

//---------------------------------------------------------------------------
// Collision sphere structure
//---------------------------------------------------------------------------
class CSphere: public CObject
{
public:
	//---------------------------------------------------------------------------
	// Constructors
	//---------------------------------------------------------------------------
	CSphere()
	{}

	CSphere(const Vector& xPos, float fRad, float fMass)
	: CObject(xPos, Vector(0, 0, 0), fMass)
	, m_fRad(fRad)
	{}

	//---------------------------------------------------------------------
	// Modifiers / Selectors
	//---------------------------------------------------------------------
	float GetRadius() const { return m_fRad; }

	//---------------------------------------------------------------------
	// Virtual Interface
	//---------------------------------------------------------------------
	virtual bool Intersect(const CObject& xObject, Vector& xP0, Vector &xP1) const
 	{
  		return xObject.Intersect(*this, xP1, xP0);
  	}
	virtual bool Intersect(const class CAABBox& xBox,  Vector& xP0, Vector &xP1) const;
	virtual bool Intersect(const CSphere &xSphere, Vector& xP0, Vector& xP1) const;
	virtual bool Intersect(const CSemisphere &xSemisphere, Vector& xP0, Vector& xP1) const;	
	virtual void Render(void) const
	{
		 	
		glPushMatrix();
		glTranslatef(m_xPos.x, m_xPos.y, m_xPos.z);
		glutSolidSphere(m_fRad, 8, 8);
		glPopMatrix();
	}

protected:
	float  m_fRad;
};

//-------------------------------------------------------------------------
// Semisphere object
//-------------------------------------------------------------------------
class CSemisphere: public CSphere {

  public:
	CSemisphere()
	{}
	
	CSemisphere(const Vector& xPos, const Vector& xUp, float fRad, float fMass)
	: CSphere(xPos, fRad, fMass)
	, m_xUp(xUp)
	{}
	
	// 
	const Vector& GetUpVec() const { return m_xUp; }
	
	// Virtual interface
	virtual bool Intersect(const CObject& xObject, Vector& xP0, Vector &xP1) const
 	{
  		return xObject.Intersect(*this, xP1, xP0);
  	}
	virtual bool Intersect(const class CAABBox& xBox,  Vector& xP0, Vector &xP1) const;
	virtual bool Intersect(const CSphere &xSphere, Vector& xP0, Vector& xP1) const;
	virtual bool Intersect(const CSemisphere &xSemisphere, Vector& xP0, Vector& xP1) const;
	virtual void Render(void) const
	{}

  protected:
	Vector m_xUp; // must be normalised, always
  
};

//---------------------------------------------------------------------------
// Axis-aligned box collision object
//---------------------------------------------------------------------------
class CAABBox: public CObject
{
public:
	//---------------------------------------------------------------------------
	// Constructors
	//---------------------------------------------------------------------------
	CAABBox()
	{}

	CAABBox(const Vector& xPos, const Vector& xExt, float fMass)
	: CObject(xPos, Vector(0, 0, 0), fMass)
	, m_xExt(xExt)
	{}

	//---------------------------------------------------------------------
	// Modifiers / Selectors
	//---------------------------------------------------------------------
	const Vector& GetExt() const { return m_xExt; }

	//---------------------------------------------------------------------
	// Virtual Interface
	//---------------------------------------------------------------------
	virtual bool Intersect(const CObject& xObject, Vector& xP0, Vector &xP1) const
 	{
 		return xObject.Intersect(*this, xP1, xP0);
    }
	virtual bool Intersect(const class CSphere& xSphere, Vector& pBox, Vector &pSphere) const;
	virtual bool Intersect(const class CAABBox& xBox, Vector& xP0, Vector &xP1) const;
	virtual bool Intersect(const CSemisphere &xSemisphere, Vector& xP0, Vector& xP1) const;
	virtual void Render(void) const
	{
		
		if (IsStatic())
		 	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    	else
		 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      	glPushMatrix();
		glTranslatef(m_xPos.x, m_xPos.y, m_xPos.z);
		glScalef(m_xExt.x, m_xExt.y, m_xExt.z);
		glutSolidCube(2.0f);
		glPopMatrix();
	}

protected:
	Vector m_xExt;
};


#endif // COLLISION_H
