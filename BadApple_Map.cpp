#include <iostream>
#include <glad43/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <UtilLibary/WindowsUtil.h>
#include <UtilLibary/Shader.h>
#include <UtilLibary/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <UtilLibary/Camera.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "opencv2/video.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <windows.h>
#include <Mmsystem.h>
#include <mciapi.h>
#pragma comment(lib, "Winmm.lib")
using namespace std;

const int VIEW_PORT_WIDTH = 1920;
const int VIEW_PORT_HEIGHT = 1080;
glm::vec3 ligthPosition = glm::vec3(0.0f, 300.0f, 3.f);
Camera* camera = new Camera(glm::vec3(0.0f, 310.0f, -300.f), 80);

WindowsUtil* windowUtil = new WindowsUtil();

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool close = false;


struct Transform
{
    /*SPACE INFORMATION*/
    //Local space information
    glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
    glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    //Global space information concatenate in matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

enum RenderMode {
    wireframeMode,
    pointMode,
    fillMode
};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    windowUtil->mouse_callback(window, xpos, ypos, camera);
}
using namespace cv;

void extract_frames(const string& videoFilePath, vector<Mat>& frames) {

    try {
        //open the video file
        VideoCapture cap(videoFilePath); // open the video file
        if (!cap.isOpened())  // check if we succeeded
            CV_Error(-2, "Can not open Video file");

        //cap.get(CV_CAP_PROP_FRAME_COUNT) contains the number of frames in the video;
        for (int frameNum = 0; frameNum < cap.get(CAP_PROP_FRAME_COUNT); frameNum++)
        {
            Mat frame;
            cap >> frame; // get the next frame from video
            frames.push_back(frame);
        }
    }
    catch (cv::Exception& e) {
        cerr << e.msg << endl;
        exit(1);
    }

}

void save_frames(vector<Mat>& frames, const string& outputDir) {
    vector<int> compression_params;
    compression_params.push_back(1);
    compression_params.push_back(100);
    int frameNumber = 0;
    for (Mat frame : frames) {
        string filePath = outputDir + to_string(static_cast<long long>(frameNumber)) + ".jpg";
        imwrite(filePath, frame, compression_params);
        frameNumber++;
    }
}

int main1(int argc, char** argv)
{
    
    return 0;
}

int main()
{
    //window
    GLFWwindow* window = windowUtil->InitWindowV4(VIEW_PORT_WIDTH, VIEW_PORT_HEIGHT, "dProxy_Bad_Apple_Map", NULL, NULL);
    glEnable(GL_DEPTH_TEST);


    Shader* framesShaders = new Shader("./frameShader.vs", "./frameShader.frag");
    Shader* mapTessShader = new Shader("./mapShader.vs", "./mapShader.frag",
        "./mapShader.tcs", "./mapShader.tes");

    //user input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //render mode
    RenderMode renderMode = fillMode;

    //load the bad apple frames
    GLuint* textures = new GLuint[6572];
    glGenTextures(6572, textures);
    int width, height, nChannels;
    for (int i = 0; i < 6572 /*number of frames*/; i++) {
        //configure the texture
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //loag texture
        string path = "C:\\Users\\user\\Desktop\\Patio\\codigos\\BadApple_Map\\BadAppleFrames\\BadAppleFrames";
        path.append(to_string(static_cast<long long>(i)) + ".jpg");
        unsigned char* data =  stbi_load(path.c_str(),
        &width, &height, &nChannels,
        0);

        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            cout << "bad bad apple :(... something went wrong with the frames..." << endl;
        }
        // free data and unbind texture
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    framesShaders->use();
    framesShaders->setVec2("u_resolution", glm::vec2(width, height));

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    //frame buffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, VIEW_PORT_WIDTH, VIEW_PORT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);


    //create the render buffer object for the depth testing
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, VIEW_PORT_WIDTH, VIEW_PORT_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    vector<float> vertices;
    unsigned int rez = 20;
    for (unsigned int i = 0; i <= rez - 1; i++)
    {
        for (unsigned int j = 0; j <= rez - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v
        }
    }
    std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
    std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;


    unsigned int terrainVAO, terrainVBO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    mapTessShader->setInt("heightMap", 0);
    mapTessShader->setFloat("uTexelSize", (1 / width));
    int frameCount = 0;
    float fps = 30.0f;
    float frameDelay = 1000.0f / fps;
    float frameTime = 0.0f;
    mciSendString("play badapple.mp3", NULL, 0, NULL);
    //render loop
    while (!close)
    {
        float currentFrame = glfwGetTime();
        //hot reaload
        if (glfwGetKey(window, GLFW_KEY_R)) {
            mapTessShader->reloadTellShaderProgram("./mapShader.vs", "./mapShader.frag",
                "./mapShader.tcs", "./mapShader.tes");
            mapTessShader->setInt("heightMap", 0);
            mapTessShader->setFloat("uTexelSize", (1 / width));
            framesShaders->reloadShaderProgram("./frameShader.vs", "./frameShader.frag");
            framesShaders->use();
            framesShaders->setVec2("u_resolution", glm::vec2(width, height));
        }
        close = windowUtil->processInput(window, camera, deltaTime);

        if (frameCount != 6572);
        {
            frameCount++;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            renderMode = wireframeMode;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            renderMode = pointMode;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            renderMode = fillMode;
        }

        switch (renderMode) {
        case fillMode:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el modo wireframe
            break;
        case pointMode:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // el modo wireframe
            break;
        case wireframeMode:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // el modo wireframe
            break;
        default:
            break;
        }
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        framesShaders->use();

        framesShaders->use();
        framesShaders->setFloat("u_time", currentFrame);
        glBindTexture(GL_TEXTURE_2D, textures[frameCount]);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);

        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        mapTessShader->use();
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)VIEW_PORT_WIDTH / (float)VIEW_PORT_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera->GetViewMatrix();
        mapTessShader->setMat4("projection", projection);
        mapTessShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        mapTessShader->setMat4("model", model);
        mapTessShader->setVec3("LigthPos", ligthPosition);

        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);

        /*framesShaders->use();
        glBindTexture(GL_TEXTURE_2D, textures[1691]);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);*/
        glfwSwapBuffers(window);
        glfwPollEvents();
        frameTime = (glfwGetTime() * 1000.0f) - (currentFrame * 1000.0f);
        if (frameDelay > frameTime) {
            Sleep(frameDelay - frameTime);
        }
    }

    return 0;
}
