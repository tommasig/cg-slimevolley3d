/*
* collision.cpp - contains implementation of collision detection routines
*                 on generic objects 
* 
*
*
*
*/


#include <stdlib.h>
#include <stdio.h>
#include "collision.h"

//using namespace std;

// Friend operators for Vector class

ostream& operator <<(ostream& os, const Vector &V) { 
	os << "x: " << V.x << ", y: " << V.y << ", z: " << V.z << endl; 
	return os; 
}

//---------------------------------------------------------------------------
// MAIN INTERSECTION ROUTINES
// --------------------------
// return if two objects intersect, and where on the surface of the objects
// Note : for AABBox vs. AABBox, the test only returns teh separation vector
// -----  This is sufficient, because that's all that is needed in the collison
// -----  response. The other objects return the actual points on the surface
// -----  for demonstration pruposes.
//---------------------------------------------------------------------------
bool CSphere::Intersect(const class CAABBox& xBox,  Vector& xP0, Vector &xP1) const
{
	return xBox.Intersect(*this, xP1, xP0);
}

//---------------------------------------------------------------------------
// Sphere vs. Sphere collision
//---------------------------------------------------------------------------
bool CSphere::Intersect(const CSphere &xSphere, Vector& pP0, Vector& pP1) const
{
	Vector pDist    = xSphere.GetPosition() - GetPosition(); 			// relative position of sphere centre to the box centre
	float dist2 	= pDist * pDist;									// distance of the point on the box to sphere centre, squared
	float r 		= GetRadius() + xSphere.GetRadius();
	float r2    	= r * r;
	if (dist2 > r2) return false;									// point outside sphere, no intersection

	//------------------------------------------------------
	// calcualte point on sphere surface closest to point on box.
	//------------------------------------------------------
	pDist /= sqrt(dist2); // normalise
	pP0    =         GetPosition() + pDist *         GetRadius();
	pP1    = xSphere.GetPosition() - pDist * xSphere.GetRadius();
	return true;
}

//---------------------------------------------------------------------------
// Sphere vs. Semisphere collision
//---------------------------------------------------------------------------
bool CSphere::Intersect(const CSemisphere &xSemisphere, Vector& xP0, Vector& xP1) const {

  Vector dist = xSemisphere.GetPosition() - GetPosition();
  float dist2 = dist * dist;
  float r     = GetRadius() + xSemisphere.GetRadius();
  float r2    = r * r;
  
  Vector p1   = GetPosition() + xSemisphere.GetUpVec() * GetRadius(); // point on the sphere closest to the plane of the semisphere
  Vector distp1 = xSemisphere.GetPosition() - p1;
  float side2 = distp1 * xSemisphere.GetUpVec();
  
  Vector cross = distp1 ^ xSemisphere.GetUpVec();  
  cross.Normalise();
  Vector planar = cross ^ xSemisphere.GetUpVec();                     // unitary vector on the semisphere plane, 
                                                                      // in the direction of collision point
  
  if (dist2 > r2) return false;                                       // too far, no intersection
  
  float side = dist * xSemisphere.GetUpVec();                         // Dot product, gives the side of the sphere related to the plane
                                                                      // of the semisphere
  if (side <= 0) {                                                    // The sphere is colliding with the upper 
                                                                      // side of the semisphere (sphere vs. sphere)
	dist.Normalise();
	xP0 =             GetPosition() + dist *             GetRadius();
	xP1 = xSemisphere.GetPosition() - dist * xSemisphere.GetRadius();
  } else {                                                            // The sphere could collide with the bottom 
                                                                      // side of the semisphere (sphere vs. plane)
	if (side2 > 0) return false;                                      // Too far from the bottom plane of the semisphere
	xP0 = p1;
	xP1 = xSemisphere.GetPosition() + (((-distp1) * planar) * planar);
  }
  
  return true;

}

//---------------------------------------------------------------------------
// tool to calculate the amount of overlap between two spans along an axis
//---------------------------------------------------------------------------
bool AxisIntersect(float min0, float max0, float min1, float max1, float& d)
{
	float d0 = max1 - min0;
 	float d1 = max0 - min1;

	if (d0 < 0.0f || d1 < 0.0f)
 		return false;

	if (d0 < d1)
		d = d0;
    else
    	d = -d1;
					
	return true;
}

