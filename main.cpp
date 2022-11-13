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
float rot_x = 0, rot_y = 1.f, rot_z = 0;
float theta_x = 0, theta_y = 90.0f, theta_z = 0;

/* Requirement: Camera Class */
class Camera {
public:
    glm::mat4 projection;

    glm::mat4 getProjection() {
        return projection;
    }
};

// orthographic camera class
class OrthoCamera : public Camera {
public:
    OrthoCamera() {
        // old code used from previous implementation
        // TODO: view from top
        projection = glm::ortho(-1.0f,
            1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            2.0f);
    }
};

// perspective camera class
class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera() {
        // old code used from previous implementation
        projection = glm::perspective(
            glm::radians(60.0f),
            window_height / window_width,
            0.1f,
            100.0f
        );
    }
};

/* Create Camera Classes */
OrthoCamera camera1;
PerspectiveCamera camera2;

// int cameranum
int cameraNum = 2;

/* Requirement Light Class for light Sources */
class Light {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

// direction  light
class DirLight : public Light {
public:
    glm::vec3 direction;

    void assignlightDir(glm::vec3 newDir) {
        direction = newDir;
    }

    glm::vec3 getlightDir() {
        return direction;
    }
};

// point light
class PointLight : public Light {
public:
    float constant;
    float linear;
    float quadratic;

    void assignNums(float cons, float lin, float quad) {
        constant = cons;
        linear = lin;
        quadratic = quadratic;
    }

    float getConstant() {
        return constant;
    }

    float getLinear() {
        return linear;
    }

    float getQuadratic() {
        return quadratic;
    }
};

// make the classes
PointLight pointLight;
DirLight dirLight;

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    if (key == GLFW_KEY_W &&
        action == GLFW_REPEAT) {
        theta_x -= 2.f;
    }

    if (key == GLFW_KEY_A &&
        action == GLFW_REPEAT) {
        theta_y -= 2.f;
    }

    if (key == GLFW_KEY_S &&
        action == GLFW_REPEAT) {
        theta_x += 2.f;
    }

    // when user presses D
    if (key == GLFW_KEY_D &&
        action == GLFW_REPEAT) {
        theta_y += 2.f;
    }

    if (key == GLFW_KEY_Q &&
        action == GLFW_REPEAT) {
        theta_z -= 2.f;
    }

    if (key == GLFW_KEY_E &&
        action == GLFW_REPEAT) {
        theta_z += 2.f;
    }

    /* Requirement: Swap Cameras */
    if (key == GLFW_KEY_1 &&
        action == GLFW_PRESS) {
        cameraNum = 1;
    }

