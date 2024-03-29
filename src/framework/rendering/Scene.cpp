#include "Scene.h"

namespace Framework
{
	void Scene::rebuildOctree()
	{
		_octree.clear();
		for (const auto& obj : _objects)
		{
			if (obj->collideable())
				_octree.insert(obj->rigidbody());
		}
	}

	Scene::Scene()
		: _octree(4096)
	{}

	void Scene::add(Renderable* object)
	{
		if (object->collideable())
			_octree.insert(object->rigidbody());
		_objects.push_back(object);
	}

	void Scene::render(glm::mat4 proj, glm::mat4 view)
	{
		for (const auto& obj : _objects)
		{
			if (obj->isActive())
				obj->draw(view, proj);
		}
	}

	void Scene::update(double delta)
	{
		rebuildOctree();

		for (auto& obj : _objects)
		{
			_octreeNeighbours.clear();

			obj->update(delta);

			if (!obj->collideable()) continue;

			// calculate collision with octree neighbours
			_octreeNeighbours = _octree.neighbours(obj->rigidbody());
			for (auto& neighbour : _octreeNeighbours)
			{
				if (obj->rigidbody() == neighbour) continue;
				
				Collision c;
				if (GJK::intersect(*obj->rigidbody(), *neighbour, &c))
				{
					obj->onCollide(c, neighbour);
				}
			}
		}
	}
}