//---------------------------------------------------------------------------
// AABBox vs. AABBox intersection test.
// (as stated above, it only returns the separation vector, not the collison points).
//---------------------------------------------------------------------------
bool CAABBox::Intersect(const class CAABBox& xBox, Vector& xP0, Vector &xP1) const
{
	//---------------------------------------------------------------------------
	// calculate the box boundaries, as it is easier that way for the test
 	//---------------------------------------------------------------------------
	Vector xMin0 =      GetPosition() -      GetExt();
	Vector xMax0 =      GetPosition() +      GetExt();
	Vector xMin1 = xBox.GetPosition() - xBox.GetExt();
	Vector xMax1 = xBox.GetPosition() + xBox.GetExt();
	
	//---------------------------------------------------------------------------
	// test intersection along x axis
 	//---------------------------------------------------------------------------
 	Vector N(0, 0, 0);
 	if (!AxisIntersect(xMin0.x, xMax0.x, xMin1.x, xMax1.x, N.x))
 		return false;

	//---------------------------------------------------------------------------
	// test intersection along y axis
 	//---------------------------------------------------------------------------
 	if (!AxisIntersect(xMin0.y, xMax0.y, xMin1.y, xMax1.y, N.y))
 		return false;

	//---------------------------------------------------------------------------
	// test intersection along z axis
 	//---------------------------------------------------------------------------
 	if (!AxisIntersect(xMin0.z, xMax0.z, xMin1.z, xMax1.z, N.z))
 		return false;
 		
	//---------------------------------------------------------------------------
	// select the axis with the minimum of separation as the collision axis
 	//---------------------------------------------------------------------------
  	float mindist = fabs(N.x);
  	
  	if (fabs(N.y) < mindist)
  	{
     	mindist = fabs(N.y);
     	N.x = 0.0f;
  	}
  	else
  	{
 	 	N.y = 0.0f;
  	}
  	if (fabs(N.z) < mindist)
  	{
     	N.x = N.y = 0.0f;
  	}
  	else
  	{
 	 	N.z = 0.0f;
  	}
  	
  	xP0 = Vector(0, 0, 0);
  	xP1 = N;
  	
  	return true;
}

