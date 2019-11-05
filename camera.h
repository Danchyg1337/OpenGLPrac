#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
	
	glm::vec3 v_forward;
	glm::vec3 v_up;
	glm::vec3 v_position;

public:
	Camera(const glm::vec3 &pos, float fov, float aspect, float zNear, float zFar) {
		m_perspective = glm::perspective(fov, aspect, zNear, zFar);
		v_position = pos;
		v_forward = glm::vec3(0, 0, 1);
		v_up = glm::vec3(0, 1, 0);
	}
	void update(const glm::vec3 &pos, const glm::vec3 &cam) {
		v_position = pos;

		if (cam.x == 0 && cam.y == 0 && cam.z == 0)
			return;
		v_forward = cam;
	}
	glm::mat4 GetViewProjection() {
		return m_perspective * glm::lookAt(v_position, v_position + v_forward, v_up);
	}
	glm::mat4 GetView() {
		return glm::lookAt(v_position, v_position + v_forward, v_up);
	}
	glm::mat4 GetProjection() {
		return m_perspective;
	}

	glm::mat4 m_perspective;
};

#endif