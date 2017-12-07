#ifndef RENDERABLE_H
#define RENDERABLE_H
#pragma once

#include "framework/graphics/Texture2D.h"
#include "framework/graphics/Shader.h"
#include "framework/graphics/Mesh.h"
#include "framework/math/Transform.h"
#include "framework/physics/Rigidbody.h"

namespace Framework
{
	class Renderable
	{
		Mesh *_pMesh;
		Shader *_pShader;
		Texture2D *_pTexture;
		Transform _transform;
		bool _collision;
		Rigidbody *_pRigidbody;

		AABB calculateBoundingVolume() const;
	public:
		Renderable(Mesh *mesh, Shader *shader, Texture2D *tex, Transform t = Transform())
			: _pMesh(mesh), _pShader(shader), _pTexture(tex), _transform(t), _collision(false), _pRigidbody(nullptr) { }
		Renderable(Mesh *mesh, Shader *shader, Texture2D *tex, bool collision, Transform t = Transform());
		Renderable(Mesh *mesh, Shader *shader, Texture2D *tex, Shape *collisionShape, Transform t = Transform());
		~Renderable();

		void set_shader(Shader* shader) { this->_pShader = shader; }
		void set_texture(Texture2D* tex) { this->_pTexture = tex; }
		Transform& transform() { return this->_transform; }
		const std::vector<glm::vec3> rigidbody_points() const;
		Rigidbody *rigidbody() const { return _pRigidbody; }

		void draw(glm::mat4 view, glm::mat4 proj) const;
	};
}

#endif // RENDERABLE_H