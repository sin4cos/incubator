#include "opengl_program.h"

#include <algorithm>

OpenGLProgram::OpenGLProgram() noexcept {
}

OpenGLProgram::OpenGLProgram(OpenGLProgram&& rhs) noexcept = default;

OpenGLProgram::~OpenGLProgram() noexcept = default;

OpenGLProgram& OpenGLProgram::Shader(ShaderStage shader, const std::string& path) {

}

OpenGLProgram& OpenGLProgram::Shader(ShaderStage shader,
                                     void const* data, size_t size) {
    ShaderBlob blob(size);
    std::copy_n((const uint8_t*)data, size, blob.data());
    shader_source_[size_t(shader)] = std::move(blob);
    return *this;
}