#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Variables cámara
float cameraDistance = 15.0f;
float cameraYaw = 90.0f;
float cameraPitch = 10.0f;

bool isDragging = false;

double previousMouseX = 0.0;
double previousMouseY = 0.0;

// Movimiento cámara
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!isDragging) return;

    float sensitivity = 0.2f;

    float offsetX = xpos - previousMouseX;
    float offsetY = previousMouseY - ypos;

    previousMouseX = xpos;
    previousMouseY = ypos;

    cameraYaw += offsetX * sensitivity;
    cameraPitch += offsetY * sensitivity;

    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;

    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;
}

// Zoom cámara
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraDistance -= (float)yoffset * 0.3f;

    if (cameraDistance < 1.0f)
        cameraDistance = 1.0f;

    if (cameraDistance > 40.0f)
        cameraDistance = 40.0f;
}

// Datos de vértice
struct Vertex
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};

// Crear curva Bézier
std::vector<glm::vec3> createBezierCurve(
    glm::vec3 p0,
    glm::vec3 p1,
    glm::vec3 p2,
    glm::vec3 p3,
    int segments)
{
    std::vector<glm::vec3> points;

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        float u = 1.0f - t;

        glm::vec3 point =
            u * u * u * p0 +
            3.0f * u * u * t * p1 +
            3.0f * u * t * t * p2 +
            t * t * t * p3;

        points.push_back(point);
    }

    return points;
}

// Posición sobre curva
glm::vec3 getBezierPosition(
    glm::vec3 p0,
    glm::vec3 p1,
    glm::vec3 p2,
    glm::vec3 p3,
    float t)
{
    float u = 1.0f - t;

    glm::vec3 point =
        u * u * u * p0 +
        3.0f * u * u * t * p1 +
        3.0f * u * t * t * p2 +
        t * t * t * p3;

    return point;
}

// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    Normal = mat3(transpose(inverse(model))) * aNormal;

    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 texColor = texture(texture1, TexCoord).rgb;

    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(lightPos - FragPos);

    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * texColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * texColor;

    float specularStrength = 0.5;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec =
        pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 specular =
        specularStrength * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
)";

// ======================================
// MAIN
// ======================================

