/*

 NOTE:
 --> Use of .png images enabled by default. Need to enable other formats manually if you want to work with them
 --> Main Menu, Gameover page and leaderboard is of size 800*500 ie size of game window
 --> Size of the game window must not be changed. Mouse buttons will not work otherwise
 
 */

/*

WHAT'S NEW:
Version 3.1
--> LeaderBoard Displayed
--> Timer API replaced with a normal scoring system
--> Primitives replaced with Images
--> Collision Detection Included
--> Scoring System Included
--> All menus included

*/

/*

COMPUTER GRAPHICS PROJECT

MAN WITH A PLAN

Developers: Vishal Soni		1PE13CS181
	    Priyanka Agrawal	1PE13CS184

Objective: Score the highest score by moving character along the upper or lower plane using the keyboard and dodge incoming obstacles

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

//For Windows
//#ifdef WIN32
//	#include <windows.h>
//#endif

#include <GL/glut.h>

//Library to Load Images
#include <SOIL/SOIL.h>


using namespace std;

/*
For Man movements
STRAIGHT = Man is standing normally
INVERTED = Man is UpsideDown
*/
#define STRAIGHT 0
#define INVERTED 1

/*
Type of spike to be generated
*/
#define LONG_SPIKE_DOWN 0
#define LONG_SPIKE_UP 1
#define SHORT_SPIKE_DOWN 2
#define SHORT_SPIKE_UP 3

/*
Represents Various different states of game
game = MENU; Game is at Menu. Load Menu at this stage
game = PLAY; Player is playing the game
game = HIGHSCORE; Display Leaderboard
game = GAMEOVER; Display Gameover Page
*/
#define MENU 0
#define PLAY 1
#define SCOREBOARD 2
#define GAMEOVER 3

/*
Initialised values
Game always starts from MENU
*/
int man=STRAIGHT, game=MENU;

/*
Initialised Values
x-coordinate at zero, implies spike will be generated on far RHS of screen
level_val = 1; start at lowest level
*/
int select_spike, x=0, level_val=1;

//To obtain current position of spikes
int xmin, xmax;

//Declarations for BitMap String and Score calculation
char double_to_char[30];
double score;
void* font=GLUT_BITMAP_TIMES_ROMAN_24;
int int_score;
int highscore[5];

/*
To ensure that the scores get updated only ONCE in the idle function
*/
#define TRUE 1
#define FALSE 0

int check=FALSE;

/*
File descriptors for all images
*/
GLint image_man_straight, image_man_inverted;
GLint image_wall, image_main_menu, image_gameover, image_leaderboard;
GLint image_spike_long_down, image_spike_long_up, image_spike_short_up, image_spike_short_down;

/*
To maintain aspect ratio of game even if screen size is changed
*/
void changeViewPort(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}


/*
Function to draw image loaded from file
NOTE: All images MUST be present in the current working directory
*/
void drawImage(GLint left, GLint right, GLint bottom, GLint top, GLuint texture)
{
    glColor3f(1.0, 1.0, 1.0);

	//To create and use a named texture
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBegin(GL_QUADS);
    
    glTexCoord2i(1,1); glVertex2i(right , top);
    glTexCoord2i(1,0); glVertex2i(right , bottom);
    glTexCoord2i(0,0); glVertex2i(left , bottom);
    glTexCoord2i(0,1); glVertex2i(left , top);
    
    glEnd();
}

/*
Draws Menu <-- Start page of game
Options included:
START : To start the game
HIGHSCORES : To display Leaderboard
EXIT : To exit game
*/
void Menu()
{
	
	/*
	Specifies type of interpolation to be used
	GL_TEXTURE_MIN_FILTER: Scale image down
	GL_TEXTURE_MAG_FILTER: Scale image up

	GL_LINEAR: Returns the weighted average of the 4 pixels surrounding the given coordinates.
	*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_main_menu == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	drawImage(0, 800, 0, 500, image_main_menu);

}

//Draws Gameover screen
void Gameover()
{
	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_gameover == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	drawImage(0, 800, 0, 500, image_gameover);

}

//Draws Leaderboard screen
void Leaderboard()
{
	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_leaderboard == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	drawImage(0, 800, 0, 500, image_leaderboard);

}

/*
Draws background for game
*/
void outline()
{
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_wall == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	//Upper wall
	drawImage(0, 800, 375, 500, image_wall);

	//Lower Wall
	drawImage(0, 800, 0, 125, image_wall);

}

/*
Draws scoreboard for clear reading of scores
*/
void scoreboard()
{
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_POLYGON);
		glVertex2i(600, 415);
		glVertex2i(800, 415);
		glVertex2i(800, 375);
		glVertex2i(600, 375);
	glEnd();
}

