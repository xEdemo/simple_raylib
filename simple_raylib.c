#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "raygui.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define START_MENU 0
#define GAMEPLAY 1

#define CONTINUE_BUTTON_WIDTH 100
#define CONTINUE_BUTTON_HEIGHT 50

#define DEFAULT_FOVY 20.0f
#define CAMERA_ROTATE_SPEED 1.25f
#define PLAYER_RADIUS 0.5f
#define CAMERA_VELOCITY 0.0f
#define MAX_VELOCITY 2.5f
#define CAMERA_ACCELERATION 0.01f
#define CAMERA_DECELERATION CAMERA_ACCELERATION * 2.5f
#define CAMERA_GRAVITATIONAL_ACCELERATION 0.5f

// Global Variables Definition------------------------------------
const int default_window_height = 800;
const int default_window_width = 600;
int camera_zoom_mode = 1; // Temp: Changes camera zoom
float stored_camera_fovy = DEFAULT_FOVY; // Camera fovy position
int game_state = START_MENU;
bool is_paused = false;
Camera3D camera = { 0 }; // Initializes camera
Vector3 movement = { 0.0f, 0.0f, 0.0f }; // movement.x, movement.y, movement.z
Vector3 camera_velocity = { CAMERA_VELOCITY, CAMERA_VELOCITY, 0.0f };
float camera_acceleration = CAMERA_ACCELERATION;
float camera_deceleration = CAMERA_DECELERATION;
// End Global Variables Definition--------------------------------

// Module Functions Declaration-----------------------------------
// Update and Draw one frame (Web)
void UpdateDrawFrame(void);
// Updates camera position based on input
void MoveCamera(void);
// Zooms camera when the grave key is pressed
void ZoomCamera(void);
// Orbits camera counterclockwise
void OrbitCCW(void);
// Checks to see if 'p' key is pressed; stops game updates
void CheckPause(void);
// Start meniu w/ gui
void DrawStartMenu(void);
// Pause menu w/ gui
void DrawPauseMenu(void);
// Draws player cube
void DrawPlayer(void);
// Draws a :) with DrawCube & DrawCircle3D
void DrawSmile(void);
// Temp; Draws simple terrain
void Draw3DEnvironment(void);
// Changes velocity when a BoundingBox comes into contact with player's sphere
void HandleCollision(BoundingBox box, bool is_collision);
// Detects face collision; 0 = left, 1 = right, 2 = back, 3 = front, 4 = bottom, and 5 = top
int GetBoundingBoxFaceOnCollision(BoundingBox box);
// End Module Functions Declaration-------------------------------

// Program Entry Point--------------------------------------------
int main(void) {

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Lets you resize your window; also fixes web dimensions

    InitWindow(default_window_height, default_window_width, "Simple Raylib Project");

    // Note: The target distance is related to the render distance in the orthographic projection
    camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
    camera.target = (Vector3){ 0.0f, PLAYER_RADIUS, 0.0f }; // Second value changes the character's position (height)
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.projection = CAMERA_ORTHOGRAPHIC;
    camera.fovy = DEFAULT_FOVY; // near plane width in CAMERA_ORTHOGRAPHIC
    CameraYaw(&camera, -135 * DEG2RAD, true); // 2nd value doesn't really matter
    CameraPitch(&camera, -45 * DEG2RAD, true, true, false);

    // TODO: Create collision with walls
    // Set position of camera relative to floor instead of hard coding value
    // Change v if two keys are held at the same time

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
        SetTargetFPS(60);    // Set our game to run at 60 frames-per-second
        // Main game loop
        while (!WindowShouldClose()) { // Detect window close button or ESC key

            if (game_state == START_MENU) {
                OrbitCCW();

                if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    game_state = GAMEPLAY;
                }
                
                BeginDrawing();
                
                    DrawStartMenu();
                    
                EndDrawing();  
            } else if (game_state == GAMEPLAY) {
                CheckPause();

                if (is_paused) {
                    BeginDrawing();

                        DrawPauseMenu();

                    EndDrawing();
                } else {
                    // Camera Manipulation
                    ZoomCamera();
                    MoveCamera();

                    // Draw
                    BeginDrawing();

                        Draw3DEnvironment();

                        // GUI here
                        DrawRectangle(600, 5, 195, 100, ColorAlpha(SKYBLUE, 0.5f));
                        DrawRectangleLines(600, 5, 195, 100, BLUE);

                        DrawText("Camera status:", 610, 15, 10, BLACK);
                        DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 30, 10, BLACK);
                        DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 45, 10, BLACK);
                        DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 60, 10, BLACK);
                        DrawText(TextFormat("- Current Velocity (X): (%06.3f)", camera_velocity.x), 610, 75, 10, BLACK);
                        DrawText(TextFormat("- Current Velocity (Y): (%06.3f)", camera_velocity.y), 610, 90, 10, BLACK);

                    EndDrawing();
                }
            }
        }
    #endif

    // De-Initialization
    CloseWindow();

    return 0;
}
// End Entry Point------------------------------------------------

