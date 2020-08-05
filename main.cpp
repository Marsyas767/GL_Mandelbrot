#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

float vertices[18] ={
    -1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
};

double xPos = 0.0f;
double yPos = 0.0f;
double zoom = 1.0f;
float camSpeed = 0.05f;

int rRes = 128; //Initial red resolution

const char* vertSrc = R""(
    #version 410 core
    in vec3 pos;
    out vec3 fPos;
    void main(){
        fPos = pos;
        gl_Position = vec4(pos, 1.0);
    }
)"";

const char* fragSrc = R""(
    #version 410 core
    in vec3 fPos;
    out vec4 outColor;
    uniform double xPos;
    uniform double yPos;
    uniform double zoom;
    uniform int rRes;
    float mandelbrot(vec2 coord, int iter){
        double x = coord.x/zoom;
        double y = coord.y/zoom;
        int n = 0;
        double zX = 0;
        double zY = 0;
        double cX = (x) + xPos;
        double cY = (y) + yPos;
        while((zX * zX + zY * zY) < 4 && n < iter){
            double tempX = zX * zX - zY * zY + cX;
            zY = 2*zX*zY + cY;
            zX = tempX;
            n+=1;
        }
        return n/iter;
    }
    void main(){
        vec2 tPos = vec2(fPos.x, fPos.y);
        float r = mandelbrot(tPos, rRes);
        float g = mandelbrot(tPos, rRes/4);
        float b = mandelbrot(tPos, rRes/16);
        if(b > 0 && r == 0 && g == 0){
            outColor = vec4(0.0, 0.0, b, 1.0);
        }else if(g > 0 && r == 0){
            outColor = vec4(0.0, g, 0.0, 1.0);
        }else{
            outColor = vec4(r, 0.0, 0.0, 1.0);
        }
    }
)"";


int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    GLFWwindow* gWindow = glfwCreateWindow(800, 800, "", NULL, NULL);
    glfwMakeContextCurrent(gWindow);

    gl3wInit();

    //add shaders

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertSrc, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout<<"Error::Shader::Vertex: "<<infoLog<<std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout<<"Error::Shader::Fragment: "<<infoLog<<std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout<<"Error::Shader::Program: "<<infoLog<<std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //draw screen surface

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    
    GLuint xPosLoc = glGetUniformLocation(shaderProgram, "xPos");
    GLuint yPosLoc = glGetUniformLocation(shaderProgram, "yPos");
    GLuint zoomLoc = glGetUniformLocation(shaderProgram, "zoom");
    GLuint rResLoc = glGetUniformLocation(shaderProgram, "rRes");

    while(!glfwWindowShouldClose(gWindow)){
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1d(xPosLoc, xPos);
        glUniform1d(yPosLoc, yPos);
        glUniform1d(zoomLoc, zoom);
        glUniform1i(rResLoc, rRes);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(gWindow);
        glfwPollEvents();
        camSpeed = 0.01/(zoom); //make sure that the camera movement changes with zoom
        if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE)){
            glfwSetWindowShouldClose(gWindow, GLFW_TRUE); //close window
        }
        //movement
        if(glfwGetKey(gWindow, GLFW_KEY_W)){
            yPos += camSpeed;
        }
        if(glfwGetKey(gWindow, GLFW_KEY_S)){
            yPos -= camSpeed;
        }
        if(glfwGetKey(gWindow, GLFW_KEY_A)){
            xPos -= camSpeed;
        }
        if(glfwGetKey(gWindow, GLFW_KEY_D)){
            xPos += camSpeed;
        }
        //zoom
        if(glfwGetKey(gWindow, GLFW_KEY_Z)){
            zoom *= 1.1f;
        }
        if(glfwGetKey(gWindow, GLFW_KEY_X)){
            zoom /= 1.1f;
        }
        //resolution
        if(glfwGetKey(gWindow, GLFW_KEY_UP)){
            rRes += 1;
            std::cout<<"Iterations: "<<rRes<<std::endl;
        }
        if(glfwGetKey(gWindow, GLFW_KEY_DOWN)){
            if(rRes > 2){
                rRes -= 1;
                std::cout<<"Iterations: "<<rRes<<std::endl;
            }
        }
    }
    glfwDestroyWindow(gWindow);
    glfwTerminate();
    return 0;
}
