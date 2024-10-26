#include <chrono>
#include <thread>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE       // 作用: 避免 GLFW 重复加载 OpenGL 相关的头文件
#include <GLFW/glfw3.h>
#define GL_SILENCE_DEPRECATION  // 作用: 屏蔽 GL 中 deprecated 的函数调用警告

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "transform example"

static void glfw_error_callback(int error_code, const char *description);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);


struct Vertex {
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec2 tex;
};

static const Vertex vertices[4] = {
    // 顶点坐标                 颜色                   纹理坐标
    { { -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },   // 左上
    { {  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },   // 右上
    { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 左下
    { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },   // 右下
};

static unsigned int indices[] = {
    0, 1, 2,    // 第一个三角形
    1, 2, 3,    // 第二个三角形
};

static const char *vertex_shader_source = R"(
    #version 330
    layout (location = 0) in vec3 vPos;
    layout (location = 1) in vec3 vCol;
    layout (location = 2) in vec2 vTex;
    out vec3 fCol;
    out vec2 fTex;
    uniform mat4 MVP;
    void main()
    {
        fCol = vCol;
        fTex = vTex;
        gl_Position = MVP * vec4(vPos, 1.0f);
    }
)";

static const char *fragment_shader_source = R"(
    #version 330
    in vec3 fCol;
    in vec2 fTex;
    out vec4 outCol;
    uniform sampler2D textureContainer;
    void main()
    {
        outCol = texture(textureContainer, fTex) * vec4(fCol, 1.0f);
    }
)";

int main(void)
{
    // ========================================================================
    // GLFW 环境初始化
    // ========================================================================
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
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr) {
        printf("glfwCreateWindow() failed\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);    // Enable vsync

    // load OpenGL functions
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        printf("gladLoadGLLoader() failed\n");
        return -1; 
    }

    // ========================================================================
    // 初始化 ImGUI，设置相关配置
    // ========================================================================
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Render backends.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ========================================================================
    // OpenGL 数据准备
    // ========================================================================
    int success;
    char info_log[128];

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
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, col));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tex));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLuint textureContainer;
    glGenTextures(1, &textureContainer);
    glBindTexture(GL_TEXTURE_2D, textureContainer);
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

    // ========================================================================
    // 定义程序运行中的状态数据
    // ========================================================================
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    GLenum polygon_mode = GL_FILL;
    float radians = 0.0f, scale = 1.0f, translate_x = 0.0f, translate_y = 0.0f, translate_z = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // ====================================================================
        // 清屏
        // ====================================================================
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 mvp(1.0f);

        // ====================================================================
        // 更新设置项
        // ====================================================================
        glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);

        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // glm::mat4 view = glm::mat4(1.0f);
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // glm::mat4 projection = glm::mat4(1.0f);
        // projection = glm::perspective(glm::radians(45.0f), (float)display_w/(float)display_h, 0.1f, 100.0f);
        // mvp = projection * view * model;

        mvp = glm::rotate(mvp, glm::radians(radians), glm::vec3(0.0, 0.0, 1.0));    // 围绕 (0.0, 0.0, 1.0) 该点旋转
        mvp = glm::scale(mvp, glm::vec3(scale));                                    // 缩放指定倍数
        mvp = glm::translate(mvp, glm::vec3(translate_x, translate_y, translate_z));

        // ====================================================================
        // OpenGL 渲染逻辑
        // ====================================================================
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureContainer);

        glUseProgram(program);
        glBindVertexArray(VAO);
        glUniform1i(glGetUniformLocation(program, "textureContainer"), 0);  // 设置纹理单元0
        glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // ====================================================================
        // ImGUI 渲染逻辑
        // ====================================================================
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGUI 菜单
        {
            ImGui::Begin("Tools");

            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            
            ImGui::SliderFloat("rorate radian", &radians, -360.0f, 360.0f, "rorate = %.2f");

            ImGui::SliderFloat("scale", &scale, 0.0f, 2.0f, "scale = %.2f");

            ImGui::SliderFloat("x", &translate_x, -1.0f, 1.0f, "x = %.2f");
            ImGui::SliderFloat("y", &translate_y, -1.0f, 1.0f, "y = %.2f");
            ImGui::SliderFloat("z", &translate_z, -1.0f, 1.0f, "z = %.2f");

            const char* items[] = { "FILL", "LINE", "POINT" };
            static int item_current = 0;
            if (item_current == 0)
                polygon_mode = GL_FILL;
            else if (item_current == 1)
                polygon_mode = GL_LINE;
            else
                polygon_mode = GL_POINT;
            ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));

            printf("radians:%.2f, scale:%.2f, translate:%.2f/%.2f/%2.f, item_current:%d\n", radians, scale, translate_x, translate_y, translate_z, item_current);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // ========================================================================
    // 释放资源
    // ========================================================================
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
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