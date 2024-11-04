#ifndef LEARN_OPENGL_UTILS_OPENGL_PROGRAM_H_
#define LEARN_OPENGL_UTILS_OPENGL_PROGRAM_H_

#include <sys/types.h>

#include <array>
#include <vector>

#include <glad/glad.h>

enum class ShaderStage : uint8_t {
    VERTEX = 0,
    FRAGMENT = 1,
    COMPUTE = 2,
};


class Shader {

};

class ShaderManager {

};

class Program {

};

class OpenGLProgram {
public:
    static constexpr size_t SHADER_TYPE_COUNT = 3;

    using ShaderBlob = std::vector<uint8_t>;
    using ShaderSource = std::array<ShaderBlob, SHADER_TYPE_COUNT>;

    OpenGLProgram() noexcept;

    OpenGLProgram(const OpenGLProgram& rhs) = delete;
    OpenGLProgram& operator=(const OpenGLProgram& rhs) = delete;

    OpenGLProgram(OpenGLProgram&& rhs) noexcept;
    OpenGLProgram& operator=(OpenGLProgram&& rhs) noexcept = delete;

    ~OpenGLProgram() noexcept;

    OpenGLProgram& Shader(ShaderStage shader, const std::string& path);

    OpenGLProgram& Shader(ShaderStage shader, void const* data, size_t size);

private:
    std::array<GLuint, OpenGLProgram::SHADER_TYPE_COUNT> shaders_;

private:
    ShaderSource shader_source_;
};



#endif  // LEARN_OPENGL_UTILS_OPENGL_PROGRAM_H_