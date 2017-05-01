#pragma once
#include "ga_component.h"

class ga_unit_component : public ga_component
{
public:
	ga_unit_component(class ga_entity* ent, const char* texture_file);
	virtual ~ga_unit_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	class ga_material* _material;
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;

	ga_vec3f previousdirection = ga_vec3f::z_vector();
};