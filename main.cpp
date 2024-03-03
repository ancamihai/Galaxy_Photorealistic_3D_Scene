#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <chrono>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

glm::mat4 modelRobot;
glm::mat4 modelTank;
glm::mat4 modelBB8_1;
glm::mat4 modelBB8_2;
glm::mat4 initialModel;

glm::mat3 normalMatrixRobot;
glm::mat3 normalMatrixTank;
glm::mat3 normalMatrixBB8_1;
glm::mat3 normalMatrixBB8_2;
glm::mat3 normalMatrixInitial;

// directional light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
GLfloat lightAngle;
glm::mat4 lightRotation;

// shadows
const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;
GLuint shadowMapFBO;
GLuint depthMapTexture;

gps::Shader depthMapShader;

bool showDepthMap;

gps::Shader screenQuadShader;

gps::Model3D screenQuad;

bool shadowOn=false;

// rain
bool rainOn = false;

// pointlight
int pointLightOn = 0;
glm::vec3 pointLightPos; // on rocket
GLuint pointLightPosLoc;

// spotlight
glm::vec3 spotLightPos;
glm::vec3 spotLightDirection;

GLuint spotLightPosLoc; // on aliens
GLuint spotLightDirectionLoc;

int spotLightOn = 0;
float spotlight1;
float spotlight2;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 2.0f, -5.0f),
    glm::vec3(0.0f, 2.0f, -12.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

bool cameraAnimationFinished = false;
bool cameraAnimationPhase1 = true;
bool cameraAnimationPhase2 = false;
bool cameraAnimationPhase3 = false;
bool cameraAnimationPhase4 = false;
bool cameraAnimationPhase5 = false;
bool cameraAnimationPhase6 = false;

int to_the_left = 0;
int to_the_right = 0;
int to_the_front = 0;
int to_the_back = 0;

// models
gps::Model3D moonland;
gps::Model3D robot;
gps::Model3D tank;
gps::Model3D planet;
gps::Model3D ballCube;
gps::Model3D alien1, alien2, alien3, alien4;
gps::Model3D rocket;
gps::Model3D part1, part2, part3, part4;
gps::Model3D raindropModel;
gps::Model3D bush1;
gps::Model3D airship;
gps::Model3D C3PO;
gps::Model3D top, bottom;

GLfloat angle;
GLfloat planetAngle = 0.0f;
GLfloat robotAngle = 0.0f;

//BB8

int forward = 0;
int rotate = 0;

bool collisionWithBB8Detected = false;

// modes for viewing the objects
enum RenderingMode {
    SOLID,
    WIREFRAME,
    POINT
};

RenderingMode currentRenderingMode = SOLID;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;

// skybox
gps::SkyBox mySkyBox;

// mouse
bool firstMouse = true;
float lastX = 400, lastY = 300;
float pitch = 0.0f, yaw = 90.0f;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// collision with tank
glm::vec3 tankPosition(0.0f, 0.0f, 5.0f);
glm::vec3 robotPosition(0.0f, 0.0f, 0.0f);

bool startDetectionCollisionWithTank = true;
bool collisionWithTankDetected = false;
bool robotDead = false;

// fog
int fogOn = 0;
float fogDensity = 0.04f;

// ball generation
gps::Model3D generatedBall;
bool ballGenerated = false;
glm::vec3 ballGenerationPosition(7.0f,0.0f,1.0f);
glm::vec3 ballFalling(0.0f, 0.0f, 0.0f);
bool firstBallColision = false;
bool secondBallColision = false;
bool thirdBallColision = false;
bool BallRiseUp = false;
int ballFalls = 0;

bool generatedBallOnTheGround = false;
bool generatedBallCanBePicked = false;
bool generatedBallIsPicked = false;

auto startTime = std::chrono::high_resolution_clock::now();
auto currentTime = std::chrono::high_resolution_clock::now();

bool generatedBallsArePicked[500] = { false };
bool generatedBallsAreLeft[500] = { false };
int currentBall = 0;

glm::mat4 modelGeneratedBalls[500];

gps::Model3D pickedBall;


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << " | " << errorCode << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // determine whether to show the depth map or not
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (cameraAnimationFinished == true)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        robotAngle -= xoffset;
        modelRobot = glm::rotate(modelRobot, glm::radians(-xoffset), glm::vec3(0.0f, 1.0f, 0.0f));
        normalMatrixRobot = glm::mat3(glm::inverseTranspose(view * modelRobot));

        if (pitch > 90.0f)
            pitch = 90.0f;
        if (pitch < -90.0f)
            pitch = -90.0f;

        myCamera.rotate(pitch, yaw);
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
    }
}