    /* Requirement: Swap Cameras */
    if (key == GLFW_KEY_2 &&
        action == GLFW_PRESS) {
        cameraNum = 2;
    }

}

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
        stbi_load("3D/ayaya.png",
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
        GL_RGBA,
        img_width,
        img_height,
        0,
        GL_RGBA,
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
    std::string path = "3D/guitar.obj";
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

    GLuint VAO, VBO, VAO2, VBO2;
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

    // 2nd obj (light source)
    std::string path2 = "3D/lamp.obj";
    std::vector<tinyobj::shape_t> shapes2;
    std::vector<tinyobj::material_t> material2;
    std::string warning2, error2;
    tinyobj::attrib_t attributes2;

    /* Load the Mesh */
    bool success2 = tinyobj::LoadObj(&attributes2,
        &shapes2,
        &material2,
        &warning2,
        &error2,
        path2.c_str());

    std::vector<GLuint> mesh_indices2;
    for (int i = 0; i < shapes2[0].mesh.indices.size(); i++) {
        mesh_indices2.push_back(shapes2[0].mesh.indices[i].vertex_index);
    }

    std::vector<GLfloat> fullVertexData2;
    for (int i = 0; i < shapes2[0].mesh.indices.size(); i++) {

        tinyobj::index_t vData = shapes2[0].mesh.indices[i];

        int vertexIndex = vData.vertex_index * 3;
        int uvIndex = vData.texcoord_index * 2;
        int normsIndex = vData.normal_index * 3;

        // X
        fullVertexData2.push_back(
            attributes2.vertices[vertexIndex]
        );

        // Y
        fullVertexData2.push_back(
            attributes2.vertices[vertexIndex + 1]
        );

        // Z
        fullVertexData2.push_back(
            attributes2.vertices[vertexIndex + 2]
        );

        // normals
        fullVertexData2.push_back(
            attributes2.normals[normsIndex]
        );

        fullVertexData2.push_back(
            attributes2.normals[normsIndex + 1]
        );

        fullVertexData2.push_back(
            attributes2.normals[normsIndex + 2]
        );

        // U
        fullVertexData2.push_back(
            attributes2.texcoords[uvIndex]
        );

        // V
        fullVertexData2.push_back(
            attributes2.texcoords[uvIndex + 1]
        );

    }

    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData2.size(),
        fullVertexData2.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0);

    // norm ptr
    GLintptr normPtr2 = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr2
    );

    // uv ptr
    GLintptr uvPtr2 = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr2
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
    y = -0.25f;

    glm::mat4 translation =
        glm::translate(identity_matrix4,
            glm::vec3(x, y, z));

    float scale_x, scale_y, scale_z;
    scale_x = scale_y = scale_z = 1.0f;

    glm::mat4 scale =
        glm::scale(identity_matrix4,
            glm::vec3(scale_x, scale_y, scale_z));

    

    glm::mat4 rotation =
        glm::rotate(identity_matrix4,
            glm::radians(theta_x),
            glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));

    // perspective camera
    glm::mat4 projection = glm::perspective(
        glm::radians(60.0f),
        window_height / window_width,
        0.1f,
        100.0f
    );

    // current projection set to perspective
    projection = camera2.getProjection();

    /* Lighting Variables */
    glm::vec3 lightPos; /*= glm::vec3(-10, 3, 0); */
    glm::vec3 lightColor = glm::vec3(1, 1, 1);

    float ambientStr = 0.1f;
    glm::vec3 ambientColor = lightColor;

    float specStr = 0.5f;
    float specPhong = 16.0f;

    // direction light
    dirLight.assignlightDir(glm::vec3(-4, 11, -3));

    // point light
    pointLight.assignNums(1.f, 0.045f, 0.0075f);

    // other variables
    int objectNum;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::vec3 cameraPos;

        // switch statement to switch between cameras
        switch (cameraNum) {
        case 1: projection = camera1.getProjection(); cameraPos = glm::vec3(0.f, 1.f, 0.5f); break;
        case 2: projection = camera2.getProjection(); cameraPos = glm::vec3(0.f, 0.f, 1.f);  break;
        }

        objectNum = 1;

        /* Camera */
        // camera position
        //glm::vec3 cameraPos = glm::vec3(0.f, 1.f, 0.5f);
        //glm::vec3 cameraPos = glm::vec3(0.f, 0.f, 1.f);

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
            glm::vec3(x, y, z));

        // scale
        transformation_matrix = glm::scale(transformation_matrix,
            glm::vec3(scale_x, scale_y, scale_z));

        glm::vec3 xRot(1.f, 0, 0);
        glm::vec3 yRot(0, 1.f, 0);
        glm::vec3 zRot(0, 0, 1.f);

        // rotate
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta_x),
            glm::normalize(xRot));

        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta_y),
            glm::normalize(yRot));

        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta_z),
            glm::normalize(zRot));


        GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0Address, 0);

        lightPos = glm::vec3(x + 0.4f, y + 0.6f, z + 0.1f);

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

        // directional light
        unsigned int dirLightAddress = glGetUniformLocation(shaderProg, "dirLight");
        glUniform3fv(dirLightAddress,
            1,
            glm::value_ptr(dirLight.getlightDir()));

        // point light
        unsigned int consLightAddress = glGetUniformLocation(shaderProg, "constant");
        glUniform1f(consLightAddress, pointLight.getConstant());

        unsigned int linLightAddress = glGetUniformLocation(shaderProg, "linear");
        glUniform1f(linLightAddress, pointLight.getLinear());

        unsigned int quadLightAddress = glGetUniformLocation(shaderProg, "quadratic");
        glUniform1f(quadLightAddress, pointLight.getQuadratic());

        unsigned int objNumAddress = glGetUniformLocation(shaderProg, "objNum");
        glUniform1i(objNumAddress, objectNum);
            
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

        // 2nd object load
        glBindVertexArray(VAO2);

        objectNum = 2;

        transformation_matrix = glm::mat4(1.0f);

        // translation
        transformation_matrix = glm::translate(transformation_matrix,
            glm::vec3(x + 0.4f, y + 0.5f, z - 0.4f));

        // scale
        transformation_matrix = glm::scale(transformation_matrix,
            glm::vec3(0.01f, 0.01f, 0.01f));

        // rotate
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(90.f),
            glm::normalize(glm::vec3(0, 1.f, 0)));

        //glBindTexture(GL_TEXTURE_2D, texture);
        //glUniform1i(tex0Address, 0);

        glUniform1i(objNumAddress, objectNum);

        unsigned int colorAddress = glGetUniformLocation(shaderProg, "color");
        glUniform3fv(colorAddress,
            1,
            glm::value_ptr(glm::vec3(1, 1, 1)));

        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(transformation_matrix));

        glDrawArrays(GL_TRIANGLES, 0, fullVertexData2.size() / 8);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);

    glfwTerminate();
    return 0;
}