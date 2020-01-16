#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string.h>

#include "objekti.h"

static void on_display();
static void on_reshape(int width, int height);
static void on_key_press(unsigned char key, int x, int y);
static void on_key_release(unsigned char key, int x, int y);
static void on_timer(int timer_id);
static void on_timer1(int id);

#define MAX_METKOVA (255)
Metak metkovi[MAX_METKOVA];
int br_ispaljeniih_metaka=0;

Prepreka prepreke[MAX_PREPREKA];
int br_pogodjenih_prepreka=0;

/* GLOBALNE PROMENLJIVE */

float pozicija = 0.0f;

bool go_left = false;
bool go_right = false;

int timer_id = 0;
int timer_interval = 15;
bool kraj_simulacije = false;


int screen_width = 0;
int screen_height = 0;

float cooldown = 0;

int animation_ongoing = 0;

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(800, 600);

	glutInitWindowPosition(100, 100);

	glutCreateWindow("Reflex check");

	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutKeyboardFunc(on_key_press);
	glutKeyboardUpFunc(on_key_release);
    	/*glutPassiveMotionFunc(on_mouse_move);*/
    	/*glutMotionFunc(on_mouse_move);*/
    	glutTimerFunc(timer_interval, on_timer, timer_id);

	glClearColor(1, 1, 1, 0);

	glEnable(GL_DEPTH_TEST);

	GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
	GLfloat light_diffuse[] = { 1, 1, 1, 1 };
	GLfloat light_specular[] = { 1, 1, 1, 1 };
	
	// Ambijentalno osvetljenje scene. 
	GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_position[] = { 1, 1, 1, 0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

	// Normalizacija normala 
	glEnable(GL_NORMALIZE);

	// Pozicionira se svijetlo 
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);	
	
	   
	// Inicijalizacija metkova i prepreka
	for (int i=0; i < MAX_METKOVA; i++) {
		metkovi[i].x = 0;
		metkovi[i].z = -0.5;
	}

	inicijalizuj_prepreke();

	glutMainLoop();

	return 0;
}

static void on_display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 1, 2, 0, 0, 0, 0, 1, 0);
	
	//Pistolj
    	glPushMatrix();
        	glTranslatef(pozicija, 0, 0);
        	iscrtaj_pistolj();
    	glPopMatrix();

	//Metak
    	for (int i=0; i < br_ispaljeniih_metaka; i++) {
        	glPushMatrix();
        	    	glTranslatef(metkovi[i].x,0,metkovi[i].z);
            	iscrtaj_metak();
        	glPopMatrix();
    	}

	//Prepreke
	nacrtaj_prepreke();
	
	//Zid
	glPushMatrix();
		glScalef(1000, 1000, 1);
		glTranslatef(0, 0, -20);
		iscrtaj_zid();
	glPopMatrix();

	//Tekst
	if(animation_ongoing){
   		char str[255];
   		sprintf(str, "Metkovi: %d / %d", br_ispaljeniih_metaka, MAX_METKOVA);
    		ispisi_tekst(str, 2, 10, 0, 1, 0, screen_width, screen_height);
    		char str1[255];
    		sprintf(str1, "Br. pogodaka: %d / %d", br_pogodjenih_prepreka, MAX_PREPREKA);
   		ispisi_tekst(str1, screen_width - strlen(str1) - 210, 10, 0, 1, 0, screen_width, screen_height);

		if (kraj_simulacije || br_pogodjenih_prepreka==10) {
			glPushMatrix();
				glScalef(1000, 1000, 1);
				glTranslatef(0, 0, 1);
				iscrtaj_zid();
			glPopMatrix();
			
			if(br_pogodjenih_prepreka==10){
				char str2[255];
				sprintf(str2, "Pobedio si, pogodjeno: %d", br_pogodjenih_prepreka);
				ispisi_tekst(str2, screen_width/2 - strlen(str1) - 120, screen_height/2-5, 0, 1, 0, screen_width, screen_height);
				ispisi_tekst("          ESC - izlaz ", screen_width/2 - strlen(str1) - 120, screen_height/2-40, 0, 1, 0, screen_width, screen_height);
			}
			else{	
        			char str2[255];
        			sprintf(str2, "Izgubio si, pogodjeno: %d", br_pogodjenih_prepreka);
				ispisi_tekst(str2, screen_width/2 - strlen(str1) - 120, screen_height/2-5, 0, 1, 0, screen_width, screen_height);
				ispisi_tekst("          ESC - izlaz ", screen_width/2 - strlen(str1) - 120, screen_height/2-40, 0, 1, 0, screen_width, screen_height);
			}
		}
	}

	if(!animation_ongoing){
		glPushMatrix();
			glScalef(1000, 1000, 1);
			glTranslatef(0, 0, 1);
			iscrtaj_zid();
		glPopMatrix();

		char str1[255] = "'K/k' - Pokretanje simulacije";
		ispisi_tekst(str1, screen_width/2 - strlen(str1) - 120, screen_height/2+67, 0, 1, 0, screen_width, screen_height);

		char str2[255] = "'D/d' - Pomeranje pistolja desno";
		ispisi_tekst(str2, screen_width/2 - strlen(str1) - 120, screen_height/2-10, 0, 1, 0, screen_width, screen_height);

		char str3[255] = "'A/a' - Pomeranje pistolja levo";
		ispisi_tekst(str3, screen_width/2 - strlen(str1) - 120, screen_height/2+17, 0, 1, 0, screen_width, screen_height);

		char str4[255] = "'G/g' - Ispaljivanje metkova";
		ispisi_tekst(str4, screen_width/2 - strlen(str1) - 120, screen_height/2-36, 0, 1, 0, screen_width, screen_height);

	}
	glutSwapBuffers();
}

