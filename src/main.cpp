#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <cmath>

const int WINDOW_WIDTH = 960;
const int WINDOW_HEIGHT = 540;
const char* WINDOW_TITLE = "Lorenz Attractor Simulation";

// Lorenz system parameters
const float SIGMA = 10.0f;
const float RHO = 28.0f;
const float BETA = 8.0f / 3.0f;

// Simulation parameters
const float DT = 0.01f;                 // Time step for integration
const int TRAIL_LENGTH = 100;           // Number of points in each particle's trail
const int PARTICLE_COUNT = 100;         // Number of particles
const float PARTICLE_RADIUS = 0.5f;     // Visual size of particles
const float SCALE = 10.0f;              // Scale factor for visualization

// Camera control parameters
const float CAMERA_MOVE_SPEED = 0.5f;
const float CAMERA_ZOOM_SPEED = 2.0f;
const float CAMERA_ROTATION_SPEED = 0.1f;

// Lorenz attractor center (approximate center of the butterfly attractor)
const Vector3 ATTRACTOR_CENTER = { 0.0f, 0.0f, 25.0f };

// Colors for particles
const Color PARTICLE_COLORS[] = {
    RED, GREEN, BLUE, YELLOW, MAGENTA, ORANGE, PURPLE, SKYBLUE, LIME, PINK
};

struct Particle {
    Vector3 position;
    Vector3 velocity;
    Color color;
    std::vector<Vector3> trail;
    
    Particle(Vector3 startPos, Color col) : position(startPos), color(col) {
        trail.reserve(TRAIL_LENGTH);
    }
    
    void update() {
        // Lorenz equations
        float dx = SIGMA * (position.y - position.x);
        float dy = position.x * (RHO - position.z) - position.y;
        float dz = position.x * position.y - BETA * position.z;
        
        velocity = {dx, dy, dz};
        
        // Euler integration
        position.x += velocity.x * DT;
        position.y += velocity.y * DT;
        position.z += velocity.z * DT;
        
        // Add current position to trail
        trail.push_back(position);
        
        // Keep trail at fixed length
        if (trail.size() > TRAIL_LENGTH) {
            trail.erase(trail.begin());
        }
    }
    
    void draw() const {
        // Draw trail
        for (size_t i = 1; i < trail.size(); i++) {
            Vector3 prev = trail[i-1];
            Vector3 curr = trail[i];
            
            // Fade trail based on age (older points are more transparent)
            float alpha = (float)i / trail.size() * 255.0f;
            Color trailColor = color;
            trailColor.a = static_cast<unsigned char>(alpha);
            
            DrawLine3D(prev, curr, trailColor);
        }
        
        // Draw particle as a sphere
        DrawSphere(position, PARTICLE_RADIUS, color);
        
        // Also draw a small circle at the particle position (2D projection)
        // This helps visibility when the particle is behind other elements
        Vector2 screenPos = {
            WINDOW_WIDTH/2 + position.x * SCALE,
            WINDOW_HEIGHT/2 + position.y * SCALE
        };
        DrawCircle(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), 
                   PARTICLE_RADIUS * 1.5f, color);
    }
};