void animateCamera()
{   
    if (cameraAnimationPhase1 == true)
    {
        yaw += 0.5f;
        myCamera.rotate(pitch, yaw);
        if (yaw == 450.0f)
        {
            cameraAnimationPhase1 = false;
            cameraAnimationPhase2 = true;
        }
    }
    else if (cameraAnimationPhase2 == true)
    {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed/2);
        to_the_left++;
        if (to_the_left == 60)
        {
            cameraAnimationPhase2 = false;
            cameraAnimationPhase3 = true;
            to_the_left = 0;
        }
    }

    else if (cameraAnimationPhase3 == true)
    {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed / 2);
        to_the_right++;
        if (to_the_right == 120)
        {
            cameraAnimationPhase3 = false;
            cameraAnimationPhase4 = true;
        }
    }

    else if (cameraAnimationPhase4 == true)
    {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed / 2);
        to_the_left++;
        if (to_the_left == 60)
        {
            cameraAnimationPhase4 = false;
            cameraAnimationPhase5 = true;
        }
    }


    else if (cameraAnimationPhase5 == true)
    {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2);
        to_the_front++;
        if (to_the_front == 80)
        {
            cameraAnimationPhase5 = false;
            cameraAnimationPhase6 = true;
        }
    }

    else if (cameraAnimationPhase6 == true)
    {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed / 2);
        to_the_back++;
        if (to_the_back == 80)
        {
            cameraAnimationPhase6 = false;
            cameraAnimationFinished = true;
        }
    }
    
}

void collisionWithTank()
{
    GLfloat length = glm::length(tankPosition-robotPosition);
    if ((tankPosition.z > 0 && robotPosition.z > 0 && (glm::dot(normalize(tankPosition), normalize(robotPosition)) > 0.8)) || (tankPosition.z > 0 && robotPosition.z < 0 && (glm::dot(normalize(tankPosition), normalize(robotPosition)) < -0.9)) || (tankPosition.z < 0 && robotPosition.z < 0 && (glm::dot(normalize(tankPosition), normalize(robotPosition)) > 0.99)))
    {
        collisionWithTankDetected = true;
    }
    else {
        collisionWithTankDetected = false;
    }

    if (length < 1.0f && collisionWithTankDetected)
    {
        robotDead = true;
        startDetectionCollisionWithTank = false;
        collisionWithTankDetected = false;
    }
}

void enteredBallGenerationZone()
{
    GLfloat length1 = glm::length(ballGenerationPosition - robotPosition);
    GLfloat length2 = glm::length(robotPosition - ballGenerationPosition);
    if ((length1 < 1.5f || length2 < 1.5f)  && ballGenerated==false)
    {
        ballGenerated = true;
    }

    if ((length1 < 1.5f || length2 < 1.5f) && generatedBallOnTheGround == true && generatedBallIsPicked==false)
    {
        generatedBallCanBePicked = true;
    }
}

void collisionWithBB8()
{
    glm::vec3 BB8position;
    if (forward <= 150)
    {
        BB8position = glm::vec3(12.93f, 0.0f, 7.16f) + glm::vec3(-forward * 0.03f, 0.0f, -forward * 0.03f);
    }
    else
    {
        BB8position = glm::vec3(12.93f, 0.0f, 7.16f) - glm::vec3(-150 * 0.03f, 0.0f, -150 * 0.03f)+ glm::vec3((forward - 150) * 0.03f, 0.0f, (forward- 150) * 0.03f);
    }
    GLfloat length1 = glm::length(BB8position - robotPosition);
    GLfloat length2 = glm::length(robotPosition - glm::vec3(12.93f, 0.0f, 7.16f));
    if ((length1 < 2.0f || length2 < 2.0f))
    {
        collisionWithBB8Detected = true;
    }
    else
    {
        collisionWithBB8Detected = false;
    }
}

void initSkybox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/left.png");
    faces.push_back("skybox/right.png");
    faces.push_back("skybox/top.png");
    faces.push_back("skybox/bottom.png");
    faces.push_back("skybox/front.png");
    faces.push_back("skybox/back.png");
    mySkyBox.Load(faces);
}

