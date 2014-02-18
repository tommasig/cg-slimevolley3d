/*
* main.cpp - Slime Volley main application
*
*
*
*/

#include "main.h"
#include "env.h"
#include "player.h"
#include "ball.h"
#include "game.h"
#include <fstream>
#include <iostream>
#include <SDL_console.h>
#include <DT_drawtext.h>
#include <SDL_image.h>
#include "split.h"
//#include <cmath>
using namespace std;

void glShadowProjection(const Vector &l, const Vector &e, const Vector &n);
bool loadTexture(char* file, GLuint* texture);
void loadFonts(vid_mode mode);
void display_bmp(char *file_name);

// console commands (2.1)
void consoleCmdHandler(ConsoleInformation *console, char* command);
char* consoleTabCompletion(char* command);

void KillProgram(ConsoleInformation *console, int argc, char *argv[]);
void ShowFPS(ConsoleInformation *console, int argc, char *argv[]);
void ChangePlayersName(ConsoleInformation *console, int argc, char *argv[]);
void ListCommands(ConsoleInformation *console, int argc, char *argv[]);
void VidMode(ConsoleInformation *console, int argc, char *argv[]);
void VidRestart(ConsoleInformation *console, int argc, char *argv[]);

// console command structure
typedef struct {
    char* commandname;
    void (*my_func)(ConsoleInformation *console, int argc, char *argv[]);
} command_t;

// console commands list
command_t cmd_table[] = {
			{"quit", KillProgram},
			{"hud_showfps", ShowFPS},
			{"pl_chgname", ChangePlayersName},
			{"cmdlist", ListCommands},			
			{"vid_mode", VidMode},
			{"vid_restart", VidRestart},
			{NULL, NULL}
			};

// global structures
struct Screen screen;
Camera      *cam;
Wall        walls[7]; // walls, ceiling, floor, net
Environment *env;
Player      players[2];
Ball        ball;
CObjectList olist;    // list of all the objects affected by collision detection engine
Game        game;

int main( int argc, char *argv[] ) {

    // Information about the current video settings. 
    const SDL_VideoInfo* info         = NULL;
    SDL_Surface          *icon_srfc;
    char                 videodrv[20] = "";

    cout << "================================================== " << endl
	 << " GL Slime volley Demo" << endl
	 << "-------------------------------------------------- " << endl
	 << "Initializing Video subsystem... " << endl;
	   
    
    // SDL inizialization
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) == -1) {
        fprintf(stderr,"\tCould not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }

    // Let's get some video information. 
    info = SDL_GetVideoInfo( );

    if( !info ) {
        // This should probably never happen. 
        fprintf( stderr, "\tVideo query failed: %s\n", SDL_GetError( ) );
        exit(-1);
    }

    screen.mode = VID640;     
    screen.width = WIDTHS[VID640];   //320 //640 //800;
    screen.height = HEIGHTS[VID640]; //200 //380 //480;
    screen.bpp = 24; //info->vfmt->BitsPerPixel; // 24 is necessary for sdl surface alpha-blending (console)
    
    // Reading configuration file
    parseCfgFile("slimev.cfg");
	
    screen.flags  = SDL_OPENGLBLIT;      // necessary for blitting on OPENGL surfaces (console)
    //screen.flags |= SDL_RESIZABLE;
    screen.flags |= SDL_GL_DOUBLEBUFFER; // ?
    screen.flags |= SDL_HWPALETTE;       // ?
    
    printf("\tVideo driver: %s\n",SDL_VideoDriverName(videodrv, sizeof(videodrv)));
    if (info->hw_available) {
	printf("\tHardware acceleration available \n");
	screen.flags |= SDL_HWSURFACE;
    } else {
        printf("\tHardware acceleration not available \n");
	screen.flags |= SDL_SWSURFACE;
    }
    
    if (info->blit_hw) {
	printf("\tHardware blits available \n");
	screen.flags |= SDL_HWACCEL;
    }
    
    if (info->wm_available) {
        printf("\tWM available \n");
        SDL_WM_SetCaption("GL Slime volley","GL Slime volley");
        icon_srfc = SDL_LoadBMP("slimev.bmp");
        if (icon_srfc != NULL)
    	    SDL_WM_SetIcon(icon_srfc, NULL);
	else
	    printf("\tError loading icon ! \n");
    } else
        printf("\tWM not available \n");
    
    printf("\t%d Kbytes of video memory available \n", info->video_mem);
    printf("\t%d bits per pixel \n", screen.bpp /*info->vfmt->BitsPerPixel*/);

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    // Note that we do not use SDL_DOUBLEBUF in
    // the flags to SDL_SetVideoMode. That does
    // not affect the GL attribute state, only
    // the standard 2D blitting setup.
    //
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1);       // we are going to use the stencil buffer
    SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 0);     // this and the next three lines set the bits allocated per pixel -
    SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 0);   // - for the accumulation buffer to 0
    SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, 0);

    // On X11, VidMode can't change
    // resolution, so this is probably being overly
    // safe. Under Win32, ChangeDisplaySettings
    // can change the bpp.
    
    //
    // Set the video mode
    //
    screen.surface = SDL_SetVideoMode( screen.width, screen.height, screen.bpp, screen.flags );
    if( !screen.surface ) {
        //
        // This could happen for a variety of reasons,
        // including DISPLAY not being set, the specified
        // resolution not being available, etc.
        //
        fprintf( stderr, "Video mode set failed: %s\n", SDL_GetError( ) );
        exit(-1);
    }

    printf("\tWindow resolution: %dx%d  \n", screen.width, screen.height);
    
    // environment and players initialization
    initenv();
	
    // OpenGL context inizialization
    initgfx();

    // console initialization
    initconsole();

    // Font loading depending on video mode
    loadFonts(screen.mode);
		
    cout << "... done !" << endl
	 << endl 
	 << "F1 Key	- toggles fullscreen mode" << endl
	 << "F2 Key	- grabs input into the application window" << endl
	 << "r  Key	- rotates camera views" << endl
	 << "c  Key	- rotate camera by mouse" << endl
	 << "t  Key	- enables/disables textures on objects" << endl
	 << "y  Key	- enables/disables skybox environment" << endl
	 << "h  Key	- enables/disables on-screen aids for players" << endl
//	 << "i  Key	- prints some debug information" << endl 
	 << "~ or \\ Key - opens/closes the console" << endl
	 << "Escape Key	- quit the program" << endl
	 << "================================================== " << endl;
    
    drawSplashScreen();
    //display_bmp("textures/splash640.bmp");

    // application main loop 
    while( 1 ) {
        // Process incoming events (keypress, Ctrl-c, ...) 
        process_events( );
        
	// still displaying the splash screen
	if (game.splash) {
	    // waiting for input from player
	} else
	    drawScene( );

    }

    // code never reached
    quit_app( 0 );
    return 0;
}

int initconsole() {
    SDL_Rect rect;
    
    // Init the console
    rect.x = rect.y = 0;
    rect.w = screen.width;
    rect.h = screen.height / 2;
    if((screen.console = CON_Init("console/console_font.png", screen.surface, 100 /*lines*/, rect)) == NULL)
	return 1;

    // Attach the command handling function to the console
    CON_SetExecuteFunction(screen.console, consoleCmdHandler);
    
    // Attach the tab completion function
    CON_SetTabCompletion(screen.console, consoleTabCompletion);
    
    // set the console background image
    switch (screen.mode) { 
	case VID400:
	    CON_Background(screen.console, "console/console_bg400.jpg", 1, 1);
	    break;
	case VID640:
	    CON_Background(screen.console, "console/console_bg640.jpg", 1, 1);
	    break;
	case VID800:
	    CON_Background(screen.console, "console/console_bg800.jpg", 1, 1);
	    break;
	default:
	    break;
    }

    // is the console displayed ?
    screen.console_disp = false;
    
    return 0;
}

