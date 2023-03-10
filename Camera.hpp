#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <vector>
#include <fstream>
#include <iostream>
namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT};
    
    class Camera
    {
    public:
        //Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        //return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();
        glm::mat4 getLightViewMatrix();
        glm::vec3 getLightPosition();
        glm::vec3 getLightFront();
        glm::mat4 getAlignmentMatrix();
        glm::mat4 computeLightSpaceTrMatrix();
        void startAnimation();
        bool nextFrame();
        void saveCoordinates();
        //update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        //update the camera internal parameters following a camera rotate event
        //yaw - camera rotation around the y axis
        //pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

        
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection, front;
        glm::vec3 cameraRightDirection, right;
        glm::vec3 cameraUpDirection, up;
        float pitch = 0, yaw = 0;
        bool animation = false;
        std::vector<glm::vec3> coordinates, fronts, ups;
        int index = 0;
        float iterator = 0.0f;
    };
    
}

#endif /* Camera_hpp */