// Module Functions Definition------------------------------------
void UpdateDrawFrame(void) {
    //Update variables here
    if (game_state == START_MENU) {
        OrbitCCW();
                
        BeginDrawing();
        
            DrawStartMenu();
            
        EndDrawing(); 
    } else if (game_state == GAMEPLAY) {
        CheckPause();

        if (is_paused) {
            OrbitCCW();

            BeginDrawing();

                DrawPauseMenu();

            EndDrawing();
        } else {
            // Camera Manipulation
            ZoomCamera();
            MoveCamera();

            // Draw
            BeginDrawing();

                Draw3DEnvironment();

                // GUI here

            EndDrawing();
        }
    }
}

void MoveCamera(void) {
    movement = Vector3Zero();
    // Rotation Controls
    if (IsKeyDown(KEY_E)) {
        CameraYaw(&camera, CAMERA_ROTATE_SPEED * DEG2RAD, true);  // Rotate camera to the right
    }
    if (IsKeyDown(KEY_Q)) {
        CameraYaw(&camera, -CAMERA_ROTATE_SPEED * DEG2RAD, true);  // Rotate camera to the left
    }

     // Adjust camera velocity based on acceleration; makes it so when w and s are held nothing happens
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) || 
        IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            // Moves the ball with greater velocity over time
            if (camera_velocity.x > 0) {
                camera_velocity.x += camera_acceleration;
            } else {
                camera_velocity.x += camera_deceleration;
            }
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            // Moves the ball with greater velocity over time
            if (camera_velocity.x < 0 ) {
                camera_velocity.x -= camera_acceleration;
            } else {
                camera_velocity.x -= camera_deceleration;
            }
        }
        // Slows down left or right velocities when moving in the forward direction
        // if (camera_velocity.y > 0) {
        //     camera_velocity.y -= (camera_acceleration / 1.25f);
        // } else if (camera_velocity.y < 0) {
        //     camera_velocity.y += (camera_acceleration / 1.25f);
        // }
    }

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT) ||
        IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            // Moves the ball with greater velocity over time
            if (camera_velocity.y > 0) {
                camera_velocity.y += camera_acceleration;
            } else {
                camera_velocity.y += camera_deceleration;
            }
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            // Moves the ball with greater velocity over time
            if (camera_velocity.y < 0 ) {
                camera_velocity.y -= camera_acceleration;
            } else {
                camera_velocity.y -= camera_deceleration;
            }
        }
        // Slows down up or down velocities when moving in the forward direction
        // if (camera_velocity.x > 0) {
        //     camera_velocity.x -= (camera_acceleration / 1.25f);
        // } else if (camera_velocity.x < 0) {
        //     camera_velocity.x += (camera_acceleration / 1.25f);
        // }
    } 


    // Will need to add an if to detect collision
    // Apply deceleration if no movement keys are pressed; simulates friction
    if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
        if (camera_velocity.x > 0.0f) {
            camera_velocity.x -= camera_acceleration / 2.75f;
            if (camera_velocity.x < 0.0f) camera_velocity.x = 0.0f;
        } else if (camera_velocity.x < 0.0f) {
            camera_velocity.x += camera_acceleration / 2.75f;
            if (camera_velocity.x > 0.0f) camera_velocity.x = 0.0f;
        }
    }

    if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)) {
        if (camera_velocity.y > 0.0f) {
            camera_velocity.y -= camera_acceleration / 2.75f;
            if (camera_velocity.y < 0.0f) camera_velocity.y = 0.0f;
        } else if (camera_velocity.y < 0.0f) {
            camera_velocity.y += camera_acceleration / 2.75f;
            if (camera_velocity.y > 0.0f) camera_velocity.y = 0.0f;
        }
    }

    // Clamp velocity within the defined range
    camera_velocity.x = Clamp(camera_velocity.x, -MAX_VELOCITY, MAX_VELOCITY);
    camera_velocity.y = Clamp(camera_velocity.y, -MAX_VELOCITY, MAX_VELOCITY);

    // Calculate movement based on velocity
    movement.x += camera_velocity.x * 0.1f; // Forward/backward movement; 0.1 to slow down
    movement.y += camera_velocity.y * 0.1f; // Left/right movement; 0.1 to slow down

    UpdateCameraPro(&camera, movement, Vector3Zero(), 0.0f);

    // UpdateCameraPro(&camera,
    //     (Vector3){
    //         (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.1f -       // Move forward-backward
    //         (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.1f,    
    //         (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.1f -    // Move right-left
    //         (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.1f,
    //         0.0f                            // Move up-down
    //     },
    //     (Vector3){
    //         0.0f,                           // Rotation: yaw
    //         0.0f,                           // Rotation: pitch
    //         0.0f                            // Rotation: roll
    //     }, 
    //     0.0f);                              // Move to target (zoom)
}

