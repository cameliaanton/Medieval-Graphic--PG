#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include "Model3D.hpp"

#include <iostream>
#include <irrKlang.h>

using namespace irrklang;
ISoundEngine *engine;
ISound *sound_cal_oftat;
ISound* sound_cal_nechezat;
ISound* sound_cal_troptrop;
ISound* sound_dog_marait;
ISound* sound_dog_tired;
ISound* sound_dog_small;
ISound* sound_batman;
ISound *sound_village;

bool play_background = true;
bool play_batman,play_cal_oftat,play_cal_nechezat,play_cal_troptrop,play_dog_marai,play_dog_tired,play_dog_small = false;
int first_batman = 0;
// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::vec3 lightfelinarPos = glm::vec3(-13.913, 11.276 , 50.441 );
glm::vec3 lightfelinarPos1 = glm::vec3(20.353, 9.8828, 42.916);
glm::vec3 lightfelinarPos2 = glm::vec3(-14.958, 9.8708, 36.439);
glm::vec3 lightfelinarPos3 = glm::vec3(11.591, 9.8864, 23.221);
glm::vec3 lightfelinarPos4 = glm::vec3(17.031, 9.9012, 57.738);
int activPointLight = 0;

//parametrii ceata
int ceata = 0;


// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(20.0f, 40.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];
//mouse
bool firstMouse = true;
float lastX = 400, lastY = 300;
float pitch = 0.0f, yaw = -90.0f;

// models
gps::Model3D teapot;
gps::Model3D scena_statica;
gps::Model3D minge;
gps::Model3D cal_caruta;
gps::Model3D dog1_coada;
gps::Model3D dog1_corp;
gps::Model3D dog2;
gps::Model3D felinar;
gps::Model3D cos_cartofi;
gps::Model3D batman_balloon;
gps::Model3D piatra;

//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

glm::mat4 modelMinge = glm::mat4(1.0f);
glm::mat4 modelCal_Caruta = glm::mat4(1.0f);
glm::mat4 modelDog1_coada = glm::mat4(1.0f);
glm::mat4 modelDog2 = glm::mat4(1.0f);
glm::mat4 modelBattman_balloon = glm::mat4(1.0f);
bool battman_balloon_up=false;
bool battman_balloon_down=false;

const float mingeRotationSpeed = 50.0f;
const float cal_carutaRotationSpeed = 50.0f;
const float dog1_coadaRotationSpeed = 20.0f;

GLfloat angle;
GLfloat mingeAngle = 0.0f;
GLfloat cal_carutaAngle = 0.0f;
GLfloat dog1_coadaAngle = 0.0f;


glm::vec3 cal_carutaPos = glm::vec3(-3.70546, 6.75304, 38.2399);
float cal_carutaHorizontalPos = 0.0f;
float cal_carutaHorizontalSpeed = 1.0f;  
float max_cal_caruta = 20.0f;
float min_cal_caruta = -10.0f;
glm::vec3 mingePos = glm::vec3(1.45166f, 5.42802, 26.1533);
float mingeVerticalPos = 0.0f;
float mingeVerticalSpeed = 1.0f;  
float maxHeight = 10.0f;
glm::vec3 dog1_coadaPos = glm::vec3(6.21993f, 6.81438f, 46.7217f);

glm::vec3 dog2_Pos = glm::vec3(0.7752, 5.1352, 55.577);
float dog2Pos_x, dog2Pos_y, dog2Pos_z=0.0f;
float dog2_sunet=0;
glm::vec3 cos_cartofi_Pos = glm::vec3(-10.201, 5.7204, 2.0377);
glm::vec3 battman_balloon_Pos = glm::vec3(-0.25726, 7.8795, 45.279);
float balloonVerticalPos = 0.0f;

// shaders
gps::Shader myBasicShader;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

bool showDepthMap;

