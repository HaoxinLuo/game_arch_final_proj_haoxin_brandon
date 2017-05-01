#include "ga_ai_component.h"


#include "entity/ga_entity.h"
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

ga_ai_component::ga_ai_component(ga_entity * ent, ga_navigation_component* nav, ga_entity_world* w) : ga_component(ent)
{
	_nav = nav;
	_world = w;
	_index = _world->get_entities().size();
	get_entity()->set_id(_index);
	_world->add_entity(get_entity());
}

ga_ai_component::~ga_ai_component()
{
	_world->remove_entity(get_entity());
}

void ga_ai_component::update(ga_frame_params * params)
{
	ga_vec3f current_location = get_entity()->get_transform().get_translation();
	if (params->_button_mask > 0 || params->_mouse_click_mask > 0)
	{
		// checks for movement input
		uint64_t interrupt_buttons = k_button_w | k_button_a | k_button_s | k_button_d | k_button_z;
		if (params->_button_mask & interrupt_buttons)
		{
			_path.clear();
			ga_vec3f translate_matrix = ga_vec3f::zero_vector();
			if (params->_button_mask & k_button_d)
			{
				translate_matrix += ga_vec3f::x_vector();
			}
			if (params->_button_mask & k_button_a)
			{
				translate_matrix -= ga_vec3f::x_vector();
			}
			if (params->_button_mask & k_button_s)
			{
				translate_matrix += ga_vec3f::z_vector();
			}
			if (params->_button_mask & k_button_w)
			{
				translate_matrix -= ga_vec3f::z_vector();
			}
			_nav->move_toward_dir(&translate_matrix);
		}

		// check for size change
		if (params->_button_mask & (k_button_q | k_button_e))
		{
			float scale_factor = 1.0f;
			if (params->_button_mask & k_button_q)
			{
				scale_factor += 0.1f;
			}
			if (params->_button_mask & k_button_e)
			{
				scale_factor -= 0.1f;
			}
			ga_mat4f transform;
			transform.make_scaling(scale_factor);
			transform.data[1][1] = 1.0f;
			transform = transform * get_entity()->get_transform();
			get_entity()->set_transform(transform);
		}

		// check for path finding
		if (params->_mouse_click_mask & uint64_t(2)) // left mouse button click
		{
			std::vector<ga_entity*> entities = _world->get_entities();
			if (entities.size() > 1) {
				int otherindex = _index;
				while (otherindex == _index) {
					otherindex = rand() % entities.size();
				}
				ga_entity* other = entities[otherindex];
				ga_vec3f otherlocation = other->get_transform().get_translation();
				path_find(&current_location, &otherlocation);
				std::cerr << "found path of length " << _path.size() << "\n";
			}
			else {
				std::cerr << "no other entity to find path to\n";
			}
		}
		if (params->_button_mask &  k_button_x)
		{
			int width = _nav->get_navigation_mesh()->get_width();
			int height = _nav->get_navigation_mesh()->get_height();
			ga_vec3f targets[] = {
				{0.5f, 0.0f, 0.5f},
				{width - 0.5f, 0.0f, 0.5f},
				{ 0.5f, 0.0f, height - 0.5f },
				{ width / 2.0f, 0.0f, 0.5f},
				{ 0.5f, 0.0f, height / 2.0f },
			};
			ga_vec3f target = targets[rand() % (sizeof(targets) / sizeof(ga_vec3f))];
			std::cerr << "Path find to " << target.x << "," << target.z << "\n";
			if (!path_find(&current_location, &target))
				std::cerr << "no path found: " << _path.size() << "\n";
		}
	}
	else if (_path.size() > 0) {
		if ((get_entity()->get_transform().get_translation() - _path.back()).mag() < 0.01f + FLT_EPSILON)
		{
			_path.pop_back();
		}
		if (_path.size() >= 1)
		{
			//change direction if there is an entity ahead
			ga_vec3f move_direction = (_path[0] - current_location).normal();
			ga_vec3f center = current_location + move_direction;
			if (!_world->has_entity(_index, center, _nav->get_speed() / 2.0f))
			{
				//std::cerr << "IMMA CRASH (" << _index << ")\n";
				_nav->move_toward_dir_of_pt(&_path.back());
			}			
		}
	}
}