GLvoid initgfx( GLvoid ) {
    int maxs;
    GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};

    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glShadeModel( GL_SMOOTH );
    //glOrtho(-2.0, 2.0, -2.0, 2.0, -1.0, 1.0);
    //glFrustum(-2.0, 2.0, -2.0, 2.0, -1.0, 1.0);
    glMatrixMode( GL_PROJECTION );
    gluPerspective(env->cam.getFov(), 
		    (GLdouble)screen.width/(GLdouble)screen.height , 
	            env->cam.getNearPlane(), env->cam.getFarPlane());
    glMatrixMode( GL_MODELVIEW );

    glEnable( GL_DEPTH_TEST );
    glCullFace( GL_BACK ) ;
    glEnable( GL_CULL_FACE );

    /// NEW
    glClearStencil( 0 );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ///- NEW

    //glGetIntegerv( GL_MAX_LIGHTS, &maxLights );
    //printf("Supported lights: %d \n", maxLights);

    glEnable( GL_LIGHT0 );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    //glEnable( GL_LIGHT1 );
    //glEnable( GL_LIGHT2 );

    env->GL_info.extensions = string( (const char *)glGetString(GL_EXTENSIONS) );
    env->GL_info.vendor     = string( (const char *)glGetString(GL_VENDOR) );
    env->GL_info.renderer   = string( (const char *)glGetString(GL_RENDERER) );
    env->GL_info.version    = string( (const char *)glGetString(GL_VERSION) );
	
    cout << "\tGL_VENDOR: "     << env->GL_info.vendor     << endl;
    cout << "\tGL_RENDERER: "   << env->GL_info.renderer   << endl;
    cout << "\tGL_VERSION: "    << env->GL_info.version    << endl;
    cout << "\tGL_EXTENSIONS: " << env->GL_info.extensions << endl;
	
    printf("\tLoading textures....");
    // loading environmental textures
	
    if (
        env->setSkyboxTexture( "textures/stage1/front.jpg",  T_FRONT) &&
        env->setSkyboxTexture(  "textures/stage1/back.jpg",   T_BACK) &&
        env->setSkyboxTexture(  "textures/stage1/left.jpg",   T_LEFT) &&
        env->setSkyboxTexture( "textures/stage1/right.jpg",  T_RIGHT) &&
        env->setSkyboxTexture("textures/stage1/bottom.jpg", T_BOTTOM) &&
        env->setSkyboxTexture(   "textures/stage1/top.jpg",    T_TOP) &&
    	//players[0].slime.SetRGBTexture( string("textures/slime_test4.rgb") ) &&
    	//players[1].slime.SetRGBTexture( string("textures/slime_test4.rgb") ) &&
        env->walls[4].SetRGBTexture( string("textures/field256.rgb") ) &&
    	env->walls[6].SetRGBTexture( string("textures/net.rgb") ) 
        ) 
        printf(" done !\n");
    else
        printf(" failed !\n");

    //cout << "Loading Splash Screen" << endl;
    loadTexture("textures/splash1024.jpg", &screen.splash_tex[0]);
	

/*	// la luce 0 ha gia' dei parametri utili
	// settati per default.
    // parametri di illuminazione della luce 1 e 2
    glLightfv( GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv( GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
*/

	// posizione della luce 0 -> al centro dell'ambiente
    //light0_pos[0] = (xmax-xmin)/2; //x
    //light0_pos[1] = ymax + 30; //y luce lontana
    //light0_pos[2] = (-zmax+zmin)/2; //z
    //light0_pos[3] = 1.0; //1.0 = luce locale, 0.0 = luce all'infinito

/*	// posizione della luce 1
    light1_pos[0] = (xmax-xmin)/4; //x
    light1_pos[1] = ymax - 0.5; //y
    light1_pos[2] = (-zmax+zmin)/2; //z
    light1_pos[3] = 0.0; //1.0 = luce locale, 0.0 = luce all'infinito

	// posizione della luce 2
    light2_pos[0] = ((xmax-xmin)/2 + (xmax-xmin)/4); //x
    light2_pos[1] = ymax - 0.5; //y
    light2_pos[2] = (-zmax+zmin)/2; //z
    light2_pos[3] = 0.0; //1.0 = luce locale, 0.0 = luce all'infinito
*/
	//glDisable( GL_NORMALIZE );

	//ball
	//glMaterialfv(GL_FRONT,GL_SPECULAR,spec);
	//glMaterialfv(GL_FRONT,GL_SHININESS,&df);

}

void initenv() {

    float xext = 21.0, yext = 10.0, zext = -10.0;
    keys_t keys[K_NUMBER];

    // clockwise generation of the walls (position_of_the_center, positive_extension)
    walls[0] = Wall( Vector(    -0.1f,   yext/2,   zext/2), Vector( 0.2f/2, yext/2, -zext/2) ); // wall behind player1
    walls[1] = Wall( Vector(   xext/2,   yext/2, zext-0.1), Vector( xext/2, yext/2,  0.2f/2) );
    walls[2] = Wall( Vector( xext+0.1,   yext/2,   zext/2), Vector( 0.2f/2, yext/2, -zext/2) ); // wall behind player2
    walls[3] = Wall( Vector(   xext/2,   yext/2,     0.1f), Vector( xext/2, yext/2,  0.2f/2) );
    walls[4] = Wall( Vector(   xext/2,    -1.5f,   zext/2), Vector( xext/2, 3.0f/2, -zext/2) ); // floor
    walls[4].SetLabel( string("floor") );
    walls[5] = Wall( Vector(   xext/2, yext+1.5,   zext/2), Vector( xext/2, 3.0f/2, -zext/2) ); // ceiling
    walls[6] = Wall( Vector(   xext/2,     1.0f,   zext/2), Vector( 0.2f/2, 2.0f/2, -zext/2) ); // net
    walls[6].SetLabel( string("net") );
	
    // camera parameters
    Vector CamPos[5] = { Vector( (xext/2)-2.0f,     yext, 4.0f )    // C_PLAYER1 
	        	, Vector( (xext/2)+2.0f,     yext, 4.0f )    // C_PLAYER2
			, Vector( (xext/2)-2.0f, (yext/2), -4.0f )   // C_AUX1
			, Vector( (xext/2)-5.0f, (yext/2), 5.0f )    // C_AUX2
			, Vector( (xext/2)+5.0f, (yext/2), 5.0f ) }; // C_AUX3			
    Vector CamCenter = Vector(      (xext/2), 0.0f, (zext/2) );
    Vector CamUp	 = Vector(          0.0f, 1.0f,     0.0f );

    cam = new Camera(CamPos[0] /*initial position*/, CamCenter, 70.0f /*fov*/, CamUp, 1.0f /*zNear*/, 200.0f /*zFar*/);
    
    env = new Environment(walls, sizeof(walls)/sizeof(*walls), *cam);
	
    env->setGAcc( Vector(0, -20.9, 0) );
	
    // definition of field box
    env->field.xmin = 0;
    env->field.xmax = xext;
    env->field.ymin = 0;
    env->field.ymax = yext;
    env->field.zmin = zext;
    env->field.zmax = 0;
	
    // invisible walls, to let the camera see what's happening inside the field
    //env->walls[3].SetVisibility(false);
    //env->walls[5].SetVisibility(false);
	
    // setting possible camera positions
    env->loadCameraPositions(CamPos);
	
    // initial positions
    Vector InitPos[4] = { Vector(  xext/4,   0.0f, zext/2)    // player1 position
	                , Vector(xext*3/4,   0.0f, zext/2)    // player2 position
			, Vector(  xext/4, yext/2, zext/2)    // ball on player1 position
			, Vector(xext*3/4, yext/2, zext/2) }; // ball on player2 position
	
    env->loadInitPositions(InitPos);
	
    // creating the ball
    ball = Ball( InitPos[2] /*pos*/, 0.5f /*rad*/, 1.0f /*mass*/);
    ball.SetLabel( string("ball") );
	
    // binding players' keys
    keys[K_UP] = SDLK_w;
    keys[K_DOWN] = SDLK_s;
    keys[K_LEFT] = SDLK_a;
    keys[K_RIGHT] = SDLK_d;
    keys[K_JUMP] = SDLK_LSHIFT; //SDLK_SPACE;	
    players[0] = Player( string("Player1"), InitPos[0], keys );
    players[0].slime.SetLabel( string("slime1") );
	
    keys[K_UP] = SDLK_UP;
    keys[K_DOWN] = SDLK_DOWN;
    keys[K_LEFT] = SDLK_LEFT;
    keys[K_RIGHT] = SDLK_RIGHT;
    keys[K_JUMP] = SDLK_RCTRL; //SDLK_KP0;
    players[1] = Player( string("Player2"), InitPos[1], keys );
    players[1].slime.SetLabel( string("slime2") );
	
    // Adding objects to a list for collision detection purposes
    olist.RegisterObject(&ball);
    olist.RegisterObject(&(players[0].slime));
    olist.RegisterObject(&(players[1].slime));
    for (int i=0; i<env->getNWalls(); i++)
        olist.RegisterObject(&(env->walls[i]));
	
    // Creating game rules
    game = Game(ball, walls[6], walls[4], players, olist, env);
	
    // if hardware acceleration is not available then disables game textures
    //cout << (screen.flags & SDL_HWSURFACE) << endl;
    //cout << (screen.flags & SDL_SWSURFACE) << endl;
    //cout << (screen.flags & SDL_HWACCEL) << endl;
    if (screen.flags & SDL_HWSURFACE)
	game.skybox = false;
	
    for (int i=0; i<env->getNWalls(); i++) {
	env->walls[i].SetTextureEnabled(game.textures);
    }
    players[0].slime.SetTextureEnabled(game.textures);
    players[1].slime.SetTextureEnabled(game.textures);

    screen.show_fps = false;
}

