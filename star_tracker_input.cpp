/*
 * star_tracker_input.cpp: 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/* OPENGL includes */
#include <GL/glew.h>
#include <glfw3.h>

// math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.hpp> // shader loading and compiling functions
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Sean Barrett's stb_image library - http://nothings.org
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // http://nothings.org
#include "hip_parser.hpp"

#define N_PARAMS 27
#define N_RECORDS 117955
#define N_RECORDS_MAX 117955
#define BRIGHTEST -1.0876
#define DARKEST 14.5622

#define RGB_CHANNELS 3

// static const double PI = 3.14159265358979323846;
/* globals ( TODO: implement without them or protect them )
 * --What’s the best naming prefix for a global variable? 
 * --// 
 * but then again useless class wraps are not the goal either
 */
Star** records;										// array of stars
Star* target;                                       // target spacecraft

bool add_target_flag = true;                        // show/hide target

static float vfov = glm::radians(10.69);			// Vertical field of view
// static float vfov = glm::radians(20.00);			// Vertical field of view
static int width = 2592;							// hor. resolution (pixels)
static int height = 1944;							// ver. resolution (pixels)
static float speed = .5;							// speed of rotation

GLFWwindow* window;

GLuint programID;									// handle for shaders

GLuint vertexPosition_modelspaceID;					// handle
GLuint vertexbuffer;								// buffer 
static GLfloat g_vertex_buffer_data[(N_RECORDS+1)*3];	// data + TARGET
GLuint vertexColorID;
GLuint colorbuffer;
static GLfloat g_color_buffer_data[(N_RECORDS+1)*RGB_CHANNELS];

GLuint matrixID;
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
glm::mat4 mvp;

float is_tgt_moving = false;                        // flag to move target
float rotate_delta, rotate_alpha;	                // declination, RA
float target_delta, target_alpha;                   // target coords
float tgt_delta_speed = 0.01, tgt_alpha_speed = 0.01; // TODO: make this proper

float yellow[] = { (float) 255/255, (float) 255/255, (float) 0/255 };
float blue[] = { (float) 0/255, (float) 0/255, (float) 255/255 };
float green[] = { (float) 0/255, (float) 255/255, (float) 0/255 };
float red[] = { (float) 255/255, (float) 0/255, (float) 0/255 };
float cyan[] = { (float) 0/255, (float) 255/255, (float) 255/255 };
float magenta[] = { (float) 255/255, (float) 0/255, (float) 255/255 };

/**
 * \brief Active sleep function
 */
int msleep(unsigned long millisec)   
{   
    struct timespec req={0};   
    time_t sec=(int)(millisec/1000);   
    millisec=millisec-(sec*1000);   
    req.tv_sec=sec;   
    req.tv_nsec=millisec*1000000L;   
    while(nanosleep(&req,&req)==-1)   
        continue;   
    return 1;   
} 

glm::vec3 spherical_to_cartesian(float r, float delta, float alpha)
{
	return glm::vec3(
		r*sin(alpha)*cos(delta),// x
		r*sin(delta), 			// y
		r*cos(alpha)*cos(delta)	// z
		);
	//return glm::vec3(
	//	r*cos(alpha)*cos(delta),// x
	//	r*sin(alpha)*cos(delta),// y
	//	r*sin(delta)	        // z
	//	);
}

glm::vec3 unit_sph_to_cart(float delta, float alpha)
{
	return spherical_to_cartesian(1, delta, alpha);
}

/**
 * TODO: make this realistic
 */
float Vmag_to_color(float Vmag)
{
	//float upper_lim = DARKEST - BRIGHTEST + 0; // constant offset for visibility
	//float inv_perc = 1 - (Vmag/upper_lim);
    float curve = 4.01;
    float offset = Vmag - BRIGHTEST;
    float normalized = offset/DARKEST;
    float inv_perc = exp(-curve*normalized);
    return inv_perc;
}

