#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "BezierCurve.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "basic_camera.h"
#include "camera.h"
#include "pointLight.h"
#include "sphere.h"
#include "cube.h"
#include "stb_image.h"

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

using namespace std;
static bool qKeyPressed = false;
bool toggleTexture = true;
//function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawTableChair(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c);

void drawCube(unsigned int& VAO, Shader& lightingShader, glm::mat4 model, glm::vec3 color);
void readingRoom(Shader& lightingShaderWithTexture,Cube& cube,Cube& gg,Cube& cube2,Cube& cubeback,Cube& cube3,Cube& win,Cube& sofa,Cube& cube5, Cube& keycab,Cube& win2);
void bedroom(Shader& lightingShaderWithTexture, Cube& cube, Cube& bed, Cube& cube2, Cube& cubeback, Cube& cube3, Cube& win, Cube& sofa, Cube& cube5, Cube& keycab, Cube& win2);

void building(Shader& lightingShaderWithTexture, Cube& cube, Cube& glassCube, Cube& cube2, Cube& cubeback, Cube& cube3, Cube& win, Cube& sofa, Cube& t1, Cube& water, Cube& woodf, float time, Cube& bookcab, Cube& boun, Cube& stove, Cube& sc, Cube& crok, Cube& st);
void drawFractalTree(glm::mat4 model, Shader& shader, int depth, float length, float angle, Cube& branchCube,float time);

void drawScene(Shader& shader, BezierCurve& curve) {
    // Define model transformation for the curve
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.5f, 1.1f, -4.0f)); // Adjust position
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); // Adjust scale

    // Set the model matrix uniform in the shader
    shader.setMat4("model", model);

    // Draw the Bezier curve
    curve.drawBezierCurve(shader, model);
}



unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);



//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//screen
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float aspectRatio = 4.0f / 3.0f;

//camera
float eyeX = -20.0f, eyeY = 5.0f, eyeZ = 40.5f;
float lookAtX = 2.0f, lookAtY = 1.5f, lookAtZ = 4.75f;
Camera camera(glm::vec3(eyeX, eyeY, eyeZ));

//timing
float deltaTime = 0.0f;     // time between current frame and last frame
float lastFrame = 0.0f;

//fan
float r = 0.0f;
bool fanOn = false;

//bird's eye view
bool birdEyeView = false;
glm::vec3 birdEyePosition(2.0f, 3.5f, 13.5f);
glm::vec3 birdEyeTarget(2.0f, 0.0f, 7.5f);
float birdEyeSpeed = 5.0f;

//rotation around a point
float theta = 0.0f; // Angle around the Y-axis
float radius = 2.0f;

//directional light
bool directionLightOn = false;
bool directionalAmbient = true;
bool directionalDiffuse = true;
bool directionalSpecular = true;

//spot light
bool spotLightOn = true;

//point light
bool point1 = true;
bool point2 = true;
bool point3 = true;
bool point4 = true;
bool point5 = true;
bool point6 = true;
bool point7 = true;
bool point8 = true;
bool point9 = true;
bool point10 = true;
bool point11 = true;
bool point12 = true;
bool point13 = true;
bool point14 = true;
bool point15 = true;
bool point16 = true;
bool point17 = true;
bool point18 = true;



//custom projection matrix
float fov = glm::radians(camera.Zoom);
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
float near = 0.1f;
float far = 100.0f;
float tanHalfFOV = tan(fov / 2.0f);

//doors and windows
bool openDoor = true;
float doorAngle = 90.0f;

//positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(-2.9f + 1.5f, 2.0f + 6.3f, 5.0f + 4.5f),
    glm::vec3(6.7f + 1.5f, 2.0f + 6.3f, 5.0f + 4.5f),
    glm::vec3(9.0f, 9.7f, 28.0f),
    glm::vec3(3.5f, 9.7f, 28.0f),
    glm::vec3(-8.0f, 9.7f, 8.0f),
    glm::vec3(-8.0f, 9.7f, 22.0f),
    glm::vec3(-28.8f, 20.4f, -20.0f),//star
    glm::vec3(-34.0f, 17.7f, -20.0f),//star
    // Additional 10 positions
    glm::vec3(-28.0f, -2.0f, 5.0f),
    glm::vec3(11.0f, 1.8f, 3.0f), 
    glm::vec3(-35.0f, 9.7f, -3.0f),
    glm::vec3(4.0f, 8.7f, -7.0f),//done
    glm::vec3(3.5f, 1.1f, -4.0f),
    glm::vec3(3.5f, 8.6f, -4.0f),
    glm::vec3(6.0f, 3.7f, 23.0),
    glm::vec3(6.0f, 3.7f, 16.0),
    glm::vec3(-31.8f, 16.4f, -20.0f),
    glm::vec3(-31.0f, 15.7f, -20.0f)
    
};



// Define the stairs zone (example values, adjust to your scene)
//9.0f, -1.5f + j, 18.0f - i
glm::vec3 stairsStart(10.0f, -1.5f, 26.0f); // Bottom of stairs
glm::vec3 stairsEnd(10.0f, 7.0f, 15.0f);   // Top of stairs
float transitionSpeed = 0.5f;             // Speed of the camera during transition
bool transitioning = false;               // Flag for automatic movement
float transitionProgress = 0.0f;          // Progress for interpolation