GLvoid reshape( GLsizei width, GLsizei height) {
    GLdouble aspect;

    //sistemo le dimensioni della nuova viewport
    glViewport(0,0, screen.width, screen.height);

    aspect = (GLdouble)screen.width/(GLdouble)screen.height;

    //sistemo le caratteristiche del volume di vista col nuovo aspect ratio della viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(env->cam.getFov(), aspect, env->cam.getNearPlane(), env->cam.getFarPlane());
    glMatrixMode(GL_MODELVIEW);
}

GLvoid drawSplashScreen( GLvoid ) {
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, screen.splash_tex[0] );
    
    glPushMatrix();
    glTranslatef( 0.0f, 0.0f, -1.4f);
    glBegin( GL_QUADS );
	glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -(float)screen.width / (float)screen.height, -1.0f, 0.0f );
	glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  (float)screen.width / (float)screen.height, -1.0f, 0.0f );
	glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  (float)screen.width / (float)screen.height,  1.0f, 0.0f );
	glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -(float)screen.width / (float)screen.height,  1.0f, 0.0f );
    glEnd();
    glPopMatrix();
    
    glDisable( GL_TEXTURE_2D );
    
    SDL_GL_SwapBuffers( );

}

GLvoid drawScene( GLvoid ) {

    GLfloat sli1Ambient[] = { 1.0, 0.0, 0.1, 1.0 };
    GLfloat sli1Diffuse[] = { 1.0, 0.2, 0.3, 1.0 };

    GLfloat sli2Ambient[] = { 0.0, 0.2, 1.0, 1.0 };
    GLfloat sli2Diffuse[] = { 0.1, 0.3, 1.0, 1.0 };

    GLfloat ballAmbient[] = { 1.0, 0.7, 0.0, 1.0 };
    GLfloat ballDiffuse[] = { 1.0, 0.8, 0.1, 1.0 };

    GLfloat shadowColor[]  = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat whitSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	
    GLfloat aidblue[]    = {0.0, 0.0, 1.0};
    GLfloat aidred[]     = {1.0, 0.0, 0.0};
    GLfloat aidyellow[]  = {1.0, 1.0, 0.0};
    Vector  ballDown = Vector(0, 0, 0);
	
    static int whichplayer = 0;
    static int winner = 0;
    bool balldown = false;
    bool playerswin = false;

    // default light position
    GLfloat light0_pos[4];
    light0_pos[0] = env->field.xmax - 0.5; //x
    light0_pos[1] = env->field.ymax - 0.5; //y
    light0_pos[2] = env->field.zmax + 10.0; //z
    light0_pos[3] = 1.0; //1.0 = local light, 0.0 = infinite light

    // These are only safe starting values since the variables are declared static
    static GLint T0 = 0;
    static GLint Frames = 0;
    static GLfloat fps = 60.0f;
    static GLfloat tempfps = 0;
	
    static GLint tscore = 0;
	
    Vector projpos;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Gather our fps 
    Frames++;
    GLint t = SDL_GetTicks();
    if (t - T0 >= 1000) { // update fps every second
        GLfloat seconds = (t - T0) / 1000.0;
        tempfps = Frames / seconds;
        //printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
        T0 = t;
        Frames = 0;

    	// if fps is too low, collision detection can't be effective
    	if (tempfps < 20.0f)
	    fps = 20.0f;
	else
	    fps = tempfps;
    }
	
	
    glPushMatrix();
	// things that remain still in front of the camera
	// must be drawn before gluLookAt	
	

	// games counting
	//glColor3f(0.0, 1.0, 0.0);
	//drawStr(-0.2, 0.65, -(env->cam.getNearPlane()+0.1), "Games");
	//drawStr(-0.15, 0.55, -(env->cam.getNearPlane()+0.1), "%d - %d", players[0].getGamesWon(), players[1].getGamesWon());
		
	// player1 score
	//glColor3f(1.0, 0.0, 0.0);
	//drawStr(-1.2, 0.65, -(env->cam.getNearPlane()+0.1), "%s : %d", players[0].getName().c_str(), players[0].getScore());

	// player2 score
	//glColor3f(0.0, 0.0, 1.0);
	//drawStr(0.7, 0.65, -(env->cam.getNearPlane()+0.1), "%s : %d", players[1].getName().c_str(), players[1].getScore());
		
	// positioning the camera
	gluLookAt(env->cam.getPos().x, env->cam.getPos().y, env->cam.getPos().z, 
		  env->cam.getCenter().x, env->cam.getCenter().y, env->cam.getCenter().z,
		  env->cam.getUpVec().x, env->cam.getUpVec().y, env->cam.getUpVec().z);
			  
	// color modulating with the skybox textures (GL_LIGHTING disabled)
	glColor3f(1.0, 1.0, 1.0);
	
	// if game textures are enabled
	if (game.skybox) {		
	    glEnable( GL_TEXTURE_2D );
	    env->renderSkybox( env->cam.getPos() /*pos*/, Vector(80, 60, 80) /*ext*/);
	    glDisable( GL_TEXTURE_2D );
	}
			
	// we enable LIGHTING after drawing environment textures
	// 'cos we want shading on slimes, ball and walls only
	glEnable( GL_LIGHTING );
	glLightfv( GL_LIGHT0, GL_POSITION, light0_pos);

	// material for wireframe walls
	glMaterialfv( GL_FRONT, GL_AMBIENT, whitSpecular );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, whitSpecular );
	glMaterialfv( GL_FRONT, GL_SPECULAR, whitSpecular );
	glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	
/*	/// NEW
	
	// Turn off writing to the Color Buffer and Depth Buffer
	// We want to draw to the Stencil Buffer only
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	// Enable the Stencil Buffer
	glEnable(GL_STENCIL_TEST);

	// Set 1 into the stencil buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	///- NEW
	
	//for (int i=0; i<env->getNWalls(); i++) {
	    env->walls[4].Render(); // floor
	//}
	
	/// NEW
	
	// Turn on Color Buffer and Depth Buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	// Only write to the Stencil Buffer where 1 is set
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	// Keep the content of the Stencil Buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

*/	///- NEW
	
	for (int i=0; i<env->getNWalls(); i++) {
	    env->walls[i].Render();
	}
	
/*	/// NEW
	
	// Disable light
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	// Enable blending
	glEnable(GL_BLEND);

	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	// Calculate the projected shadow

	glColor4f(0.0, 0.0, 0.0, 0.5f);
*/	///- NEW

	// First, we draw all the shadows on the ground, then the objects
		
	// Ball shadow
	glPushMatrix();
	    //glTranslatef(ball.GetPosition().x, env->field.ymin, ball.GetPosition().z);
	    //glMaterialfv( GL_FRONT, GL_AMBIENT, shadowColor );
	    //glMaterialfv( GL_FRONT, GL_DIFFUSE, shadowColor );
	    //glMaterialfv( GL_FRONT, GL_SPECULAR, whitSpecular );
	    //glMaterialf( GL_FRONT, GL_SHININESS, 30.0 );
	    //ball.drawShadow(ball.GetRadius() * (1 - (ball.GetPosition().y - env->field.ymin)/(env->field.ymax - env->field.ymin)));
	    glDisable(GL_LIGHTING);
	    glEnable(GL_BLEND);
	    glDisable(GL_DEPTH_TEST);
	    glColor4f(0.4, 0.4, 0.4, 0.5);
	    glShadowProjection(Vector(ball.GetPosition().x, env->field.ymax + 10, ball.GetPosition().z), 
				Vector(env->walls[4].GetPosition().x, env->field.ymin + 0.1, env->walls[4].GetPosition().z), 
				Vector(0.0, -1.0, 0.0) ); // the normal to the projection plane must be inverted
	    ball.Render();
	    glEnable(GL_DEPTH_TEST);
	    glDisable(GL_BLEND);
	    glEnable(GL_LIGHTING);
	glPopMatrix();

	// Slime 1 shadow
	glPushMatrix();
	    //glTranslatef(players[0].slime.GetPosition().x, env->field.ymin, players[0].slime.GetPosition().z);
	    //glMaterialfv( GL_FRONT, GL_AMBIENT, shadowColor );
	    //glMaterialfv( GL_FRONT, GL_DIFFUSE, shadowColor );
	    //glMaterialfv( GL_FRONT, GL_SPECULAR, whitSpecular );
	    //glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	    //players[0].slime.drawShadow(players[0].slime.GetRadius() * 
	//				    (1 - 
	//					(players[0].slime.GetPosition().y - env->field.ymin) /
	//					(env->field.ymax - env->field.ymin)
	//				    )
	//				);
	    glDisable(GL_LIGHTING);
	    glEnable(GL_BLEND);
	    glDisable(GL_DEPTH_TEST);
	    glColor4f(0.4, 0.4, 0.4, 0.7);
	    glShadowProjection(Vector(players[0].slime.GetPosition().x, env->field.ymax + 10, players[0].slime.GetPosition().z), 
				Vector(env->walls[4].GetPosition().x, env->field.ymin + 0.1, env->walls[4].GetPosition().z), 
				Vector(0.0, -1.0, 0.0) ); // the normal to the projection plane must be inverted
	    players[0].slime.Render();
	    glEnable(GL_DEPTH_TEST);
	    glDisable(GL_BLEND);
	    glEnable(GL_LIGHTING);
	glPopMatrix();

	// Slime 2 shadow
	glPushMatrix();
	    //glTranslatef(players[1].slime.GetPosition().x, env->field.ymin, players[1].slime.GetPosition().z);
	    //glMaterialfv( GL_FRONT, GL_AMBIENT, shadowColor );
	    //glMaterialfv( GL_FRONT, GL_DIFFUSE, shadowColor );
	    //glMaterialfv( GL_FRONT, GL_SPECULAR, whitSpecular );
	    //glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	    //players[0].slime.drawShadow(players[1].slime.GetRadius() * 
	//				    (1 - 
	//					(players[1].slime.GetPosition().y - env->field.ymin)/
	//					(env->field.ymax - env->field.ymin)
	//				    )
	//				);
	    glDisable(GL_LIGHTING);
	    glEnable(GL_BLEND);
	    glDisable(GL_DEPTH_TEST);
	    glColor4f(0.4, 0.4, 0.4, 0.7);
	    glShadowProjection(Vector(players[1].slime.GetPosition().x, env->field.ymax + 10, players[1].slime.GetPosition().z), 
				Vector(env->walls[4].GetPosition().x, env->field.ymin + 0.1, env->walls[4].GetPosition().z), 
				Vector(0.0, -1.0, 0.0) ); // the normal to the projection plane must be inverted
	    players[1].slime.Render();
	    glEnable(GL_DEPTH_TEST);
	    glDisable(GL_BLEND);
	    glEnable(GL_LIGHTING);
	glPopMatrix();
		