void ZoomCamera(void) {
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
}

void OrbitCCW(void) {
    CameraYaw(&camera, (CAMERA_ROTATE_SPEED * DEG2RAD) / 2, true);
}

void CheckPause(void) {
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
        if (!is_paused) {
            is_paused = true;
        } else {
            is_paused = false;
        }
    }
}

void DrawStartMenu(void) {
    ClearBackground(RAYWHITE);
                    
    BeginMode3D(camera);
    
        DrawPlane(Vector3Zero(), (Vector2){ 32.0f, 32.0f }, DARKGRAY); // Draw ground
        
        DrawSmile();   

        DrawPlayer();
        
    EndMode3D();
        
    if (GuiButton((Rectangle){ (GetScreenWidth() / 2) - (CONTINUE_BUTTON_WIDTH / 2), (GetScreenHeight() / 2) + CONTINUE_BUTTON_HEIGHT, CONTINUE_BUTTON_WIDTH, CONTINUE_BUTTON_HEIGHT }, "Play")){
        game_state = GAMEPLAY;
    }
}

void DrawPauseMenu(void) {
    // Fade not supported on browser
    #if defined(PLATFORM_DESKTOP)
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(SKYBLUE, 0.01f));
    #else
        Draw3DEnvironment();
    #endif

    // Calculate the width of the "Paused" text
    int paused_text_width = MeasureText("Paused", 40);

    // Center the "Paused" text horizontally
    int paused_text_x = (GetScreenWidth() - paused_text_width) / 2;

    DrawText("Paused", paused_text_x, (GetScreenHeight() / 2), 40, RED);
    if (GuiButton((Rectangle){ (GetScreenWidth() / 2) - (CONTINUE_BUTTON_WIDTH / 2), (GetScreenHeight() / 2) + CONTINUE_BUTTON_HEIGHT, CONTINUE_BUTTON_WIDTH, CONTINUE_BUTTON_HEIGHT }, "Resume")){
        is_paused = false;
    }
}

void DrawPlayer(void) {
    DrawSphere(camera.target, PLAYER_RADIUS, PURPLE);
    DrawSphereWires(camera.target, PLAYER_RADIUS, 4, 4, DARKPURPLE);
}

void DrawSmile(void) {
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
}

