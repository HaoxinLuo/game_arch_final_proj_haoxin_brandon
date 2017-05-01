#pragma once

#include "entity/ga_component.h"
#include "mesh/ga_navigation_mesh.h"

class ga_navigation_component : public ga_component 
{
public:
	ga_navigation_component(class ga_entity* ent, class ga_navigation_mesh* mesh);
	virtual ~ga_navigation_component();

	virtual void update(struct ga_frame_params* params) override;
	virtual void late_update(struct ga_frame_params* params) override;

	bool valid_move(ga_vec3f * move);

	unsigned int get_max_x() { return _mesh->get_width(); }
	unsigned int get_max_z() { return _mesh->get_height(); }
	int get_current_triangle() const { return _player_triangle_id; }

	std::vector<int> get_neighbors(int trig_id);
	ga_mesh_triangle* get_triangle(int trig_id) { return _mesh->get_triangle(trig_id); }
	int get_triangle_of_point(ga_vec3f& pos) { return _mesh->get_triangle_of_point(pos); }


	void move_toward_dir_of_pt(ga_vec3f * pt);
	void move_toward_dir(ga_vec3f * direction);
	
	bool set_speed(float spd) { _speed = spd; return true; }
	float get_speed() const { return _speed; }
	bool valid_location(ga_vec3f *loc);

	class ga_navigation_mesh* get_navigation_mesh() const {  return _mesh;  }

private:
	class ga_navigation_mesh* _mesh;
	struct ga_vec3f _pending_move;
	float _speed;
	int _player_triangle_id;
};