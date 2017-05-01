#pragma once
#include "entity/ga_entity_world.h"
#include "entity/ga_component.h"
#include "ai/ga_navigation_component.h"

class ga_ai_component : public ga_component
{
public:
	ga_ai_component(class ga_entity* ent, ga_navigation_component* nav, ga_entity_world* w);
	virtual ~ga_ai_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	bool path_find(ga_vec3f* current_loc, ga_vec3f* target_loc);
	void smooth_path(std::vector<int> &path, ga_vec3f* start_loc, ga_vec3f* target_loc);

	ga_navigation_component* _nav;
	std::vector<ga_vec3f> _path;

	ga_entity_world* _world;

	int _index;
	int avoidradius = 5;

	//ga_vec3f direction = ga_vec3f::x_vector();
};