/*	/// NEW
	
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	glDisable(GL_STENCIL_TEST);	
*/	///- NEW
		
	// Ball rendering
	glPushMatrix();
	    glMaterialfv( GL_FRONT, GL_AMBIENT, ballAmbient );
	    glMaterialfv( GL_FRONT, GL_DIFFUSE, ballDiffuse );
	    glMaterialfv( GL_FRONT, GL_SPECULAR, whitSpecular );
	    glMaterialf( GL_FRONT, GL_SHININESS, 30.0 );
	    ball.Render();
	glPopMatrix();
				
	// Slime 1
	glPushMatrix();
	    glMaterialfv( GL_FRONT, GL_AMBIENT, sli1Ambient );
	    glMaterialfv( GL_FRONT, GL_DIFFUSE, sli1Diffuse );
	    glMaterialfv( GL_FRONT, GL_SPECULAR, whitSpecular );
	    glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	    players[0].slime.Render();
	glPopMatrix();
		
	// Slime 2
	glPushMatrix();
	    glMaterialfv( GL_FRONT, GL_AMBIENT, sli2Ambient );
	    glMaterialfv( GL_FRONT, GL_DIFFUSE, sli2Diffuse );
	    glMaterialfv( GL_FRONT, GL_SPECULAR, whitSpecular );
	    glMaterialf( GL_FRONT, GL_SHININESS, 200.0 );
	    players[1].slime.Render();
	glPopMatrix();
		
	glDisable( GL_LIGHTING );
		
	// rendering visual aids on screen
	if (game.osaid) {
		
		// general visual aids
		//if ( ball.GetPosition().x >  env->walls[6].GetPosition().x )
		//  glColor3fv(aidplayer2);
		//else
		//  glColor3fv(aidplayer1);
		
		// rendering ball signals on 	
		// walls[1]
		//glPushMatrix();
		//	glTranslatef(ball.GetPosition().x, ball.GetPosition().y, env->field.zmin);	
		//	ball.drawSignal();
		//glPopMatrix();
	
	    //---------------------------------------------
	    // Jump to hit the ball !
	    //
		
	    // projection time in the future to see whether
	    // there can be collision between slime and ball
	    float dt = 8.0f/fps;
		
	    // current slime positions
	    Vector playPos[2] = { players[0].slime.GetPosition(),
	                	players[1].slime.GetPosition() };
	    // projected velocities in the future (if a player jumps)
	    Vector maybeVel[2] = { players[0].slime.GetVelocity() + Vector(0, players[0].slime.getSpeed(), 0),
	                	players[1].slime.GetVelocity() + Vector(0, players[1].slime.getSpeed(), 0) };
	    // projected positions in the future (if a player jumps)
	    Vector maybePos[2] = { playPos[0] + maybeVel[0] * dt,
	                	playPos[1] + maybeVel[1] * dt };
	    // projected slimes in the future
	    Slime maybeSlime[2] = { Slime(maybePos[0], players[0].slime.GetRadius(), players[0].slime.GetMass()),
	                        Slime(maybePos[1], players[1].slime.GetRadius(), players[1].slime.GetMass()) };
	    // setting velocities for the projected slimes
	    maybeSlime[0].SetVelocity(maybeVel[0]);
	    maybeSlime[1].SetVelocity(maybeVel[1]);
		
	    // current conditions of the ball
	    Vector ballPos = ball.GetPosition();
	    Vector ballVel = ball.GetVelocity();
		
	    // projected ball in the future (if it continues to move as it's doing now)
	    Ball maybeBall = Ball( (ballPos + ballVel * dt), ball.GetRadius(), ball.GetMass() );
		
	    Vector xP0, xP1;
		
	    // if player1 jumps, can it impact with the ball ? or
	    // is it impacting now ?
	    if ( maybeSlime[0].Intersect((const CSphere&)maybeBall, xP0, xP1) ||
	         players[0].slime.Intersect((const CSphere&)ball, xP0, xP1)
		)
		glColor3fv(aidred);
	    else
		// if player2 jumps, can it impact with the ball ? or
		// is it impacting now ?
		if ( maybeSlime[1].Intersect((const CSphere&)maybeBall, xP0, xP1) ||
		    players[1].slime.Intersect((const CSphere&)ball, xP0, xP1)
		    )
		    glColor3fv(aidblue);
		else
		    glColor3fv(aidyellow);
		
	    //
	    // Jump to hit the ball !
	    //---------------------------------------------------
	    
	    //---------------------------------------------------
	    // Where the ball will hit the floor 
	    //
	    
	    // find the position where the ball will collide with floor
	    ballDown = game.findBallDown();
	    float xext = env->field.xmax - env->field.xmin;
		
	    // we draw the aid on the floor only if it's not too distant from the field boundaries
	    if ( ((ballDown.x > env->field.xmin - xext/5) && (ballDown.x < env->field.xmax + xext/5)) && 
		 ((ballDown.z > env->field.zmin - xext/4) && (ballDown.z < env->field.zmax + xext/4)) ) 
	    {
		glPushMatrix();
		    glTranslatef(ballDown.x, ballDown.y + 0.02, ballDown.z);
		    glRotatef(90.0, 1.0, 0.0, 0.0);
		    ball.drawFloorSignal();
		glPopMatrix();
	    }
		
	    //
	    // Where the ball will hit the floor
	    //---------------------------------------------------
	    
	    //---------------------------------------------------
	    // Vertical line from the ball to the floor
	    //
	    
	    // we show the vertical from the ball to the floor
	    glPushMatrix();
		//glTranslatef(ball.GetPosition().x, ball.GetPosition().y, ball.GetPosition().z);
		glColor3fv(aidyellow);
		glLineWidth(1);
		//glLineStipple(/*factor*/2, /*pattern*/0xFFF0);
		//glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINES);
		    glVertex3f(ball.GetPosition().x, ball.GetPosition().y, ball.GetPosition().z);
		    glVertex3f(ball.GetPosition().x, env->field.ymin, ball.GetPosition().z);
		glEnd();
		//glDisable(GL_LINE_STIPPLE);
	    glPopMatrix();
		
	    //
	    // Vertical line from the ball to the floor
	    //---------------------------------------------------

	    
	    //---------------------------------------------------
	    // Impact point on slime, from the vertical of the ball
	    //
	    
	    // the perpendicular projection of ball position on the floor
	    Vector ballOnFloor = Vector(ball.GetPosition().x, env->field.ymin, ball.GetPosition().z);
	    // distance from slimes to ball
	    Vector delta[2] = { ballOnFloor - players[0].slime.GetPosition(),
				ballOnFloor - players[1].slime.GetPosition() };
	    // angle from floor
	    float alpha;
	    // altitude from the slime base
	    float alt;
	    // is delta vector oriented up or down ?
	    //float up[2] = { delta[0].y, delta[1].y };
	    
	    // don't care about distance on y direction
	    delta[0].y = 0;
	    delta[1].y = 0;
	    
	    glColor3fv(aidyellow);
	    
	    if ( delta[0].GetLength() < players[0].slime.GetRadius() ) { // the ball is vertical to slime 1
		// we determine the point on the slime surface
		alpha = acos(delta[0].GetLength() / players[0].slime.GetRadius());
		alt = players[0].slime.GetRadius() * sin(alpha);
		delta[0].y = alt;
		
		// we render the visual aid
		//if (up[0] > 0.01) {    // if the ball is over the slime
		    glPushMatrix();
			// on the slime surface (center + delta)
			glTranslatef(players[0].slime.GetPosition().x + delta[0].x, 
				    players[0].slime.GetPosition().y + delta[0].y, 
				    players[0].slime.GetPosition().z + delta[0].z);
			glutSolidSphere(players[0].slime.GetRadius() / 5, 10, 10);
		    glPopMatrix();
		//}
	    } else
		if ( delta[1].GetLength() < players[1].slime.GetRadius() ) { // the ball is vertical to slime 2
		    // we determine the point on the slime surface
		    alpha = acos(delta[1].GetLength() / players[1].slime.GetRadius());
		    alt = players[1].slime.GetRadius() * sin(alpha);
		    delta[1].y = alt;
		
		    // we render the visual aid
		    glPushMatrix();
			// on the slime surface (center + delta)
			glTranslatef(players[1].slime.GetPosition().x + delta[1].x, 
				    players[1].slime.GetPosition().y + delta[1].y, 
				    players[1].slime.GetPosition().z + delta[1].z);
			glutSolidSphere(players[1].slime.GetRadius() / 5, 10, 10);
		    glPopMatrix();
		
		} //else {
		
		//}
	    
	    //
	    // Impact point on slime, from the vertical of the ball
	    //---------------------------------------------------
	    
		//glColor3fv(aidyellow);
		/*	
		glPushMatrix();
		  glTranslatef(players[0].slime.GetPosition().x, players[0].slime.GetPosition().y, env->field.zmin);
		  players[0].slime.drawSignal();
		glPopMatrix();
		
		glPushMatrix();
		  glTranslatef(players[1].slime.GetPosition().x, players[1].slime.GetPosition().y, env->field.zmin);
		  players[1].slime.drawSignal();
		glPopMatrix();
		
		// rendering ball signals on 	
		// walls[0]
		glPushMatrix();
			glTranslatef(env->field.xmin, ball.GetPosition().y, ball.GetPosition().z);
			glRotatef(90.0, 0.0, 1.0, 0.0);
			ball.drawSignal();
		glPopMatrix();
		
		glPushMatrix();
		  glTranslatef(env->field.xmin, players[0].slime.GetPosition().y, players[0].slime.GetPosition().z);
		  glRotatef(90.0, 0.0, 1.0, 0.0);
		  players[0].slime.drawSignal();
		glPopMatrix();
		
		// rendering ball signals on 	
		// walls[2]
		glPushMatrix();
			glTranslatef(env->field.xmax, ball.GetPosition().y, ball.GetPosition().z);
			glRotatef(-90.0, 0.0, 1.0, 0.0);
			ball.drawSignal();
		glPopMatrix();
		
		glPushMatrix();
		  glTranslatef(env->field.xmax, players[1].slime.GetPosition().y, players[1].slime.GetPosition().z);
		  glRotatef(-90.0, 0.0, 1.0, 0.0);
		  players[1].slime.drawSignal();
		glPopMatrix();
		*/
	} // endif osaid
		
		// rendering slime projections on walls
		//glPushMatrix();
		//  projpos = players[0].slime.GetPosition();
		//  projpos.z = 10.0f;
		//  glShadowProjection(projpos /*light source*/, env->walls[1].GetPosition()/**/, Vector(0, 0, 1)/**/);
		//  glColor3f(1.0, 1.0, 1.0);
		//  players[0].slime.Render();
		//glPopMatrix();

    glPopMatrix();
    
    // auxiliary string to draw all the hud text 
    char str[20];

    int fwidth  = DT_FontWidth(screen.fonts[3]);
    int fheight = DT_FontHeight(screen.fonts[3]);
    int namel;

    // a player has scored
    if (game.scores) {
        t = SDL_GetTicks();
		  
        // we pauses everything for a second, so
        // if that second is over
        if (t - tscore >= 1000) {
    	    game.scores = false;
			
	    // if someone has won the game
	    if (game.winning) {
	        game.winning = false;
	        players[0].zeroScore();
	        players[1].zeroScore();
	        players[whichplayer].winGame();
	    }
			
	    // we reconfigure slimes and ball to the original positions
	    players[0].slime.SetPosition(env->init.playerspos[0]);
	    players[0].slime.SetVelocity( Vector(0,0,0) );
	    players[1].slime.SetPosition(env->init.playerspos[1]);
	    players[1].slime.SetVelocity( Vector(0,0,0) );
	    ball.SetPosition(env->init.ballpos[whichplayer]);
	    ball.SetVelocity( Vector(0,0,0) );
		  
	    // the camera is set to the side of the player that scored
	    env->setCamPos((cam_pos)whichplayer);
	}
		  
	// the writing in the centre of the screen, "Player scores !!"
	//glColor3f(1.0, 1.0, 0.0);
	//drawStr(-0.3, 0.45, -(env->cam.getNearPlane()+0.1), "%s scores !", players[whichplayer].getName().c_str());
	sprintf(str, "%s scores !", players[whichplayer].getName().c_str());
	namel = strlen(str);	
	DT_DrawText(str, screen.surface, screen.fonts[3], screen.width/2 - (namel/2)*fwidth, screen.height/2 - 2*fheight);
		  
	if (game.winning) 
	    //drawStr(-0.4, 0.35, -(env->cam.getNearPlane()+0.1), "and wins the game !!");
	    DT_DrawText("and wins the game !!", screen.surface, screen.fonts[3], screen.width/2 - 10*fwidth, screen.height/2 - fheight);
		
	    
    } else    // no scoring, we are playing
        if (game.pause) {   // game is paused => nothing is updated
		
    	    // the writing in the centre of the screen, "Pause"
	    //glColor3f(1.0, 1.0, 0.0);
	    //drawStr(-0.4, 0.05, -(env->cam.getNearPlane()+0.1), "Pause (press p again)");
	    DT_DrawText("Pause (press p again)", screen.surface, screen.fonts[3], screen.width/2 - 11*fwidth, screen.height/2);
		
	} else {            // we are playing => objects positions are updated
	    // Update of the objects position, according to our fps and gravity of the world
	    olist.Update(1.0f/fps, env->getGAcc() );
		
	    //-------------------------------------------------------
	    // Check for game rules appliance
	    //-------------------------------------------------------
	    balldown = game.checkBallDown(whichplayer);
		
	    // if ball touches the ground someone scores 
	    if (balldown) {
	        game.scores = true;
	        tscore = SDL_GetTicks();
	    }
		
	    game.winning = game.checkPlayerWin();
	    //-------------------------------------------------------
	    // End of check for game rules appliance
	    //-------------------------------------------------------	  
	} // end checking game.pause


    //if (screen.mode == VID640 || screen.mode == VID800) {
	// 24x14 points fonts are used
    
    //} else {
	// 18x11 points fonts are used
    
    //}

    // at last we draw the text on the screen
    // since it must be on the top    
    
    // write the games count
    fwidth  = DT_FontWidth(screen.fonts[0]);
    fheight = DT_FontHeight(screen.fonts[0]);
    DT_DrawText("Games", screen.surface, screen.fonts[0], screen.width/2 - 3*fwidth , fheight/2);
    memset(str, 0, sizeof(str));
    sprintf(str, "%d - %d",  players[0].getGamesWon(), players[1].getGamesWon());
    DT_DrawText(str, screen.surface, screen.fonts[0], screen.width/2 - 3*fwidth, 3*fheight/2);

    // player1 score
    fwidth  = DT_FontWidth(screen.fonts[1]);
    fheight = DT_FontHeight(screen.fonts[1]);
    memset(str, 0, sizeof(str));
    sprintf(str, "%s : %d", players[0].getName().c_str(), players[0].getScore());
    DT_DrawText(str, screen.surface, screen.fonts[1], fwidth, fheight/2);

    // player2 score
    fwidth    = DT_FontWidth(screen.fonts[2]);
    fheight   = DT_FontHeight(screen.fonts[2]);
    namel = strlen(players[1].getName().c_str());
    memset(str, 0, sizeof(str));
    sprintf(str, "%s : %d", players[1].getName().c_str(), players[1].getScore());
    DT_DrawText(str, screen.surface, screen.fonts[2], screen.width - (namel+5)*fwidth, fheight/2);

    if (screen.show_fps) {
	fwidth  = DT_FontWidth(0);
	fheight = DT_FontHeight(0);
        memset(str, 0, sizeof(str));
	sprintf(str, "%d fps", (int)tempfps);
	DT_DrawText(str, screen.surface, 0, screen.width - (10)*fwidth, screen.height - 2*fheight);
    }

    // draw the console if open (on top of everything)
    if (screen.console_disp) 
	CON_DrawConsole(screen.console);

    //checkError("drawScene");

    SDL_GL_SwapBuffers( );
	
}


