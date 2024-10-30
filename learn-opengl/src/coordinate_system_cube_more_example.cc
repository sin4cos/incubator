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

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "coordinate system cube more example"

static void glfw_error_callback(int error_code, const char *description);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);


struct Vertex {
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec2 tex;
};

static const Vertex vertices[] = {
    // 顶点坐标                  颜色                  纹理坐标
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },   // 0: 前: 左上
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },   // 1: 前: 右上
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 2: 前: 左下
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },   // 3: 前: 右下

    { { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },   // 4: 后: 左上
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },   // 5: 后: 右上
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 6: 后: 左下
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },   // 7: 后: 右下

    { { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },   // 8: 左: 左上
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },   // 9: 左: 右上
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 10: 左: 左下
    { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },   // 11: 左: 右下

    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },   // 12: 右: 左上
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },   // 13: 右: 右上
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 14: 右: 左下
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },   // 15: 右: 右下

    { { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },   // 16: 上: 左上
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },   // 17: 上: 右上
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 18: 上: 左下
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },   // 19: 上: 右下

    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },   // 20: 下: 左上
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },   // 21: 下: 右上
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },   // 22: 下: 左下
    { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },   // 23: 下: 右下
};

static unsigned int indices[] = {
    0, 1, 2, 1, 2, 3,           // 前面
    4, 5, 6, 5, 6, 7,           // 后面
    8, 9, 10, 9, 10, 11,        // 左面
    12, 13, 14, 13, 14, 15,     // 右面
    16, 17, 18, 17, 18, 19,     // 上面
    20, 21, 22, 21, 22, 23,     // 下面
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
    // GL 3.3 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
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

    // 加载中文字体
    io.Fonts->AddFontFromFileTTF("SourceHanSansCN-Medium.ttf", 12.0f, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    // 设置 ImGui 主题
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
    glEnable(GL_DEPTH_TEST);    // 开启 OpenGL 深度测试

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    GLenum polygon_mode = GL_FILL;

    float rotate_radians = -55.0f, perspective_radians = 45.0f;
    ImVec4 translate = ImVec4(0.0f, 0.0f, -3.0f, 0.0f);
    bool cube_random_rotate = true;

    glm::vec3 cube_positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // ====================================================================
        // 清屏
        // ====================================================================
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp(1.0f);

        // ====================================================================
        // 更新设置项
        // ====================================================================
        glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);

        // GLM 创建正射投影矩阵
        // 前两个参数指定了平截头体的左右坐标，第三和第四个参数指定了平截头体的底部和顶部。这四个参数定义了近平面和远平面的大小。
        // 第五和第六个参数则定义了近平面和远平面的距离。
        // glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

        // GLM 创建透视投影矩阵
        // 第一个参数定义了 fov 的值，它表示的是视野的角度。对于一个真实的观察效果，它的值经常设置为 45.0。
        // 第二个参数设置了宽高比。
        // 第三和第四个参数设置了平截头体的近平面和远平面。我们通常设置近平面距离为 0.1，远平面距离为 100.0。
        // glm::mat4 proj = glm::perspective(45.0f, (float)width/(float)height, 0.1f, 100.0f);

        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(rotate_radians), glm::vec3(0.0f, 1.0f, 0.0f));  // glm::vec3(0.0f, 1.0f, 0.0f) 表示沿 y 轴旋转
        // glm::mat4 view = glm::mat4(1.0f);
        // view = glm::translate(view, glm::vec3(translate.x, translate.y, translate.z));
        // glm::mat4 projection = glm::mat4(1.0f);
        // projection = glm::perspective(glm::radians(perspective_radians), (float)display_w/(float)display_h, 0.1f, 100.0f);
        // mvp = projection * view * model;
        

        // ====================================================================
        // OpenGL 渲染逻辑
        // ====================================================================
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureContainer);

        glUseProgram(program);
        glBindVertexArray(VAO);
        glUniform1i(glGetUniformLocation(program, "textureContainer"), 0);  // 设置纹理单元0

        // 设置每个小立方体的 model、view、projection 矩阵
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(translate.x, translate.y, translate.z));
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(perspective_radians), (float)display_w/(float)display_h, 0.1f, 100.0f);
        for (GLuint i = 0; i < 10; ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);
            GLfloat angle = 20.0f * (i + 1) * glfwGetTime();
            if (cube_random_rotate)
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            else
                model = glm::rotate(model, glm::radians(rotate_radians), glm::vec3(1.0f, 0.3f, 0.5f));
            mvp = projection * view * model;

            glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

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

            ImGui::Checkbox("cube random rotate", &cube_random_rotate);

            
            ImGui::SliderFloat("rotate radian", &rotate_radians, -360.0f, 360.0f, "rotate = %.2f");

            ImGui::SliderFloat("perspective radians", &perspective_radians, 0.0f, 90.0f, "rotate = %.2f");

            ImGui::SliderFloat("x", &translate.x, -1.0f, 1.0f, "x = %.2f");
            ImGui::SliderFloat("y", &translate.y, -1.0f, 1.0f, "y = %.2f");
            ImGui::SliderFloat("z", &translate.z, -1.0f, 1.0f, "z = %.2f");

            const char* items[] = { "FILL", "LINE", "POINT" };
            static int item_current = 0;
            if (item_current == 0)
                polygon_mode = GL_FILL;
            else if (item_current == 1)
                polygon_mode = GL_LINE;
            else
                polygon_mode = GL_POINT;
            ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));

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