int main()
{
    // Inicializar GLFW
    glfwInit();

    // Configuración OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    // Crear ventana
    GLFWwindow* window =
        glfwCreateWindow(
            1280,
            720,
            "Whale Ocean",
            NULL,
            NULL
        );

    // Verificar ventana
    if (!window)
    {
        std::cout << "Error ventana\n";

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    // Eventos mouse
    glfwSetCursorPosCallback(
        window,
        mouse_callback
    );

    glfwSetScrollCallback(
        window,
        scroll_callback
    );

    // Click derecho cámara
    glfwSetMouseButtonCallback(
        window,
        [](GLFWwindow* window,
            int button,
            int action,
            int mods)
        {
            if (button == GLFW_MOUSE_BUTTON_RIGHT)
            {
                if (action == GLFW_PRESS)
                {
                    isDragging = true;

                    glfwGetCursorPos(
                        window,
                        &previousMouseX,
                        &previousMouseY
                    );
                }

                else if (action == GLFW_RELEASE)
                {
                    isDragging = false;
                }
            }
        });

    // Inicializar GLAD
    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error GLAD\n";

        return -1;
    }

    // Tamaño viewport
    glViewport(0, 0, 1280, 720);

    // Profundidad 3D
    glEnable(GL_DEPTH_TEST);

    // =========================
    // SHADERS PRINCIPALES
    // =========================

    GLuint vertexShader =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        NULL
    );

    glCompileShader(vertexShader);

    GLuint fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        NULL
    );

    glCompileShader(fragmentShader);

    GLuint shaderProgram =
        glCreateProgram();

    glAttachShader(
        shaderProgram,
        vertexShader
    );

    glAttachShader(
        shaderProgram,
        fragmentShader
    );

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // =========================
    // SHADER CURVA
    // =========================

    GLuint lineVS =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        lineVS,
        1,
        &lineVertexShader,
        NULL
    );

    glCompileShader(lineVS);

    GLuint lineFS =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        lineFS,
        1,
        &lineFragmentShader,
        NULL
    );

    glCompileShader(lineFS);

    GLuint lineProgram =
        glCreateProgram();

    glAttachShader(lineProgram, lineVS);
    glAttachShader(lineProgram, lineFS);

    glLinkProgram(lineProgram);

    glDeleteShader(lineVS);
    glDeleteShader(lineFS);

    // =========================
    // CARGAR MODELO OBJ
    // =========================

    Assimp::Importer importer;

    const aiScene* scene =
        importer.ReadFile(
            "models/whale.obj",
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenNormals
        );

    if (!scene || !scene->mRootNode)
    {
        std::cout << "Error OBJ\n";

        return -1;
    }

    aiMesh* mesh = scene->mMeshes[0];

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Guardar vértices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.Position =
        {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        vertex.TexCoords =
        {
            mesh->mTextureCoords[0]
            ? mesh->mTextureCoords[0][i].x
            : 0.0f,

            mesh->mTextureCoords[0]
            ? mesh->mTextureCoords[0][i].y
            : 0.0f
        };

        vertex.Normal =
        {
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        };

        vertices.push_back(vertex);
    }

    // Guardar índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // =========================
    // VAO VBO EBO
    // =========================

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        EBO
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // Posición
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, TexCoords)
    );

    glEnableVertexAttribArray(1);

    // Normales
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Normal)
    );

    glEnableVertexAttribArray(2);

    // =========================
    // TEXTURA
    // =========================

    GLuint texture;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    int w, h, c;

    unsigned char* data =
        stbi_load(
            "models/whale.jpg",
            &w,
            &h,
            &c,
            0
        );

    if (data)
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            w,
            h,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);

    // =========================
    // PUNTOS CURVA
    // =========================

    glm::vec3 p0(-10.0f, 0.0f, -10.0f);
    glm::vec3 p1(-5.0f, 5.0f, 10.0f);
    glm::vec3 p2(5.0f, -5.0f, -10.0f);
    glm::vec3 p3(10.0f, 0.0f, 10.0f);

    std::vector<glm::vec3> curve =
        createBezierCurve(
            p0,
            p1,
            p2,
            p3,
            100
        );

    GLuint curveVAO, curveVBO;

    glGenVertexArrays(1, &curveVAO);
    glGenBuffers(1, &curveVBO);

    glBindVertexArray(curveVAO);

    glBindBuffer(GL_ARRAY_BUFFER, curveVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        curve.size() * sizeof(glm::vec3),
        curve.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Velocidad movimiento
    float speed = 0.05f;

    // =========================
    // LOOP PRINCIPAL
    // =========================

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Fondo océano
        glClearColor(
            0.0f,
            0.2f,
            0.4f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        // Posición cámara orbital
        glm::vec3 cameraPos;

        cameraPos.x =
            cameraDistance *
            cos(glm::radians(cameraYaw)) *
            cos(glm::radians(cameraPitch));

        cameraPos.y =
            cameraDistance *
            sin(glm::radians(cameraPitch));

        cameraPos.z =
            cameraDistance *
            sin(glm::radians(cameraYaw)) *
            cos(glm::radians(cameraPitch));

        // Matriz view
        glm::mat4 view =
            glm::lookAt(
                cameraPos,
                glm::vec3(0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );

        // Perspectiva
        glm::mat4 projection =
            glm::perspective(
                glm::radians(45.0f),
                1280.0f / 720.0f,
                0.1f,
                100.0f
            );

        // Movimiento ballena
        float t =
            (sin(glfwGetTime() * speed) + 1.0f)
            / 2.0f;

        glm::vec3 whalePos =
            getBezierPosition(
                p0,
                p1,
                p2,
                p3,
                t
            );

        // Matriz modelo
        glm::mat4 model =
            glm::mat4(1.0f);

        model = glm::translate(
            model,
            whalePos
        );

        model = glm::rotate(
            model,
            glm::radians(-90.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        model = glm::scale(
            model,
            glm::vec3(15.0f)
        );

        // Dibujar modelo
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(
            glGetUniformLocation(
                shaderProgram,
                "model"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(
                shaderProgram,
                "view"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(
                shaderProgram,
                "projection"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        glUniform3f(
            glGetUniformLocation(
                shaderProgram,
                "lightPos"
            ),
            10.0f,
            10.0f,
            10.0f
        );

        glUniform3fv(
            glGetUniformLocation(
                shaderProgram,
                "viewPos"
            ),
            1,
            glm::value_ptr(cameraPos)
        );

        glBindTexture(
            GL_TEXTURE_2D,
            texture
        );

        glBindVertexArray(VAO);

        glDrawElements(
            GL_TRIANGLES,
            indices.size(),
            GL_UNSIGNED_INT,
            0
        );

        // Dibujar curva
        glUseProgram(lineProgram);

        glUniformMatrix4fv(
            glGetUniformLocation(
                lineProgram,
                "view"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(
                lineProgram,
                "projection"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        glBindVertexArray(curveVAO);

        glDrawArrays(
            GL_LINE_STRIP,
            0,
            curve.size()
        );

        glfwSwapBuffers(window);
    }

    // Liberar memoria
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    return 0;
}