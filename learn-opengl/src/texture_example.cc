#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <math.h>
#include <stdio.h>

static void glfw_error_callback(int error_code, const char *description);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static float vertices[] = {
    // 顶点坐标           颜色               纹理坐标
    -0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,     // 左上
     0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,     // 右上
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,     // 左下
     0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,     // 右下
};

static unsigned int indices[] = {
    0, 1, 2,    //  第一个三角形
    1, 2, 3,    // 第二个三角形
};

static const char *vertex_shader_source = R"(
    #version 330
    layout (location = 0) in vec3 vPos;
    layout (location = 1) in vec3 vColor;
    layout (location = 2) in vec2 vTexCoord;
    out vec3 Color;
    out vec2 TexCoord;
    void main()
    {
        Color = vColor;
        TexCoord = vTexCoord;
        gl_Position = vec4(vPos, 1.0f);
    }
)";

static const char *fragment_shader_source = R"(
    #version 330
    in vec3 Color;
    in vec2 TexCoord;
    out vec4 fColor;
    uniform sampler2D texture1;
    uniform sampler2D texture2;
    void main()
    {
        // fColor = texture(texture1, TexCoord) * vec4(Color, 1.0f);
        fColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.3);
    }
)";


int main(void)
{
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
    
    // Decide GL+GLSL versions
#if defined(__APPLE__)
    // GL 3.3 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);                          // Required on Mac
#endif

    // create GLFW window
    GLFWwindow *window = glfwCreateWindow(640, 480, "improfiler", nullptr, nullptr);
    if (window == nullptr) {
        printf("glfwCreateWindow() failed\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // load OpenGL functions
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        printf("gladLoadGLLoader() failed\n");
        return -1; 
    }

    // OpenGL
    int success;
    char info_log[512];

    GLuint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vertex_shader, sizeof(info_log), nullptr, info_log);
        printf("glCompileShader() failed, info: %s", info_log);
        glfwTerminate();
        return -1;
    }

    GLuint fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(fragment_shader, sizeof(info_log), nullptr, info_log);
        printf("glCompileShader() failed, info: %s", info_log);
        glfwTerminate();
        return -1;
    }

    GLuint program;
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
        printf("glLinkProgram() failed, info: %s", info_log);
        glfwTerminate();
        return -1;
    }
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    {
        stbi_set_flip_vertically_on_load(true);     // 垂直翻转图片
        int width, height, channels;
        unsigned char *data = stbi_load("./container.jpg", &width, &height, &channels, 0);
        if (data) {
            printf("image width:%d, height:%d, channels:%d\n", width, height, channels);
            // 制作纹理
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            printf("stbi_load() failed\n");
        }
        stbi_image_free(data);
    }


    GLuint texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    {
        stbi_set_flip_vertically_on_load(true);     // 垂直翻转图片
        int width, height, channels;
        unsigned char *data = stbi_load("./awesomeface.png", &width, &height, &channels, 0);
        if (data) {
            printf("image width:%d, height:%d, channels:%d\n", width, height, channels);
            // 制作纹理
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            printf("stbi_load() failed\n");
        }
        stbi_image_free(data);
    }

    
    // render loop
    while (glfwWindowShouldClose(window) == false) {
        // poll and handle events
        glfwPollEvents();

        // 每次绘制前需要清屏，否则可能遗留的上一次绘制的内容
        // glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glUseProgram(program);
        glBindVertexArray(VAO);
        glUniform1i(glGetUniformLocation(program, "texture1"), 0);  // 设置纹理单元0
        glUniform1i(glGetUniformLocation(program, "texture2"), 1);  // 设置纹理单元1
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    // cleanup
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(program);
    glfwTerminate();

    return 0;
}

static void glfw_error_callback(int error_code, const char *description) {
    printf("GLFW ERROR - error_code:%d, description:%s", error_code, description);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}