void updateCamera(float deltaTime, GLFWwindow* window) {
    if (!transitioning) {
        // Normal camera movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if (!birdEyeView)
                camera.ProcessKeyboard(FORWARD, deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            if (!birdEyeView)
                camera.ProcessKeyboard(LEFT, deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if (!birdEyeView)
                camera.ProcessKeyboard(RIGHT, deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            if (!birdEyeView)
                camera.ProcessKeyboard(UP, deltaTime);
        }

        
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            transitioning = true; // Start transitioning
            transitionProgress = 0.0f; // Reset progress
        }
    }
    else {
        // Automatic stairs movement
        transitionProgress += transitionSpeed * deltaTime;
        
        if (transitionProgress >= 1.0f) {
            transitioning = false; // End transition
            transitionProgress = 1.0f; // Clamp progress
            camera.ProcessKeyboard(P_DOWN, deltaTime);
        }

        // Interpolate position and orientation
        camera.Position = glm::mix(stairsStart, stairsEnd, transitionProgress);

       
    }
}



PointLight pointlight1(
    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,       // position
    1.0f, 1.0f, 0.8f,       //ambient
    1.0f, 1.0f, 0.6f,       //diffuse
    1.0f, 1.0f, 0.9f,       //specular
    1.0f,       //k_c
    0.09f,      //k_l
    0.032f,     //k_q
    1       //light number
);

PointLight pointlight2(
    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    2
);

PointLight pointlight3(
    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    3
);
PointLight pointlight4(
    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    4
);
PointLight pointlight5(
    pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    5
);
PointLight pointlight6(
    pointLightPositions[5].x, pointLightPositions[5].y, pointLightPositions[5].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    6
);
PointLight pointlight7(
    pointLightPositions[6].x, pointLightPositions[6].y, pointLightPositions[6].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    7
);
PointLight pointlight8(
    pointLightPositions[7].x, pointLightPositions[7].y, pointLightPositions[7].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    8
);

//pointlight cube color
glm::vec4 cubeColor1(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor2(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor3(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor5(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor6(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor7(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor8(1.0f, 1.0f, 1.0f, 1.0f);


PointLight pointlight9(
    pointLightPositions[8].x, pointLightPositions[8].y, pointLightPositions[8].z,
    0.5f, 0.0f, 0.5f,        // Ambient (Purple color)
    0.6f, 0.0f, 0.6f,        // Diffuse (Purple color)
    0.7f, 0.0f, 0.7f,        // Specular (Purple color)
    1.0f,                    // k_c
    0.09f,                   // k_l
    0.032f,                  // k_q
    9                        // Light number
);

PointLight pointlight10(
    pointLightPositions[9].x, pointLightPositions[9].y, pointLightPositions[9].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    10
);

PointLight pointlight11(
    pointLightPositions[10].x, pointLightPositions[10].y, pointLightPositions[10].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    11
);

PointLight pointlight12(
    pointLightPositions[11].x, pointLightPositions[11].y, pointLightPositions[11].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    12
);

PointLight pointlight13(
    pointLightPositions[12].x, pointLightPositions[12].y, pointLightPositions[12].z,
    0.5f, 0.0f, 0.5f,        // Ambient (Purple color)
    0.6f, 0.0f, 0.6f,        // Diffuse (Purple color)
    0.7f, 0.0f, 0.7f,        // Specular (Purple color)
    1.0f,                    // k_c
    0.09f,                   // k_l
    0.032f,
    13
);

PointLight pointlight14(
    pointLightPositions[13].x, pointLightPositions[13].y, pointLightPositions[13].z,
    0.5f, 0.0f, 0.5f,        // Ambient (Purple color)
    0.6f, 0.0f, 0.6f,        // Diffuse (Purple color)
    0.7f, 0.0f, 0.7f,        // Specular (Purple color)
    1.0f,                    // k_c
    0.09f,                   // k_l
    0.032f,
    14
);

PointLight pointlight15(
    pointLightPositions[14].x, pointLightPositions[14].y, pointLightPositions[14].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    15
);

PointLight pointlight16(
    pointLightPositions[15].x, pointLightPositions[15].y, pointLightPositions[15].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    16
);

PointLight pointlight17(
    pointLightPositions[16].x, pointLightPositions[16].y, pointLightPositions[16].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    17
);

PointLight pointlight18(
    pointLightPositions[17].x, pointLightPositions[17].y, pointLightPositions[17].z,
    1.0f, 1.0f, 0.8f,
    1.0f, 1.0f, 0.6f,
    1.0f, 1.0f, 0.7f,
    1.0f,
    0.09f,
    0.032f,
    18
);

// Colors for additional lights
glm::vec4 cubeColor9(0.5f, 0.0f, 0.5f, 1.0f); // Purple color
glm::vec4 cubeColor10(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor11(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor12(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor13(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor14(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor15(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor16(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor17(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 cubeColor18(1.0f, 1.0f, 1.0f, 1.0f);


int main()
{
    //glfw initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    //glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hasib Hasan Mantion", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //glad load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //configure global opengl state
    glEnable(GL_DEPTH_TEST);

    //build and compile our shader program
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    //Shader constantShader("vertexShader.vs", "fragmentShaderV2.fs");

    //set up vertex data (and buffer(s)) and configure vertex attributes


    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };

    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    //second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    //note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //ourShader.use();
    //constantShader.use();
    /**/
    //floor texture
    string diffuseMapPaths = "woodfloor.jpg";
    string specularMapPaths = "woodfloor.jpg";

    // Load textures
    unsigned int diffwMaps = loadTexture(diffuseMapPaths.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specwMaps = loadTexture(specularMapPaths.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube woodf = Cube(diffwMaps, specwMaps, 32.0f, 0.2f, 0.0f, 1.25f, 2.25f);
    // Create a sphere object
    float radius = 1.0f;
    int sectorCount = 36;
    int stackCount = 18;
    glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
    float shininess = 32.0f;

    GLfloat controlPoints[] = {
          0.2f, 0.0f, 0.0f,  // Bottom center
    0.2f, 0.0f, 0.0f,  // Narrow base
    0.4f, 0.5f, 0.0f,  // Widening towards the middle
    0.3f, 1.0f, 0.0f,  // Narrowing towards the neck
    0.0f, 1.5f, 0.0f,   // Narrow top
    0.3f, 1.8f, 0.0f   // Narrow top

    };


    // Create BezierCurve object
    BezierCurve vaseCurve(controlPoints, sizeof(controlPoints) / sizeof(controlPoints[0]), 0);


    string diffuseMapPath = "floor.jpg";
    string specularMapPath = "floor.jpg";
    unsigned int diffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube = Cube(diffMap, specMap, 32.0f, 0.2f, 0.0f, 1.25f, 2.25f);

    //roof texture
    string diffuseMapPath2 = "roof.jpg";
    string specularMapPath2 = "roof.jpg";
    unsigned int diffMap2 = loadTexture(diffuseMapPath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap2 = loadTexture(specularMapPath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube2 = Cube(diffMap2, specMap2, 32.0f, 0.0f, 0.0f, 2.5f, 4.5f);

    //wall texture
    string diffuseMapPath3 = "classroom_roof.jpg";
    string specularMapPath3 = "classroom_roof.jpg";
    unsigned int diffMap3 = loadTexture(diffuseMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap3 = loadTexture(specularMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube3 = Cube(diffMap3, specMap3, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //back wall texture
    string diffuseMapback = "classroom_roof.jpg";
    string specularMapback = "classroom_roof.jpg";
    unsigned int diffMapback = loadTexture(diffuseMapback.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMapback = loadTexture(specularMapback.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cubeback = Cube(diffMapback, specMapback, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //wood texture
    string diffuseMapPath4 = "purple.png";
    string specularMapPath4 = "purple.png";
    unsigned int diffMap4 = loadTexture(diffuseMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap4 = loadTexture(specularMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube pp = Cube(diffMap4, specMap4, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //wood texture 2
    string diffuseMapPath5 = "ttable.jpg";
    string specularMapPath5 = "ttable.jpg";
    unsigned int diffMap5 = loadTexture(diffuseMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap5 = loadTexture(specularMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube5 = Cube(diffMap5, specMap5, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapwin = "art.jpg";
    string specularMapwin = "art.jpg";
    unsigned int diffwin = loadTexture(diffuseMapwin.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specwin = loadTexture(specularMapwin.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube win = Cube(diffwin, specwin, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapwin2 = "art2.jpg";
    string specularMapwin2 = "art2.jpg";
    unsigned int diffwin2 = loadTexture(diffuseMapwin2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specwin2 = loadTexture(specularMapwin2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube win2 = Cube(diffwin2, specwin2, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapSofa = "brsofa.png";
    string specularMapSofa = "brsofa.png";
    unsigned int diffSofa = loadTexture(diffuseMapSofa.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specSofa = loadTexture(specularMapSofa.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube sofa = Cube(diffSofa, specSofa, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);


    string diffuseMapt1 = "table_tex.jpeg";
    string specularMapt1 = "table_tex.jpeg";
    unsigned int difft1 = loadTexture(diffuseMapt1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int spect2 = loadTexture(specularMapt1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube t1 = Cube(difft1, spect2, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffusegrass = "green_grass_texture.jpg";
    string speculargrass = "green_grass_texture.jpg";
    unsigned int diffg1 = loadTexture(diffusegrass.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specg2 = loadTexture(speculargrass.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube gg = Cube(diffg1, specg2, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffusebed = "bedsheet.jpg";
    string specularbed = "bedsheet.jpg";
    unsigned int diffbed = loadTexture(diffusebed.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specbed = loadTexture(specularbed.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube bed = Cube(diffbed, specbed, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffusetv = "tv.jpg";
    string speculartv = "tv.jpg";
    unsigned int difftv = loadTexture(diffusetv.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int spectv = loadTexture(speculartv.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube tv = Cube(difftv, spectv, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
  

    string diffusewa = "water.jpg";
    string specularwa = "water.jpg";
    unsigned int diffwa = loadTexture(diffusewa.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specwa = loadTexture(specularwa.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube water = Cube(diffwa, specwa, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    // Define the paths for diffuse and specular textures for glass
    string diffuseGlass = "road.jpg"; // Glass texture with transparency
    string specularGlass = "road.jpg"; // Optional specular texture (can be the same or different)

    // Load textures
    unsigned int diffGlass = loadTexture(diffuseGlass.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specGlass = loadTexture(specularGlass.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Create the Cube object for glass
    Cube glassCube = Cube(diffGlass, specGlass, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    CylinderTex cyltex = CylinderTex();
    Parabola parabola = Parabola();
    
    string vasepath = "glass.jpg";
    unsigned int glass = loadTexture(vasepath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    string diffuseMapKey = "key_cabinet.jpg";
    string specularMapKey = "key_cabinet.jpg";
    unsigned int diffkey = loadTexture(diffuseMapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int speckey = loadTexture(specularMapKey.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube keycab = Cube(diffkey, speckey, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //books
    string diffuseMapbook = "books.png";
    string specularMapbook = "books.png";
    unsigned int diffbook = loadTexture(diffuseMapbook.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specbook = loadTexture(specularMapbook.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube bookcab = Cube(diffbook, specbook, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapboun = "curtain.jpg";
    string specularMapboun = "curtain.jpg";
    unsigned int diffboun = loadTexture(diffuseMapboun.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specboun = loadTexture(specularMapboun.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube boun = Cube(diffboun, specboun, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapcrok = "kitchencrok.png";
    string specularMapcrok = "kitchencrok.png";
    unsigned int diffcrok = loadTexture(diffuseMapcrok.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int speccrok = loadTexture(specularMapcrok.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube crok = Cube(diffcrok, speccrok, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    
    string diffuseMapsc = "stovecab.jpg";
    string specularMapsc = "stovecab.jpg";
    unsigned int diffsc = loadTexture(diffuseMapsc.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specsc = loadTexture(specularMapsc.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube sc = Cube(diffsc, specsc, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapstove = "stove12.png";
    string specularMapstove = "stove12.png";
    unsigned int diffstove = loadTexture(diffuseMapstove.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specstove = loadTexture(specularMapstove.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube stove = Cube(diffstove, specstove, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapst = "stove.jpg";
    string specularMapst = "stove.jpg";
    unsigned int diffst = loadTexture(diffuseMapst.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specst = loadTexture(specularMapst.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube st = Cube(diffst, specst, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string diffuseMapsky = "sky.jpg";
    string specularMapsky = "sky.jpg";
    unsigned int diffsky = loadTexture(diffuseMapsky.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specsky = loadTexture(specularMapsky.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube sky = Cube(diffsky, specsky, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    
    string trunk = "road.jpg";
    unsigned int trunkload = loadTexture(trunk.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    r = 0.0f;
  // Starts with textures enabled

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        //per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        updateCamera(deltaTime, window);


        //input
        processInput(window);

        //render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //ourShader.use();
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        //point lights set up
        pointlight1.setUpPointLight(lightingShader);
        pointlight2.setUpPointLight(lightingShader);
        pointlight3.setUpPointLight(lightingShader);
        pointlight4.setUpPointLight(lightingShader);
        pointlight5.setUpPointLight(lightingShader);
        pointlight6.setUpPointLight(lightingShader);
        pointlight7.setUpPointLight(lightingShader);
        pointlight8.setUpPointLight(lightingShader);
        // Point lights set up
        pointlight9.setUpPointLight(lightingShader);
        pointlight10.setUpPointLight(lightingShader);
        pointlight11.setUpPointLight(lightingShader);
        pointlight12.setUpPointLight(lightingShader);
        pointlight13.setUpPointLight(lightingShader);
        pointlight14.setUpPointLight(lightingShader);
        pointlight15.setUpPointLight(lightingShader);
        pointlight16.setUpPointLight(lightingShader);
        pointlight17.setUpPointLight(lightingShader);
        pointlight18.setUpPointLight(lightingShader);


        // Directional light setup
  
        lightingShader.setVec3("directionalLight.direction", -0.5f, -1.0f, -0.3f); // Sunlight angle (right to left)
        lightingShader.setVec3("directionalLight.ambient", 0.1f, 0.1f, 0.2f);      // Dim, cool ambient light (slightly blue)
        lightingShader.setVec3("directionalLight.diffuse", 0.3f, 0.3f, 0.5f);      // Dim, cool diffuse light (moonlight effect)
        lightingShader.setVec3("directionalLight.specular", 0.2f, 0.2f, 0.4f);     // Dim, cool specular highlights
        lightingShader.setBool("directionLightOn", directionLightOn);





        //spot light set up
        lightingShader.setVec3("spotLight.position", 2.0f, 2.0f, 2.0f);
        lightingShader.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("spotLight.specular", 0.2f, 0.2f, 0.2f);
        lightingShader.setFloat("spotLight.k_c", 1.0f);
        lightingShader.setFloat("spotLight.k_l", 0.09);
        lightingShader.setFloat("spotLight.k_q", 0.032);
        //lightingShader.setFloat("spotLight.cos_theta", glm::cos(glm::radians(60.0f)));
        lightingShader.setFloat("spotLight.inner_circle", glm::cos(glm::radians(21.5f)));
        lightingShader.setFloat("spotLight.outer_circle", glm::cos(glm::radians(45.0f)));
        lightingShader.setBool("spotLightOn", spotLightOn);

        //handle for changes in directional light directly from shedder
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShaderWithTexture.setBool("ambientLight", !directionalAmbient);
                directionalAmbient = !directionalAmbient;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShaderWithTexture.setBool("diffuseLight", !directionalDiffuse);
                directionalDiffuse = !directionalDiffuse;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShaderWithTexture.setBool("specularLight", !directionalSpecular);
                directionalSpecular = !directionalSpecular;
            }
        }

        glm::mat4 projection(0.0f);
        projection[0][0] = 1.0f / (aspect * tanHalfFOV);
        projection[1][1] = 1.0f / tanHalfFOV;
        projection[2][2] = -(far + near) / (far - near);
        projection[2][3] = -1.0f;
        projection[3][2] = -(2.0f * far * near) / (far - near);
        //pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        //ourShader.setMat4("projection", projection);
        //constantShader.setMat4("projection", projection);
        lightingShader.setMat4("projection", projection);

        //camera view transformation
        //constantShader.setMat4("view", view);
        //ourShader.setMat4("view", view);

        glm::mat4 view;

        //check for bird's eye view and normal view
        if (birdEyeView) {
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(birdEyePosition, birdEyeTarget, up);
        }
        else {
            view = camera.GetViewMatrix();
        }

        lightingShader.setMat4("view", view);

        //define matrices and vectors needed
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
        glm::vec3 color;

        //initialize all elements as non-emissive
        lightingShader.setVec3("material.emissive", glm::vec3(0.0f, 0.0f, 0.0f));


        float z = 0.0f;

        //draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);

        for (unsigned int i = 0; i < 18; i++) {
            // Set position and scale
            translateMatrix = glm::translate(identityMatrix, pointLightPositions[i]);
            if (i < 2 || i>=10) {
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.0f, 0.2f));
            }
            else if (i == 6) {
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
            }
            else if (i == 7) {
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
            }
           

            else if (i == 8) {
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(7.0f, 0.6f, 5.0f));
            }
            else if (i == 9) {
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(11.0f, 2.5f, 3.0f));
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                ourShader.setMat4("model", model);
                ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
                parabola.drawParabola(lightingShader, model);
            }
            else {
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 0.1f, 0.8f));
            }
            model = translateMatrix * scaleMatrix;

            ourShader.setMat4("model", model);
            glm::vec4 lightColor;

            // Assign colors based on point light index
            if (i == 0) lightColor = cubeColor1;
            else if (i == 1) lightColor = cubeColor2;
            else if (i == 2) lightColor = cubeColor3;
            else if (i == 3) lightColor = cubeColor4;
            else if (i == 4) lightColor = cubeColor5;
            else if (i == 5) lightColor = cubeColor6;
            else if (i == 6) lightColor = cubeColor7;
            else if (i == 7) lightColor = cubeColor8;
            else if (i == 8) lightColor = cubeColor9;
            else if (i == 9) lightColor = cubeColor10;
            else if (i == 10) lightColor = cubeColor11;
            else if (i == 11) lightColor = cubeColor12;
            else if (i == 12) lightColor = cubeColor13;
            else if (i == 13) lightColor = cubeColor14;
            else if (i == 14) lightColor = cubeColor15;
            else if (i == 15) lightColor = cubeColor16;
            else if (i == 16) lightColor = cubeColor17;
            else if (i == 17) lightColor = cubeColor18;

            ourShader.setVec4("color", lightColor);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }



        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setMat4("projection", projection);
        lightingShaderWithTexture.setMat4("view", view);

        //point lights set up
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        pointlight2.setUpPointLight(lightingShaderWithTexture);
        pointlight3.setUpPointLight(lightingShaderWithTexture);
        pointlight4.setUpPointLight(lightingShaderWithTexture);
        pointlight5.setUpPointLight(lightingShaderWithTexture);
        pointlight6.setUpPointLight(lightingShaderWithTexture);
        pointlight7.setUpPointLight(lightingShaderWithTexture);
        pointlight8.setUpPointLight(lightingShaderWithTexture);
        // Point lights set up with texture shader
        pointlight9.setUpPointLight(lightingShaderWithTexture);
        pointlight10.setUpPointLight(lightingShaderWithTexture);
        pointlight11.setUpPointLight(lightingShaderWithTexture);
        pointlight12.setUpPointLight(lightingShaderWithTexture);
        pointlight13.setUpPointLight(lightingShaderWithTexture);
        pointlight14.setUpPointLight(lightingShaderWithTexture);
        pointlight15.setUpPointLight(lightingShaderWithTexture);
        pointlight16.setUpPointLight(lightingShaderWithTexture);
        pointlight17.setUpPointLight(lightingShaderWithTexture);
        pointlight18.setUpPointLight(lightingShaderWithTexture);


        lightingShaderWithTexture.setVec3("directionalLight.direction", 0.0f, -1.0f, 0.0f);  // Direction of the light
        lightingShaderWithTexture.setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.1f);     // Soft, dim yellowish ambient light
        lightingShaderWithTexture.setVec3("directionalLight.diffuse", 0.4f, 0.4f, 0.2f);     // Soft, dim yellowish diffuse light
        lightingShaderWithTexture.setVec3("directionalLight.specular", 0.1f, 0.1f, 0.05f);  // Soft, dim yellowish specular highlights
        lightingShaderWithTexture.setBool("directionLightOn", directionLightOn);




        //spot light set up
        lightingShaderWithTexture.setVec3("spotLight.position", 3.0f, 2.0f, -4.0f);
        lightingShaderWithTexture.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f); // Direct light downward
        lightingShaderWithTexture.setVec3("spotLight.ambient", 0.3f, 0.2f, 0.1f);   // Warm ambient light
        lightingShaderWithTexture.setVec3("spotLight.diffuse", 0.8f, 0.7f, 0.5f);   // Soft but focused light
        lightingShaderWithTexture.setVec3("spotLight.specular", 0.4f, 0.3f, 0.2f);  // Subtle highlights
        lightingShaderWithTexture.setFloat("spotLight.k_c", 1.0f);
        lightingShaderWithTexture.setFloat("spotLight.k_l", 0.09);
        lightingShaderWithTexture.setFloat("spotLight.k_q", 0.032);
        //lightingShaderWithTexture.setFloat("spotLight.cos_theta", glm::cos(glm::radians(30.0f)));
        lightingShaderWithTexture.setFloat("spotLight.inner_circle", glm::cos(glm::radians(30.0f)));
        lightingShaderWithTexture.setFloat("spotLight.outer_circle", glm::cos(glm::radians(60.0f)));
        lightingShaderWithTexture.setBool("spotLightOn", spotLightOn);

        // Draw the fractal tree

        
        
       
        for (float i = -50.0f;i < 40.0f;i=i+10.0f) {
            for (float j = -30.0f;j < 70.0f;j = j + 10.0f) {
                 translateMatrix = glm::translate(identityMatrix, glm::vec3(i, -1.5, j));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, -0.1f, 10.0f));
        model = translateMatrix * scaleMatrix;
        gg.drawCubeWithTexture(lightingShaderWithTexture, model);
            }
        }

        drawScene(lightingShader, vaseCurve);
        
        for (float i = -40.5f;i <= 20.0f;i += 5.0f) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(i, -1.5f, -20.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 1.5f, 0.3f));
            model = translateMatrix * scaleMatrix;
            cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(-40.5f, -1.5f, -i+5.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 1.5f, -5.0f));
            model = translateMatrix * scaleMatrix;
            cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(24.3f, -1.5f, -i+5.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 1.5f, -5.0f));
            model = translateMatrix * scaleMatrix;
            cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

        }
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-35.0f, -2.0f, -10.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 10.0f, 0.5f));
        model = translateMatrix * scaleMatrix;
        cube5.drawCubeWithTexture(lightingShaderWithTexture, model);
        for (float i = 0.0f;i < 180.0f;i += 20.0f) {
            glm::mat4 treeBaseModel = glm::translate(identityMatrix, glm::vec3(-35.0f, -2.0f, -10.0f));
            treeBaseModel = glm::rotate(treeBaseModel, glm::radians(i), glm::vec3(0.0f, 0.5f, 0.0f));
            drawFractalTree(treeBaseModel, lightingShaderWithTexture, 7.0, 8.0f, 30.0f, pp,currentFrame);
            
        }


        readingRoom(lightingShaderWithTexture, woodf, gg, cube2, cubeback, cube3, win, sofa, cube5,keycab,win2);
        bedroom(lightingShaderWithTexture, woodf, bed, cube2, cubeback, cube3, win, tv, cube5, keycab, win2);

        glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(2.8f, 0.3f, 8.0f)); // Translation
        modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotation
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.4f, 0.3f, 0.3f)); // Scaling

        cyltex.drawCylinder(lightingShaderWithTexture, glass, modelMatrixForContainer);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.5f, 5.1f, -4.0f));
        model = glm::scale(model, glm::vec3(0.17f)); // Make it a smaller cube
        ourShader.setMat4("model", model);
        ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
        parabola.drawParabola(lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.4f, 5.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.2f, 0.1f));
        model = translateMatrix * scaleMatrix;
        glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);
        building(lightingShaderWithTexture, cube, glassCube, cube2, cubeback, cube3, win, sofa, t1,water,woodf,currentFrame,bookcab,boun,stove, keycab,crok,st);

       


      

       
        //glfw swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    //glfw terminate, clearing all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

void drawTableChair(unsigned int VAO, Shader lightingShaderWithTexture, glm::mat4 matrix, Cube c) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::vec3 color;

    //table top
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.1f, 1.0f));
    model = matrix * scaleMatrix;
    color = glm::vec3(0.882f, 0.710f, 0.604f);
    //drawCube(VAO, lightingShader, model, color);
    c.drawCubeWithTexture(lightingShaderWithTexture, model);



    //table right leg back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    //color = glm::vec3(0.647f, 0.408f, 0.294f);
    //drawCube(VAO, lightingShader, model, color);
    c.drawCubeWithTexture(lightingShaderWithTexture, model);




}

void drawCube(unsigned int& VAO, Shader& lightingShader, glm::mat4 model, glm::vec3 color)
{
    //use the shadder
    lightingShader.use();

    //define lighting properties
    lightingShader.setVec3("material.ambient", color);
    lightingShader.setVec3("material.diffuse", color);
    lightingShader.setVec3("material.specular", color);
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

//process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
   



    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        fanOn = !fanOn;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        birdEyeView = !birdEyeView;
    }

    if (birdEyeView) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            birdEyePosition.z -= birdEyeSpeed * deltaTime; // Move forward along Z
            birdEyeTarget.z -= birdEyeSpeed * deltaTime;
            if (birdEyePosition.z <= 2.0) {
                birdEyePosition.z = 2.0;
            }
            if (birdEyeTarget.z <= -4.0) {
                birdEyeTarget.z = -4.0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            birdEyePosition.z += birdEyeSpeed * deltaTime; // Move backward along Z
            birdEyeTarget.z += birdEyeSpeed * deltaTime;
            if (birdEyePosition.z >= 13.5) {
                birdEyePosition.z = 13.5;
            }
            if (birdEyeTarget.z >= 7.5) {
                birdEyeTarget.z = 7.5;
            }
        }
    }

    /*
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (!birdEyeView) {
            theta += 0.01f;
            camera.Position.x = lookAtX + radius * sin(theta);
            camera.Position.y = lookAtY;
            camera.Position.z = lookAtZ + radius * cos(theta);
        }
    }
    */

 

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (pointlight1.ambientOn > 0 && pointlight1.diffuseOn > 0 && pointlight1.specularOn > 0) {
            pointlight1.turnOff();
            point1 = false;
            cubeColor1 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight1.turnOn();
            point1 = true;
            cubeColor1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        if (pointlight2.ambientOn > 0 && pointlight2.diffuseOn > 0 && pointlight2.specularOn > 0) {
            pointlight2.turnOff();
            point2 = false;
            cubeColor2 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight2.turnOn();
            point2 = true;
            cubeColor2 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        if (pointlight3.ambientOn > 0 && pointlight3.diffuseOn > 0 && pointlight3.specularOn > 0) {
            pointlight3.turnOff();
            point3 = false;
            cubeColor3 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight3.turnOn();
            point3 = true;
            cubeColor3 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight4.ambientOn > 0 && pointlight4.diffuseOn > 0 && pointlight4.specularOn > 0) {
            pointlight4.turnOff();
            point4 = false;
            cubeColor4 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight4.turnOn();
            point4 = true;
            cubeColor4 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight5.ambientOn > 0 && pointlight5.diffuseOn > 0 && pointlight5.specularOn > 0) {
            pointlight5.turnOff();
            point5 = false;
            cubeColor5 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight5.turnOn();
            point5 = true;
            cubeColor5 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight6.ambientOn > 0 && pointlight6.diffuseOn > 0 && pointlight6.specularOn > 0) {
            pointlight6.turnOff();
            point6 = false;
            cubeColor6 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight6.turnOn();
            point6 = true;
            cubeColor6 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight7.ambientOn > 0 && pointlight7.diffuseOn > 0 && pointlight7.specularOn > 0) {
            pointlight7.turnOff();
            point7 = false;
            cubeColor7 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight7.turnOn();
            point7 = true;
            cubeColor7 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight8.ambientOn > 0 && pointlight8.diffuseOn > 0 && pointlight8.specularOn > 0) {
            pointlight8.turnOff();
            point8 = false;
            cubeColor8 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight8.turnOn();
            point8 = true;
            cubeColor8 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        if (pointlight9.ambientOn > 0 && pointlight9.diffuseOn > 0 && pointlight9.specularOn > 0) {
            pointlight9.turnOff();
            point9 = false;
            cubeColor9 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight9.turnOn();
            point9 = true;
            cubeColor9 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight10.ambientOn > 0 && pointlight10.diffuseOn > 0 && pointlight10.specularOn > 0) {
            pointlight10.turnOff();
            point10 = false;
            cubeColor10 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight10.turnOn();
            point10 = true;
            cubeColor10 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight11.ambientOn > 0 && pointlight11.diffuseOn > 0 && pointlight11.specularOn > 0) {
            pointlight11.turnOff();
            point11 = false;
            cubeColor11 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight11.turnOn();
            point11 = true;
            cubeColor11 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight12.ambientOn > 0 && pointlight12.diffuseOn > 0 && pointlight12.specularOn > 0) {
            pointlight12.turnOff();
            point12 = false;
            cubeColor12 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight12.turnOn();
            point12 = true;
            cubeColor12 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight13.ambientOn > 0 && pointlight13.diffuseOn > 0 && pointlight13.specularOn > 0) {
            pointlight13.turnOff();
            point13 = false;
            cubeColor13 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight13.turnOn();
            point13 = true;
            cubeColor13 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight14.ambientOn > 0 && pointlight14.diffuseOn > 0 && pointlight14.specularOn > 0) {
            pointlight14.turnOff();
            point14 = false;
            cubeColor14 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight14.turnOn();
            point14 = true;
            cubeColor14 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight15.ambientOn > 0 && pointlight15.diffuseOn > 0 && pointlight15.specularOn > 0) {
            pointlight15.turnOff();
            point15 = false;
            cubeColor15 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight15.turnOn();
            point15 = true;
            cubeColor15 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight16.ambientOn > 0 && pointlight16.diffuseOn > 0 && pointlight16.specularOn > 0) {
            pointlight16.turnOff();
            point16 = false;
            cubeColor16 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight16.turnOn();
            point16 = true;
            cubeColor16 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight17.ambientOn > 0 && pointlight17.diffuseOn > 0 && pointlight17.specularOn > 0) {
            pointlight17.turnOff();
            point17 = false;
            cubeColor17 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight17.turnOn();
            point17 = true;
            cubeColor17 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (pointlight18.ambientOn > 0 && pointlight18.diffuseOn > 0 && pointlight18.specularOn > 0) {
            pointlight18.turnOff();
            point18 = false;
            cubeColor18 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }
        else {
            pointlight18.turnOn();
            point18 = true;
            cubeColor18 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

    }
    


    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        directionLightOn = !directionLightOn;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        spotLightOn = !spotLightOn;
    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        // Toggle ambient light for point lights 1 to 18
        if (pointlight1.ambientOn > 0 || pointlight2.ambientOn > 0 || pointlight3.ambientOn > 0 || pointlight4.ambientOn > 0 || pointlight5.ambientOn > 0 || pointlight6.ambientOn > 0 || pointlight7.ambientOn > 0 || pointlight8.ambientOn > 0 ||
            pointlight9.ambientOn > 0 || pointlight10.ambientOn > 0 || pointlight11.ambientOn > 0 || pointlight12.ambientOn > 0 || pointlight13.ambientOn > 0 || pointlight14.ambientOn > 0 || pointlight15.ambientOn > 0 || pointlight16.ambientOn > 0 ||
            pointlight17.ambientOn > 0 || pointlight18.ambientOn > 0) {

            if (point1)
                pointlight1.turnAmbientOff();
            if (point2)
                pointlight2.turnAmbientOff();
            if (point3)
                pointlight3.turnAmbientOff();
            if (point4)
                pointlight4.turnAmbientOff();
            if (point5)
                pointlight5.turnAmbientOff();
            if (point6)
                pointlight6.turnAmbientOff();
            if (point7)
                pointlight7.turnAmbientOff();
            if (point8)
                pointlight8.turnAmbientOff();
            if (point9)
                pointlight9.turnAmbientOff();
            if (point10)
                pointlight10.turnAmbientOff();
            if (point11)
                pointlight11.turnAmbientOff();
            if (point12)
                pointlight12.turnAmbientOff();
            if (point13)
                pointlight13.turnAmbientOff();
            if (point14)
                pointlight14.turnAmbientOff();
            if (point15)
                pointlight15.turnAmbientOff();
            if (point16)
                pointlight16.turnAmbientOff();
            if (point17)
                pointlight17.turnAmbientOff();
            if (point18)
                pointlight18.turnAmbientOff();
        }
        else {
            if (point1)
                pointlight1.turnAmbientOn();
            if (point2)
                pointlight2.turnAmbientOn();
            if (point3)
                pointlight3.turnAmbientOn();
            if (point4)
                pointlight4.turnAmbientOn();
            if (point5)
                pointlight5.turnAmbientOn();
            if (point6)
                pointlight6.turnAmbientOn();
            if (point7)
                pointlight7.turnAmbientOn();
            if (point8)
                pointlight8.turnAmbientOn();
            if (point9)
                pointlight9.turnAmbientOn();
            if (point10)
                pointlight10.turnAmbientOn();
            if (point11)
                pointlight11.turnAmbientOn();
            if (point12)
                pointlight12.turnAmbientOn();
            if (point13)
                pointlight13.turnAmbientOn();
            if (point14)
                pointlight14.turnAmbientOn();
            if (point15)
                pointlight15.turnAmbientOn();
            if (point16)
                pointlight16.turnAmbientOn();
            if (point17)
                pointlight17.turnAmbientOn();
            if (point18)
                pointlight18.turnAmbientOn();
        }
    }


    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        // Toggle diffuse light for point lights 1 to 18
        if (pointlight1.diffuseOn > 0 || pointlight2.diffuseOn > 0 || pointlight3.diffuseOn > 0 || pointlight4.diffuseOn > 0 || pointlight5.diffuseOn > 0 || pointlight6.diffuseOn > 0 || pointlight7.diffuseOn > 0 || pointlight8.diffuseOn > 0 ||
            pointlight9.diffuseOn > 0 || pointlight10.diffuseOn > 0 || pointlight11.diffuseOn > 0 || pointlight12.diffuseOn > 0 || pointlight13.diffuseOn > 0 || pointlight14.diffuseOn > 0 || pointlight15.diffuseOn > 0 || pointlight16.diffuseOn > 0 ||
            pointlight17.diffuseOn > 0 || pointlight18.diffuseOn > 0) {

            if (point1)
                pointlight1.turnDiffuseOff();
            if (point2)
                pointlight2.turnDiffuseOff();
            if (point3)
                pointlight3.turnDiffuseOff();
            if (point4)
                pointlight4.turnDiffuseOff();
            if (point5)
                pointlight5.turnDiffuseOff();
            if (point6)
                pointlight6.turnDiffuseOff();
            if (point7)
                pointlight7.turnDiffuseOff();
            if (point8)
                pointlight8.turnDiffuseOff();
            if (point9)
                pointlight9.turnDiffuseOff();
            if (point10)
                pointlight10.turnDiffuseOff();
            if (point11)
                pointlight11.turnDiffuseOff();
            if (point12)
                pointlight12.turnDiffuseOff();
            if (point13)
                pointlight13.turnDiffuseOff();
            if (point14)
                pointlight14.turnDiffuseOff();
            if (point15)
                pointlight15.turnDiffuseOff();
            if (point16)
                pointlight16.turnDiffuseOff();
            if (point17)
                pointlight17.turnDiffuseOff();
            if (point18)
                pointlight18.turnDiffuseOff();
        }
        else {
            if (point1)
                pointlight1.turnDiffuseOn();
            if (point2)
                pointlight2.turnDiffuseOn();
            if (point3)
                pointlight3.turnDiffuseOn();
            if (point4)
                pointlight4.turnDiffuseOn();
            if (point5)
                pointlight5.turnDiffuseOn();
            if (point6)
                pointlight6.turnDiffuseOn();
            if (point7)
                pointlight7.turnDiffuseOn();
            if (point8)
                pointlight8.turnDiffuseOn();
            if (point9)
                pointlight9.turnDiffuseOn();
            if (point10)
                pointlight10.turnDiffuseOn();
            if (point11)
                pointlight11.turnDiffuseOn();
            if (point12)
                pointlight12.turnDiffuseOn();
            if (point13)
                pointlight13.turnDiffuseOn();
            if (point14)
                pointlight14.turnDiffuseOn();
            if (point15)
                pointlight15.turnDiffuseOn();
            if (point16)
                pointlight16.turnDiffuseOn();
            if (point17)
                pointlight17.turnDiffuseOn();
            if (point18)
                pointlight18.turnDiffuseOn();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        // Toggle specular light for point lights 1 to 18
        if (pointlight1.specularOn > 0 || pointlight2.specularOn > 0 || pointlight3.specularOn > 0 || pointlight4.specularOn > 0 || pointlight5.specularOn > 0 || pointlight6.specularOn > 0 || pointlight7.specularOn > 0 || pointlight8.specularOn > 0 ||
            pointlight9.specularOn > 0 || pointlight10.specularOn > 0 || pointlight11.specularOn > 0 || pointlight12.specularOn > 0 || pointlight13.specularOn > 0 || pointlight14.specularOn > 0 || pointlight15.specularOn > 0 || pointlight16.specularOn > 0 ||
            pointlight17.specularOn > 0 || pointlight18.specularOn > 0) {

            if (point1)
                pointlight1.turnSpecularOff();
            if (point2)
                pointlight2.turnSpecularOff();
            if (point3)
                pointlight3.turnSpecularOff();
            if (point4)
                pointlight4.turnSpecularOff();
            if (point5)
                pointlight5.turnSpecularOff();
            if (point6)
                pointlight6.turnSpecularOff();
            if (point7)
                pointlight7.turnSpecularOff();
            if (point8)
                pointlight8.turnSpecularOff();
            if (point9)
                pointlight9.turnSpecularOff();
            if (point10)
                pointlight10.turnSpecularOff();
            if (point11)
                pointlight11.turnSpecularOff();
            if (point12)
                pointlight12.turnSpecularOff();
            if (point13)
                pointlight13.turnSpecularOff();
            if (point14)
                pointlight14.turnSpecularOff();
            if (point15)
                pointlight15.turnSpecularOff();
            if (point16)
                pointlight16.turnSpecularOff();
            if (point17)
                pointlight17.turnSpecularOff();
            if (point18)
                pointlight18.turnSpecularOff();
        }
        else {
            if (point1)
                pointlight1.turnSpecularOn();
            if (point2)
                pointlight2.turnSpecularOn();
            if (point3)
                pointlight3.turnSpecularOn();
            if (point4)
                pointlight4.turnSpecularOn();
            if (point5)
                pointlight5.turnSpecularOn();
            if (point6)
                pointlight6.turnSpecularOn();
            if (point7)
                pointlight7.turnSpecularOn();
            if (point8)
                pointlight8.turnSpecularOn();
            if (point9)
                pointlight9.turnSpecularOn();
            if (point10)
                pointlight10.turnSpecularOn();
            if (point11)
                pointlight11.turnSpecularOn();
            if (point12)
                pointlight12.turnSpecularOn();
            if (point13)
                pointlight13.turnSpecularOn();
            if (point14)
                pointlight14.turnSpecularOn();
            if (point15)
                pointlight15.turnSpecularOn();
            if (point16)
                pointlight16.turnSpecularOn();
            if (point17)
                pointlight17.turnSpecularOn();
            if (point18)
                pointlight18.turnSpecularOn();
        }
    }


}

//glfw whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //make sure the viewport matches the new window dimensions; note that width and height will be significantly larger than specified on retina displays.
    float aspectRatio = 4.0f / 3.0f;
    int viewWidth, viewHeight;

    if (width / (float)height > aspectRatio) {
        //Window is too wide, fit height and adjust width
        viewHeight = height;
        viewWidth = (int)(height * aspectRatio);
    }
    else {
        //Window is too tall, fit width and adjust height
        viewWidth = width;
        viewHeight = (int)(width / aspectRatio);
    }

    //Center the viewport
    int xOffset = (width - viewWidth) / 2;
    int yOffset = (height - viewHeight) / 2;

    glViewport(xOffset, yOffset, viewWidth, viewHeight);
    //glViewport(0, 0, width, height);
}

//glfw whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;       //reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//glfw whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void building(Shader& lightingShaderWithTexture,
    Cube& cube,
    Cube& glassCube,
    Cube& cube2,
    Cube& cubeback,
    Cube& cube3,
    Cube& win,
    Cube& sofa,
    Cube& t1, Cube& water, Cube& woodf,float time, Cube& bookcab, Cube& boun, Cube& stove ,  Cube& sc, Cube& crok, Cube& st) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model;
    
    //roof
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-12.0f, 10.0f, -10.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(26.0f, 2.5f,41.0f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);
    //rightwall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(13.0f, 10.0f, -10.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, -13.0f, 41.0f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);
    //floor
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-12.0f, -1.5f, -10.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(26.0f, 0.2f, 40.0f));
    model = translateMatrix * scaleMatrix;
    cube.drawCubeWithTexture(lightingShaderWithTexture, model);
    //leftwall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f, 10.0f, -10.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, -13.0f, 35.0f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);
    //carpet
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f, -1.3f, -10.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(16.0f, 0.3f, 35.0f));
    model = translateMatrix * scaleMatrix;
    woodf.drawCubeWithTexture(lightingShaderWithTexture, model);

    
    // reading room window
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 5.0f, 12.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.5f, 0.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 7.5f, 12.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.5f, 0.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 9.8f, 12.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.5f, 0.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 5.0f, 12.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 8.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, 5.0f, 12.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 8.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    

    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.5f, 5.0f, 12.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 8.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.3f, 5.0f, 12.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 8.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    //curtain down

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f , -1.5f , 2.0f ));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 6.5f, 0.5f));
    model = translateMatrix * scaleMatrix;
    boun.drawCubeWithTexture(lightingShaderWithTexture, model);

    //curtain top

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 5.0f, 12.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 6.5f, 0.5f));
    model = translateMatrix * scaleMatrix;
    boun.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.3f, 5.0f, 12.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 6.5f, 0.5f));
    model = translateMatrix * scaleMatrix;
    boun.drawCubeWithTexture(lightingShaderWithTexture, model);


    //front
    

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 5.0f, 23.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, 0.8f, 1.0f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.5f, -1.0f, 24.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 6.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(12.5f, 10.5f, 24.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, -12.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    

    //baranda
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 5.0f, 24.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, 0.3f, 4.0f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

    //stairs
    for (float i = 2.0f, j = 0.0f;i < 14.0f;i += 2.0f, j += 1.0f) {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.0f, -1.5f+j, 24.0f-i));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 0.3f, 1.2f));
        model = translateMatrix * scaleMatrix;
        glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);
    }
    float i = 1.0f, j = 0.0f, k =0.0f;

 // sofa2



    // sofa2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 3.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.4f, 0.8f, 5.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 3.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 2.0f, 5.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 2.8f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.4f, 1.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 8.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.4f, 0.8f, 5.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 8.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 2.0f, 5.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 13.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.4f, 1.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f + i, -1.0f + j, 3.5f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.4f, 0.8f, 5.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    //kitchencrok

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, 2.0f + j, -1.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.4f, 3.0f, 0.1f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, 2.0f + j, -7.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.4f, 3.0f, 6.0f));
    model = translateMatrix * scaleMatrix;
    crok.drawCubeWithTexture(lightingShaderWithTexture, model);
    // dining
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f + i, -1.0f + j, -7.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 2.0f, 6.0f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f + i, 1.0f + j, -7.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.8f, 0.1f, 6.0f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f + i, 0.4f + j, -6.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.1f, 1.5f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f + i, 0.4f + j, -3.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.1f, 1.5f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);
    
    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.1f + i, 0.4f + j, -5.4f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, -1.5f, 0.3f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(4.1f + i, 0.4f + j, -2.4f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, -1.5f, 0.3f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    //kitchencab

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, -7.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.4f, 2.0f, 6.0f));
    model = translateMatrix * scaleMatrix;
    sc.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, -1.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.4f, 2.0f, 0.1f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);
    //stove
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, 1.0f + j, -7.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.4f, 0.01f, 6.0f));
    model = translateMatrix * scaleMatrix;
    stove.drawCubeWithTexture(lightingShaderWithTexture, model);
    
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, 1.0f + j, -3.5f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.4f, 0.1f, 2.5f));
    model = translateMatrix * scaleMatrix;
    stove.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, 1.0f + j, -6.8f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.4f, 0.1f, 3.0f));
    model = translateMatrix * scaleMatrix;
    st.drawCubeWithTexture(lightingShaderWithTexture, model);


    //book
    translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0f + i, -1.0f + j, 4.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7f, 5.0f, 4.0f));
    model = translateMatrix * scaleMatrix;
    bookcab.drawCubeWithTexture(lightingShaderWithTexture, model);
    //lamp

    translateMatrix = glm::translate(identityMatrix, glm::vec3(10.9f, 1.5f, 3.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.15f, -2.5f, 0.15f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(10.5f, -1.0f, 2.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 1.0f));
    model = translateMatrix * scaleMatrix;
    glassCube.drawCubeWithTexture(lightingShaderWithTexture, model);
    
    // pool
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-35.0f, -1.5f, -1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, 1.0f, 0.7f));
    model = translateMatrix * scaleMatrix;
    cube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-35.0f, -1.5f, 19.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, 1.0f, 0.7f));
    model = translateMatrix * scaleMatrix;
    cube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-35.0f, -1.5f, -0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7f, 1.0f, 20.0f));
    model = translateMatrix * scaleMatrix;
    cube.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-20.7f, -1.5f, -0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7f, 1.0f, 20.0f));
    model = translateMatrix * scaleMatrix;
    cube.drawCubeWithTexture(lightingShaderWithTexture, model);

    //pool water
    // Pool water (realistic wave effect)
    for (float i = 0.0f; i <= 12.0f; i += 1.0f) {
        for (float j = 0.0f; j < 19.0f; j += 1.0f) {
            // Combine sine waves for more dynamic motion
            float offset = glm::sin(time * 2.0f + i * 0.5f) * 0.3f +
                glm::sin(time * 1.5f + j * 0.3f) * 0.2f;

            // Apply the floating effect to the y position
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-22.7f - i, -1.5f + offset, -0.5f + j));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.7f, 2.0f));
            model = translateMatrix * scaleMatrix;
            water.drawCubeWithTexture(lightingShaderWithTexture, model);
        }
    }
    

}




void readingRoom(Shader& lightingShaderWithTexture,
    Cube& cube,
    Cube& gg,
    Cube& cube2,
    Cube& cubeback,
    Cube& cube3,
    Cube& win,
    Cube& sofa,
    Cube& t1,Cube& keycab, Cube& win2) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix,rotateMatrix, scaleMatrix, model;
    //floor
    float i = 1.5f, j = 6.3f, k = 4.5f;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f+i, -1.0f+j, -4.0f+k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, -0.1f, 12.0f));
    model = translateMatrix * scaleMatrix;
    cube.drawCubeWithTexture(lightingShaderWithTexture, model);
   


    //roof
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f+i, 3.5f+j, -4.0f+k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 0.1f, 12.0f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

    //front wall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f+i, -1.0f+j, -4.0f+k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 4.5f, 0.1f));
    model = translateMatrix * scaleMatrix;
    cubeback.drawCubeWithTexture(lightingShaderWithTexture, model);

    //left wall section 1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f+i, -1.0f+j, -4.0f+k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 12.0f));
    model = translateMatrix * scaleMatrix;
    cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

    

   

    //left wall window part-1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f+i, 0.5f+j, 0.5f+k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.75f, 4.0f));
    model = translateMatrix * scaleMatrix;
    win2.drawCubeWithTexture(lightingShaderWithTexture, model);

    //leaving room art
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f + i, -4.5f + j, 9.5f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.0f, 5.5f));
    model = translateMatrix * scaleMatrix;
    win.drawCubeWithTexture(lightingShaderWithTexture, model);

   

    //right wall section 1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f+i, -1.0f+j, -4.0f+k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.6f, 6.0f));
    model = translateMatrix * scaleMatrix;
    cube3.drawCubeWithTexture(lightingShaderWithTexture, model);


   
    //cabinet
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f + i, -1.0f + j, -3.0f + k));
   // rotateMatrix= rotate(glm::mat4(1.0f), glm::radians(-90.0f) , glm::vec3(0.0f, 1.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 3.0f, 0.5f));
    model = translateMatrix *  scaleMatrix;
    keycab.drawCubeWithTexture(lightingShaderWithTexture, model);

    // right wall section 2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f + i, -1.0f + j, 3.5f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.6f, 4.5f));
    model = translateMatrix * scaleMatrix;
    cube3.drawCubeWithTexture(lightingShaderWithTexture, model);

  
    // sofa1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f + i, -1.0f + j, -2.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.5f, 3.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.8f + i, -1.0f + j, -2.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 1.2f, 3.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f + i, -1.0f + j, -2.2f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.8f, 0.2f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f + i, -1.0f + j, 1.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.8f, 0.2f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    // sofa2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 1.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.5f, 3.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 1.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 1.2f, 3.0f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 0.8f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.8f, 0.2f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f + i, -1.0f + j, 4.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.2f, 0.8f, 0.2f));
    model = translateMatrix * scaleMatrix;
    sofa.drawCubeWithTexture(lightingShaderWithTexture, model);

    // table1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f + i, -1.0f + j, 1.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.4f, 3.0f));
    model = translateMatrix * scaleMatrix;
    t1.drawCubeWithTexture(lightingShaderWithTexture, model);

    // table2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5f + i, -7.0f + j, 1.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.8f, 0.5f, 4.0f));
    model = translateMatrix * scaleMatrix;
    t1.drawCubeWithTexture(lightingShaderWithTexture, model);



    // football
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f + i, -0.7f + j, 7.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    model = translateMatrix * scaleMatrix;
    //sphere.drawSphereTexture(lightingShaderWithTexture, model, diffMaps, specMaps);

 

}


void bedroom(Shader& lightingShaderWithTexture,
    Cube& cube,
    Cube& bed,
    Cube& cube2,
    Cube& cubeback,
    Cube& cube3,
    Cube& win,
    Cube& tv,
    Cube& t1, Cube& keycab, Cube& win2) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateMatrix, scaleMatrix, model;
    //floor
    float i = 1.5f, j = 6.3f, k = -6.0f;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + i, -1.0f + j, -4.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, -0.1f, 12.0f));
    model = translateMatrix * scaleMatrix;
    cube.drawCubeWithTexture(lightingShaderWithTexture, model);



    //roof
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + i, 3.5f + j, -4.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 0.1f, 12.0f));
    model = translateMatrix * scaleMatrix;
    cube2.drawCubeWithTexture(lightingShaderWithTexture, model);

    //front wall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + i, -8.0f + j, -4.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(15.0f, 12.0f, 0.1f));
    model = translateMatrix * scaleMatrix;
    cubeback.drawCubeWithTexture(lightingShaderWithTexture, model);


    //front wall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + i, -1.0f + j, 6.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(9.0f, 4.5f, 0.1f));
    model = translateMatrix * scaleMatrix;
    tv.drawCubeWithTexture(lightingShaderWithTexture, model);

    //front wall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0f + i, -8.0f + j, 0.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 6.5f, 6.0f));
    model = translateMatrix * scaleMatrix;
    tv.drawCubeWithTexture(lightingShaderWithTexture, model);

    //left wall section 1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f + i, -1.0f + j, -4.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 4.5f, 12.0f));
    model = translateMatrix * scaleMatrix;
    cube3.drawCubeWithTexture(lightingShaderWithTexture, model);





    //left wall window part-1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5f + i, 0.5f + j, 0.5f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.75f, 4.0f));
    model = translateMatrix * scaleMatrix;
    win2.drawCubeWithTexture(lightingShaderWithTexture, model);

    
    //bed

    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.0f + i, -1.0f + j, -4.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.7f, 5.0f));
    model = translateMatrix * scaleMatrix;
    bed.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f + i, -0.3f + j, -2.8f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.2f, 1.2f));
    model = translateMatrix * scaleMatrix;
    bed.drawCubeWithTexture(lightingShaderWithTexture, model);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f + i, -0.3f + j, -2.8f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.2f, 1.2f));
    model = translateMatrix * scaleMatrix;
    bed.drawCubeWithTexture(lightingShaderWithTexture, model);



    //right wall section 1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f + i, -1.0f + j, -4.0f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.6f, 6.0f));
    model = translateMatrix * scaleMatrix;
    cube3.drawCubeWithTexture(lightingShaderWithTexture, model);



    // right wall section 2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f + i, -1.0f + j, 3.5f + k));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.6f, 4.5f));
    model = translateMatrix * scaleMatrix;
    cube3.drawCubeWithTexture(lightingShaderWithTexture, model);


   

}



