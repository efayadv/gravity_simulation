#define GLFW_INCLUDE_NONE        // prevent GLFW from including any GL headers
#include <GLFW/glfw3.h>

#ifdef __APPLE__
  #include <OpenGL/gl.h>        // Use gl.h instead of gl3.h for compatibility
#else
  #include <GL/gl.h>
#endif
#include <cstdio>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>

float screenHeight = 600.0f;
float screenWidth = 800.0f;

//Game state
bool gravityMouseMode = false;
double mouseX = 0.0, mouseY = 0.0;

GLFWwindow* StartGLFW();
void DrawCircle(float centerX, float centerY, float radius, int res);
void DrawButton(float x, float y, float width, float height, const char* label, bool active);
bool IsPointInRect(float px, float py, float rx, float ry, float rw, float rh);


class Object {
    public:

    std::vector<float> position;
    std::vector<float> velocity;
    float radius;
    float r, g, b; 


    Object(std::vector<float> position, std::vector<float> velocity, float radius = 15.0f){
        this->position = position;
        this->velocity = velocity;
        this->radius = radius;

        // assigning a random color 
        this->r = static_cast<float>(rand()) / RAND_MAX;
        this->g = static_cast<float>(rand()) / RAND_MAX;
        this->b = static_cast<float>(rand()) / RAND_MAX;
    }       

    void accelerate(float x, float y){
        this->velocity[0] += x;
        this->velocity[1] += y;

    };

    void updatePos() {
        this->position[0] += this->velocity[0];
        this->position[1] += this->velocity[1];
    };

    void DrawCircle(float centerX, float centerY, float radius, int res){
        glColor3f(r, g, b);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(centerX, centerY);
        for (int i = 0; i <= res; ++i){
            float angle = 2.0f * 3.141592653589 * (static_cast<float>(i) / res);
            float x = centerX + cos(angle) * radius;
            float y = centerY + sin(angle) * radius;
            glVertex2d(x, y);
        }
        glEnd();
    }

};

bool findCollision(Object& obj1, Object& obj2) {

        float horizontal_dist = obj2.position[0] - obj1.position[0];
        float vertical_dist = obj2.position[1] - obj1.position[1];
        float distance = sqrt(horizontal_dist * horizontal_dist + vertical_dist * vertical_dist);

        return distance < (obj1.radius + obj2.radius);

}

void workCollision(Object& obj1, Object& obj2){
        
        float horizontal_dist = obj2.position[0] - obj1.position[0];
        float vertical_dist = obj2.position[1] - obj1.position[1];
        float distance = sqrt(horizontal_dist * horizontal_dist + vertical_dist * vertical_dist);

        if (distance == 0) distance = 0.01f; // to prevent division by zero

        float nx = horizontal_dist / distance;
        float ny = vertical_dist / distance;

        float overlap = (obj1.radius + obj2.radius) - distance;
        obj1.position[0] -= nx * overlap * 0.5f;
        obj1.position[1] -= ny * overlap * 0.5f;
        obj2.position[0] += nx * overlap * 0.5f;
        obj2.position[1] += ny * overlap * 0.5f;
        
        float dvx = obj2.velocity[0] - obj1.velocity[0];
        float dvy = obj2.velocity[1] - obj1.velocity[1];
        
        float velocityAlongNormal = dvx * nx + dvy * ny;
        
        if (velocityAlongNormal > 0) return;
        
        float restitution = 0.8f;
        
        float impulse = -(1.0f + restitution) * velocityAlongNormal;
        impulse /= 2.0f; 
        
        obj1.velocity[0] -= impulse * nx;
        obj1.velocity[1] -= impulse * ny;
        obj2.velocity[0] += impulse * nx;
        obj2.velocity[1] += impulse * ny;
}

void applyGravityMouse(Object& obj, float mouseX, float mouseY) {
    float dx = mouseX - obj.position[0];
    float dy = mouseY - obj.position[1];
    float distance = sqrt(dx * dx + dy * dy);

    if (distance < 1.0f) return;

    //placeholder for the constant
    float gravitationalConstant = 5000.0f;
    float force = gravitationalConstant / (distance * distance);

    float nx = dx / distance;
    float ny = dy / distance;

    obj.accelerate(nx * force, ny * force);
}