class LorenzSimulation {
private:
    std::vector<Particle> particles;
    Camera3D camera;
    float time;
    
public:
    LorenzSimulation() : time(0) {
        // Initialize camera to focus on attractor center
        camera.target = ATTRACTOR_CENTER;
        camera.position = (Vector3){ 
            ATTRACTOR_CENTER.x + 50.0f, 
            ATTRACTOR_CENTER.y + 50.0f, 
            ATTRACTOR_CENTER.z + 50.0f 
        };
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.fovy = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;
        
        // Create particles with different starting positions
        particles.reserve(PARTICLE_COUNT);
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            Vector3 startPos = {
                (float)(rand() % 10 - 5),  // Random between -5 and 5
                (float)(rand() % 10 - 5),
                (float)(rand() % 10 + 20)  // Start with some z-height
            };
            Color col = PARTICLE_COLORS[i % (sizeof(PARTICLE_COLORS) / sizeof(PARTICLE_COLORS[0]))];
            particles.emplace_back(startPos, col);
        }
    }
    
    void update() {
        time += DT;
        for (auto& particle : particles) {
            particle.update();
        }
        
        // Handle camera controls
        handleCameraInput();
    }
    
    void handleCameraInput() {
        // Mouse wheel zoom
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0) {
            // Move camera forward/backward based on wheel
            Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
            camera.position = Vector3Add(camera.position, Vector3Scale(forward, wheelMove * CAMERA_ZOOM_SPEED));
            camera.target = Vector3Add(camera.target, Vector3Scale(forward, wheelMove * CAMERA_ZOOM_SPEED));
        }
        
        // WASD and arrow keys for panning
        Vector3 right = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(camera.target, camera.position), camera.up));
        Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
        Vector3 up = camera.up;
        
        // Forward/backward (W/S or UP/DOWN arrows)
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(forward, CAMERA_MOVE_SPEED));
            camera.target = Vector3Add(camera.target, Vector3Scale(forward, CAMERA_MOVE_SPEED));
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            camera.position = Vector3Subtract(camera.position, Vector3Scale(forward, CAMERA_MOVE_SPEED));
            camera.target = Vector3Subtract(camera.target, Vector3Scale(forward, CAMERA_MOVE_SPEED));
        }
        
        // Left/right (A/D or LEFT/RIGHT arrows)
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            camera.position = Vector3Subtract(camera.position, Vector3Scale(right, CAMERA_MOVE_SPEED));
            camera.target = Vector3Subtract(camera.target, Vector3Scale(right, CAMERA_MOVE_SPEED));
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(right, CAMERA_MOVE_SPEED));
            camera.target = Vector3Add(camera.target, Vector3Scale(right, CAMERA_MOVE_SPEED));
        }
        
        // Up/down (SPACE/C or PAGEUP/PAGEDOWN)
        if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_PAGE_UP)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(up, CAMERA_MOVE_SPEED));
            camera.target = Vector3Add(camera.target, Vector3Scale(up, CAMERA_MOVE_SPEED));
        }
        if (IsKeyDown(KEY_C) || IsKeyDown(KEY_PAGE_DOWN)) {
            camera.position = Vector3Subtract(camera.position, Vector3Scale(up, CAMERA_MOVE_SPEED));
            camera.target = Vector3Subtract(camera.target, Vector3Scale(up, CAMERA_MOVE_SPEED));
        }
        
        // Reset camera view (R key)
        if (IsKeyPressed(KEY_R)) {
            camera.target = ATTRACTOR_CENTER;
            camera.position = (Vector3){ 
                ATTRACTOR_CENTER.x + 50.0f, 
                ATTRACTOR_CENTER.y + 50.0f, 
                ATTRACTOR_CENTER.z + 50.0f 
            };
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        }
    }
    
    void draw() const {
        BeginDrawing();
        ClearBackground(BLACK);
        
        BeginMode3D(camera);
        
        // Draw coordinate axes for reference
        //DrawLine3D((Vector3){-50, 0, 0}, (Vector3){50, 0, 0}, RED);   // X-axis
        //DrawLine3D((Vector3){0, -50, 0}, (Vector3){0, 50, 0}, GREEN); // Y-axis
        //DrawLine3D((Vector3){0, 0, -50}, (Vector3){0, 0, 50}, BLUE);  // Z-axis
        
        // Draw particles
        for (const auto& particle : particles) {
            particle.draw();
        }
        
        EndMode3D();
        
        // Draw UI information
        DrawText("Lorenz Attractor Simulation", 10, 10, 20, WHITE);
        DrawText("Particles follow chaotic trajectories", 10, 40, 18, LIGHTGRAY);
        DrawText(TextFormat("Time: %.2f", time), 10, 70, 18, LIGHTGRAY);
        DrawText(TextFormat("Particles: %d", PARTICLE_COUNT), 10, 100, 18, LIGHTGRAY);
        
        // Camera controls
        DrawText("Camera Controls:", 10, 130, 18, LIGHTGRAY);
        DrawText("WASD/Arrows: Move camera", 10, 155, 16, LIGHTGRAY);
        DrawText("Mouse Wheel: Zoom in/out", 10, 175, 16, LIGHTGRAY);
        DrawText("SPACE/C: Move up/down", 10, 195, 16, LIGHTGRAY);
        DrawText("R: Reset particles", 10, 215, 16, LIGHTGRAY);
        EndDrawing();
    }
    
    void resetParticles() {
        particles.clear();
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            Vector3 startPos = {
                (float)(rand() % 10 - 5),
                (float)(rand() % 10 - 5),
                (float)(rand() % 10 + 20)
            };
            Color col = PARTICLE_COLORS[i % (sizeof(PARTICLE_COLORS) / sizeof(PARTICLE_COLORS[0]))];
            particles.emplace_back(startPos, col);
        }
    }
};

int main() {
    // Initialize window
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);
    
    // Initialize simulation
    LorenzSimulation simulation;
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Update simulation
        simulation.update();
        
        // Handle input
        if (IsKeyPressed(KEY_R)) {
            simulation.resetParticles();
        }
        
        // Draw simulation
        simulation.draw();
    }
    
    CloseWindow();
    return 0;
}
