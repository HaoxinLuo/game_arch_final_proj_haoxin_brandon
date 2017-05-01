/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_camera.h"

#include "ga_frame_params.h"
#include "math/ga_math.h"

#include "math/ga_mat4f.h"

ga_camera::ga_camera(const ga_vec3f& eye)
{
	_transform.make_translation(eye);
	_mesh = nullptr;
}

ga_camera::~ga_camera()
{
}

void ga_camera::update(ga_frame_params* params)
{
	if (params->_button_mask & k_button_r && _mesh != nullptr) {
		ga_mat4f centerMeshTrans;
		//centerMeshTrans.make_orthographic(0.0f, (float)_mesh->get_width(), (float)_mesh->get_height(), 0.0f, (float)_mesh->get_height(), 0.0f);
		//centerMeshTrans.make_perspective_rh(-55.0f, 1.0f, 0.0f, 10.0f);
		centerMeshTrans.make_identity();
		centerMeshTrans.make_translation({ _mesh->get_width() / 2.0f, 200.0f, _mesh->get_height() * 1.1f });
		_transform = centerMeshTrans;
		ga_quatf rotation;
		rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
		rotate(rotation);
		rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(55.0f));
		rotate(rotation);


		ga_vec3f eye = _transform.get_translation();
		ga_vec3f at = eye + _transform.get_forward();
		ga_vec3f up = ga_vec3f::y_vector();

		ga_mat4f view;
		view.make_lookat_rh(eye, at, up);

		params->_view = view;
	}
	else {
		const float k_move_speed = 0.1f;
		const float k_rotate_speed = 0.5f;

		// Use WASD to control the position.
		ga_vec3f translation = { 0.0f, 0.0f, 0.0f };
		translation += (params->_button_mask & k_button_l) ? -_transform.get_right() : ga_vec3f::zero_vector();
		translation += (params->_button_mask & k_button_j) ? _transform.get_right() : ga_vec3f::zero_vector();
		translation += (params->_button_mask & k_button_k) ? -_transform.get_forward() : ga_vec3f::zero_vector();
		translation += (params->_button_mask & k_button_i) ? _transform.get_forward() : ga_vec3f::zero_vector();
		translation += (params->_button_mask & k_button_u) ? -_transform.get_up() : ga_vec3f::zero_vector();
		translation += (params->_button_mask & k_button_o) ? _transform.get_up() : ga_vec3f::zero_vector();
		translation.scale(k_move_speed);

		// By using the camera's directional vectors, we've defined the translation in world space.
		ga_mat4f world_translation;
		world_translation.make_translation(translation);
		_transform = _transform * world_translation;

		// Use arrow keys to pitch and rotate.
		float rotation = 0.0f;
		rotation += (params->_button_mask & k_button_left) ? k_rotate_speed : 0.0f;
		rotation += (params->_button_mask & k_button_right) ? -k_rotate_speed : 0.0f;
		float pitch = 0.0f;
		pitch += (params->_button_mask & k_button_up) ? -k_rotate_speed : 0.0f;
		pitch += (params->_button_mask & k_button_down) ? k_rotate_speed : 0.0f;

		rotation = ga_degrees_to_radians(rotation);
		pitch = ga_degrees_to_radians(pitch);

		ga_quatf rotation_axis_angle;
		rotation_axis_angle.make_axis_angle(ga_vec3f::y_vector(), rotation);
		ga_mat4f rotation_matrix;
		rotation_matrix.make_rotation(rotation_axis_angle);

		rotation_axis_angle.make_axis_angle(ga_vec3f::x_vector(), pitch);
		rotation_matrix.rotate(rotation_axis_angle);
		_transform = rotation_matrix * _transform;

		ga_vec3f eye = _transform.get_translation();
		ga_vec3f at = eye + _transform.get_forward();
		ga_vec3f up = ga_vec3f::y_vector();

		ga_mat4f view;
		view.make_lookat_rh(eye, at, up);

		params->_view = view;
	}
}

void ga_camera::rotate(const ga_quatf& rotation)
{
	ga_mat4f rotation_matrix;
	rotation_matrix.make_rotation(rotation);
	_transform = rotation_matrix * _transform;
}
