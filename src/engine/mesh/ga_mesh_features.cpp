#include "mesh/ga_mesh_features.h"

bool ga_mesh_edge::is_boundary() const
{
	if (_opposite == nullptr || _tri->is_traversable() != _opposite->_tri->is_traversable())
	{
		return true;
	}
	return false;
}

void ga_mesh_edge::set_opposite(ga_mesh_edge* o) 
{
	if (o == nullptr)
		return;
	_opposite = o; 
	o->_opposite = this;
}

ga_vec3f ga_mesh_triangle::get_centroid() const
{
	return (*(*this)[0]->get_pos() + *(*this)[1]->get_pos() + *(*this)[2]->get_pos()).scale_result(1 / 3.0f);
}