bool animationEnabled = true;


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
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void initSoundEngine()
{
    engine = createIrrKlangDevice();
    if (!engine)
        return; // error starting up the engine
}
void play_sound_cal_nechezat()
{
    sound_cal_nechezat = engine->play2D("media/nechezat_cal.ogg", GL_TRUE);
}
void play_sound_cal_troptrop()
{
    sound_cal_troptrop = engine->play2D("media/troptrop_cal.ogg", GL_TRUE);
}
void play_sound_cal_oftat()
{
    sound_cal_oftat = engine->play2D("media/oftat_cal.ogg", GL_TRUE);
}
void play_sound_village()
{
    sound_village = engine->play2D("media/village.ogg", GL_TRUE);
}
void play_sound_batman()
{
    sound_batman = engine->play2D("media/batman.ogg", GL_TRUE);
}
void play_sound_tired_dog()
{
    sound_dog_tired = engine->play2D("media/tired_dog.ogg", GL_TRUE);
}
void play_sound_marait_dog()
{
    sound_dog_marait = engine->play2D("media/marait_dog.ogg", GL_TRUE);
}
void play_sound_small_dog()
{
    sound_dog_small = engine->play2D("media/small_dog.ogg", GL_TRUE);
}
void stop_sound()
{
    engine->stopAllSounds();
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_G && GLFW_PRESS)
    {
        ceata = 1;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "ceata"), ceata);
    }
    if (key == GLFW_KEY_H && GLFW_PRESS)
    {
        ceata = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "ceata"), ceata);
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;
    
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        // Invertează starea de animație când tasta 'M' este apăsată
        animationEnabled = !animationEnabled;
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        if (activPointLight == 0) {
            activPointLight = 1;
        }
        else {
            activPointLight = 0;
        }
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activPointLight"), activPointLight);
    }

    if (key == GLFW_KEY_R && GLFW_PRESS)
    {
        if (play_batman==false)
        {
            play_batman = true;
            stop_sound();
            play_sound_batman();
        }
        if (balloonVerticalPos >= 30)
            balloonVerticalPos = balloonVerticalPos;
        else balloonVerticalPos += 0.05;
        if (balloonVerticalPos == 0)
            balloonVerticalPos = balloonVerticalPos;
    }
    if (key == GLFW_KEY_F && GLFW_PRESS)
    {
        if (play_batman==false)
        {
            play_batman = true;
            stop_sound();
            play_sound_batman();
        }
        if (balloonVerticalPos <= 0.0)
            balloonVerticalPos = balloonVerticalPos;
        else balloonVerticalPos -= 0.05;
    }
    if (key == GLFW_KEY_T && action==GLFW_PRESS)
    {
        stop_sound();
        play_sound_village(); 
        play_batman = false;
    }
    if (key == GLFW_KEY_KP_9 && GLFW_PRESS)
    {
        dog2Pos_y -= 0.05;
        dog2Pos_x += 0.05;
    }
    if (key == GLFW_KEY_KP_8 &&  GLFW_PRESS)
    {
        dog2Pos_y -= 0.05;
    }
    if (key == GLFW_KEY_KP_7 && GLFW_PRESS)
    {
        dog2Pos_y -= 0.05;
        dog2Pos_x -= 0.05;
    }
    if (key == GLFW_KEY_KP_4 && GLFW_PRESS)
    {
        dog2Pos_x -= 0.05;
    }
    if (key == GLFW_KEY_KP_5 && GLFW_PRESS)
    {
        dog2Pos_y += 0.05;
    }
    if (key == GLFW_KEY_KP_6 && GLFW_PRESS)
    {
        dog2Pos_x += 0.05;
    }
    if (key == GLFW_KEY_KP_2 && action==GLFW_PRESS)
    {
        play_sound_small_dog();
        if (play_dog_small == false)
        {
            play_sound_small_dog();
            play_dog_small = true;
        }
        else {
            play_dog_small = false;
            stop_sound();
            play_sound_village();
        }
    }
    if (key == GLFW_KEY_KP_1 && action==GLFW_PRESS)
    {
        if (play_dog_tired == false)
        {
            play_sound_tired_dog();
            play_dog_tired = true;
        }
        else {
            play_dog_tired = false;
            stop_sound();
            play_sound_village();
        }
    }
    if (key == GLFW_KEY_KP_3 && action==GLFW_PRESS)
    {
        if (play_dog_marai == false)
        {
            play_sound_marait_dog();
            play_dog_marai = true;
        }
        else {
            play_dog_marai = false;
            stop_sound();
            play_sound_village();
        }
        
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
                pressedKeys[key] = true;
            }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
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

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
    myBasicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
}
void sceneMode() {
    if (pressedKeys[GLFW_KEY_Z])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (pressedKeys[GLFW_KEY_X])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_C])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
}
void processMovement() {
    sceneMode();
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, 2 * cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, 2 * cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, 2 * cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, 2 * cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
}

