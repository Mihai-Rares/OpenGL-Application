#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        using namespace std;
        //ofstream f("coordinates.txt");
        ifstream f("coordinates.txt");
        while (f) {
            float x, y, z;
            f >> x >> y >> z;
            coordinates.emplace_back(x, y, z);
            f >> x >> y >> z;
            fronts.emplace_back(x, y, z);
            f >> x >> y >> z;
            ups.emplace_back(x, y, z);
        }
        f.close();
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUp));
        front = cameraFrontDirection;
        right = cameraRightDirection;
        up = cameraUpDirection;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, up);
    }
    glm::mat4 Camera::getLightViewMatrix() {
        glm::vec3 offset = glm::normalize(cameraFrontDirection + cameraRightDirection - cameraUpDirection);
        return glm::lookAt(cameraPosition + offset, cameraTarget + offset, cameraUpDirection);
    }
    glm::vec3 Camera::getLightPosition() {
        glm::vec3 offset = front + 0.3f * right - 0.3f * up;
        return cameraPosition + offset;
    }
    glm::vec3 Camera::getLightFront() {
        return front;
    }
    glm::mat4 Camera::getAlignmentMatrix() {
        glm::mat4 a(1.0f);
        a[0] = glm::vec4(right, 0.0f);
        a[1] = glm::vec4(front, 0.0f);
        a[2] = glm::vec4(up, 0.0f);
        return a;
    }
    glm::mat4 Camera::computeLightSpaceTrMatrix() {
        glm::vec3 poz = getLightPosition();
        return glm::lookAt(poz, poz + front, up);
    }
    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 dir;
        switch (direction)
        {
        case MOVE_FORWARD:
            dir = speed * front;
            break;
        case MOVE_BACKWARD:
            dir = -speed * front;
            break;
        case MOVE_RIGHT:
            dir = speed * right;
            break;
        case MOVE_LEFT:
            dir = -speed * right;
            break;
        default:
            break;
        }
        glm::mat4 t = glm::translate(glm::mat4(1.0f), dir);
        cameraPosition = glm::vec3(t * glm::vec4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f));
        cameraTarget = glm::vec3(t * glm::vec4(cameraTarget, 1.0f));
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        this->pitch += pitch;
        this->yaw += yaw;
        if (this->pitch > glm::radians(89.0f))
            this->pitch = glm::radians(89.0f);
        if (this->pitch + pitch < glm::radians(-89.0f))
            this->pitch = glm::radians(-89.0f);
        front = glm::normalize(glm::yawPitchRoll(this->yaw, this->pitch, 0.0f) * glm::vec4(cameraFrontDirection, 0.0f));
        right = glm::normalize(glm::cross(front, cameraUpDirection));
        up = glm::cross(right, front);
        cameraTarget = cameraPosition + front;
        /*if(pitch > glm::radians(89.0f))
            pitch = glm::radians(89.0f);
        if (pitch < glm::radians(-89.0f))
            pitch = glm::radians(-89.0f);
        if (yaw > glm::radians(89.0f))
            yaw = glm::radians(89.0f);
        if (yaw < glm::radians(-89.0f))
            yaw = glm::radians(-89.0f);
        glm::mat4 t = glm::translate(glm::mat4(1.0f), -cameraPosition);
        glm::mat4 tb = glm::translate(glm::mat4(1.0f), cameraPosition);
        glm::mat4 horizontal = glm::rotate(yaw, cameraUpDirection);
        glm::vec4 cameraTargetAux = horizontal * t * glm::vec4(cameraTarget, 1.0f);
        cameraRightDirection = glm::normalize(glm::cross(glm::vec3(cameraTargetAux), cameraUpDirection));

        glm::mat4 vertical = glm::rotate(pitch, cameraRightDirection);
        cameraTarget = glm::vec3(tb * vertical * cameraTargetAux);
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraUpDirection = glm::cross(cameraRightDirection, cameraFrontDirection);*/
    }
    void Camera::startAnimation() {
        index = 0;
        iterator = 0.0f;
        animation = true;
    }
    bool Camera::nextFrame() {
        cameraPosition = (1 - iterator) * coordinates[index] + iterator * coordinates[index + 1];
        front = (1 - iterator) * fronts[index] + iterator * fronts[index + 1];
        up = (1 - iterator) * ups[index] + iterator * ups[index + 1];
        right = glm::cross(front, up);
        cameraTarget = cameraPosition + front;
        iterator += 0.005;
        if (iterator >= 1.0f) {
            iterator = 0.0f;
            ++index;
        }
        if (index == coordinates.size() - 1) {
            index = 0;
            animation = false;
        }
        return animation;
    }
    void Camera::saveCoordinates() {
        using namespace std;
        ofstream f;
        f.open("coordinates.txt", ios::app);
        f << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z 
            << front.x << " " << front.y << " " << front.z
                << up.x << " " << up.y << " " << up.z << endl;
        f.close();
        cout << "printed\n";
    }
}