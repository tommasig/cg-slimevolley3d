#ifndef ENV_H
#define ENV_H

#include "wall.h"
#include "camera.h"
#include <string>
#include <GL/glut.h>
using namespace std;

// faces of the skybox
enum {T_LEFT, T_FRONT, T_RIGHT, T_BACK, T_BOTTOM, T_TOP};

// camera positions
typedef enum {C_PLAYER1, C_PLAYER2, C_AUX1, C_AUX2, C_AUX3, C_AUX4, C_AUX5} cam_pos;

class Environment {

  public:
	
	// attributes
	Camera cam;
	Wall *walls;
	
	struct {      // defines the dimension of the box containing the field of play
	  float xmin;
	  float xmax;
	  float ymin;
	  float ymax;
	  float zmin;
	  float zmax;
	} field;
	
	struct {
	  string extensions; // opengl extensions available on system
	  string version;
	  string vendor;
	  string renderer;
	} GL_info;
	
	struct {
	  Vector playerspos[2];
	  Vector ballpos[2];
	} init;
	
	// Constructors & destructor
	Environment();
	Environment(Wall ewalls[], int nwalls, const Camera& camera, const Vector& gAcc = Vector(0,0,0) );
	~Environment();
	
	// Getter
	int     getNWalls        () const;
	Vector  getGAcc          () const;
	Vector  getCameraPosition(cam_pos pos) const;
	cam_pos getCurrentCamPos () const;
	int     getNumCamPos     () const;
	
	// Setter
	void setGAcc            (const Vector& gAcc);
	bool setSkyboxTexture   (const char *filename, int textureID);
	void setCamPos          (cam_pos pos);         // positions the camera to a given fixed point
	void loadCameraPositions(const Vector xPos[]); // loads all the possible positions where the camera can be
	void loadInitPositions  (const Vector xPos[]); // loads all the initial positions vector
	
	// Rendering
	void renderSkybox(const Vector &xPos, const Vector &xExt);
  
  private:
	
	int     m_nWalls;
	Vector  m_gAcc;
	GLuint  m_aTexture[6]; // skybox textures
	
	int     m_nCamPos;     // number of camera positions
	Vector  m_camPos[5];   // positions of the camera
	cam_pos m_currCamPos;  // current camera position
	
	bool LoadGLTexture(GLuint textureArray[], const char *filename, int textureID);
	
};

#endif // ENV_H
