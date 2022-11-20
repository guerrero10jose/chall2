#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// tinyloader
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// textures
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* Global Variables */
float window_height = 600.0f, window_width = 600.0f;
float x_mod = 0;

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    // when user presses D
    if (key == GLFW_KEY_D &&
        action == GLFW_PRESS) {
        // move bunny to the right
        x_mod += 10.0f;
    }
}

/* Requirement: Camera Class */
class Camera {

};

// orthographic camera class
class Ortho: public Camera {

};

// perspective camera class
class Persp : public Camera {

    // old code used for previous implementation
    glm::mat4 projection = glm::ortho(-2.0f,
        2.0f,
        -2.0f,
        2.0f,
        -1.0f,
        1.0f);
};

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /* Initialize GLAD */
    gladLoadGL();

    int img_width,
        img_height,
        colorChannels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* tex_bytes =
        stbi_load("3D/partenza.jpg",
            &img_width,
            &img_height,
            &colorChannels,
            0);

    GLuint texture;

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width,
        img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    // depth testing
    glEnable(GL_DEPTH_TEST);

    /* Screen Space */
    // Should be same size as window
    //glViewport(0, 0, 640, 480);

    /* Set Callback function */
    glfwSetKeyCallback(window, Key_Callback);

    /* Load Vertex/Fragment Shaders*/
    // vertex shader
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    // fragment shader
    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    // compile
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);


    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    // finalize
    glLinkProgram(shaderProg);

    /* Initialize Mesh Stuff*/
    std::string path = "3D/djSword.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;
    tinyobj::attrib_t attributes;

    GLfloat UV[]{
    0.f, 1.f,
    0.f, 0.f,
    1.f, 1.f,
    1.f, 0.f,
    1.f, 1.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 0.f
    };

    /* Load the Mesh */
    bool success = tinyobj::LoadObj(&attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str());

    std::vector<GLuint> mesh_indices;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }

    std::vector<GLfloat> fullVertexData;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {

        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        int vertexIndex = vData.vertex_index * 3;
        int uvIndex = vData.texcoord_index * 2;
        int normsIndex = vData.normal_index * 3;

        // X
        fullVertexData.push_back(
            attributes.vertices[vertexIndex]
        );

        // Y
        fullVertexData.push_back(
            attributes.vertices[vertexIndex + 1]
        );

        // Z
        fullVertexData.push_back(
            attributes.vertices[vertexIndex + 2]
        );

        // normals
        fullVertexData.push_back(
            attributes.normals[normsIndex]
        );

        fullVertexData.push_back(
            attributes.normals[normsIndex + 1]
        );

        fullVertexData.push_back(
            attributes.normals[normsIndex + 2]
        );

        // U
        fullVertexData.push_back(
            attributes.texcoords[uvIndex]
        );

        // V
        fullVertexData.push_back(
            attributes.texcoords[uvIndex + 1]
        );

    }

    GLfloat vertices[]{
        -0.5f, -0.5f, 0,
        0, 0.5f, 0,
        0.5, -0.5f, 0
    };

    GLuint indices[]{
        0, 1, 2
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData.size(),
        fullVertexData.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0);

    // norm ptr
    GLintptr normPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr
    );

    // uv ptr
    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Create Matrix */
    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);

    float x, y, z;
    x = y = z = 0.0f;
    y = -0.5f;

    glm::mat4 translation =
        glm::translate(identity_matrix4,
            glm::vec3(x, y, z));

    float scale_x, scale_y, scale_z;
    scale_x = scale_y = scale_z = 0.1f;

    glm::mat4 scale =
        glm::scale(identity_matrix4,
            glm::vec3(scale_x, scale_y, scale_z));

    float rot_x, rot_y, rot_z;
    rot_x = rot_y = rot_z = 0;
    rot_y = 1.0f;

    float theta = 90.0f;

    glm::mat4 rotation =
        glm::rotate(identity_matrix4,
            glm::radians(theta),
            glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));

    // perspective camera
    glm::mat4 projection = glm::perspective(
        glm::radians(60.0f),
        window_height / window_width,
        0.1f,
        100.0f
    );

    /* Lighting Variables */
    glm::vec3 lightPos = glm::vec3(-10, 3, 0);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);

    float ambientStr = 0.1f;
    glm::vec3 ambientColor = lightColor;

    float specStr = 0.5f;
    float specPhong = 16.0f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        theta += 0.1f;

        /* Camera */
        // camera position
        glm::vec3 cameraPos = glm::vec3(0, 0, 10.f);

        glm::mat4 cameraPositionMatrix =
            glm::translate(glm::mat4(1.0f),
                cameraPos * -1.0f);

        // world's up / center
        glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
        glm::vec3 Center = glm::vec3(0, 0.0f, 0);

        // forward
        glm::vec3 F = glm::vec3(Center - cameraPos);
        F = glm::normalize(F);

        // right
        glm::vec3 R = glm::cross(F, WorldUp);

        // up
        glm::vec3 U = glm::cross(R, F);

        // orientation matrix
        glm::mat4 cameraOrientation = glm::mat4(1.0f);

        cameraOrientation[0][0] = R.x;
        cameraOrientation[1][0] = R.y;
        cameraOrientation[2][0] = R.z;

        cameraOrientation[0][1] = U.x;
        cameraOrientation[1][1] = U.y;
        cameraOrientation[2][1] = U.z;

        cameraOrientation[0][2] = -F.x;
        cameraOrientation[1][2] = -F.y;
        cameraOrientation[2][2] = -F.z;

        //glm::mat4 viewMatrix = cameraOrientation * cameraPositionMatrix;
        glm::mat4 viewMatrix = glm::lookAt(cameraPos, Center, WorldUp);

        glm::mat4 transformation_matrix = glm::mat4(1.0f);

        // translation
        transformation_matrix = glm::translate(transformation_matrix,
            glm::vec3(x, y, z -5.f));

        // scale
        transformation_matrix = glm::scale(transformation_matrix,
            glm::vec3(scale_x, scale_y, scale_z));

        // rotate
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));

        GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0Address, 0);


        // diffuse stuff
        unsigned int lightAddress = glGetUniformLocation(shaderProg, "lightPos");
        glUniform3fv(lightAddress,
            1,
            glm::value_ptr(lightPos));

        unsigned int lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
        glUniform3fv(lightColorAddress,
            1,
            glm::value_ptr(lightColor));

        // ambient stuff
        unsigned int ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        unsigned int ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
        glUniform3fv(ambientColorAddress,
            1,
            glm::value_ptr(ambientColor));

        // specphong stuff
        unsigned int cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
        glUniform3fv(cameraPosAddress,
            1,
            glm::value_ptr(cameraPos));

        unsigned int specStrAddress = glGetUniformLocation(shaderProg, "specStr");
        glUniform1f(specStrAddress, specStr);

        unsigned int specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
        glUniform1f(specPhongAddress, specPhong);
            
        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection));

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(viewMatrix));

        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(transformation_matrix));

        /* Apply Shaders */
        glUseProgram(shaderProg);

        //glBindVertexArray(VAO);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}