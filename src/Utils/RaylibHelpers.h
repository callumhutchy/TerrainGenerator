#include "raylib.h"
#include <algorithm>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1200;

Vector2 cameraAngle = {0.7f, 0.7f};
float cameraDistance = 200.0f;

Camera3D Create3DPerspectiveCamera(float _distance = 200.0f)
{
    cameraDistance = _distance;
    Camera3D camera = {
        {cos(cameraAngle.y) * cos(cameraAngle.x) * cameraDistance,
         sin(cameraAngle.x) * cameraDistance,
         sin(cameraAngle.y) * cos(cameraAngle.x) * cameraDistance},
        {0.0f, -20.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        45.0f,
        CAMERA_PERSPECTIVE};

    return camera;
}

void ManageCameraControls(Camera3D * camera)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 delta = GetMouseDelta();
        cameraAngle.x += delta.y * 0.003;
        cameraAngle.y += delta.x * 0.003;
        cameraAngle.x = std::clamp(cameraAngle.x, -PI / 3.0f, PI / 3.0f);
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        cameraDistance *= (1.0f - wheel * 0.02);
        cameraDistance = std::clamp(cameraDistance, 10.0f, 500.0f);
    }

    Vector3 newCamPos = {
        cos(cameraAngle.y) * cos(cameraAngle.x) * cameraDistance,
        sin(cameraAngle.x) * cameraDistance,
        sin(cameraAngle.y) * cos(cameraAngle.x) * cameraDistance};
    camera->position = newCamPos;

    if (IsKeyPressed(KEY_Z))
    {
        cameraAngle = {0.7f, 0.7f};
        cameraDistance = 200.0f;
    }
}