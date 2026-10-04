#pragma once

#include <Transform_Component.hpp>
#include <Entity.hpp>
namespace open_gl_engine
{
	Transform_Component::Transform_Component(Entity * my_entity)
	{
		entity = my_entity;
		parent = nullptr;
		inicializate();
	}
	Transform_Component::Transform_Component(Entity* my_entity, Transform_Component * my_parent): parent (my_parent)
	{
		entity = my_entity;
		inicializate();
	}
}