void Draw3DEnvironment() {
    ClearBackground(RAYWHITE);
                
    BeginMode3D(camera);
        // bottom right corner; top left corner
        BoundingBox blue_rectangle_box = { (Vector3){ -16.5f, 0.0f, -16.0f }, (Vector3){ -15.5f, 5.0f, 16.0f } };
        BoundingBox green_rectangle_box = { (Vector3){ 15.5f, 0.0f, -16.0f }, (Vector3){ 16.5f, 5.0f, 16.0f } };
        bool is_blue_collision = CheckCollisionBoxSphere(blue_rectangle_box, camera.target, PLAYER_RADIUS);
        bool is_green_collision = CheckCollisionBoxSphere(green_rectangle_box, camera.target, PLAYER_RADIUS);

        DrawPlane(Vector3Zero(), (Vector2){ 32.0f, 32.0f }, DARKGRAY);             // Draw ground
        DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
        DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
        //DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

        DrawBoundingBox(blue_rectangle_box, is_blue_collision ? RED : GREEN);
        DrawBoundingBox(green_rectangle_box, is_green_collision ? RED : GREEN);
        
        DrawPlayer();

    EndMode3D();

    HandleCollision(blue_rectangle_box, is_blue_collision);
    HandleCollision(green_rectangle_box, is_green_collision);
}

void HandleCollision(BoundingBox box, bool is_collision) {
    if (is_collision) {
        // Calculate normal vector of the wall (assuming the wall is vertical)
        Vector3 wall_normal = { 1.0f, 0.0f, 0.0f };

        // Determine which face of the wall the ball collided with
        int face = GetBoundingBoxFaceOnCollision(box);

        // Get the normal vector of the blue wall
        Vector3 incoming_direction = Vector3Subtract(camera.target, camera_velocity);
        incoming_direction = Vector3Normalize(incoming_direction);

        // Calculate dot product of incoming direction and wall normal
        float dot_product = Vector3DotProduct(incoming_direction, wall_normal);

        // Calculate angle of incidence in radians
        float angle_radians = asinf(dot_product);

        float angle_degrees = angle_radians * RAD2DEG;

        printf("Camera Velocity Before Impact (x, y, z): %f, %f, %f\n", camera_velocity.x, camera_velocity.y, camera_velocity.z);
        
        // Since ~0 degrees is reflect in opposite direction, might be able to use degrees as some type of factor instead of complex math
        // if (camera_velocity.x > 0.0f) {
        //     camera_velocity.y += camera_velocity.x;
        //     camera_velocity.x = 0.0f;
        // }

        //if (angle_degrees) ...
        // This is here just for testing purposes
        camera_velocity.x = -camera_velocity.x;
        camera_velocity.y = -camera_velocity.y;

        printf("Camera Velocity After Impact (x, y, z): %f, %f, %f\n", camera_velocity.x, camera_velocity.y, camera_velocity.z);
        printf("Angle of Incidence (degrees): %f\n", angle_degrees);
        switch (face) {
            case 0:
                printf("Collision occurred with the left face of the wall.\n");
                break;
            case 1:
                printf("Collision occurred with the right face of the wall.\n");
                break;
            case 2:
                printf("Collision occurred with the back face of the wall.\n");
                break;
            case 3:
                printf("Collision occurred with the front face of the wall.\n");
                break;
            case 4:
                printf("Collision occurred with the bottom face of the wall.\n");
                break;
            case 5:
                printf("Collision occurred with the top face of the wall.\n");
                break;
            default:
                break;
        }
    }
}

int GetBoundingBoxFaceOnCollision(BoundingBox box) {
    int face = -1; // Initialize 'face' to indicate no collision
    if (camera.target.x <= box.min.x) face = 0; // Left face
    else if(camera.target.x >= box.max.x) face = 1; // Right face
    else if(camera.target.z <= box.min.z) face = 2; // Back face
    else if (camera.target.z >= box.max.z) face = 3; // Front face
    else if (camera.target.y <= box.min.y) face = 4; // Bottom face
    else if (camera.target.y >= box.max.y) face = 5; // Top face
    return face;
}
// End Module Functions Definition---------------------------------

// F6 to compile in notepad++ with gcc