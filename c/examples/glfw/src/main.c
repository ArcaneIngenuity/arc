#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> //for vprintf -- to console

//#include <windows.h>

#define GLEW_STATIC
#include "glew.h" //this includes gl.h AND extension wrangling capabilities. Superb... if it works with clang. 
#include "glfw3.h"


#include "../../../core/Disjunction.h"

#include "BasicModel.h"

int c = 0; //DEV

void error_callback(int error, const char* description)
{
    //fputs(description, stderr);
	printf("%s\n", description);
}

int main(int argc, char *argv[])
{
	glfwSetErrorCallback(error_callback);
	
	if (!glfwInit())
		exit(EXIT_FAILURE);
	else
		printf("Hello World\n");

	GLFWwindow* window;        
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	window = glfwCreateWindow(1024, 768, "GLFW Test", NULL, NULL);
	//printf("%d\n", glGetError());
	
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
	//glfwSetWindowTitle(window, "GLFW Test");	
	//printf("%d\n", glGetError());

	glfwMakeContextCurrent(window);
	
	//TODO GLFW setup to be done as part of App (start, finish)
	//TODO GLFW setup to be done possible as custom start / finish functions for Disjunction object
	//DJ...//

	Disjunction disjunction;
	disjunction.timer = Timer_constructor(0.016666667);
	disjunction.timer->disjunction = &disjunction;
	
	App app;
	app.disjunction = &disjunction;
	disjunction.apps[0] = &app; //more likely, we would pass pointer to existing element at 0 (and so forth) to Builder
	
	//BasicModel * model = malloc(sizeof(BasicModel));
	//model->a = 15;
	//printf("boo %i", model->a);
	
	Disjunction_start((Disjunction * const)&disjunction);
	Timer * timer = disjunction.timer;
	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		Timer_updateDeltaSec(timer);
		printf("deltaSec %.10f\n", timer->deltaSec);
		
		Timer_accumulate(timer);
		while (Timer_canUpdate(timer))
		{
			Timer_update(timer);
			c++;
		}
		
		printf("c=%i\n", c);
		c = 0;
		glfwSwapBuffers(window);
	}
	
	//...conclude glfW
	//TODO also move to App or Disjunction
	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}