//--------------------------------------------------------------------
// Event handling functions
//--------------------------------------------------------------------
void handle_key_down( SDL_keysym* keysym ) {

    switch( keysym->sym ) {
	case SDLK_BACKQUOTE:  // show/hide the console
	case SDLK_BACKSLASH:
	    screen.console_disp = !screen.console_disp;   // rendering flag for the console
	    if (!CON_isVisible(screen.console)) {         // if the console is to be shown
		CON_Show(screen.console);
		CON_Topmost(screen.console);                 // input is activated on the console
		//SDL_EnableUNICODE(1);                        
		SDL_EnableKeyRepeat(250 /*delay*/,30 /*interval*/);
		game.pause = true;
	    } else {                                      // if the console is to be hidden
		CON_Hide(screen.console);
		CON_Topmost(NULL);                           // input is deactivated on the console
		//SDL_EnableUNICODE(0);
		SDL_EnableKeyRepeat(0,0);
		game.pause = false;
	    }
	    break;
	case SDLK_F1:
	    SDL_WM_ToggleFullScreen( screen.surface );
	    break;
	case SDLK_F2:
	    if ( SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_OFF ) {
		SDL_WM_GrabInput(SDL_GRAB_ON);                                            
		if ( SDL_ShowCursor(SDL_DISABLE) != SDL_DISABLE )
		    cerr << SDL_GetError() << endl;
	    } else {
		SDL_WM_GrabInput(SDL_GRAB_OFF);                                            
		if ( SDL_ShowCursor(SDL_ENABLE) != SDL_ENABLE )
	  	    cerr << SDL_GetError() << endl;
	    }
	    break;
	case SDLK_ESCAPE:
	    if (CON_isVisible(screen.console)) {    // if the console is opened, let's close it
		CON_Hide(screen.console);
		CON_Topmost(NULL);                           // input is deactivated on the console
		//SDL_EnableUNICODE(0);
		SDL_EnableKeyRepeat(0,0);		
		game.pause = false;
		screen.console_disp = false;
	    } else                              // otherwise let's quit the program
		quit_app( 0 );
	    break;
	case SDLK_i:
	    //cout << "************** Player 1" << endl << players[0];
	    //cout << "************** Player 2" << endl << players[1];
	    cout << "************** Ball " << endl << ball;
	    cout << "************** Floor " << endl << env->walls[4];
	    break;
	case SDLK_r:
	    // rotate between camera views
	    env->setCamPos((cam_pos)((env->getCurrentCamPos()+1) % env->getNumCamPos()) );
	    break;
	case SDLK_t:
	    // enables/disables textures on objects
	    game.textures = !game.textures;
	    for (int i=0; i<env->getNWalls(); i++) {
		env->walls[i].SetTextureEnabled(game.textures);
	    }
	    players[0].slime.SetTextureEnabled(game.textures);
	    players[1].slime.SetTextureEnabled(game.textures);
	    break;
	case SDLK_y:      // enables/disables the skybox
	    game.skybox = !game.skybox;
	    break;
	case SDLK_h:      // on screen aids for players
	    game.osaid = !game.osaid;
	    break;
	case SDLK_c:      // move camera by mouse
	    game.movecam = !game.movecam;
	    break;
	case SDLK_p:
	    //if (!screen.console_disp)     // pause state can be changed only when not showing the console
	                                    // but if the console is open, it captures all the keystrokes
		game.pause = !game.pause;
	    break;
	default:
	    break;
    }

    // gestione pressione tasti player1
    if ( keysym->sym == players[0].getKey(K_UP).code ) {
    	players[0].slime.vImpulse(V_UP);
	players[0].switchKey(K_UP);
    }
    if ( keysym->sym == players[0].getKey(K_DOWN).code ) {
	players[0].slime.vImpulse(V_DOWN);
	players[0].switchKey(K_DOWN);
    }

    if ( keysym->sym == players[0].getKey(K_LEFT).code ) {
	players[0].slime.hImpulse(H_LEFT);
	players[0].switchKey(K_LEFT);
    }
    if ( keysym->sym == players[0].getKey(K_RIGHT).code ) {
	players[0].slime.hImpulse(H_RIGHT);
	players[0].switchKey(K_RIGHT);
    }
    if ( keysym->sym == players[0].getKey(K_JUMP).code ) {
	if (! players[0].slime.jumping && ! game.pause) {    // non sta saltando e non siamo in pausa
	    players[0].slime.jImpulse(J_UP);
	}
	players[0].switchKey(K_JUMP);
    }

    // gestione pressione tasti player2
    if ( keysym->sym == players[1].getKey(K_UP).code ) {
	players[1].slime.vImpulse(V_UP);
	players[1].switchKey(K_UP);
    }
    if ( keysym->sym == players[1].getKey(K_DOWN).code ) {
	players[1].slime.vImpulse(V_DOWN);
	players[1].switchKey(K_DOWN);
    }

    if ( keysym->sym == players[1].getKey(K_LEFT).code ) {
	players[1].slime.hImpulse(H_LEFT);
	players[1].switchKey(K_LEFT);
    }
    if ( keysym->sym == players[1].getKey(K_RIGHT).code ) {
	players[1].slime.hImpulse(H_RIGHT);
	players[1].switchKey(K_RIGHT);
    }
	
    if ( keysym->sym == players[1].getKey(K_JUMP).code ) {
	if (! players[1].slime.jumping && ! game.pause) {    // non sta saltando e non siamo in pausa
	    players[1].slime.jImpulse(J_UP);
	}
	players[1].switchKey(K_JUMP);
    }

}