void change_mvp()//(float d_alpha, float d_delta)
{
	// rotate_alpha += d_alpha;
	// rotate_delta += d_delta;
	
	// Projection matrix
	float near_range = 0.1f;
	float far_range = 100.0f;
	Projection = glm::perspective(vfov, (float) width/(float) height,
											near_range, far_range);
	// Camera matrix
	glm::vec3 eye = glm::vec3(0,0,0);
	glm::vec3 direction = unit_sph_to_cart(rotate_delta, rotate_alpha);
	glm::vec3 center = direction + eye;	
	// keep the above direction even when off the origin
	// Right vector (could change this)
	glm::vec3 right = glm::vec3(
	    sin(rotate_alpha - PI/2.0f),
	    0,
	    cos(rotate_alpha - PI/2.0f)
	);
	// up is perpedicular to right and direction
	glm::vec3 up = glm::cross(right, direction); 
	// glm::vec3 up = glm::vec3(0,1,0);
	View = glm::lookAt(
	    eye,
	    center,
	    up
    );

	mvp = Projection * View * Model;

}

void compute_angles_from_inputs()
{

	// glfwGetTime is called only once, the first time this function is called
	static double last_time = glfwGetTime();

	// Compute time difference between current and last frame
	double curr_time = glfwGetTime();
	float delta_time = float(curr_time - last_time);

	// look up
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		if (rotate_delta+(delta_time*speed) > glm::radians(90.0f))		
		{
			// Don't let the camera go higher
			rotate_delta = glm::radians(90.0f);
		} else {
			rotate_delta += delta_time * speed;
		}
		std::cout << "RA: " << glm::degrees(rotate_alpha) 
			<< ", DE: " << glm::degrees(rotate_delta) << std::endl;
	}
	// look down
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		if (rotate_delta-(delta_time*speed) < glm::radians(-90.0f))		
		{
			// Don't let the camera go lower
			rotate_delta = glm::radians(-90.0f);
		} else {
			rotate_delta -= delta_time * speed;
		}
		std::cout << "RA: " << glm::degrees(rotate_alpha) 
			<< ", DE: " << glm::degrees(rotate_delta) << std::endl;
	}
	// look left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		if (rotate_alpha+(delta_time*speed) > glm::radians(360.0f))		
		{
			// restart the angle
			rotate_alpha = rotate_alpha-(delta_time*speed)-glm::radians(360.0f);
		} else {
			rotate_alpha += delta_time * speed;
		}
		std::cout << "RA: " << glm::degrees(rotate_alpha) 
			<< ", DE: " << glm::degrees(rotate_delta) << std::endl;
	}
	// look right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		if (rotate_alpha-(delta_time*speed) < glm::radians(0.0f))		
		{
			// restart the angle
			rotate_alpha = glm::radians(360.0f) + rotate_alpha-(delta_time*speed);
		} else {
			rotate_alpha -= delta_time * speed;
		}
		std::cout << "RA: " << glm::degrees(rotate_alpha) 
			<< ", DE: " << glm::degrees(rotate_delta) << std::endl;
	}

	// For the next frame, the "last time" will be "now"
	last_time = curr_time;
}

void move_target() 
{
    float seconds_passed = 0.01f; // assume constant time for now (TODO)
    target_delta = target_delta + (seconds_passed*tgt_delta_speed);
    target_alpha = target_alpha + (seconds_passed*tgt_alpha_speed);

    /********************* Update the buffers ********************************/
    // make new star
    target = new Star(0,target_alpha, target_delta, 0.0, 0.0, 0.0, target->Hpmag);
    g_vertex_buffer_data[3*0+0] = target->y;
    g_vertex_buffer_data[3*0+1] = target->z;
    g_vertex_buffer_data[3*0+2] = target->x;
	
    printf("RA: %f, DE: %f target\n", glm::degrees(target_alpha), 
            glm::degrees(target_delta)); 
    //printf("x,y,z: %f,%f,%f\n", g_vertex_buffer_data[0], 
    //        g_vertex_buffer_data[1], g_vertex_buffer_data[2]);
    // Re-bind the buffers
    glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), 
		g_vertex_buffer_data, GL_STATIC_DRAW);
}

