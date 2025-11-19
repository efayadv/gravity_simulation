#define GLFW_INCLUDE_NONE        // prevent GLFW from including any GL headers
#include <GLFW/glfw3.h>

#ifdef __APPLE__
  #include <OpenGL/gl.h>        // Use gl.h instead of gl3.h for compatibility
#else
  #include <GL/gl.h>
#endif
#include <cstdio>
#include <iostream>

float screenHeight = 600.0f;
float screenWidth = 800.0f;

GLFWwindow* StartGLFW();
void DrawCircle(float centerX, float centerY, float radius, int res);

class Object {
    public:

    std::vector<float> position;
    std::vector<float> velocity;
    float radius;
    Object(std::vector<float> position, std::vector<float> velocity, float radius = 15.0f){
        this->position = position;
        this->velocity = velocity;
        this->radius = radius;
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

int main(){
    GLFWwindow* window = StartGLFW();

    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f; 
    float radius = 15.0f;   
    int res = 100;

    std::vector<Object> objs = {
        Object(std::vector<float>{200.0f, 500.0f}, std::vector<float>{5.0f, 0.0f}),
        Object(std::vector<float>{700.0f, 500.0f}, std::vector<float>{5.0f, 0.0f}),
        Object(std::vector<float>{500.0f, 300.0f}, std::vector<float>{5.0f, 0.0f})
    };
    
    //to be continued...

    while(!glfwWindowShouldClose(window)){

        glClear(GL_COLOR_BUFFER_BIT);
 
        //DrawCircle(position[0], position[1], 50.0f, 50);
        
        for(auto& obj : objs) {
            obj.accelerate(0.0f, -9.81);
            obj.updatePos();
            obj.DrawCircle(obj.position[0], obj.position[1], obj.radius, res);  

            if(obj.position[1] < 0) {
                obj.position[1] = obj.radius;
                obj.velocity[1] *= -0.95;
            }

            if(obj.position[1] > screenHeight) {
                obj.position[1] = screenHeight - obj.radius;
                obj.velocity[1] *= -0.95;
            }

            if(obj.position[0] < 0) {
                obj.position[0] = obj.radius;
                obj.velocity[0] *= -0.95;
            }

            if(obj.position[0] > screenWidth) {
                obj.position[0] = screenWidth - obj.radius;
                obj.velocity[0] *= -0.95;
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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
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

