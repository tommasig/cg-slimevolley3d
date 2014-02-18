
#include "env.h"
#include <iostream>
#include <SDL_image.h>
using namespace std;

//--------------------------------------------------------------------
// Constructors and destructor
//--------------------------------------------------------------------

Environment::Environment() {

}

Environment::Environment(Wall ewalls[], int nwalls, const Camera& camera, const Vector& gAcc /*=Vector(0,0,0)*/) {
  
	walls = ewalls;
	m_nWalls = nwalls;
  
	cam = camera;

	m_gAcc = gAcc;
	m_nCamPos = 5;
	
}

Environment::~Environment() {

}

//--------------------------------------------------------------------
// Getter
//--------------------------------------------------------------------

int Environment::getNWalls() const {
    return m_nWalls;
}

Vector Environment::getGAcc() const {
    return m_gAcc;
}

Vector Environment::getCameraPosition(cam_pos pos) const {
	return m_camPos[pos];
}

cam_pos Environment::getCurrentCamPos() const {
	return m_currCamPos;
}

int Environment::getNumCamPos() const {
	return m_nCamPos;
}

//--------------------------------------------------------------------
// Setter
//--------------------------------------------------------------------

void Environment::setGAcc(const Vector& gAcc) {
    m_gAcc = gAcc;
}

bool Environment::setSkyboxTexture(const char *filename, int textureID) {
    return LoadGLTexture(m_aTexture, filename, textureID);
}

void Environment::setCamPos(cam_pos pos) {
  m_currCamPos = pos;
  cam.setPos(m_camPos[pos]);
}

void Environment::loadCameraPositions(const Vector xPos[]) {
  
  for (int i=0; i<m_nCamPos; i++) 
    m_camPos[i] = xPos[i];
  
}

void Environment::loadInitPositions(const Vector xPos[]) {
  
  init.playerspos[0] = xPos[0];
  init.playerspos[1] = xPos[1];
  init.ballpos[0]    = xPos[2];
  init.ballpos[1]    = xPos[3];
  
}


//--------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------

void Environment::renderSkybox(const Vector &xPos, const Vector &xExt) {
    
    float x = xPos.x;
    float y = xPos.y;
    float z = xPos.z;
    float width = xExt.x;
    float height = xExt.y;
    float length = xExt.z;
    // 
    
    // Since we want the sky box to be centered around X, Y, and Z for ease,
    // we do a little math to accomplish this.  We just change the X, Y and Z
    // to perform this task.  If we just minus half the width, height and length
    // from x, y and z it will give us the desired result.  Now when we create the
    // box it will center it around (x, y, z)

    // This centers the sky box around xPos 
    x = x - width  / 2;
    y = y - height / 2;
    z = z - length / 2;

    // Bind the BACK texture of the sky map to the BACK side of the cube
    glBindTexture(GL_TEXTURE_2D, m_aTexture[T_BACK]);

    // Start drawing the side as a QUAD
    glBegin(GL_QUADS);        
        
    // Assign the texture coordinates and vertices for the BACK Side
    glTexCoord2f(0.0f, 0.0f); glVertex3f(        x,          y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width,          y, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(        x, y + height, z);
    glEnd();

    // Bind the FRONT texture of the sky map to the FRONT side of the box
    glBindTexture(GL_TEXTURE_2D, m_aTexture[T_FRONT]);

    // Start drawing the side as a QUAD
    glBegin(GL_QUADS);    
    
        // Assign the texture coordinates and vertices for the FRONT Side
        glTexCoord2f(1.0f, 0.0f); glVertex3f(        x,          y, z + length);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(        x, y + height, z + length);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width,          y, z + length);
    glEnd();

    // Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
    glBindTexture(GL_TEXTURE_2D, m_aTexture[T_BOTTOM]);

    // Start drawing the side as a QUAD
    glBegin(GL_QUADS);        
    
        // Assign the texture coordinates and vertices for the BOTTOM Side
        glTexCoord2f(1.0f, 0.0f); glVertex3f(        x, y,          z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(        x, y, z + length);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z + length); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,          z);
    glEnd();

    // Bind the TOP texture of the sky map to the TOP side of the box
    glBindTexture(GL_TEXTURE_2D, m_aTexture[T_TOP]);

    // Start drawing the side as a QUAD
    glBegin(GL_QUADS);        
        
        // Assign the texture coordinates and vertices for the TOP Side
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(        x, y + height, z + length); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f(        x, y + height,          z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,          z);
    glEnd();

        // Bind the LEFT texture of the sky map to the LEFT side of the box
    glBindTexture(GL_TEXTURE_2D, m_aTexture[T_LEFT]);

    // Start drawing the side as a QUAD
    glBegin(GL_QUADS);        
        
        // Assign the texture coordinates and vertices for the LEFT Side
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x,          y, z + length);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x,          y,          z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height,          z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z + length); 
    glEnd();

    // Bind the RIGHT texture of the sky map to the RIGHT side of the box
    glBindTexture(GL_TEXTURE_2D, m_aTexture[T_RIGHT]);

    // Start drawing the side as a QUAD
    glBegin(GL_QUADS);        

        // Assign the texture coordinates and vertices for the RIGHT Side
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width,          y,          z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width,          y, z + length);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,          z);
    glEnd();
    
}


//--------------------------------------------------------------------
// Private members
//--------------------------------------------------------------------

// not an Environment member
static void swap(unsigned char & a, unsigned char & b)
{
    unsigned char temp;

    temp = a;
    a    = b;
    b    = temp;

    return;
}