void screencapture()
{
    int g_gl_width = width;
    int g_gl_height = height; 
    
	unsigned char *buffer = (unsigned char *)malloc(g_gl_width*g_gl_height*3);
	glReadPixels(0, 0, g_gl_width, g_gl_height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	char name[1024];
	printf( " writing screenshot_ra_%.2f_de_%.2f.png", glm::degrees(rotate_alpha),
				glm::degrees(rotate_delta) );
	sprintf( name, "screenshot_ra_%.2f_de_%.2f__tra_%.2f_tde_%.2f.png", 
            glm::degrees(rotate_alpha), glm::degrees(rotate_delta),
            glm::degrees(target_alpha), glm::degrees(target_delta));
	unsigned char *last_row = buffer + ( g_gl_width * 3 * ( g_gl_height - 1 ) );
	if ( !stbi_write_png( name, g_gl_width, g_gl_height, 3, last_row,
												-3 * g_gl_width ) ) {
		fprintf( stderr, "ERROR: could not write screenshot file %s\n", name );
	}
	free( buffer );
	return ;    
}

void free_resources(int depth)
{
	// free the stars
	for (int i = 0; i < N_RECORDS; ++i) { delete records[i]; }
	delete [] records;

    delete target;

	if (depth > 0)
	{
		// free Gl buffers, program, terminate glfw
		// Cleanup VBO
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteBuffers(1, &colorbuffer);
		glDeleteProgram(programID);

	    // Close OpenGL window and terminate GLFW
	    glfwTerminate();
	}

	return;
}

 
void init()
{
	/* initialize random seed: */
	srand (time(NULL));

    /******************************** Add target ******************************/
    float target_HpMag;
    if (add_target_flag == true) {
        target_HpMag = BRIGHTEST;
    } else {
        // make target black
        target_HpMag = DARKEST;
    }
    // TODO: make it easy to generate the target elsewhere
    target = new Star(); // starts at origin
    target->Hpmag = target_HpMag;
    printf("Hpmag: %f\n", target_HpMag);

	/************************** Parse star catalog ****************************/
	records = new Star*[N_RECORDS_MAX];
	for (int i = 0; i < N_RECORDS_MAX; ++i)
	{
		records[i] = new Star();
	}

	// parse returns true if there is an error
	if (parse_stcatalog(0, records)) {
		std::cerr << "parse_stcatalog failed" << std::endl;
		free_resources(0);
		std::exit(1);
	}

    // sort catalog according to Hpmag
    // TODO: sort doesn't work
    printf("Highest Hpmag: %f, Lowest: %f\n", records[0]->Hpmag, 
            records[N_RECORDS_MAX-1]->Hpmag);
    sort_stars(records, N_RECORDS_MAX);
    printf("Highest Hpmag: %f, Lowest: %f\n", records[0]->Hpmag, 
            records[N_RECORDS_MAX-1]->Hpmag);
	/**************************** Initialise GLFW *****************************/
	if( !glfwInit() )
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		free_resources(0);
		std::exit(1);
	}

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_RESIZABLE,GL_TRUE); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Open a window & create its context; window is a global var for simplicity
	window = glfwCreateWindow(width, height, "ST Simulator", NULL, NULL);
	if( window == NULL ){
		fprintf(stderr, "Failed to open GLFW window\n");
		getchar();
		free_resources(1);
		std::exit(1);
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		free_resources(1);
		std::exit(1);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test (probably don't need this for stars)
	// glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	// glDepthFunc(GL_LESS);
	glEnable(GL_PROGRAM_POINT_SIZE);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("VertexShader.vertexshader",
							"FragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	matrixID = glGetUniformLocation(programID, "MVP");

	// Get a handle for our buffers
	vertexPosition_modelspaceID = glGetAttribLocation(programID,
		"vertexPosition_modelspace");
	vertexColorID = glGetAttribLocation(programID,
		"vertexColorID");



	/***************************** Initialise MVP *****************************/
	// Projection matrix
	float near_range = 0.1f;
	float far_range = 100.0f;
	glm::mat4 Projection = glm::perspective(vfov, (float) width/(float) height,
											near_range, far_range);
	// Camera matrix
	glm::vec3 eye = glm::vec3(0,0,0);
	rotate_delta = 0;
	rotate_alpha = 0;
	glm::vec3 center = unit_sph_to_cart(rotate_delta, rotate_alpha); //direction
	center = center + eye;	// keep the above direction even when off the origin
	glm::vec3 up = glm::vec3(0,1,0); // upright
	glm::mat4 View = glm::lookAt(
	    eye,
	    center,
	    up
	    );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	mvp = Projection * View * Model;

    /****************************** Add target ********************************/
    float clr;
    g_vertex_buffer_data[3*0+0] = target->y;
    g_vertex_buffer_data[3*0+1] = target->z;
    g_vertex_buffer_data[3*0+2] = target->x;
    
    clr = Vmag_to_color(target->Hpmag);
    printf("color: %f\nx,y,z: %f,%f,%f\n",clr, g_vertex_buffer_data[0], g_vertex_buffer_data[0], g_vertex_buffer_data[0]);

    g_color_buffer_data[3*0+0] = clr;
    g_color_buffer_data[3*0+1] = clr; // monochrome
    g_color_buffer_data[3*0+2] = clr; // monochrome

    //g_vertex_buffer_data[3*N_RECORDS+0] = target->x;
    //g_vertex_buffer_data[3*N_RECORDS+1] = target->y;
    //g_vertex_buffer_data[3*N_RECORDS+2] = target->z;
    //
    //clr = Vmag_to_color(target->Hpmag);
    //printf("%f",clr);

    //g_color_buffer_data[3*N_RECORDS+0] = clr;
    //g_color_buffer_data[3*N_RECORDS+1] = 0.0f; // monochrome
    //g_color_buffer_data[3*N_RECORDS+2] = 0.0f; // monochrome

	/************************* Initialise buffer data *************************/
	//float clr;
	for (int i = 1; i < N_RECORDS+1; i++)
	{
		// TODO: scale to increase distance
		//g_vertex_buffer_data[3*i+0] = records[i-1]->x;
		//g_vertex_buffer_data[3*i+1] = records[i-1]->y;
		//g_vertex_buffer_data[3*i+2] = records[i-1]->z;
		g_vertex_buffer_data[3*i+0] = records[i-1]->y;
		g_vertex_buffer_data[3*i+1] = records[i-1]->z;
		g_vertex_buffer_data[3*i+2] = records[i-1]->x;
		
		clr = Vmag_to_color(records[i-1]->Hpmag);
		// clr = 1.0f; // all white
		// clr = 0.0f; // all black
		g_color_buffer_data[3*i+0] = clr;
		g_color_buffer_data[3*i+1] = clr; // monochrome
		g_color_buffer_data[3*i+2] = clr; // monochrome
        
        // // if Sirius A
        // if (records[i-1]->id == 32349) {
    		// g_color_buffer_data[3*i+0] = yellow[0];
    		// g_color_buffer_data[3*i+1] = yellow[1]; // monochrome
    		// g_color_buffer_data[3*i+2] = yellow[2]; // monochrome
        // }
        // // if Vega 
        // if (records[i-1]->id == 91262) {
    		// g_color_buffer_data[3*i+0] = cyan[0];
    		// g_color_buffer_data[3*i+1] = cyan[1]; // monochrome
    		// g_color_buffer_data[3*i+2] = cyan[2]; // monochrome
        // }
	}
    printf("end color: %f\nx,y,z: %f,%f,%f\n",clr, g_vertex_buffer_data[3*N_RECORDS], g_vertex_buffer_data[3*N_RECORDS+1], g_vertex_buffer_data[3*N_RECORDS+2]);
    
	/********************* Generate and bind the buffers **********************/	
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), 
		g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), 
		g_color_buffer_data, GL_STATIC_DRAW);

}

void display()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	// register arrow keys and move the camera around
	compute_angles_from_inputs();
	change_mvp();
    
    // Use 'M' to start moving the target
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        is_tgt_moving = !(is_tgt_moving);

    if (is_tgt_moving)
        move_target();

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(vertexPosition_modelspaceID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		vertexPosition_modelspaceID, // The attribute we want to configure
		3,                           // size
		GL_FLOAT,                    // type
		GL_FALSE,                    // normalized?
		0,                           // stride
		(void*)0                     // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(vertexColorID);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		vertexColorID,               // The attribute we want to configure
		3,                           // size
		GL_FLOAT,                    // type
		GL_FALSE,                    // normalized?
		0,                           // stride
		(void*)0                     // array buffer offset
	);

	// Draw the points !
	glDrawArrays(GL_POINTS, 0, (N_RECORDS+1)*3);

    // Take screenshot on S key press
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        screencapture();

	// msleep(100);
	glDisableVertexAttribArray(vertexPosition_modelspaceID);
	glDisableVertexAttribArray(vertexColorID);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main(int argc, char const *argv[])
{
	init();

	do{
		display();	
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	free_resources(1);
	return 0;
}