//Mouse callback
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;

        glfwGetCursorPos(window, &xpos, &ypos);

        float glX = static_cast<float>(xpos);
        float glY = screenHeight - static_cast<float>(ypos);

        std::vector<Object>* objs = static_cast<std::vector<Object>*>(glfwGetWindowUserPointer(window));

        float buttonY = screenHeight - 50;
        float button1X = 50;
        float button2X = 250;
        float buttonWidth = 150;
        float buttonHeight = 40;

        if (IsPointInRect(glX, glY, button1X, buttonY, buttonWidth, buttonHeight)) {
            float randX = 100 + static_cast<float>(rand()) / RAND_MAX * (screenWidth - 200);
            float randY = 100 + static_cast<float>(rand()) / RAND_MAX * (screenHeight - 200);
            float randVelX = -2.0f + static_cast<float>(rand()) / RAND_MAX * 4.0f;
            float randVelY = -2.0f + static_cast<float>(rand()) / RAND_MAX * 4.0f;
            
            objs->push_back(Object(std::vector<float>{randX, randY}, std::vector<float>{randVelX, randVelY}));
        }

        else if (IsPointInRect(glX, glY, button2X, buttonY, buttonWidth, buttonHeight)) {
            gravityMouseMode = !gravityMouseMode;
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = screenHeight - ypos; // Flip Y coordinate
}

int main(){
    srand(time(NULL));

    GLFWwindow* window = StartGLFW();
    if (!window) return -1;

    float radius = 15.0f;   
    int res = 100;

    std::vector<Object> objs = {
        Object(std::vector<float>{200.0f, 500.0f}, std::vector<float>{5.0f, 0.0f}),
        Object(std::vector<float>{700.0f, 500.0f}, std::vector<float>{5.0f, 0.0f}),
        Object(std::vector<float>{500.0f, 300.0f}, std::vector<float>{5.0f, 0.0f})
    };
    
    glfwSetWindowUserPointer(window, &objs);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT);

        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
 
        //DrawCircle(position[0], position[1], 50.0f, 50);
        
        for(auto& obj : objs) {
            if (gravityMouseMode) {
                // Apply gravitational attraction to mouse
                applyGravityMouse(obj, mouseX, mouseY);
            } else {
                // Normal gravity
                obj.accelerate(0.0f, -0.15f);
            }
            
            obj.updatePos();
            
            // Velocity damping (air resistance)
            obj.velocity[0] *= 0.995f;
            obj.velocity[1] *= 0.995f;
            
            // Draw object
            obj.DrawCircle(obj.position[0], obj.position[1], obj.radius, res);  

            // Boundary collision
            if(obj.position[1] < obj.radius) {
                obj.position[1] = obj.radius;
                obj.velocity[1] *= -0.7;
            }

            if(obj.position[1] > screenHeight - obj.radius) {
                obj.position[1] = screenHeight - obj.radius;
                obj.velocity[1] *= -0.7;
            }

            if(obj.position[0] < obj.radius) {
                obj.position[0] = obj.radius;
                obj.velocity[0] *= -0.7;
            }

            if(obj.position[0] > screenWidth - obj.radius) {
                obj.position[0] = screenWidth - obj.radius;
                obj.velocity[0] *= -0.7;
            }
        }
        
        //collision loop
        for(size_t i = 0; i < objs.size(); i++) {
            for(size_t j = i + 1; j < objs.size(); j++) {
                if(findCollision(objs[i], objs[j])) {
                    workCollision(objs[i], objs[j]);
                }
            }
        }

        float buttonY = screenHeight - 50;
        DrawButton(50, buttonY, 150, 40, "Add Object", false);
        DrawButton(250, buttonY, 150, 40, "Gravity Mouse", gravityMouseMode);

        if (gravityMouseMode) {
            glColor3f(1.0f, 1.0f, 0.0f);
            DrawCircle(mouseX, mouseY, 10.0f, 30);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow* StartGLFW(){
    if(!glfwInit()) {
        std::cerr<<"failed to initialize glfw, panic!"<<std::endl;
        return nullptr;
    }
    
    // Request OpenGL 2.1 compatibility profile (supports legacy functions)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "gravity_sim", NULL, NULL);
    
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    
    glfwMakeContextCurrent(window);  // This is crucial!  
    
    // Setup orthographic projection for pixel coordinates
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return window;
}

void DrawCircle(float centerX, float centerY, float radius, int res){
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(centerX, centerY);
    for (int i = 0; i <= res; ++i){
        float angle = 2.0f * 3.141592653589 * (static_cast<float>(i) / res);
        float x = centerX + cos(angle) * radius;
        float y = centerY + sin(angle) * radius;
        glVertex2d(x, y);
    }
    glEnd();
}

void DrawButton(float x, float y, float width, float height, const char* label, bool active) {
    if (active) {
        glColor3f(0.3f, 0.6f, 0.3f);
    } else {
        glColor3f(0.5f, 0.5f, 0.5f);
    }

    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

bool IsPointInRect(float px, float py, float rx, float ry, float rw, float rh) {
    return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}



