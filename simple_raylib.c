#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include <stdio.h>

#define DEFAULT_FOVY 20.0f

int main() {
    int default_window_height = 800;
    int default_window_width = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(default_window_height, default_window_width, "Simple Raylib Project");

    Camera3D camera = { 0 };
    // Note: The target distance is related to the render distance in the orthographic projection
    camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.projection = CAMERA_ORTHOGRAPHIC;
    camera.fovy = DEFAULT_FOVY; // near plane width in CAMERA_ORTHOGRAPHIC
    CameraYaw(&camera, -135 * DEG2RAD, true);
    CameraPitch(&camera, -45 * DEG2RAD, true, true, false);

    int camera_zoom_mode = 1;
    float stored_camera_fovy = DEFAULT_FOVY;

    SetTargetFPS(60);    // Set our game to run at 60 frames-per-second
    
    // Main game loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key
        //float dt = GetFrameTime(); // delta time
        
        //UpdateCamera(&camera, camera_mode);

        if (IsKeyDown(KEY_E)) {
            CameraYaw(&camera, 1.5f * DEG2RAD, true);  // Rotate camera to the right
        }

        if (IsKeyDown(KEY_Q)) {
            CameraYaw(&camera, -1.5f * DEG2RAD, true);  // Rotate camera to the left
        }

        if (IsKeyPressed(KEY_GRAVE)) {
            switch (camera_zoom_mode) {
                case 1:
                    stored_camera_fovy -= 2.5f;
                    camera.fovy = stored_camera_fovy;
                    camera_zoom_mode += 1;
                    break;
                case 2:
                    stored_camera_fovy -= 2.5f;
                    camera.fovy = stored_camera_fovy;
                    camera_zoom_mode += 1;
                    break;
                case 3: 
                    stored_camera_fovy -= 2.5f;
                    camera.fovy = stored_camera_fovy;
                    camera_zoom_mode += 1;
                    break;
                default:
                    camera.fovy = DEFAULT_FOVY;
                    stored_camera_fovy = DEFAULT_FOVY;
                    camera_zoom_mode = 1;
                    break;
            } 
        }

        UpdateCameraPro(&camera,
            (Vector3){
                (IsKeyDown(KEY_W))*0.1f -       // Move forward-backward
                (IsKeyDown(KEY_S))*0.1f,    
                (IsKeyDown(KEY_D))*0.1f -       // Move right-left
                (IsKeyDown(KEY_A))*0.1f,
                0.0f                            // Move up-down
            },
            (Vector3){
                0.0f,                           // Rotation: yaw
                0.0f,                           // Rotation: pitch
                0.0f                            // Rotation: roll
            }, 
            0.0f);                              // Move to target (zoom)

        // Draw
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);


                DrawPlane(Vector3Zero(), (Vector2){ 32.0f, 32.0f }, BLACK); // Draw ground
                DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
                DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
                DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall
                
                // Player cube
                DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
                DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
        
            EndMode3D();

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}

// F6 to compile in notepad++ with gcc