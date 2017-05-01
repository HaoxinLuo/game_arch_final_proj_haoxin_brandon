#pragma once
#include "entity\ga_component.h"
#include "mesh\ga_navigation_mesh.h"

class ga_floor_component : ga_component
{
public:
	ga_floor_component(ga_entity* ent, ga_navigation_mesh* mesh, const char* texture_file);
	~ga_floor_component();

	virtual void update(struct ga_frame_params* params) override;
private:
	ga_navigation_mesh* _mesh;

	class ga_material* _material;
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;
};
