#ifndef VAO_H
#define VAO_H

#include <GL/glew.h>

class VAO {
public:
	unsigned int ID;

	VAO(int count) {
		glGenVertexArrays(1, &ID);
		glBindVertexArray(ID);
	}
	void Bind() {
		glBindVertexArray(ID);
	}
};

#endif
