#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

int main() 
{   
	// Inicializar GLFW
	glfwInit();

	// Mencionar que version de OpenGl estamos usado
	// En este caso estamos usando 3.3

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Mencionar que usamos el Core Profile
	// Solo tenemos funciones modernas
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// Crear una GLWwindow un objeto de 800 x 800 pixeles de Nombre OPENGL
	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGl",NULL, NULL);

	// Gestion de Errores si la ventana tiene algun erro al crearce
	if (window == NULL)
	{
     
		std::cout << "Error al crear GLFW window" << std::endl;
		glfwTerminate();
		return -1;

	}

	// Introductir la ventana dentro del cotexto actual
	glfwMakeContextCurrent(window);
	
	//Cargar Glad configurado OpenGl
	gladLoadGL();

	// Especificar la ventana grafica de OpenGl en la ventana
	// En este caso la venan grafica se abre desde x=0, y=0, a x=800, y=800 
	glViewport(0, 0, 800, 800);

	// Especificar el color del Fondo
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	//limpiar the back buffer y asinar el nuevo color a este
	glClear(GL_COLOR_BUFFER_BIT);
	// intercambio de back buffer con el buffer frontal
	glfwSwapBuffers(window);

	//Bucle principal While
	while (!glfwWindowShouldClose(window)) 
	{

		//tener cuidado con todos los GFLW eventos
		glfwPollEvents();
	}

	//Borrar la ventana despues de finalizar el programa
	glfwDestroyWindow(window);
	//Terminar GLFW depues de finalizar
	glfwTerminate();
	return 0;




}