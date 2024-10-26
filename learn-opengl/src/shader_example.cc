#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>

static void glfw_error_callback(int error, const char* description);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static float vertices[] = {
    // 顶点位置          颜色
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,       // 左下角
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,       // 右下角
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f,       // 右上角
    -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f,       // 左上角
};

static unsigned int indices[] = {
    0, 1, 2,    //  第一个三角形
    0, 2, 3,    // 第二个三角形
};

static const char *vertex_shader_source = R"(
    #version 330
    layout (location = 0) in vec3 vPos;
    layout (location = 1) in vec3 vCol;
    out vec3 outColor;
    void main()
    {
        outColor = vCol;
        gl_Position = vec4(vPos, 1.0f);
    }
)";

static const char *fragment_shader_source = R"(
    #version 330
    in vec3 outColor;
    out vec4 fColor;
    uniform float uSlat1;
    uniform float uSlat2;
    void main()
    {
        fColor = vec4(outColor.x + uSlat1, outColor.y + uSlat2, outColor.z, 1.0f);
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
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // 注意：GL_ELEMENT_ARRAY_BUFFER必须在VertexArray之后解绑

    // render loop
    while (glfwWindowShouldClose(window) == false) {
        // poll and handle events
        glfwPollEvents();

        // 每次绘制前需要清屏，否则可能遗留的上一次绘制的内容
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float slat1 = (sin(glfwGetTime()) / 2.0f) + 0.5f;
        float slat2 = (cos(glfwGetTime()) / 2.0f) + 0.5f;

        glUseProgram(program);
        glUniform1f(glGetUniformLocation(program, "uSlat1"), slat1);
        glUniform1f(glGetUniformLocation(program, "uSlat2"), slat2);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glfwSwapBuffers(window);
    }

    // cleanup
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(program);
    glfwTerminate();

    return 0;
}

static void glfw_error_callback(int error, const char* description) {
    printf("GLFW Error %d:%s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}