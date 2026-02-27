#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

// Código fuente del Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// Código fuente del Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

int main()
{
	// Inicializar GLFW
	glfwInit();

	// Indicar a GLFW qué versión de OpenGL usar
	// En este caso usamos OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Indicar a GLFW que usaremos el perfil CORE
	// Esto significa que solo tendremos las funciones modernas
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Crear una ventana GLFW de 800x800 píxeles con nombre "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);

	// Comprobar si la ventana no se creó correctamente
	if (window == NULL)
	{
		std::cout << "No se pudo crear la ventana GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Hacer la ventana parte del contexto actual
	glfwMakeContextCurrent(window);

	// Cargar GLAD para configurar OpenGL
	gladLoadGL();

	// Especificar el viewport de OpenGL en la ventana
	// En este caso va desde x = 0, y = 0, hasta x = 800, y = 800
	glViewport(0, 0, 800, 800);

	// Crear el objeto Vertex Shader y obtener su referencia
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Adjuntar el código fuente del Vertex Shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compilar el Vertex Shader
	glCompileShader(vertexShader);

	// Crear el Fragment Shader y obtener su referencia
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Adjuntar el código fuente del Fragment Shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compilar el Fragment Shader
	glCompileShader(fragmentShader);

	// Crear el Shader Program y obtener su referencia
	GLuint shaderProgram = glCreateProgram();
	// Adjuntar los shaders al Shader Program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// Enlazar los shaders en el Shader Program
	glLinkProgram(shaderProgram);

	// Borrar los shaders ahora que ya no son necesarios
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Coordenadas de los vértices
	GLfloat vertices[] =
	{
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Esquina inferior izquierda
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Esquina inferior derecha
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f // Esquina superior
	};

	// Crear contenedores de referencia para el VAO y VBO
	GLuint VAO, VBO;

	// Generar el VAO y el VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Hacer que el VAO sea el actual
	glBindVertexArray(VAO);

	// Enlazar el VBO indicando que es un GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introducir los vértices en el VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configurar los atributos del vértice para que OpenGL los entienda
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Habilitar el atributo de vértice
	glEnableVertexAttribArray(0);

	// Desenlazar el VAO y VBO para evitar cambios accidentales
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Bucle principal
	while (!glfwWindowShouldClose(window))
	{
		// Especificar el color de fondo
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Limpiar el buffer de color aplicando el color anterior
		glClear(GL_COLOR_BUFFER_BIT);
		// Indicar a OpenGL qué Shader Program usar
		glUseProgram(shaderProgram);
		// Enlazar el VAO
		glBindVertexArray(VAO);
		// Dibujar el triángulo
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// Intercambiar buffers (back buffer y front buffer)
		glfwSwapBuffers(window);
		// Procesar eventos de GLFW
		glfwPollEvents();
	}

	// Borrar todos los objetos creados
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	// Destruir ventana antes de terminar el programa
	glfwDestroyWindow(window);
	// Terminar GLFW
	glfwTerminate();
	return 0;
}