void handle_key_up( SDL_keysym* keysym ) {

    // gestione rilascio tasti player1...
    // viene gestito il caso in cui siano stati premuti
    // i due tasti per le direzioni opposte
    if ( keysym->sym == players[0].getKey(K_UP).code ) {
	if (players[0].getKey(K_DOWN).state == K_STATE_UP)
	    players[0].slime.vImpulse(V_CENTER);
	else
	    players[0].slime.vImpulse(V_DOWN);
	players[0].switchKey(K_UP);
    }
    if ( keysym->sym == players[0].getKey(K_DOWN).code ) {
	if (players[0].getKey(K_UP).state == K_STATE_UP)
	    players[0].slime.vImpulse(V_CENTER);
	else
	    players[0].slime.vImpulse(V_UP);
	players[0].switchKey(K_DOWN);
    }

    if ( keysym->sym == players[0].getKey(K_LEFT).code ) {
    	if (players[0].getKey(K_RIGHT).state == K_STATE_UP)
	    players[0].slime.hImpulse(H_CENTER);
	else
	    players[0].slime.hImpulse(H_RIGHT);
	players[0].switchKey(K_LEFT);
    }
    if ( keysym->sym == players[0].getKey(K_RIGHT).code ) {
	if (players[0].getKey(K_LEFT).state == K_STATE_UP)
	    players[0].slime.hImpulse(H_CENTER);
	else
	    players[0].slime.hImpulse(H_LEFT);
	players[0].switchKey(K_RIGHT);
    }
    if ( keysym->sym == players[0].getKey(K_JUMP).code )
	players[0].switchKey(K_JUMP);

    // gestione rilascio tasti player2
    if ( keysym->sym == players[1].getKey(K_UP).code ) {
	if (players[1].getKey(K_DOWN).state == K_STATE_UP)
	    players[1].slime.vImpulse(V_CENTER);
	else
	    players[1].slime.vImpulse(V_DOWN);
	players[1].switchKey(K_UP);
    }
    if ( keysym->sym == players[1].getKey(K_DOWN).code ) {
	if (players[1].getKey(K_UP).state == K_STATE_UP)
	    players[1].slime.vImpulse(V_CENTER);
	else
	    players[1].slime.vImpulse(V_UP);
	players[1].switchKey(K_DOWN);
    }

    if ( keysym->sym == players[1].getKey(K_LEFT).code ) {
	if (players[1].getKey(K_RIGHT).state == K_STATE_UP)
	    players[1].slime.hImpulse(H_CENTER);
	else
	    players[1].slime.hImpulse(H_RIGHT);
	players[1].switchKey(K_LEFT);
    }
    if ( keysym->sym == players[1].getKey(K_RIGHT).code ) {
	if (players[1].getKey(K_LEFT).state == K_STATE_UP)
	    players[1].slime.hImpulse(H_CENTER);
	else
	    players[1].slime.hImpulse(H_LEFT);
	players[1].switchKey(K_RIGHT);
    }
	
    if ( keysym->sym == players[1].getKey(K_JUMP).code )
	players[1].switchKey(K_JUMP);

}