/*
Draws a Straight standing man
*/
void Man()
{
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_man_straight == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	//ymin ~ ymax of lower wall
	drawImage(20, 100, 123, 300, image_man_straight);

}

/*
Draws a man standing Upside Down
*/
void UpsideDownMan()
{
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_man_inverted == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	//ymax ~ ymin of upper wall
	drawImage(20, 100, 200, 377, image_man_inverted);

}

/*
Draws a long spike on the lower plane of game
*/
void Spike_long_down()
{
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_spike_long_down == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	drawImage(500-x, 800-x, 123, 190, image_spike_long_down);

}

/*
Draws a long spike on the upper plane of game
*/
void Spike_long_up()
{
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_spike_long_up == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	drawImage(500-x, 800-x, 310, 377, image_spike_long_up);

}

/*
Draws a short spike on the upper plane of game
*/
void Spike_short_up()
{
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_spike_short_up == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	drawImage(650-x, 800-x, 310, 377, image_spike_short_up);

}

/*
Draws a short spike on the lower plane of game
*/
void Spike_short_down()
{
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if( image_spike_short_down == 0 )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result());
    }

	drawImage(650-x, 800-x, 123, 190, image_spike_short_down);

}

/* Rendering characters in OpenGL */
void renderBitmapString(float x, float y, void *font,const char *string)
{
    const char *c;

	//Disable Texture colouring to enable colouring of text
	glDisable(GL_TEXTURE_2D);

    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) 
	{
		glutBitmapCharacter(font, *c);
    }

	//Re-enable texture
	glEnable(GL_TEXTURE_2D);
}

/*
Display Function.
To redraw page for animation
*/
void display(void)
{
	/*
	To clear screen and COlour Buffer
	Default Screen background set to White
	*/
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
   
    if(game==PLAY)
	{

		//Display Game Background
		outline();

		//Display Current Score
		scoreboard();
		
		//Function to write strings
		glColor3f(1.0, 1.0, 1.0);
		renderBitmapString(620,390, (void*)font, double_to_char);
		
		//Display Standing man
		if(man==STRAIGHT)
			Man();

		//Display Inverted man
		else if(man==INVERTED)
			UpsideDownMan();
			
	}
	
	/*
	Render now
	For immediate computation of objects
	*/
	glFlush();
}

//To Increase Speed of Spikes
void check_level()
{
	if((int)score<=100)
		level_val=10;

	else if((int)score>100 && (int)score<=200)
		level_val=13;

	else if((int)score>200 && (int)score<=400)
		level_val=16;

	else if((int)score>400 && (int)score<=600)
		level_val=20;

	else
		level_val=25;
}

/*
To obtain value of (xmin, xmax) of spikes for collision detection
*/
void getminmax()
{
	
	if(select_spike==LONG_SPIKE_DOWN || select_spike==LONG_SPIKE_UP)
	{
		xmin=500-x;
		xmax=800-x;
	}

	else if(select_spike==SHORT_SPIKE_DOWN || select_spike==SHORT_SPIKE_UP)
	{
		xmin=650-x;
		xmax=800-x;
	}
}

/*
To check if Charater has collided with incoming spikes
*/
void collision_detection()
{
	getminmax();
	
	//Check against min and max coordinates of character
	if((xmin < 100) && (xmax > 20))
	{
		if(man==STRAIGHT)
		{
			if(select_spike==SHORT_SPIKE_DOWN || select_spike==LONG_SPIKE_DOWN)
			{
				//Collision detected. Gameover
				game=GAMEOVER;

				glutPostRedisplay();
			}
		}

		else if(man==INVERTED)
		{
			if(select_spike==SHORT_SPIKE_UP || select_spike==LONG_SPIKE_UP)
			{
				//Collision detected. Gameover
				game=GAMEOVER;

				glutPostRedisplay();
			}
		}
	}
	
	//No Collision detected
	else
	{
		game=PLAY;
	}
}