//---------------------------------------------------------------------------
// AABBox vs. Sphere
//---------------------------------------------------------------------------
bool CAABBox::Intersect(const class CSphere& xSphere, Vector& pBox, Vector &pSphere) const
{
	Vector pDiff = xSphere.GetPosition() - GetPosition();  // relative position of sphere centre to the box centre
	Vector pExt = GetExt();									// size of the box along X, Y and Z direction.

	//------------------------------------------------------
 	// see if sphere coords are within the box coords
	//------------------------------------------------------
	float dx = pExt.x - fabs(pDiff.x);	// distance of sphere centre to one of the X-Face of the box
	float dy = pExt.y - fabs(pDiff.y);	// distance of sphere centre to one of the Y-Face of the box
	float dz = pExt.z - fabs(pDiff.z);	// distance of sphere centre to one of the Z-Face of the box

	bool outx  =  (dx < 0.0f);		// sphere centre between the two X-Faces of the box
	bool outy  =  (dy < 0.0f);		// sphere centre between the two Y-Faces of the box
	bool outz  =  (dz < 0.0f);		// sphere centre between the two Z-Faces of the box
	bool in    = !(outx|outy|outz);	// sphere centre inside all the faces of the box

	//------------------------------------------------------
	// sphere centre in the box. deep intersection
	//------------------------------------------------------
	if (in)
	{
		//------------------------------------------------------
		// find closest plane on box to the sphere centre.
		//------------------------------------------------------
		float mindist;

		if (1)																	// one of the X-Face closest to the spehre centre?
		{																		//
			mindist = dx;														//
			pBox    = Vector(dx * sgn(pDiff.x), 0.0f, 0.0f);					// which X-Face of the box closest to sphere
			pSphere = Vector(-xSphere.GetRadius() * sgn(pDiff.x), 0.0f, 0.0f);	// point on the sphere furthest from the face
		}

		if (dy < mindist)														// one of the Y-Face closest to the spehre centre?
		{																		//
			mindist = dy;														//
			pBox    = Vector(0.0f, dy * sgn(pDiff.y), 0.0f);					// which Y-Face of the box closest to sphere
			pSphere = Vector(0.0f, -xSphere.GetRadius() * sgn(pDiff.y), 0.0f);	// point on the sphere furthest from the face
		}
		if (dz < mindist)														// one of the Z-Face closest to the spehre centre?
		{																		//
			mindist = dz;														//
			pBox    = Vector(0.0f, 0.0f, dz * sgn(pDiff.z));					// which Z-Face of the box closest to sphere
			pSphere = Vector(0.0f, 0.0f, -xSphere.GetRadius() * sgn(pDiff.z));	// point on the sphere furthest from the face
		}
		pBox    += xSphere.GetPosition();
		pSphere += xSphere.GetPosition();

		return true;
	}
	//------------------------------------------------------
	// sphere centre not in the box. This is the general case
	//------------------------------------------------------
	else
	{
		//------------------------------------------------------
		// find the closest plane on the box to the sphere
		// (could be a corner, an edge or a face of the box).
		//------------------------------------------------------
		pBox = pDiff;
		if (outx) pBox.x = sgn(pDiff.x) * pExt.x;
		if (outy) pBox.y = sgn(pDiff.y) * pExt.y;
		if (outz) pBox.z = sgn(pDiff.z) * pExt.z;
		pBox += GetPosition();

		//------------------------------------------------------
		// see if the point on  the box surface is in the sphere,
		// by checking the distance of the point from the sphere
		// centre against the sphere radius.
		//------------------------------------------------------
		Vector pDist 	= pBox - xSphere.GetPosition();						// relative position of point in box to the sphere centre
		float dist2 	= pDist * pDist;								// distance of the point on the box to sphere centre, squared
		float r2    	= xSphere.GetRadius() * xSphere.GetRadius();	// radius of sphere, squared
		if (dist2 > r2) return false;									// point outside sphere, no intersection

		//------------------------------------------------------
		// calculate point on sphere surface closest to point on box.
		//------------------------------------------------------
		pDist /= sqrt(dist2); // normalise
		pDist *= xSphere.GetRadius();

		pSphere  = pDist;
		pSphere += xSphere.GetPosition();

		return true;
	}
}

//-----------------------------------------------------------------------------
// AABBox vs. Semisphere
//-----------------------------------------------------------------------------
bool CAABBox::Intersect(const CSemisphere &xSemisphere, Vector& pBox, Vector& pSemisphere) const {

	// it's like an intersection between two AABBox	
	
	// construct the AABBox surrounding the semisphere
	float rad = xSemisphere.GetRadius();
	Vector pos = xSemisphere.GetPosition() + (rad / 2)  * xSemisphere.GetUpVec();
	Vector ext = Vector(rad, rad/2, rad);
	
	CAABBox xBox(pos, ext, xSemisphere.GetMass());
	
	return Intersect(xBox, pBox, pSemisphere);

}

//--------------------------------------------------------------------------
// Semisphere vs. AABBox
//--------------------------------------------------------------------------
bool CSemisphere::Intersect(const class CAABBox& xBox,  Vector& xP0, Vector &xP1) const
{
	return xBox.Intersect(*this, xP1, xP0);
}

//---------------------------------------------------------------------------
// Semisphere vs. Sphere collision
//---------------------------------------------------------------------------
bool CSemisphere::Intersect(const CSphere &xSphere, Vector& pP0, Vector& pP1) const
{
	return xSphere.Intersect(*this, pP1, pP0);
}

//---------------------------------------------------------------------------
// Semisphere vs. Semisphere collision
//---------------------------------------------------------------------------
bool CSemisphere::Intersect(const CSemisphere &xSemisphere, Vector& xP0, Vector& xP1) const {
	
	// not managed
	return false;
}



float frand(float range)
{
	return range * (rand() / (float)RAND_MAX);
}

float sgn(float x)
{
	return (x < 0.0f)? -1.0f : 1.0f;
}

