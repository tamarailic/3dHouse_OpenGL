// Autor: Nedeljko Tesanovic
// Opis: Testiranje dubine, Uklanjanje lica, Transformacije, Prostori i Projekcije

#define _CRT_SECURE_NO_WARNINGS
 
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

//GLM biblioteke
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "model.hpp"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);
float fov = 45.0f;

int main(void)
{
   
    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 800;
    unsigned int wHeight = 800;
    const char wTitle[] = "[Zuta kuca 3D]";
    
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    
    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }


    
  
    unsigned int stride = (3 + 3) * sizeof(float); 
    unsigned int stride_texture = (2 + 2) * sizeof(float);


    Model kuca("res/cela_kuca_1.obj");
    Model prozori("res/prozori.obj");
    Model ograda("res/fence.obj");
    Model pas("res/pas.obj");
    Model vrata_ograde("res/fence_door.obj");
    Model cikica("res/cikica.obj");
    Model sljiva("res/sljiva.obj");
    Model sunce("res/sunce.obj");
    Model trava("res/trava.obj");
    Model dim("res/dim.obj");

    Shader unifiedShader("basic.vert", "basic.frag");
    Shader sunceShader("basic.vert", "sun.frag");

    unifiedShader.use();
    unifiedShader.setVec3("viewPos", 0, 0.5, 3);
    unifiedShader.setBool("transparent", false);
    glm::mat4 projectionM = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
    unifiedShader.setMat4("uP", projectionM);
    glm::mat4 viewM = glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    unifiedShader.setMat4("uV", viewM);
    glm::mat4 modelM = glm::mat4(1.0f);
    unifiedShader.setMat4("uM", modelM);

    sunceShader.use();
    sunceShader.setMat4("uP", projectionM);
    sunceShader.setMat4("uV", viewM);
    sunceShader.setMat4("uM", modelM);
    sunceShader.setVec3("uCol", 1.0f,1.0f,0.0f);
   


    // Lights
    // direction
    unifiedShader.use();
    unifiedShader.setVec3("dirLight.direction", 0.6f, -0.7f, -0.3f);
    unifiedShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
    unifiedShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    unifiedShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    //point
    glm::vec3 pointLightPositions[] = {
         glm::vec3(0.0f, 0.14f, -0.01f),
      glm::vec3(0.0f, 0.4f, -0.01f),
    };

    unifiedShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    unifiedShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    unifiedShader.setVec3("pointLights[0].diffuse", 0.4f, 0.4f, 0.4f);
    unifiedShader.setVec3("pointLights[0].specular", 0.1f, 0.1f, 0.1f);
    unifiedShader.setFloat("pointLights[0].constant", 1.0f);
    unifiedShader.setFloat("pointLights[0].linear", 0.7f);
    unifiedShader.setFloat("pointLights[0].quadratic", 1.8f);
    unifiedShader.setBool("turnedOn[0]", false);

    unifiedShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    unifiedShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    unifiedShader.setVec3("pointLights[1].diffuse", 0.4f, 0.4f, 0.4f);
    unifiedShader.setVec3("pointLights[1].specular", 0.1f, 0.1f, 0.1f);
    unifiedShader.setFloat("pointLights[1].constant", 1.0f);
    unifiedShader.setFloat("pointLights[1].linear", 0.7f);
    unifiedShader.setFloat("pointLights[1].quadratic", 1.8f);
    unifiedShader.setBool("turnedOn[1]", false);

    unifiedShader.setVec3("pointLights[2].position", glm::vec3(0.15f, 0.0f, 0.0f));
    unifiedShader.setVec3("pointLights[2].ambient", 0.2f, 0.2f, 0.2f);
    unifiedShader.setVec3("pointLights[2].diffuse", 0.5f, 0.5f, 0.5f);
    unifiedShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    unifiedShader.setFloat("pointLights[2].constant", 1.0f);
    unifiedShader.setFloat("pointLights[2].linear", 0.09f);
    unifiedShader.setFloat("pointLights[2].quadratic", 0.032f);
    unifiedShader.setBool("turnedOn[2]", true);

    // spot
    unifiedShader.setVec3("spotLight.position", 0.0f, 0.12,0.2);
    unifiedShader.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
    unifiedShader.setFloat("spotLight.cutOff", 10.0f);
    unifiedShader.setFloat("spotLight.outerCutOff", 15.0f);
    unifiedShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
    unifiedShader.setVec3("spotLight.diffuse", 0.9f, 0.9f, 0.9f);
    unifiedShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    unifiedShader.setFloat("spotLight.constant", 1.0f);
    unifiedShader.setFloat("spotLight.linear", 0.014f);
    unifiedShader.setFloat("spotLight.quadratic", 0.0007f);

    unsigned int textureShader = createShader("texture.vert", "texture.frag");

    float name[] = {
         -1, -0.8,    0.0, 1.0,
         -1, -1,    0.0, 0.0,
         -0.6, -1,       1.0, 0.0,
          -0.6, -1,       1.0, 0.0,
           -0.6, -0.8,       1.0, 1.0,
         -1, -0.8,    0.0, 1.0,
    };

    unsigned int VAO[1];
    glGenVertexArrays(1, VAO);
    glBindVertexArray(VAO[0]);

    unsigned int VBO[1];
    glGenBuffers(1, VBO);

    unsigned nameTexture = loadImageToTexture("res/ime.png");
    glBindTexture(GL_TEXTURE_2D, nameTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLocName = glGetUniformLocation(textureShader, "uTex");
    glUniform1i(uTexLocName, 0);
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(name), name, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride_texture, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride_texture, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glClearColor(135.0f/255, 206.0f/255, 250.0f/255, 1.0);
    //glClearColor(0.5f,0.5f,0.5f,1.0);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    float opened_door = 0.0;
    glm::mat4 modelD = glm::mat4(1.0f);
    glm::vec3 dogPos = glm::vec3(0.15f, 0.0f, 0.0f);
    bool transparent = false;
    int side = 1;
    float G = 215.0;
    float G_step = 0.03;
    float dim_translate = 0.0130887;
    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            transparent = true;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
            transparent = false;
            glDisable(GL_BLEND);
        }
        
        unifiedShader.use();
        if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        {
            fov -= (float)0.01;
            if (fov < -90.0f)
                fov = -90.0f;

            glm::mat4 projectionM = glm::perspective(glm::radians(fov), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
            unifiedShader.use();
            unifiedShader.setMat4("uP", projectionM);
        }

        if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        {
            fov += (float)0.01;
            if (fov > 180.0f)
                fov = 180.0f;

            glm::mat4 projectionM = glm::perspective(glm::radians(fov), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
            unifiedShader.use();
            unifiedShader.setMat4("uP", projectionM);
            
        }
       
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            unifiedShader.use();
            unifiedShader.setBool("turnedOn[1]", true);          
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            unifiedShader.use();
            unifiedShader.setBool("turnedOn[1]", false);
        }

        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        {
            unifiedShader.use();
            unifiedShader.setBool("turnedOn[0]", true);
        }

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        {
            unifiedShader.use();
            unifiedShader.setBool("turnedOn[0]", false);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
             modelM = glm::rotate(modelM, glm::radians(-0.05f), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotiranje (Matrica transformacije, ugao rotacije u radijanima, osa rotacije)

             unifiedShader.use();
            unifiedShader.setMat4("uM", modelM);
            unifiedShader.setVec3("spotLight.position",modelM * glm::vec4( 0.0f, 0.12, 0.2,1));
           
      }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            modelM = glm::rotate(modelM, glm::radians(0.05f), glm::vec3(0.0f, 1.0f, 0.0f));

            unifiedShader.use();
            unifiedShader.setMat4("uM", modelM);
            unifiedShader.setVec3("spotLight.position", modelM * glm::vec4(0.0f, 0.12, 0.2, 1));
            
        }
        //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        //{
        //    modelM = glm::rotate(modelM, glm::radians(-0.05f), glm::vec3(1.0f, 0.0f, 0.0f)); //Rotiranje (Matrica transformacije, ugao rotacije u radijanima, osa rotacije)
        //    unifiedShader.use();
        //    unifiedShader.setMat4("uM", modelM);
        //    unifiedShader.setVec3("spotLight.position", modelM * glm::vec4(0.0f, 0.12, 0.2, 1));
        //}
        //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        //{
        //    modelM = glm::rotate(modelM, glm::radians(0.05f), glm::vec3(1.0f, 0.0f, 0.0f));
        //    unifiedShader.use();
        //    unifiedShader.setMat4("uM", modelM);
        //    unifiedShader.setVec3("spotLight.position", modelM * glm::vec4(0.0f, 0.12, 0.2, 1));
        //}

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            if (opened_door > -90) {
                opened_door -= 0.05f;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            if (opened_door < -0.05) {
                opened_door += 0.05f;
            }
        }

        unifiedShader.use();
        unifiedShader.setBool("transparent", false);
        unifiedShader.setMat4("uM", modelM);
        glDisable(GL_CULL_FACE);
        kuca.Draw(unifiedShader);
        glEnable(GL_CULL_FACE);
        cikica.Draw(unifiedShader);
        ograda.Draw(unifiedShader);
        sljiva.Draw(unifiedShader);
        trava.Draw(unifiedShader);

        unifiedShader.setBool("transparent", transparent);
        prozori.Draw(unifiedShader);
        unifiedShader.setBool("transparent", false);
       
        modelD = glm::translate(modelM, glm::vec3(-0.041361, 0.0, 0.405781));
        modelD = glm::rotate(modelD, glm::radians(opened_door), glm::vec3(0.0f, 1.0f, 0.0f));
        modelD = glm::translate(modelD, glm::vec3(0.041361, 0.0, -0.405781));
        unifiedShader.setMat4("uM", modelD);
        vrata_ograde.Draw(unifiedShader);

        dim_translate += 0.0001f;
        if (dim_translate > 0.1) {
            dim_translate = 0.0130887;
        }
        modelD = glm::translate(modelM, glm::vec3(0.013755, dim_translate, -0.005044));
        unifiedShader.setMat4("uM", modelD);
        //unifiedShader.setBool("transparent", true);
        dim.Draw(unifiedShader);
        //unifiedShader.setBool("transparent", false);

      if (dogPos.x >= 0.33f && dogPos.z >= 0.0f) {
            side = 2;
        }
        if (dogPos.z <= -0.5 && dogPos.x >= 0.33) {
            side = 3;
        }
        if (dogPos.x <= 0.0 && dogPos.z <= -0.5) {
            side = 4;
        }
        if (dogPos.x <= 0.0 && dogPos.z >= 0.0) {
            side = 1;
        }

        
        if (side == 1) {
            dogPos.x += 0.00091f;
        }
        if (side == 2) {
            dogPos.z -= 0.00091f;
        }
        if (side == 3) {
            dogPos.x -= 0.00091f;
        }
        if (side == 4) {
            dogPos.z += 0.00091f;
        }
        modelD = glm::translate(modelM, dogPos);
        unifiedShader.setMat4("uM", modelD);
        unifiedShader.setVec3("pointLights[2].position", dogPos);
        pas.Draw(unifiedShader);

        if (G >= 215 || (G <= 140 && G_step < 0)) {
            G_step *= -1;
        }

        G += G_step;
        sunceShader.use();
        glm::mat4 modelS = glm::translate(modelM, glm::vec3(-0.6f, 0.7f, 0.3f));
        modelS = glm::rotate(modelS, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        modelS = glm::translate(modelS, glm::vec3(0.6f, -0.7f, -0.3f));
        sunceShader.setMat4("uM", modelS);
        glm::mat4 projectionM = glm::perspective(glm::radians(fov), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
        sunceShader.setMat4("uP", projectionM);
        sunceShader.setVec3("uCol", 255, G / 255.0, 0);
        sunce.Draw(sunceShader);

        glUseProgram(textureShader);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glBindVertexArray(VAO[0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, nameTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    glDeleteProgram(textureShader);
    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}