/* Enables movement of spikes along both planes */
void animate_spikes()
{
	int delay;

	if(select_spike==LONG_SPIKE_DOWN)
	{
		Spike_long_down();
	}

	else if(select_spike==LONG_SPIKE_UP)
	{
		Spike_long_up();
	}
	
	else if(select_spike==SHORT_SPIKE_DOWN)
	{
		Spike_short_down();
	}

	else if(select_spike==SHORT_SPIKE_UP)
	{
		Spike_short_up();
	}
	
	//Sleep to enable object to be displayed on the screen for sometime
	for(delay=0;delay<1000000;delay++);

	//Swap Buffers for faster computation
	glutSwapBuffers();

	//To Redisplay window with object positions changed
	glutPostRedisplay();

	//Change x-coordinate of spikes to enable their movement
	x+=level_val;

	//To increase speed of spike
	check_level();

	//Check for collision detection
	collision_detection();
}

/*
Function to generate incoming spikes for game
*/
void generate_spikes()
{
	/*
	Initially, x=0
	Spikes will be drawn at far right corner of screen
	*/
	if(x>=900)
	{
		//Randomize type of spike to be displayed
		select_spike=rand()%4;

		//Reset x after a spike has moved to the end
		x=0;
	}

	else
	{
		//Update score only after a time interval
		if(x%20==0)
			score++;

		//Convert double to int
		int_score = (int)score;

		//Store value as a string
		sprintf(double_to_char, "Score : %2d", int_score);

		animate_spikes();
	}
}

void update_leaderboard(int new_score)//int score[5], int new_score)
{
	int pos;
	
	if(new_score>=highscore[0])
		pos=0;

	else
	{
		for(int i=0; i<4; i++)
			if(new_score<=highscore[i] && new_score>=highscore[i+1])
			{
				pos=i+1;
				break;
			}
	}

	for(int j=4; j>=pos; j--)
		highscore[j]=highscore[j-1];

	highscore[pos]=new_score;

	for(int i=0; i<5; i++)
		cout<<highscore[i]<<"\t";

	ofstream ofile("scores.dat", ios::binary|ios::trunc);

	if(!ofile)
	{
		printf("Open error");
	}

	for(int i=0; i<5; i++)
		ofile<<highscore[i]<<endl;

	ofile.close();

	//remove("scores.dat");
	//rename("temp.dat", "scores.dat");

}

void get_leaderboard()
{
	ifstream ifile("scores.dat", ios::binary);

	if(!ifile)
	{
		printf("Open Error");
	}

	for(int i=0; i<5; i++)
		ifile>>highscore[i];

	ifile.close();
}

void check_leaderboard(int new_score)
{

	get_leaderboard();
	
	if(new_score>highscore[4])
		update_leaderboard(new_score);

}

void display_leaderboard()
{
	get_leaderboard();

	glColor3f(0.0, 0.0, 0.0);

	for(int i=0; i<5; i++)
	{
		sprintf(double_to_char, "%d.           %2d", i+1, highscore[i]);
		renderBitmapString(300,330-(30*i), (void*)font, double_to_char);
	}
}

