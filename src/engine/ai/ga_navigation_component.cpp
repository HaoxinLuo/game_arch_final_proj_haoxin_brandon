#include "ga_navigation_component.h"

#include "entity/ga_entity.h"

#include <iostream>

ga_navigation_component::ga_navigation_component(ga_entity * ent, ga_navigation_mesh * mesh) : ga_component(ent)
{
	_mesh = mesh;
	_pending_move = ga_vec3f::zero_vector();
	_speed = 1.0f;
	_player_triangle_id = _mesh->get_triangle_of_point(get_entity()->get_transform().get_translation(), _player_triangle_id);
}

ga_navigation_component::~ga_navigation_component()
{
}

void ga_navigation_component::update(ga_frame_params * params)
{
}

void ga_navigation_component::late_update(ga_frame_params * params)
{
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	ga_vec3f move = _pending_move.scale_result(dt);
	if (valid_move(&move) && move.mag() > FLT_EPSILON) {
		get_entity()->translate(move);
		_player_triangle_id = _mesh->get_triangle_of_point(get_entity()->get_transform().get_translation(), _player_triangle_id);
		get_entity()->set_direction(atan2(-move.z, move.x) * 180.0f / GA_PI);
	}	
	_pending_move = ga_vec3f::zero_vector();
}

bool ga_navigation_component::valid_move(ga_vec3f* move)
{
	ga_vec3f cur_position = get_entity()->get_transform().get_translation();
	ga_vec3f new_position = cur_position + *move;
	if (_mesh->legal_position(&new_position))
	{
		return true;
	}
	return false;
}


std::vector<int> ga_navigation_component::get_neighbors(int trig_id)
{
	std::vector<int> tmp = _mesh->get_neighbors(trig_id);
	std::vector<int> ans;
	for (int i = 0; i < tmp.size(); ++i)
	{
		if (_mesh->get_triangle(tmp[i])->is_traversable())
		{
			ans.push_back(tmp[i]);
		}
	}
	return ans;
}

void ga_navigation_component::move_toward_dir_of_pt(ga_vec3f* pt)
{
	_pending_move = *pt - get_entity()->get_transform().get_translation();
	if (_pending_move.mag() > _speed)
	{
		_pending_move.normalize();
		_pending_move.scale(_speed);
	}	
	else/* if (_pending_move.mag() < _speed / 2.0f)*/
	{
		_pending_move.normalize();
		_pending_move.scale(_speed / 2.0f);
	}
}

void ga_navigation_component::move_toward_dir(ga_vec3f* direction)
{
	_pending_move = ga_vec3f(*direction);
	if (_pending_move.mag() > FLT_EPSILON)
	{
		_pending_move.normalize();
	}
	_pending_move.scale(_speed);
}

bool ga_navigation_component::valid_location(ga_vec3f * loc)
{
	return _mesh->legal_position(loc);
}