void process_events( void ) {
    // Our SDL event placeholder.
    SDL_Event event;
    
    // Grab all the events off the queue.
    while( SDL_PollEvent( &event ) ) {

	switch( event.type ) {
//	    case SDL_VIDEORESIZE:
		// handle resize event 
		// first we modify the SDL surface
//	  	screen.surface = SDL_SetVideoMode( event.resize.w,
//				        	    event.resize.h,
//				            	    screen.bpp, 
//				            	    screen.flags );
//		if ( !screen.surface ) {
//		    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
//		    quit_app( 1 );
//	  	}
		// then we modify the OpenGL context
//	  	reshape( event.resize.w, event.resize.h );
//	  	break;
  	    case SDL_KEYDOWN:
        	// Handle key presses.
		if (! game.splash) {     // if we are not showing the initial splash screen
		    if ((! CON_isVisible(screen.console)) ||  // if the console is not visible is not capturing events
		        (event.key.keysym.sym == SDLK_BACKQUOTE) || // or the key pressed is "`"
			(CON_isVisible(screen.console) && event.key.keysym.sym == SDLK_ESCAPE) || // esc key can shut down the console
			(event.key.keysym.sym == SDLK_BACKSLASH))   // or the key pressed is "\"
			handle_key_down( &event.key.keysym ); // handle the keypress
		    else                        // otherwise let the console code handle the keypress event
			CON_Events(&event);
		}
        	break;
	    case SDL_KEYUP:
	  	if (! game.splash) {     // if we are not showing the initial splash screen
		    if (! CON_isVisible(screen.console))  // if the console is not visible then is not capturing events
			handle_key_up( &event.key.keysym ); // handle the keypress
		    else                     // otherwise let the console code handle the keypress
			CON_Events(&event);
		} else                   // if we are showing the splash screen, after a keypress we play
		    game.splash = false;
	  	break;
	    case SDL_MOUSEMOTION:    // If the mouse is moved 
		if (game.movecam)
        	    env->cam.MoveCameraByMouse(screen.width, screen.height);         // Check the mouse camera movement
    		break;
    	    case SDL_QUIT:
        	// Handle quit requests (like Ctrl-c)
        	quit_app( 0 );
        	break;
        } // end switch on events

    } // end loop on polling for events

}

void quit_app( int code ) {
    // Quit SDL so we can release the fullscreen
    // mode and restore the previous video settings,
    // etc.
    SDL_Quit( );

    // free up memory used in the program
    delete cam;
    delete env;
    CON_Destroy(screen.console);

    // exit program
    exit( code );
}

//--------------------------------------------------------------------
// Console Callbacks 
//--------------------------------------------------------------------

void consoleCmdHandler(ConsoleInformation *console, char* command) {
    int argc;
    char* argv[128];
    char* linecopy;

    linecopy = strdup(command);
    argc = splitline(argv, (sizeof argv)/(sizeof argv[0]), linecopy);
    if(!argc) {
    	free(linecopy);
	return;
    }

    command_t* cmd;
		
    // loop to find the command typed;
    // the stop condition is valid since the last command
    // in the list has NULL name
    for (cmd = cmd_table; cmd->commandname; cmd++) {
        if(!strcasecmp(cmd->commandname, argv[0])) {
	    // command found, now start the function and return
	    cmd->my_func(console, argc, argv);
	    return;
	}
    }
    // if we are here we could not find the command
    CON_Out(console, "Error: command not found.");

}

char* consoleTabCompletion(char* command) {
    command_t* cmd;
    int n = 0;
    int l = strlen(command);
    char *cmdstr;

    //cout << "cmd = " << command << endl;
    cmdstr = strdup(command);
    // first we find out how many commands begin with
    // the string we already typed
    for (cmd = cmd_table; cmd->commandname; cmd++) {
        if(!strncmp(cmdstr, cmd->commandname, l)) {
	    // command found for completion
	    // increment the counter
	    n++;
	    strcpy(command, cmd->commandname);
	}
    }
    
    //cout << "n = " << n << endl;
    
    if (n == 0)         // no command matching
	return NULL;    
    else if (n == 1)    // only one command matching, let's return the only one
    	return command;
    else {              // multiple choices, let's print them all
	CON_Out(screen.console, " ");
	for (cmd = cmd_table; cmd->commandname; cmd++) {
    	    if(!strncmp(cmdstr, cmd->commandname, l)) {
		CON_Out(screen.console, cmd->commandname);
	    }
	}
	return NULL;
    }
	
}