void initOpenGLWindow() {
    myWindow.Create(1920, 768, "OpenGL Project Core");
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
    //teapot.LoadModel("models/teapot/teapot20segUT.obj");
    scena_statica.LoadModel("models/scena_statica/scena_statica.obj");
    piatra.LoadModel("models/scena_statica/piatra.obj");
    minge.LoadModel("models/minge/minge.obj");
    cal_caruta.LoadModel("models/cal_caruta/cal_caruta.obj");
    dog1_coada.LoadModel("models/dog1/dog1_coada.obj");
    dog1_corp.LoadModel("models/dog1/dog1_corp.obj");
    dog2.LoadModel("models/dog2/dog2.obj");
    felinar.LoadModel("models/felinar/felinar.obj");
    cos_cartofi.LoadModel("models/cos_cartofi/cos_cartofi.obj");
    batman_balloon.LoadModel("models/batman_balloon/batman_balloon.obj");

}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}
void initUniforms() {
    myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 2000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // set the light direction(direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
        // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.position"), 1, glm::value_ptr(lightfelinarPos));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.quadratic"), 0.0075f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.specular"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.color"), 1.0f, 0.0f, 0.0f);

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight1.position"), 1, glm::value_ptr(lightfelinarPos1));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight1.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight1.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight1.quadratic"), 0.0075f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight1.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight1.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight1.specular"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight1.color"), 0.0f, 0.0f, 1.0f);

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight2.position"), 1, glm::value_ptr(lightfelinarPos2));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight2.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight2.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight2.quadratic"), 0.0075f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight2.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight2.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight2.specular"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight2.color"), 0.0f, 1.0f, 1.0f);

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight3.position"), 1, glm::value_ptr(lightfelinarPos3));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight3.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight3.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight3.quadratic"), 0.0075f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight3.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight3.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight3.specular"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight3.color"), 0.0f, 1.0f, 0.0f);

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight4.position"), 1, glm::value_ptr(lightfelinarPos4));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight4.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight4.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight4.quadratic"), 0.0075f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight4.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight4.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight4.specular"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight4.color"), 1.0f, 1.0f, 0.0f);

    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activPointLight"), activPointLight);
    
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix\

    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 200.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

void initSkyBox()
{
    std::vector<const GLchar*> faces;
    faces.push_back("Skybox/right.tga");
    faces.push_back("Skybox/left.tga");
    faces.push_back("Skybox/top.tga");
    faces.push_back("Skybox/bottom.tga");
    faces.push_back("Skybox/back.tga");
    faces.push_back("Skybox/front.tga");
    mySkyBox.Load(faces);

}