void changeSkyBoxFog()
{
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/foggy.jpg");
    faces.push_back("skybox/foggy.jpg");
    faces.push_back("skybox/foggy.jpg");
    faces.push_back("skybox/foggy.jpg");
    faces.push_back("skybox/foggy.jpg");
    faces.push_back("skybox/foggy.jpg");
    mySkyBox.Load(faces);
}

void processMovement() {

    // move forward
	if (pressedKeys[GLFW_KEY_W]) {
        if (robotPosition.z <= 15)
        {
            glm::vec3 translateRobot = myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
            translateRobot.y = 0.0f;
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // update normal matrix
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

            //robot
            modelRobot = glm::rotate(modelRobot, glm::radians(-robotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelRobot = glm::translate(modelRobot, translateRobot);
            modelRobot = glm::rotate(modelRobot, glm::radians(robotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            normalMatrixRobot = glm::mat3(glm::inverseTranspose(view * modelRobot));

            robotPosition += translateRobot;
        }
	}

    // move backward
	if (pressedKeys[GLFW_KEY_S]) {
        if (robotPosition.z >=-15)
        {
            glm::vec3 translateRobot = myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
            translateRobot.y = 0.0f;
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // update normal matrix
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

            //robot
            modelRobot = glm::rotate(modelRobot, glm::radians(-robotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelRobot = glm::translate(modelRobot, translateRobot);
            modelRobot = glm::rotate(modelRobot, glm::radians(robotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            normalMatrixRobot = glm::mat3(glm::inverseTranspose(view * modelRobot));

            robotPosition += translateRobot;
        }
	}

    // move left
	if (pressedKeys[GLFW_KEY_A]) {
        if (robotPosition.x <= 15)
        {
            glm::vec3 translateRobot = myCamera.move(gps::MOVE_LEFT, cameraSpeed);
            translateRobot.y = 0.0f;
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // update normal matrix
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

            //robot
            modelRobot = glm::rotate(modelRobot, glm::radians(-robotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelRobot = glm::translate(modelRobot, translateRobot);
            modelRobot = glm::rotate(modelRobot, glm::radians(robotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            normalMatrixRobot = glm::mat3(glm::inverseTranspose(view * modelRobot));

            robotPosition += translateRobot;
        }
	}

    // move right
	if (pressedKeys[GLFW_KEY_D]) {
        if (robotPosition.x >= -15)
        {
            glm::vec3 translateRobot = myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
            translateRobot.y = 0.0f;
            //update view matrix
            view = myCamera.getViewMatrix();
            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // update normal matrix 
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

            //robot
            modelRobot = glm::rotate(modelRobot, glm::radians(-robotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelRobot = glm::translate(modelRobot, translateRobot);
            modelRobot = glm::rotate(modelRobot, glm::radians(robotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            normalMatrixRobot = glm::mat3(glm::inverseTranspose(view * modelRobot));

            robotPosition += translateRobot;
        }
	}

    // rotate robot/camera
    if (pressedKeys[GLFW_KEY_Q])
    {
        yaw += 1.0f;
        robotAngle -= 1.0f;
        modelRobot = glm::rotate(modelRobot, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        normalMatrixRobot = glm::mat3(glm::inverseTranspose(view * modelRobot));

        myCamera.rotate(pitch, yaw);
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
    }

    // rotate robot/camera in the other direction
    if (pressedKeys[GLFW_KEY_E])
    {
        yaw -= 1.0f;
        robotAngle += 1.0f;
        modelRobot = glm::rotate(modelRobot, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        normalMatrixRobot = glm::mat3(glm::inverseTranspose(view * modelRobot));

        myCamera.rotate(pitch, yaw);
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
    }

    // rotate directional light
    if (pressedKeys[GLFW_KEY_Z]) {
        lightAngle -= 1.0f;
    }

    // rotate directional light in the other direction
    if (pressedKeys[GLFW_KEY_X]) {
        lightAngle += 1.0f;
    }

    // activate point light
    if (pressedKeys[GLFW_KEY_C]) {
        pointLightOn = 1;

        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightOn"), pointLightOn);
    }

    // stop point light
    if (pressedKeys[GLFW_KEY_V]) {
        pointLightOn = 0;

        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightOn"), pointLightOn);
    }

    // activate spot light
    if (pressedKeys[GLFW_KEY_B]) {
        spotLightOn = 1;

        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "spotLightOn"), spotLightOn);
    }

    // stop spot light
    if (pressedKeys[GLFW_KEY_N]) {
        spotLightOn = 0;

        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "spotLightOn"), spotLightOn);
    }

    // display shadows
    if (pressedKeys[GLFW_KEY_F]) {
        shadowOn = true;
    }

    // make shadows non-visible
    if (pressedKeys[GLFW_KEY_G]) {
        shadowOn = false;
    }

    // activate fog
    if (pressedKeys[GLFW_KEY_K]) {
        changeSkyBoxFog();
        fogOn = 1;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "fogOn"), fogOn);
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);
    }

    // make fog non-visible
    if (pressedKeys[GLFW_KEY_L]) {
        initSkybox();
        fogOn = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "fogOn"), fogOn);
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);
    }

    // activate rain
    if (pressedKeys[GLFW_KEY_H]) {
        rainOn = true;
    }

    // stop rain
    if (pressedKeys[GLFW_KEY_J]) {
        rainOn = false;
    }

    // solid surfaces
    if (pressedKeys[GLFW_KEY_Y]) {
        currentRenderingMode = SOLID;
    }

    // wireframe surfaces
    if (pressedKeys[GLFW_KEY_U]) {
        currentRenderingMode = WIREFRAME;
    }

    // point surfaces
    if (pressedKeys[GLFW_KEY_I]) {
        currentRenderingMode = POINT;
    }

    // revive robot
    if (pressedKeys[GLFW_KEY_R]) {
        if (robotDead == true) {
            robotDead = false;
        }
    }

    // if robot is revived, start collision with tank detection
    if (pressedKeys[GLFW_KEY_T]) {
        if (robotDead == false && startDetectionCollisionWithTank==false) {
            startDetectionCollisionWithTank = true;
        }
    }

    // pick generated ball
    if (pressedKeys[GLFW_KEY_P]) {
        if (generatedBallCanBePicked==true && robotDead==false) {
            generatedBallsArePicked[currentBall] = true;
            startTime = std::chrono::high_resolution_clock::now();
            currentTime = std::chrono::high_resolution_clock::now();
            ballFalling = glm::vec3(0.0f, 0.0f, 0.0f);
            generatedBallIsPicked=true;
            generatedBallCanBePicked = false;
            generatedBallOnTheGround = false;
            ballGenerated = false;
            firstBallColision = false;
            secondBallColision = false;
            thirdBallColision = false;
            BallRiseUp = false;
        }
    }

    // leave picked ball on the ground
    if (pressedKeys[GLFW_KEY_O]) {
        if (generatedBallIsPicked == true && robotDead==false)
        {
            generatedBallIsPicked = false;
            generatedBallsAreLeft[currentBall] = true;
            currentBall++;
        }
    }
}

void initOpenGLWindow() {
    myWindow.Create(1920, 1001, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    moonland.LoadModel("models/moonterrain/moonterrain.obj");
    robot.LoadModel("models/robot/robot.obj");    
    tank.LoadModel("models/tank/tank.obj");
    planet.LoadModel("models/planet/planet.obj");
    generatedBall.LoadModel("models/ball1/ball1.obj");
    ballCube.LoadModel("models/ballCube/ballCube.obj");
    pickedBall.LoadModel("models/ball2/ball2.obj");
    alien1.LoadModel("models/aliens/alien1.obj");
    alien2.LoadModel("models/aliens/alien2.obj");
    alien3.LoadModel("models/aliens/alien3.obj");
    alien4.LoadModel("models/aliens/alien4.obj");
    rocket.LoadModel("models/rocket/rocket.obj");
    part1.LoadModel("models/lamp/part1.obj");
    part2.LoadModel("models/lamp/part2.obj");
    part3.LoadModel("models/lamp/part3.obj");
    part4.LoadModel("models/lamp/part4.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    raindropModel.LoadModel("models/raindrop/raindrop.obj");
    bush1.LoadModel("models/bush1/bush1.obj");
    airship.LoadModel("models/airship/airship.obj");
    C3PO.LoadModel("models/C3PO/C3PO.obj");
    top.LoadModel("models/BB8/top.obj");
    bottom.LoadModel("models/BB8/bottom.obj");
}

void initShaders() {
	myBasicShader.loadShader( "shaders/basic.vert","shaders/basic.frag");
    myBasicShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/shadow.vert", "shaders/shadow.frag");
    depthMapShader.useShaderProgram();
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}

void initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);

    //create the texture
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightView = glm::lookAt(glm::vec3(lightRotation * glm::vec4(lightDir, 1.0f)), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.001f;
    const GLfloat far_plane = 250.0f; 
    glm::mat4 lightProjection = glm::ortho(-20.0f, 60.0f, -20.0f, 60.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;
}

void initUniforms() {

    myBasicShader.useShaderProgram();

    // create model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    initialModel = model;
    modelRobot = model;
    modelTank = model;
    modelBB8_1 = model;
    modelBB8_2 = model;

    for (int i = 0; i < 500; i++)
    {
        modelGeneratedBalls[i] = model;
    }

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    normalMatrixRobot = normalMatrix;
    normalMatrixTank = normalMatrix;
    normalMatrixInitial = normalMatrix;

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.001f, 2000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    // set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // pointlight
    pointLightPos = glm::vec3(18.2f, 5.0f, -7.7f);
    pointLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPos");
    glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPos));
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightOn"), pointLightOn);

    // spotlight
    spotLightPos = glm::vec3(-9.3f, 6.1f, 15.1f);
    spotLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightPos");
    glUniform3fv(spotLightPosLoc, 1, glm::value_ptr(spotLightPos));

    spotLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
    spotLightDirectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightDirection");
    glUniform3fv(spotLightDirectionLoc, 1, glm::value_ptr(spotLightDirection));

    spotlight1 = glm::cos(glm::radians(40.5f));
    spotlight2 = glm::cos(glm::radians(100.5f));

    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight1"), spotlight1);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight2"), spotlight2);

    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "spotLightOn"), spotLightOn);

    // fog
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "fogOn"), fogOn);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);
}

struct Raindrop {
    glm::vec3 position;
    glm::vec3 velocity;;
};


void initializeRaindrops(std::vector<Raindrop>& raindrops, int screenWidth, int screenHeight) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (auto& raindrop : raindrops) {
        raindrop.position.x = static_cast<float>(std::rand()) / RAND_MAX * 60.0f - (30.0f);
        raindrop.position.y = static_cast<float>(std::rand() % (screenHeight + 1));
        raindrop.position.y = static_cast<float>(std::rand() % screenHeight);
        raindrop.position.y = (raindrop.position.y / static_cast<float>(screenHeight)) * 10.0f;
        raindrop.position.z = static_cast<float>(std::rand()) / RAND_MAX * 60.0f - (30.0f);
        raindrop.velocity.x = 0.0f;
        raindrop.velocity.y = -4.0f;
        raindrop.velocity.z = 0.0f;
    }
}