void drawFractalTree(glm::mat4 model, Shader& shader, int depth, float length, float angle, Cube& branchCube, float time) {
    if (depth == 0) return;

    // Increased frequency, kept amplitude small
    float swayAngle = glm::radians(2.5f) * sin(time * 1.5f + depth * 0.3f); // Faster frequency, subtle amplitude

    glm::mat4 swayMatrix = glm::rotate(glm::mat4(1.0f), swayAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, length, 0.1f));
    glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, length / 2.0f, 0.0f));
    glm::mat4 branchModel = model * swayMatrix * translateMatrix * scaleMatrix;

    // Draw branch
    branchCube.drawCubeWithTexture(shader, branchModel);

    // Transformations for branches
    glm::mat4 rotateLeft = glm::rotate(glm::mat4(1.0f), glm::radians(angle) + swayAngle, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotateRight = glm::rotate(glm::mat4(1.0f), glm::radians(-angle) - swayAngle, glm::vec3(1.0f, 0.0f, 0.0f));

    // Left branch
    drawFractalTree(model * swayMatrix * translateMatrix * rotateLeft, shader, depth - 1, length * 0.7f, angle, branchCube, time);

    // Right branch
    drawFractalTree(model * swayMatrix * translateMatrix * rotateRight, shader, depth - 1, length * 0.7f, angle, branchCube, time);
}







unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}