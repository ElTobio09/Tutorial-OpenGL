//  Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"layout(location=1) in vec4 aColor;\n"

"out vec4 ourColor;\n" //Variable que se pasa al fragment shader.

"uniform mat4 mvp;\n"// matriz que transforma el vértice.

"void main()\n"
"{\n"
" gl_Position = mvp * vec4(aPos, 1.0);\n"
" ourColor = aColor;\n" //Pasa el color al fragment shader.
"}\0";

//  Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"in vec4 ourColor;\n"
"out vec4 FragColor;\n"
//Recibe el color interpolado.
"void main()\n"
"{\n"
" FragColor = ourColor;\n"
"}\n\0";
