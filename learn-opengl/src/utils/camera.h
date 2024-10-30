#ifndef LEARN_OPENGL_UTILS_CAMERA_H_
#define LEARN_OPENGL_UTILS_CAMERA_H_

class Camera {
public:
    enum class Projection : int {
        PERSPECTIVE,    // 透视投影
        ORTHO,          // 正射投影
    };

    enum class Fov : int {
        VERTICAL,           // 垂直轴
        HORIZONTAL,         // 水平轴
    };

    

};

#endif  // LEARN_OPENGL_UTILS_CAMERA_H_