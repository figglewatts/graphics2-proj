#ifndef MESH_H
#define MESH_H
#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Framework
{
	class Mesh
	{
	private:
		std::vector<glm::vec3> _verts;
		std::vector<unsigned> _indices;
		unsigned _vao;
		unsigned _vbo;
		unsigned _ebo;

	public:
		Mesh(std::vector<glm::vec3> verts, std::vector<unsigned> indices);
		~Mesh();

		void draw() const;
	};
}

#endif // MESH_H