void renderScena_Staica(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    scena_statica.Draw(shader);
    piatra.Draw(shader);
}
void render_dog1_coada(gps::Shader shader,float delta) {
    // select active shader program
    shader.useShaderProgram();
    //og1_coadaAngle += 15.0f*dog1_coadaRotationSpeed* delta;
    dog1_coadaAngle = glm::radians(120.0f * sin(dog1_coadaRotationSpeed * glfwGetTime()));

    modelDog1_coada = glm::mat4(1.0f);
    modelDog1_coada = glm::rotate(modelDog1_coada, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
    modelDog1_coada = translate(modelDog1_coada, dog1_coadaPos);
    modelDog1_coada = rotate(modelDog1_coada, glm::radians(dog1_coadaAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelDog1_coada = translate(modelDog1_coada, -dog1_coadaPos);
    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelDog1_coada));

    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    dog1_coada.Draw(shader);
}
void render_dog1_corp(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
   // glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    dog1_corp.Draw(shader);
}
void render_dog2(gps::Shader shader,float delta) {
    // select active shader program
    
    shader.useShaderProgram();

    modelDog2 = glm::mat4(1.0f);
    modelDog2 = glm::rotate(modelDog2, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
    modelDog2 = glm::translate(modelDog2, glm::vec3(dog2Pos_x, 0.0f, dog2Pos_y));

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelDog2));

    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    dog2.Draw(shader);
}
void render_cal_caruta(gps::Shader shader,float delta) {
    // select active shader program
    shader.useShaderProgram();
    //max=20
    // min=-10
    //cal_carutaAngle += cal_carutaRotationSpeed * delta;
    if ((cal_carutaHorizontalPos >= max_cal_caruta))//||(cal_carutaHorizontalPos <= min_cal_caruta))//|| cal_carutaHorizontalPos <= min_cal_caruta)
    {
        cal_carutaHorizontalSpeed = -cal_carutaHorizontalSpeed;
    }
    else if(cal_carutaHorizontalPos <= min_cal_caruta)
        cal_carutaHorizontalSpeed = -cal_carutaHorizontalSpeed;
    if (play_batman==false && ((cal_carutaHorizontalPos > 10 && cal_carutaHorizontalPos < 12) ||(cal_carutaHorizontalPos > -9 && cal_carutaHorizontalPos < -7)))
    {
        if (play_cal_nechezat == false)
        {
            play_sound_cal_nechezat();
            play_cal_nechezat = true;
        }
    }
    else if(play_cal_nechezat==true){
            play_cal_nechezat = false;
            stop_sound();
            play_sound_village();
            play_sound_cal_troptrop();
        }

    if (play_batman==false && cal_carutaHorizontalPos > 5 && cal_carutaHorizontalPos < 8)
    {
        if (play_cal_oftat == false)
        {
            play_sound_cal_oftat();
            play_cal_oftat = true;
        }
    }
    else if(play_cal_oftat==true) {
        play_cal_oftat = false;
        stop_sound();
        play_sound_village();
        play_sound_cal_troptrop();
    }
    

    cal_carutaHorizontalPos += cal_carutaHorizontalSpeed * delta;

    modelCal_Caruta = glm::mat4(1.0f);
    modelCal_Caruta = glm::rotate(modelCal_Caruta, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
    modelCal_Caruta = glm::translate(modelCal_Caruta, glm::vec3(0.0f, 0.0f, cal_carutaHorizontalPos));
    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCal_Caruta));
    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    cal_caruta.Draw(shader);
}
void render_minge(gps::Shader shader,float delta) {
    // select active shader program
    shader.useShaderProgram();

    mingeAngle += mingeRotationSpeed * delta;
    
    if (mingeVerticalPos >= maxHeight || mingeVerticalPos <= 0.0f) {
        mingeVerticalSpeed = -mingeVerticalSpeed;
    }
    mingeVerticalPos += mingeVerticalSpeed * delta;

    modelMinge = glm::mat4(1.0f);
    modelMinge = glm::rotate(modelMinge, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
    modelMinge = glm::translate(modelMinge, mingePos);
    modelMinge = glm::rotate(modelMinge, glm::radians(mingeAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMinge = glm::translate(modelMinge, glm::vec3(0.0f, mingeVerticalPos, 0.0f));
    modelMinge = glm::translate(modelMinge, -mingePos);

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMinge));

    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    minge.Draw(shader);
}
void renderFelinar(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    felinar.Draw(shader);
}
void renderCos_cartofi(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    cos_cartofi.Draw(shader);
}
void renderBatman_balloon(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();
    
    //send teapot model matrix data to shader
    modelBattman_balloon = glm::mat4(1.0f);
    modelBattman_balloon = glm::rotate(modelBattman_balloon, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
    modelBattman_balloon = glm::translate(modelBattman_balloon, glm::vec3(0.0f, balloonVerticalPos, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelBattman_balloon));
    //glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    batman_balloon.Draw(shader);
}
void animatie_camera()
{
    if (animationEnabled)
    {
        float cameraSpeed = 0.5f;

        // Durata unui ciclu complet în secunde (aici 20 de secunde)
        float cycleDuration = 20.0f;

        // Timpul curent în secunde
        float currentTime = fmod(glfwGetTime(), cycleDuration);

        // Coordonatele centrului elipsei
        float centerX = 1.41216f;
        float centerZ = 30.2075f;

        // Dimensiunile semiaxei mare și semiaxei mici ale elipsei
        float a = 60.0f;  // jumătate din lungimea elipsei
        float b = 50.0f;  // jumătate din lățimea elipsei

        // Unghiul de rotație în radiani
        float theta = glm::radians(45.0f);  // poți ajusta acest unghi

        // Calculul poziției camerei pe elipsă în funcție de timp și rotație
        float cameraX = centerX + a * cos(currentTime / cycleDuration * 2.0f * glm::pi<float>()) * cos(theta) -
            b * sin(currentTime / cycleDuration * 2.0f * glm::pi<float>()) * sin(theta);

        float cameraZ = centerZ + a * cos(currentTime / cycleDuration * 2.0f * glm::pi<float>()) * sin(theta) +
            b * sin(currentTime / cycleDuration * 2.0f * glm::pi<float>()) * cos(theta);

        myCamera.setPosition(glm::vec3(cameraX, 25.0f, cameraZ));
    }

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
}

void renderScene(float delta) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();

    if (play_background == true)
    {
        play_sound_village();
        play_background = false;
    }
	//render the scene
    animatie_camera();
	// render the teapot
	renderFelinar(myBasicShader);
    renderScena_Staica(myBasicShader);
    render_minge(myBasicShader,delta);
    render_cal_caruta(myBasicShader,delta);
    render_dog1_coada(myBasicShader,delta);
    render_dog2(myBasicShader,delta);
    render_dog1_corp(myBasicShader);
    renderBatman_balloon(myBasicShader);
    renderCos_cartofi(myBasicShader);
    mySkyBox.Draw(skyboxShader, view, projection);
}

void cleanup() {
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
	initUniforms();
    setWindowCallbacks();
    initSkyBox();
    initSoundEngine();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {

        static float lastTime = glfwGetTime();
        float currentTime = glfwGetTime();
        float deltaT = currentTime - lastTime;
        lastTime = currentTime;
        

        processMovement();
	    renderScene(deltaT);

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