void updateRaindrops(std::vector<Raindrop>& raindrops, float deltaTime) {
    if (rainOn == true)
    {
        for (auto& raindrop : raindrops) {
            raindrop.position += raindrop.velocity * deltaTime;

            if (raindrop.position.y < 0.0f) {
                raindrop.position.y = 10.0f;
            }
        }
    }
}

void renderRain(std::vector<Raindrop>& raindrops, gps::Shader shader) {
    shader.useShaderProgram();

    for (const auto& raindrop : raindrops) {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(raindrop.position.x, raindrop.position.y, raindrop.position.z));

        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        if (rainOn == true)
        {
            raindropModel.Draw(shader);
        }
    }
}

void renderC3PO(gps::Shader shader, bool depthPass)
{
    // select active shader program
    shader.useShaderProgram();

    model = initialModel;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = normalMatrixInitial;
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw ship
    if (shadowOn == true || (depthPass == false))
    {
        C3PO.Draw(shader);
    }
}

void renderBB8(gps::Shader shader, bool depthPass)
{
    // select active shader program
    shader.useShaderProgram();
    if (collisionWithBB8Detected == false)
    {
        if ((depthPass == false) && forward <= 150 && cameraAnimationFinished)
        {
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-forward * 0.03f, 0.0f, -forward * 0.03f));
            modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-forward * 0.03f, 0.0f, -forward * 0.03f));
            modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(12.93f, 0.0f, 7.16f));
            modelBB8_2 = glm::rotate(modelBB8_2, glm::radians(forward * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-12.93f, 0.0f, -7.16f));
            forward++;
        }
        else if ((depthPass == false) && rotate <= 180 && cameraAnimationFinished)
        {
            modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-150 * 0.03f, 0.0f, -150 * 0.03f));
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-150 * 0.03f, 0.0f, -150 * 0.03f));
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(12.93f, 0.0f, 7.16f));
            modelBB8_1 = glm::rotate(modelBB8_1, glm::radians(rotate * 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-12.93f, 0.0f, -7.16f));
            rotate++;
        }
        else if ((depthPass == false) && forward <= 300 && cameraAnimationFinished)
        {
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3((forward - 150) * 0.03f, 0.0f, (forward - 150) * 0.03f));
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-150 * 0.03f, 0.0f, -150 * 0.03f));
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(12.93f, 0.0f, 7.16f));
            modelBB8_1 = glm::rotate(modelBB8_1, glm::radians(rotate * 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-12.93f, 0.0f, -7.16f));
            modelBB8_2 = glm::translate(modelBB8_2, glm::vec3((forward - 150) * 0.03f, 0.0f, (forward - 150) * 0.03f));
            modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-150 * 0.03f, 0.0f, -150 * 0.03f));
            modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(12.93f, 0.0f, 7.16f));
            modelBB8_2 = glm::rotate(modelBB8_2, glm::radians(forward * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-12.93f, 0.0f, -7.16f));
            forward++;
        }
        else if ((depthPass == false) && rotate <= 360 && cameraAnimationFinished)
        {
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(12.93f, 0.0f, 7.16f));
            modelBB8_1 = glm::rotate(modelBB8_1, glm::radians(rotate * 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-12.93f, 0.0f, -7.16f));
            rotate++;
        }
        else if ((depthPass == false) && cameraAnimationFinished) {
            forward = 0;
            rotate = 0;
        }
    }
    else {
        if ((depthPass == false) && cameraAnimationFinished)
        {
            if (forward <= 150)
            {
                modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-forward * 0.03f, 0.0f, -forward * 0.03f));
                modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(12.93f, 0.0f, 7.16f));
                modelBB8_1 = glm::rotate(modelBB8_1, glm::radians(rotate * 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-12.93f, 0.0f, -7.16f));
                modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-forward * 0.03f, 0.0f, -forward * 0.03f));
                modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(12.93f, 0.0f, 7.16f));
                modelBB8_2 = glm::rotate(modelBB8_2, glm::radians(forward * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-12.93f, 0.0f, -7.16f));
            }
            else {
                modelBB8_1 = glm::translate(modelBB8_1, glm::vec3((forward - 150) * 0.03f, 0.0f, (forward - 150) * 0.03f));
                modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-150 * 0.03f, 0.0f, -150 * 0.03f));
                modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(12.93f, 0.0f, 7.16f));
                modelBB8_1 = glm::rotate(modelBB8_1, glm::radians(rotate * 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                modelBB8_1 = glm::translate(modelBB8_1, glm::vec3(-12.93f, 0.0f, -7.16f));
                modelBB8_2 = glm::translate(modelBB8_2, glm::vec3((forward - 150) * 0.03f, 0.0f, (forward - 150) * 0.03f));
                modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-150 * 0.03f, 0.0f, -150 * 0.03f));
                modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(12.93f, 0.0f, 7.16f));
                modelBB8_2 = glm::rotate(modelBB8_2, glm::radians(forward * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                modelBB8_2 = glm::translate(modelBB8_2, glm::vec3(-12.93f, 0.0f, -7.16f));
            }
        }
    }

    normalMatrixBB8_1 = glm::mat3(glm::inverseTranspose(view * modelBB8_1));
    normalMatrixBB8_2 = glm::mat3(glm::inverseTranspose(view * modelBB8_2));

    model = modelBB8_1;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = normalMatrixBB8_1;
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw BB8_1
    if (shadowOn == true || (depthPass == false))
    {
        top.Draw(shader);
    }

    model = modelBB8_2;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = normalMatrixBB8_2;
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw BB8_2
    if (shadowOn == true || (depthPass == false))
    {
        bottom.Draw(shader);
    }

     modelBB8_2 = initialModel;
     modelBB8_1 = initialModel;
   
}