void KillProgram(ConsoleInformation *console, int argc, char *argv[]) {
    SDL_Event event;
    
    // send a SDL_QUIT event to the event queue
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

void ChangePlayersName(ConsoleInformation *console, int argc, char *argv[]) {
    char name[CON_CHARS_PER_LINE];
    int player;

    if (argc != 3) {
	// the number of command parameters must be 2
	CON_Out(console, "Usage: %s <PlayerNumber> <Name>", argv[0]);
	return;
    }
    
    player = atoi(argv[1]);
    strcpy(name, argv[2]);
    
    // limit on player's name length
    if (strlen(name) > 10) {
	CON_Out(console, "Error: name length must be at most 10");
	return;
    }
    
    players[player-1].setName(string(name));
    CON_Out(console, "Player %d is called %s now.", player, name);

}

void ShowFPS(ConsoleInformation *console, int argc, char *argv[]) {
    screen.show_fps = !screen.show_fps;
}

void ListCommands(ConsoleInformation *console, int argc, char *argv[]) {
    command_t *cmd;
    
    for (cmd = cmd_table; cmd->commandname; cmd++) {
	CON_Out(console, cmd->commandname);    
    }

}

void VidMode(ConsoleInformation *console, int argc, char *argv[]) {
    vid_mode video_mode;
    
    if (argc != 2) {
	CON_Out(console, "Current video mode: %d (%dx%d)", screen.mode, screen.width, screen.height);
	CON_Out(console, "Selectable video modes: %d (%dx%d) \t %d (%dx%d) \t %d (%dx%d)", 
		VID400, WIDTHS[VID400], HEIGHTS[VID400],
		VID640, WIDTHS[VID640], HEIGHTS[VID640],
		VID800, WIDTHS[VID800], HEIGHTS[VID800]);
	return;
    }
    
    video_mode = (vid_mode) atoi(argv[1]);
    
    if ((video_mode < 0) || (video_mode > 2)) {
	CON_Out(console, "Usage: vide_mode [0|1|2]");
	return;
    }

    screen.mode = video_mode;
    screen.width = WIDTHS[video_mode];
    screen.height = HEIGHTS[video_mode];
    CON_Out(console, "Video mode changed. Please exec 'vid_restart' when ready.");
    
}

void VidRestart(ConsoleInformation *console, int argc, char *argv[]) {
    SDL_Rect rect;

    // first we modify the SDL surface
    screen.surface = SDL_SetVideoMode( screen.width,
		        	    screen.height,
		            	    screen.bpp, 
		            	    screen.flags );
    if ( !screen.surface ) {
        fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
        quit_app( 1 );
    }
    // then we modify the OpenGL context
    reshape(screen.width, screen.height);
    
    // reinitialization of the console
    rect.x = rect.y = 0;
    rect.w = screen.width;
    rect.h = screen.height / 2;
    CON_Transfer(console, screen.surface, rect); // this function will be replaced
						 // in the next major release
						 // of SDL_console
    // select new console background
    switch (screen.mode) { 
	case VID400:
	    CON_Background(screen.console, "console/console_bg400.jpg", 1, 1);
	    break;
	case VID640:
	    CON_Background(screen.console, "console/console_bg640.jpg", 1, 1);
	    break;
	case VID800:
	    CON_Background(screen.console, "console/console_bg800.jpg", 1, 1);
	    break;
	default:
	    break;
    }

    // select new fonts, depending on the video mode selected
    loadFonts(screen.mode);

    // the console needs to be redrawn
    CON_Hide(console);
    CON_Show(console);
    screen.console_disp = true;    
        
}

//--------------------------------------------------------------------
// OpenGL and GLUT specific functions
//--------------------------------------------------------------------
void drawStr(GLfloat x, GLfloat y, GLfloat z, char* format, ...) {
    va_list args;
    char buffer[255], *s;
    void *font_style = GLUT_BITMAP_HELVETICA_18; //TIMES_ROMAN_24;
    
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    
    //glRasterPos2i(x, y);
    glRasterPos3f(x, y, z);
    for (s = buffer; *s; s++)
	glutBitmapCharacter(font_style, *s);

}

void glShadowProjection(const Vector &l, const Vector &e, const Vector &n) {
  float d, c;
  float mat[16];

  // These are c and d (corresponding to the tutorial)
  
  d = n.x * l.x + n.y * l.y + n.z * l.z;
  c = e.x * n.x + e.y * n.y + e.z * n.z - d;

  // Create the matrix. OpenGL uses column by column
  // ordering

  mat[0]  = l.x  * n.x + c; 
  mat[4]  = n.y  * l.x; 
  mat[8]  = n.z  * l.x; 
  mat[12] = -l.x * c - l.x * d;
  
  mat[1]  = n.x * l.y;        
  mat[5]  = l.y * n.y + c;
  mat[9]  = n.z * l.y; 
  mat[13] = -l.y * c - l.y * d;
  
  mat[2]  = n.x * l.z;        
  mat[6]  = n.y * l.z; 
  mat[10] = l.z * n.z + c; 
  mat[14] = -l.z * c - l.z * d;
  
  mat[3]  = n.x;        
  mat[7]  = n.y; 
  mat[11] = n.z; 
  mat[15] = -d;

  // Finally multiply the matrices together *plonk*
  glMultMatrixf(mat);
}

//---------------------------------------------------------------------
//
// Functions to read configuration file
//
//---------------------------------------------------------------------

int parseCfgFile(char* filename) {
	
    char buffer[254];
    int i;
    char temp[254] = "";
    char comment[2] = "";
    char *token[2];

    // file di configurazione aperto in sola lettura
    ifstream cfgfile(filename);
    //assure(cfgfile, filename);
	
    //if (cfgfile == NULL) {
    //	fprintf(stderr, "Error opening configuration file: %s \n", filename);
    //	return 0;
    //}

    cout << "Reading config file: " << filename << "... " ;

    // ciclo di lettura dal file (riga per riga)
    while (cfgfile.getline(buffer, 254)) {
	
    //while (fgets(buffer,sizeof(buffer),cfgfile) != NULL) {
	//printf("\t%s", buffer);
	i = strlen(buffer);              // lunghezza di buffer senza terminatore
	strncpy(temp, buffer, i); 
	strcat(temp,"\0");              // aggiungo il terminatore di stringa a temp
	//printf("\t%s \n",temp);
	if (strlen(temp) > 1) {
	    strncpy(comment, temp, 1);
	    if (strcmp(comment, "#") == 0) {
		//cout << "\tComment found..." << endl;
	    } else
		if (strcmp(temp, "[Video]") == 0) {
		    //printf("\tVideo section found... \n");
		    //parseVideoSection(cfgfile);
		} else
		    if (strcmp(temp, "[Game]") == 0) {
			//printf("\tGame section found... \n");
			//parseGameSection(cfgfile);
		    } else
			if (strcmp(temp, "[Controls]") == 0) {
			    //printf("\tControls section found... \n");
			    //parseControlsSection(cfgfile);
			} else {
			    token[0]=strtok(temp,"=");
			    if (token[0] != NULL) {
				token[1]=strtok(NULL,"=");
				//printf("\t%s takes value %s \n",token[0], token[1]);
				overwriteDefaults(token[0], token[1]);
			    }
			}
	}  // end if (str.len > 1)
	//else {
	    //printf("\triga vuota...\n");
	//}

	memset(temp, 0, sizeof(temp));  // pulisco temp per evitare sovrapposizioni
	
    } // end file reading loop

    printf("Done ! \n");

    //if (fclose(cfgfile) == EOF) {
    //	fprintf(stderr, "Error closing configuration file: %s \n", filename);
    //	return 0;
    //}

    return 0;
}


void overwriteDefaults(char *flag, char *value) {

    //if (strcmp(flag, "fullscreen") == 0) {
    //	game.fullscreen=(strcmp(value, "true") == 0)?true:false;
    //	return;
    //}
    if (strcmp(flag, "vid_mode") == 0) {
	int mode = atoi(value);
	if (mode >= 0 && mode <= 2) {
	    screen.mode = (vid_mode)mode;
	    screen.width = WIDTHS[mode];
	    screen.height = HEIGHTS[mode];
	}
	return;
    }
    //if (strcmp(flag, "height") == 0) {
    //	screen.height=atoi(value);
    //	return;
    //}
    //if (strcmp(flag, "bpp") == 0) {
    //	screen.bpp=atoi(value);
    //	return;
    //}
    if (strcmp(flag, "skybox") == 0) {
	game.skybox=(strcmp(value, "yes") == 0)?true:false;
	return;
    }
    if (strcmp(flag, "textures") == 0) {
	game.textures=(strcmp(value, "yes") == 0)?true:false;
	return;
    }

}

// function to load in bitmap as a GL texture 
bool loadTexture(char* file, GLuint* texture) {

    // Status indicator 
    bool Status = false;
    // Create storage space for the texture 
    SDL_Surface *TextureImage[1]; 

    // Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit 
//    if (TextureImage[0] = SDL_LoadBMP(file)) {
    if (TextureImage[0] = IMG_Load(file)) {

	// Bitmap successfully loaded
	Status = true;

	// Create The Texture 
	glGenTextures( 1, texture );

	// Load in texture 
	// Typical Texture Generation Using Data From The Bitmap 
	glBindTexture( GL_TEXTURE_2D, *texture );

	// Linear Filtering 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Generate The Texture 
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB /*3*/, TextureImage[0]->w,
			TextureImage[0]->h, 0, GL_RGB /*GL_BGR*/,
			GL_UNSIGNED_BYTE, TextureImage[0]->pixels );

    }

    // Free up any memory we may have used 
    if ( TextureImage[0] )
	SDL_FreeSurface( TextureImage[0] );

    return Status;
}

void loadFonts(vid_mode mode) {

    switch (mode) {
	case VID400:
	    screen.fonts[0] = DT_LoadFont("fonts/font18_green.png", 1 /*transparency*/);
	    screen.fonts[1] = DT_LoadFont("fonts/font18_red.png", 1 /*transparency*/);
	    screen.fonts[2] = DT_LoadFont("fonts/font18_blue.png", 1 /*transparency*/);
	    screen.fonts[3] = DT_LoadFont("fonts/font18_yellow.png", 1 /*transparency*/);    
	    screen.fonts[4] = DT_LoadFont("fonts/font18_white.png", 1 /*transparency*/);    
	    break;
	case VID640:
	case VID800:
	    screen.fonts[0] = DT_LoadFont("fonts/font24_green.png", 1 /*transparency*/);
	    screen.fonts[1] = DT_LoadFont("fonts/font24_red.png", 1 /*transparency*/);
	    screen.fonts[2] = DT_LoadFont("fonts/font24_blue.png", 1 /*transparency*/);
	    screen.fonts[3] = DT_LoadFont("fonts/font24_yellow.png", 1 /*transparency*/);    
	    screen.fonts[4] = DT_LoadFont("fonts/font24_white.png", 1 /*transparency*/);    
	    break;
	default:
	    break;
    }
    
}

void display_bmp(char *file_name) {

    SDL_Surface *image;
    SDL_Rect src, dest;

    /* Load the BMP file into a surface */
    image = SDL_LoadBMP(file_name);
    if (image == NULL) {
        fprintf(stderr, "Couldn't load %s: %s\n", file_name, SDL_GetError());
        return;
    }

    /*
     * Palettized screen modes will have a default palette (a standard
     * 8*8*4 colour cube), but if the image is palettized as well we can
     * use that palette for a nicer colour matching
     */
    if (image->format->palette && screen.surface->format->palette) {
    SDL_SetColors(screen.surface, image->format->palette->colors, 0,
                  image->format->palette->ncolors);
    }

    src.x = 1;
    src.y = 1;
    src.w = image->w;
    src.h = image->h;
    
    dest.x = 1;
    dest.y = 1;
    dest.w = screen.width;
    dest.h = screen.height;
    
    /* Blit onto the screen surface */
    if(SDL_BlitSurface(image, &src, screen.surface, &dest) < 0)
        fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());

    SDL_UpdateRect(screen.surface, 1, 1, image->w, image->h);

    /* Free the allocated BMP surface */
    SDL_FreeSurface(image);
}
