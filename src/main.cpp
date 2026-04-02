#include <raylib.h>
#include <vector>
#include <cmath>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const char* WINDOW_TITLE = "Lorenz Attractor Simulation";

// Lorenz system parameters
const float SIGMA = 10.0f;
const float RHO = 28.0f;
const float BETA = 8.0f / 3.0f;

// Simulation parameters
const float DT = 0.01f;           // Time step for integration
const int TRAIL_LENGTH = 500;     // Number of points in each particle's trail
const int PARTICLE_COUNT = 5;     // Number of particles
const float PARTICLE_RADIUS = 5.0f; // Visual size of particles
const float SCALE = 10.0f;        // Scale factor for visualization

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
        // Initialize camera
        camera.position = (Vector3){ 50.0f, 50.0f, 50.0f };
        camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
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
        
        // Update camera to slowly rotate around the attractor
        float radius = 60.0f;
        camera.position.x = cosf(time * 0.1f) * radius;
        camera.position.z = sinf(time * 0.1f) * radius;
        camera.position.y = 30.0f + sinf(time * 0.05f) * 10.0f;
    }
    
    void draw() const {
        BeginDrawing();
        ClearBackground(BLACK);
        
        BeginMode3D(camera);
        
        // Draw coordinate axes for reference
        DrawLine3D((Vector3){-50, 0, 0}, (Vector3){50, 0, 0}, RED);   // X-axis
        DrawLine3D((Vector3){0, -50, 0}, (Vector3){0, 50, 0}, GREEN); // Y-axis
        DrawLine3D((Vector3){0, 0, -50}, (Vector3){0, 0, 50}, BLUE);  // Z-axis
        
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
        DrawText("Press SPACE to reset particles", 10, WINDOW_HEIGHT - 30, 18, LIGHTGRAY);
        
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
        if (IsKeyPressed(KEY_SPACE)) {
            simulation.resetParticles();
        }
        
        // Draw simulation
        simulation.draw();
    }
    
    CloseWindow();
    return 0;
}
