#pragma once

namespace prmtvs{
	struct PrCube {
	public:
		std::vector<Vertex> verts{
			//positions								//normal				    //texture
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f),  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f),  glm::vec2(0.0f, 1.0f)),
			
			Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 0.0f)),
			
			Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec2(1.0f, 0.0f)),
			
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f),	  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f),	  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f),	  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f),	  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f),	  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f),	  glm::vec2(0.0f, 0.0f)),
			
			Vertex(glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)),	
			
			Vertex(glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 1.0f)),
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f),   glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(0.5f,  0.5f, -0.5f),   glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 1.0f)),
			Vertex(glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f),   glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 1.0f)) 
		};
	} Cube;
	
	float square[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f
	};
	
	float square2[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f,  0.5f,  0.0f, 0.0f,
		-0.5f,  1.0f,  1.0f, 1.0f,
		-0.5f,  1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  1.0f, 0.0f,
		-1.0f,  0.5f,  0.0f, 0.0f
	};
}