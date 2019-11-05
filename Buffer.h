#ifndef BUFFER_H
#define BUFFER_H

#include <GL/glew.h>

class Buffer {
public:
	unsigned int ID;
	GLenum type;

	Buffer(int count, GLenum type) : type(type) {
		glGenBuffers(1, &ID);
	}
	void Bind() {
		glBindBuffer(type, ID);
	}
	void Unbind() {
		glBindBuffer(type, 0);
	}
	void Data(const void *data, GLsizeiptr size, GLenum drawType) {
		glBindBuffer(type, ID);
		glBufferData(type, size, data, drawType);
	}
	void VAP(unsigned int id, unsigned int size, GLenum type, int stride, const void* offset) {
		glBindBuffer(type, ID);
		glVertexAttribPointer(id, size, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(id);
	}
};

#endif