/*
Idle function
Contains functions for each state of game
*/
void PlayGame()
{
	if(game==MENU)
	{
		//Displays Main Menu
		Menu();

		/*
		Very essential.
		Since game has been declared with Double Buffer, Image won't load until buffers have been swapped.
		*/
		glutSwapBuffers();

		glutPostRedisplay();
	}

	else if(game==PLAY)
	{
		//Displays different types of spikes in a random order
		generate_spikes();
	}

	else if(game==GAMEOVER)
	{
		//Displays the gameover screen
		Gameover();
	
		/*
		Idle function is enterred several times.
		As a result, the score updating will also happen multiple time.
		Include a check value to ensure that the scores get updated only once
		*/
		if(check==FALSE)
		{
			check_leaderboard(int_score);
			check=TRUE;
		}

		glColor3f(1.0, 1.0, 1.0);
		sprintf(double_to_char, "Your Score : %2d", int_score);
		renderBitmapString(250,200, (void*)font, double_to_char);

		glutSwapBuffers();
		glutPostRedisplay();
	}

	else if(game==SCOREBOARD)
	{
		//Displays Leaderboard. Contains top 5 scorers
		Leaderboard();

		//Display top 5 scorers
		display_leaderboard();

		glutSwapBuffers();
		glutPostRedisplay();
	}
}


/*
Keyboard Function
To enable movements of man
s/S - down
w/W - up
*/
void keyPressed (unsigned char key, int x, int y) 
{  
	if(key=='s' || key=='S')
		man=STRAIGHT; 

	else if(key=='w' || key=='W')
		man=INVERTED;

	glutPostRedisplay();
}

/*
Mouse Function
Coordinates of buttons change depending on state of the game
*/
void  mouse(int button, int state, int x, int y)
{
	if(game==MENU)
	{
		if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
		{
			if((x>=310 && x<=456) && (y>=328 && y<=376))
				game=PLAY;

			else if((x>=463 && x<=556) && (y>=381 && y<=431))
				game=SCOREBOARD;

			else if((x>=232 && x<=314) && (y>=383 && y<=429))
				exit(0);
		}
	}

	else if(game==GAMEOVER)
	{
		if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
		{
			if((x>=49 && x<=222) && (y>=379 && y<=459))
				exit(0);
			
			else if((x>=600 && x<=760) && (y>=378 && y<=459))
				game=SCOREBOARD;
		}
	}

	else if(game==SCOREBOARD)
	{
		if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
		{
			if((x>=601 && x<=775) && (y>=408 && y<=482))
				exit(0);
		}
	}
}

/*
All images will be loaded at the beginning.
Helps in smoother movement of images as they aren't loaded from the file system everytime
*/
void LoadImages()
{
	/*
	SOIL_LOAD_AUTO: Retains format of image. Displays grayscale image otherwise
	SOIL_CREATE_NEW_ID: Creates a new idetifier for each image
	SOIL_FLAG_INVERT_Y: Flip y-coordinate of image so that window coordinates = openGL coordinates
	*/
	image_wall = SOIL_load_OGL_texture("stone_wall.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_man_straight = SOIL_load_OGL_texture("Up.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_man_inverted = SOIL_load_OGL_texture("Down.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_spike_long_down = SOIL_load_OGL_texture("spike_down_long.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_spike_long_up = SOIL_load_OGL_texture("spike_up_long.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_spike_short_up = SOIL_load_OGL_texture("spike_up_short.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_spike_short_down = SOIL_load_OGL_texture("spike_down_short.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_main_menu = SOIL_load_OGL_texture("Main_menu.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_gameover = SOIL_load_OGL_texture("game_over.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
	image_leaderboard = SOIL_load_OGL_texture("leaderboard.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);

}

int main (int argc, char **argv)
{
    glutInit (&argc, argv);

	//Enable Double Buffers. For smoother Animation
    glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGB);

	//Initialise Game Window
    glutInitWindowSize (800, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Man With a Plan");
    
	//Initialise all image files to be used
	LoadImages();

    glutDisplayFunc (display);
    
	//Enable use of textures
    glEnable(GL_TEXTURE_2D);

	//Defines 2D orthogonal projection matrix
	gluOrtho2D(0, 800, 0, 500);

	//To maintain aspect ratio even if screen size is changed
    glutReshapeFunc(changeViewPort);

	//To enable use of button with a click of the mouse
	glutMouseFunc(mouse);

	//To change states of player with keyboard
	glutKeyboardFunc(keyPressed);

	//To start timer to obtain time-lived in game. Used to calculate score
	//glutTimerFunc(25, update, 0);

	//Action to take when no event takes place
	glutIdleFunc(PlayGame);

	//Ensures functions run in a continuous loop
    glutMainLoop();

    return 0;
}