// function to load in bitmap as a GL texture 
bool Environment::LoadGLTexture(GLuint textureArray[], const char *filename, int textureID) {
	// Status indicator 
	bool Status = false;

	// Create storage space for the texture 
	SDL_Surface *TextureImage[1]; 

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit 
//	if ( ( TextureImage[0] = SDL_LoadBMP( filename ) ) ) {
    if ( TextureImage[0] = IMG_Load(filename) ) {

	    // Set the status to true 
	    Status = true;

	    // Create The Texture 
	    glGenTextures( 1, &textureArray[textureID] );

	    // Typical Texture Generation Using Data From The Bitmap 
	    glBindTexture( GL_TEXTURE_2D, textureArray[textureID] );

/////////////////////////////////////////

    // IMPORTANT : SDL loads Bitmaps differently when compared to the default windows loader. The row 0
    // corresponds to the top row and NOT the bottom row. Therefore if we don't invert the order of the rows,
    // then the textures will appear (vertically) inverted.
    // Therefore we must reverse the ordering of the rows in the data of the loaded surface ( the member
    //  'pixels' of the structure)
    
    // Rearrange the pixelData 
    
    int width  = TextureImage[0] -> w;
    int height = TextureImage[0] -> h;
    unsigned char * data = (unsigned char *) (TextureImage[0] -> pixels);         // the pixel data

    int BytesPerPixel = TextureImage[0] -> format -> BytesPerPixel;

    //////////// This is how we swap the rows :
    // For half the rows, we swap row 'i' with row 'height - i -1'. (if we swap all the rows
    // like this and not the first half or the last half, then we get the same texture again !
    //
    // Now these rows are not stored as 2D arrays, instead they are stored as a long 1D array.
    // So each row is concatenated after the previous one to make this long array. Our swap 
    // function swaps one byte at a time and therefore we swap BytesPerPixel (= total bits per pixel)
    // bytes succesively.
    //
    // so the three loops below are :
    // for the first half of the rows
    //   for all the width (which is width of image * BytesPerPixel, where BytesPerPixel = total bits per pixel).
    //   (Here, for each pixel, we have to increment j by total bits per pixel (to get to next pixel to swap))
    //      for(each byte in this pixel i.e k = 0 to BytesPerPixel - 1, i.e BytesPerPixel bytes.
    //        swap the byte with the corresponding byte in the 'height -i -1'th row ('i'th row from bottom)
    int i, j;
	for( i = 0 ; i < (height / 2) ; ++i )
        for( j = 0 ; j < width * BytesPerPixel; j += BytesPerPixel )
            for(int k = 0; k < BytesPerPixel; ++k)
                swap( data[ (i * width * BytesPerPixel) + j + k], data[ ( (height - i - 1) * width * BytesPerPixel ) + j + k]);
    
    unsigned char *pixels = new unsigned char[width * height * 3];

    int count = 0;
    
    // the following lines extract R,G and B values from any bitmap

    for( i = 0; i < (width * height); ++i)
    {
        unsigned char r,g,b;                            // R,G and B that we will put into pImage

        Uint32 pixel_value = 0;                         // 32 bit unsigned int (as dictated by SDL)

        // the following loop extracts the pixel (however wide it is 8,16,24 or 32) and 
        // creates a long with all these bytes taken together.
        
        for( j = BytesPerPixel - 1 ; j >=0; --j)        // for each byte in the pixel (from the right)
        {
            pixel_value = pixel_value << 8;                               // left shift pixel value by 8 bits
            pixel_value = pixel_value | data[ (i * BytesPerPixel) + j ];  // then make the last 8 bits of pixel value  =
        }                                                                 // the byte that we extract from pBitmap's data

        SDL_GetRGB(pixel_value, TextureImage[0] -> format, (Uint8 *)&r, (Uint8 *)&g, (Uint8 *)&b);     // here we get r,g,b from pixel_value which is stored in the form specified by pBitmap->format

        pixels[count++] = r;          // in our tImage classes we store r first
        pixels[count++] = g;          // then g
        pixels[count++] = b;          // and finally b (for bmps - three channels only)
        //pixels[(i * BytesPerPixel) + 0] = r;          // in our tImage classes we store r first
        //pixels[(i * BytesPerPixel) + 1] = g;          // then g
        //pixels[(i * BytesPerPixel) + 2] = b;          // and finally b (for bmps - three channels only)

        pixel_value = 0;                                // reset pixel , else we get incorrect values of r,g,b
    }

///////////////////////////////// 

	    // Build Mipmaps (builds different versions of the picture for distances - looks better)
	    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->w, TextureImage[0]->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	    // Lastly, we need to tell OpenGL the quality of our texture map.  GL_LINEAR is the smoothest.    
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);    
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);

/*
	    // Generate The Texture 
	    glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage[0]->w,
			      TextureImage[0]->h, 0, GL_BGR,
			      GL_UNSIGNED_BYTE, TextureImage[0]->pixels );

	    // Linear Filtering 
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
*/
	    // the texture will be modulated with other active materials
	    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
		// GL_CLAMP_TO_EDGE mode is available only on OpenGL 1.2+ (ifdef GL_VERSION_1_2)
		// we must be sure that it is available at compile-time and at run-time
		// to make it work
		int l, m;
		l = GL_info.extensions.find("texture_border_clamp");
		m = GL_info.extensions.find("texture_edge_clamp");
		if ( (l != string::npos) || (m != string::npos) ) {
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
	  	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		} else {
		  // GL_CLAMP works on some platforms, like Voodoo3, but not on ATI
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}
		
	    // Free up any memory we may have used 
	    SDL_FreeSurface( TextureImage[0] );
    	}

	return Status;
    }

