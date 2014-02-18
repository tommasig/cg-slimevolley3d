#include "collision.h"
#include "wall.h"
#include <GL/glut.h>

Wall::Wall() {

}
/*
Wall::Wall(Wall& newWall) { 
	m_xPos = newWall.GetPosition();
	m_xVel = newWall.GetVelocity();
	m_xExt = newWall.GetExt();
	m_fMass = newWall.GetMass();
	m_bVisible = newWall.isVisible();
} 
*/
Wall::Wall(const Vector &xPos, const Vector &xExt) : CAABBox(xPos, xExt, 0.0f /* mass */) {
	
}

Wall::~Wall() {

}

// Friend operators

ostream& operator <<(ostream& os, const Wall& W) {
	os << "Label     = " << W.GetLabel() << endl;
	os << "Position  = " << W.GetPosition();
	os << "Velocity  = " << W.GetVelocity();
	os << "Extension = " << W.GetExt();
	os << "Mass      = " << W.GetMass() << endl;
	return os;
}

// Overriddables methods

void Wall::Render() const {

    float x = m_xExt.x ;
    float y = m_xExt.y ;
    float z = m_xExt.z ;
	
    //GLfloat floorAmbient[]  = {1.0, 1.0, 1.0, 1.0};
    //GLfloat floorDiffuse[]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat floorModCol[] = {1.0, 1.0, 1.0, 1.0};

    if (isVisible()) {	
	
	if (!hasTexture() || !isTextureEnabled())
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
 	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPushMatrix();
	    glTranslatef(m_xPos.x, m_xPos.y, m_xPos.z);
	    if (hasTexture() && isTextureEnabled()) {
		glEnable( GL_TEXTURE_2D );
		glBindTexture(GL_TEXTURE_2D, m_aTexture[0]);
		  
		// rendering of the floor
		if (GetLabel().compare("floor") == 0) {
		  
		    //y = m_xPos.y;
		  
		    glMaterialfv( GL_FRONT, GL_AMBIENT, floorModCol );
		    glMaterialfv( GL_FRONT, GL_DIFFUSE, floorModCol );
		    glMaterialfv( GL_FRONT, GL_SPECULAR, floorModCol );
		    glMaterialf( GL_FRONT, GL_SHININESS, 5.0 );
		  
		    glBegin(GL_QUADS);
			
		    glNormal3f(0.0, 1.0, 0.0);
		    glTexCoord2f(1.0f, 1.0f); glVertex3f(- x, y, -z );
		    glTexCoord2f(0.0f, 1.0f); glVertex3f(- x, y, z );
		    glTexCoord2f(0.0f, 0.0f); glVertex3f(  0, y, z );
      		    glTexCoord2f(1.0f, 0.0f); glVertex3f(  0, y, -z );
		  
		    glTexCoord2f(0.0f, 0.0f); glVertex3f(  0, y, -z );
		    glTexCoord2f(1.0f, 0.0f); glVertex3f(  0, y, z );
		    glTexCoord2f(1.0f, 1.0f); glVertex3f(  x, y, z );
      		    glTexCoord2f(0.0f, 1.0f); glVertex3f(  x, y, -z );
			
		    glEnd();
		  
		} else // not floor
		  
		    if (GetLabel().compare("net") == 0) {
			x = 0;
			
			// the net polygon must be visible from both sides
			glDisable( GL_CULL_FACE );
			
			// we enable transparency on net holes
			glAlphaFunc( GL_GREATER, 0.1 );
			glEnable( GL_ALPHA_TEST ); 
			
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable( GL_BLEND );
			
			glBegin(GL_QUADS);
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( x,  y,  z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( x, -y,  z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( x, -y, -z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( x,  y, -z);
			glEnd();
			
			glDisable( GL_BLEND );
			glDisable( GL_ALPHA_TEST );
			
			glEnable( GL_CULL_FACE );
			
		    } // end if "net"
		  
		glDisable( GL_TEXTURE_2D );
	    
	    } else { // no textures to apply
		glScalef(m_xExt.x*2, m_xExt.y*2, m_xExt.z*2);
	  	glutSolidCube(1.0f);
	    }
	    
	glPopMatrix();
    
	if (!hasTexture() || !isTextureEnabled())
	    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    	//else
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } // end if visible
    
}