void renderAirShip(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    model = initialModel;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = normalMatrixInitial;
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw C3PO
    if (shadowOn == true || (depthPass == false))
    {
        airship.Draw(shader);
    }
}

void renderBushes(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    model = initialModel;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = normalMatrixInitial;
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw bushes
    if (shadowOn == true || (depthPass == false))
    {
        bush1.Draw(shader);
    }
}

void renderMoonland(gps::Shader shader,bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    model = initialModel;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = normalMatrixInitial;
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw moonland
    if (shadowOn == true || (depthPass == false))
    {
        moonland.Draw(shader);
    }
}

void renderRobot(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glCheckError();
    model = modelRobot;

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram,"model"), 1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass) {
        normalMatrix = normalMatrixRobot;

        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    }

    // draw robot
    if (shadowOn == true || (depthPass == false))
    {if (robotDead == false)
    {
        robot.Draw(shader);
    }
    }
}

void renderTank(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    if (collisionWithTankDetected && tankPosition.z > -15.0f)
    {
            model = glm::translate(modelTank, glm::vec3(0, 0.0f, -0.01f));
            if (!depthPass)
            {
                tankPosition += glm::vec3(0.0f, 0.0f, -0.01f);
                modelTank = model;
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            }
    }
    else if (robotDead == true)
    {
        model = initialModel;
        modelTank = model;
        if (!depthPass)
        {
            normalMatrix = normalMatrixInitial;
        }
        tankPosition=glm::vec3(0.0f, 0.0f, 5.0f);
    }
    else {
        model = modelTank;
        if (!depthPass)
        {
            normalMatrix = normalMatrixTank;
        }
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass)
    {
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw tank
    if (shadowOn == true || (depthPass == false))
    {
        tank.Draw(shader);
    }

}

void renderPlanet(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    if (!depthPass)
    {
        planetAngle += 1.0f;
    }

    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(-16.5f, -5.3f, -0.4f));

    model = glm::rotate(model, glm::radians(planetAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::translate(model, glm::vec3(16.5f, 5.3f, 0.4f));

    if (!depthPass)
    {
        normalMatrix = normalMatrixInitial;
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass)
    {
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw rotating planet
    if (shadowOn == true || (depthPass == false))
    {
        planet.Draw(shader);
    }
}


void renderGeneratedBall(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    if (!depthPass)
    {
        if (ballGenerated == true || generatedBallOnTheGround == true) {

            if (firstBallColision == false)
            {
                ballFalling -= glm::vec3(0.0f, 0.05f, 0.0f);
                ballFalls++;
                if (ballFalls == 36)
                {
                    firstBallColision = true;
                    ballFalls = 0;

                }
            }
            else if (secondBallColision == false && BallRiseUp == false)
            {
                ballFalling += glm::vec3(0.0f, 0.05f, 0.0f);
                ballFalls++;
                if (ballFalls == 18)
                {
                    BallRiseUp = true;
                    ballFalls = 0;

                }
            }
            else if (secondBallColision == false)
            {
                ballFalling -= glm::vec3(0.0f, 0.05f, 0.0f);
                ballFalls++;
                if (ballFalls == 18)
                {
                    secondBallColision = true;
                    BallRiseUp = false;
                    ballFalls = 0;

                }
            }
            else if (BallRiseUp == false)
            {
                ballFalling += glm::vec3(0.0f, 0.05f, 0.0f);
                ballFalls++;
                if (ballFalls == 9)
                {
                    BallRiseUp = true;
                    ballFalls = 0;

                }
            }

            else if (thirdBallColision == false)
            {
                ballFalling -= glm::vec3(0.0f, 0.05f, 0.0f);
                ballFalls++;
                if (ballFalls == 9)
                {
                    thirdBallColision = true;
                    ballFalls = 0;
                    generatedBallOnTheGround = true;
                }
            }

        }
    }

    model = glm::translate(initialModel,ballFalling);

    if (!depthPass)
    {
        normalMatrix = normalMatrixInitial;
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass)
    {
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    if (shadowOn == true || (depthPass == false))
    { 
    if (ballGenerated == true || generatedBallOnTheGround == true) {
        generatedBall.Draw(shader);
    }
    }
    
}

void renderPickedBall(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    for (int i = 0; i < 500; i++)
    {
        if (generatedBallsAreLeft[i] == false)
        {
            model = modelRobot;

            if (!depthPass)
            {
                normalMatrix = normalMatrixRobot;
            }

            modelGeneratedBalls[i] = model;

        }

        else {
            model = modelGeneratedBalls[i];
            model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));

            if (!depthPass)
            {
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            }
        }

        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass)
        {

            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        }

        if (shadowOn == true || (depthPass == false))
        {
            if ((generatedBallsArePicked[i] == true && generatedBallsAreLeft[i]==true) || (generatedBallsArePicked[i] == true && robotDead==false))
             {
               pickedBall.Draw(shader);
             }
        }
    }
}

void renderBallCube(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    model = initialModel;

    if (!depthPass)
    {
        normalMatrix = normalMatrixInitial;
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    
    if (!depthPass)
    {
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw ball cube
    if (shadowOn == true || (depthPass == false))
    {
        if (ballGenerated == false && generatedBallOnTheGround == false) {
            ballCube.Draw(shader);
        }
    }
}


void renderAliens(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    model = initialModel;

    if (!depthPass)
    {
        normalMatrix = normalMatrixInitial;
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass)
    {
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw aliens
    if (shadowOn == true || (depthPass == false))
    {
        alien1.Draw(shader);
        alien2.Draw(shader);
        alien3.Draw(shader);
        alien4.Draw(shader);
    }
}

void renderRocket(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    model = initialModel;

    if (!depthPass)
    {
        normalMatrix = normalMatrixInitial;
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass)
    {
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw rocket
    if (shadowOn == true || (depthPass == false))
    {
        rocket.Draw(shader);
    }
}

void renderLamp(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    model = initialModel;

    if (!depthPass)
    {
    normalMatrix = normalMatrixInitial;
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass)
    {
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw lamp
    if (shadowOn == true || (depthPass == false))
    {
        part1.Draw(shader);
        part2.Draw(shader);
        part3.Draw(shader);
        part4.Draw(shader);
    }
}

void renderScene(std::vector<Raindrop>& raindrops) {

    // Set up polygon mode based on the current rendering mode
    switch (currentRenderingMode) {
    case SOLID:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case WIREFRAME:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case POINT:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    }

    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Rendering models and skybox
    renderMoonland(depthMapShader,1);
    renderRobot(depthMapShader,1);
    renderTank(depthMapShader,1);
    renderPlanet(depthMapShader,1);
    renderRocket(depthMapShader,1);
    renderAliens(depthMapShader,1);
    renderLamp(depthMapShader,1);
    renderBushes(depthMapShader, 1);
    renderAirShip(depthMapShader, 1);
    renderC3PO(depthMapShader, 1);
    renderBB8(depthMapShader, 1);
    renderGeneratedBall(depthMapShader,1);
    renderBallCube(depthMapShader,1);
    renderPickedBall(depthMapShader,1);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (showDepthMap) {
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        renderRain(raindrops, myBasicShader);

        renderMoonland(myBasicShader, false);
        renderRobot(myBasicShader, false);
        renderTank(myBasicShader, false);
        renderPlanet(myBasicShader, false);
        renderRocket(myBasicShader, false);
        renderAliens(myBasicShader, false);
        renderLamp(myBasicShader, false);
        renderBushes(myBasicShader, false);
        renderAirShip(myBasicShader, false);
        renderC3PO(myBasicShader, false);
        renderBB8(myBasicShader, false);
        renderGeneratedBall(myBasicShader, false);
        renderBallCube(myBasicShader, false);
        renderPickedBall(myBasicShader, false);

        mySkyBox.Draw(skyboxShader, view, projection);
        
    }
}

void cleanup() {
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
    initSkybox();
	initUniforms();
    initFBO();
    std::vector<Raindrop> raindrops(2000);

    initializeRaindrops(raindrops, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    setWindowCallbacks();

    auto lastFrameTime = std::chrono::high_resolution_clock::now();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {

        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTimeFrame = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        float deltaTimeSeconds = deltaTimeFrame.count();

        currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - startTime;

        updateRaindrops(raindrops,deltaTimeSeconds);

        if (cameraAnimationFinished == false)
        {
            animateCamera();
        }
        if (cameraAnimationFinished == true)
        {
            processMovement();
        }
        
        if (cameraAnimationFinished == true && startDetectionCollisionWithTank==true)
        {
        collisionWithTank();
        }

        if (cameraAnimationFinished == true)
        {
            collisionWithBB8();
        }


        if (currentBall <499 && cameraAnimationFinished == true && robotDead== false && ((ballGenerated == false && generatedBallIsPicked== false) || (ballGenerated == false && (deltaTime.count()>10.0f && deltaTime.count()<20.0f)) || generatedBallOnTheGround==true))
        {
            
            enteredBallGenerationZone();
        }

	    renderScene(raindrops);

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