static void on_reshape(int width, int height)
{
	glViewport(0, 0, width, height);
    	screen_width = width;
    	screen_height = height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, (float) width / height, 0.1, 1500);
}

//Komande na tastaturi
static void on_key_press(unsigned char key, int x, int y)
{
	switch (key) {
        case 27:
		exit(EXIT_SUCCESS);
		break;
        case 'a':
        case 'A':
            	go_left = true;
            	break;
        case 'd':
        case 'D':
        	go_right = true;
            	break;
        case 'g':
        case 'G':
            	if (cooldown == 0) {
                	metkovi[br_ispaljeniih_metaka++].x = pozicija;
                	cooldown = 50;
            	}
            	break;
	case 'k':
	case 'K':
        	if (!kraj_simulacije) {
            		animation_ongoing=1;
            		glutTimerFunc(17, on_timer1, 0);
            		glutPostRedisplay();
        	}
		break;
	}
}


static void on_key_release(unsigned char key, int x, int y) {

	switch (key) {
        case 'a':
        case 'A':
            	go_left = false;
            	break;
        case 'd':
        case 'D':
        	go_right = false;
           	break;
	}

}


//Tajmer
static void on_timer(int id) {
   	if (id != timer_id || kraj_simulacije)
        	return;

	float parametar = 0.01;
	if (go_left && !go_right) {
	        if (pozicija > -1.0)
	            pozicija -= parametar;
    	} else if (go_right && !go_left) {
        	if (pozicija < 1.0)
			pozicija += parametar;
	}

	for (int i=0; i < br_ispaljeniih_metaka; i++) {
		if (metkovi[i].z > -100.0)
  			metkovi[i].z -= 0.1;
	}

  	if (cooldown > 0)
        	cooldown -= 1;

	//Kraj simulacije
    	if (prepreke[MAX_PREPREKA-1].x < -10)
        	kraj_simulacije = true;
    
	//Sudar prereke i metka
	float radius = 0.15;
	for (int i=0; i < br_ispaljeniih_metaka; i++) {
		for (int j=0; j < MAX_PREPREKA; j++) {
			float x_diff = prepreke[j].x - metkovi[i].x;
			float z_diff = prepreke[j].z - metkovi[i].z;

		if (!prepreke[j].is_pogodjena) {
                	if (x_diff*x_diff + z_diff*z_diff < radius) {
                		prepreke[j].is_pogodjena = true;
                		br_pogodjenih_prepreka++;
                		}
            		}
        	}
	}

	glutPostRedisplay();
	glutTimerFunc(timer_interval, on_timer, timer_id);
}

//Tajmer za prepreke
static void on_timer1(int id){
	if(id != 0 || kraj_simulacije)
		return;

	azuriraj_prepreke();
	
	if(animation_ongoing)
		glutTimerFunc(17, on_timer1, 0);
}
