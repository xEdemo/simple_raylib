#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "raygui.h"

#define START_MENU 0
#define GAMEPLAY 1

#define CONTINUE_BUTTON_WIDTH 100
#define CONTINUE_BUTTON_HEIGHT 50

#define DEFAULT_FOVY 20.0f
#define CAMERA_ROTATE_SPEED 1.25f
#define PLAYER_CUBE_DIMENSIONS 0.5f

int main() {
    int default_window_height = 800;
    int default_window_width = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(default_window_height, default_window_width, "Simple Raylib Project");

    Camera3D camera = { 0 };
    // Note: The target distance is related to the render distance in the orthographic projection
    camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
    camera.target = (Vector3){ 0.0f, PLAYER_CUBE_DIMENSIONS/2, 0.0f }; // Second value changes the character's position (height)
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.projection = CAMERA_ORTHOGRAPHIC;
    camera.fovy = DEFAULT_FOVY; // near plane width in CAMERA_ORTHOGRAPHIC
    CameraYaw(&camera, -135 * DEG2RAD, true);
    CameraPitch(&camera, -45 * DEG2RAD, true, true, false);

    int camera_zoom_mode = 1;
    float stored_camera_fovy = DEFAULT_FOVY;

    int game_state = START_MENU;

    bool is_paused = false;

    SetTargetFPS(60);    // Set our game to run at 60 frames-per-second

    // TODO: Create collision with walls
    // Set position of camera relative to floor instead of hard coding value
    // Create a start game screen with raygui
    // Change v if two keys are held at the same time
    
    // Main game loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key

        if (game_state == START_MENU) {
            CameraYaw(&camera, (CAMERA_ROTATE_SPEED * DEG2RAD)/2, true);
            
            BeginDrawing();
            
                ClearBackground(RAYWHITE);
                
                BeginMode3D(camera);
                
                    DrawPlane(Vector3Zero(), (Vector2){ 32.0f, 32.0f }, DARKGRAY); // Draw ground
                    
                    // Draw the smiley face using cubes    
                    // Draw face
                    DrawCircle3D((Vector3){0.0f, 0.01f, 0.0f}, 5.0f, (Vector3){1.0f, 1.0f, 1.0f}, 240.0f, YELLOW);
                                 
                    // Draw the eyes
                    DrawCube((Vector3){ -1.5f, 0.0f, -1.5f }, 1.0f, 1.0f, 1.0f, WHITE); // Left eye
                    DrawCubeWires((Vector3){ -1.5f, 0.0f, -1.5f }, 1.0f, 1.0f, 1.0f, BLACK); // Left eye wires
                    DrawCube((Vector3){ 1.5f, 0.0f, -1.5f }, 1.0f, 1.0f, 1.0f, WHITE);  // Right eye
                    DrawCubeWires((Vector3){ 1.5f, 0.0f, -1.5f }, 1.0f, 1.0f, 1.0f, BLACK); // Right eye wires
                    
                    // Draw the mouth
                    DrawCube((Vector3){ 0.0f, 0.0f, 2.0f }, 4.0f, 1.0f, 1.0f, RED); // Lips
                    DrawCube((Vector3){ -2.5f, 0.0f, 1.0f }, 1.0f, 1.0f, 1.0f, RED); // Left Corner
                    DrawCube((Vector3){ 2.5f, 0.0f, 1.0f }, 1.0f, 1.0f, 1.0f, RED); // Right Corner

                    // Player cube
                    DrawCube(camera.target, PLAYER_CUBE_DIMENSIONS, PLAYER_CUBE_DIMENSIONS, PLAYER_CUBE_DIMENSIONS, PURPLE);
                    DrawCubeWires(camera.target, PLAYER_CUBE_DIMENSIONS, PLAYER_CUBE_DIMENSIONS, PLAYER_CUBE_DIMENSIONS, DARKPURPLE);
                    
                EndMode3D();
                    
                if (GuiButton((Rectangle){ (GetScreenWidth() / 2) - (CONTINUE_BUTTON_WIDTH / 2), (GetScreenHeight() / 2) + CONTINUE_BUTTON_HEIGHT, CONTINUE_BUTTON_WIDTH, CONTINUE_BUTTON_HEIGHT }, "Play")){
                    game_state = GAMEPLAY;
                }
                
            EndDrawing();  
        } else if (game_state == GAMEPLAY) {
            //float dt = GetFrameTime(); // delta time

            if (IsKeyPressed(KEY_P)) {
                if (!is_paused) {
                    is_paused = true;
                } else {
                    is_paused = false;
                }
            }

            if (is_paused) {
                BeginDrawing();
                    // Fade not supported on browser?
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(SKYBLUE, 0.01f));

                    // Calculate the width of the "Paused" text
                    int paused_text_width = MeasureText("Paused", 40);

                    // Center the "Paused" text horizontally
                    int paused_text_x = (GetScreenWidth() - paused_text_width) / 2;

                    DrawText("Paused", paused_text_x, (GetScreenHeight() / 2), 40, RED);
                    if (GuiButton((Rectangle){ (GetScreenWidth() / 2) - (CONTINUE_BUTTON_WIDTH / 2), (GetScreenHeight() / 2) + CONTINUE_BUTTON_HEIGHT, CONTINUE_BUTTON_WIDTH, CONTINUE_BUTTON_HEIGHT }, "Resume")){
                        is_paused = false;
                    }

                EndDrawing();

            } else {
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

                if (IsKeyDown(KEY_E)) {
                    CameraYaw(&camera, CAMERA_ROTATE_SPEED * DEG2RAD, true);  // Rotate camera to the right
                }

                if (IsKeyDown(KEY_Q)) {
                    CameraYaw(&camera, -CAMERA_ROTATE_SPEED * DEG2RAD, true);  // Rotate camera to the left
                }

                UpdateCameraPro(&camera,
                    (Vector3){
                        (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.1f -       // Move forward-backward
                        (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.1f,    
                        (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.1f -    // Move right-left
                        (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.1f,
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

                        DrawPlane(Vector3Zero(), (Vector2){ 32.0f, 32.0f }, DARKGRAY);             // Draw ground
                        DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
                        DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
                        DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall
                        
                        // Player cube
                        DrawCube(camera.target, PLAYER_CUBE_DIMENSIONS, PLAYER_CUBE_DIMENSIONS, PLAYER_CUBE_DIMENSIONS, PURPLE);
                        DrawCubeWires(camera.target, PLAYER_CUBE_DIMENSIONS, PLAYER_CUBE_DIMENSIONS, PLAYER_CUBE_DIMENSIONS, DARKPURPLE);
                
                    EndMode3D();

                EndDrawing();
            }
        }
    }

    // De-Initialization
    CloseWindow();

    return 0;
}

// F6 to compile in notepad++ with gcc