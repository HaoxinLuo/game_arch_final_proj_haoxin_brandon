#include "ga_unit_component.h"
#include "graphics\ga_material.h"

#include "entity/ga_entity.h"

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

ga_unit_component::ga_unit_component(ga_entity* ent, const char* texture_file) : ga_component(ent)
{
	_material = new ga_unlit_texture_material(texture_file);
	_material->init();
	ga_mat4f transform;
	transform.make_scaling(0.5f);
	transform.data[1][1] = 1.0f;
	transform = transform * get_entity()->get_transform();
	get_entity()->set_transform(transform);
	static GLfloat color[] =
	{
		// Bottom
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		// Top
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};
	static GLfloat vertices[] = {
		// Bottom Triangle
		-0.5, -0.2,  -1.0,
		0.5, -0.2,  -1.0,
		0.0,  -0.2,  1.0,
		// Top Triangle
		-0.5, 0.2,  -1.0,
		0.5,  0.2,  -1.0,
		0.0,  0.2,   1.0,
	};
	static GLfloat texcoords[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};
	static GLushort indices[] = {
		// Bottom
		0,  1,  2,
		// Top
		5,  4,  3,
		// Front Left
		0,  5,  3,
		0,  2,  5,
		// Front Right
		1,  5,  2,
		1,  4,  5,
		// Back
		0,  3,  1,
		1,  3,  4,
	};

	_index_count = uint32_t(sizeof(indices) / sizeof(*indices));

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(4, _vbos);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

ga_unit_component::~ga_unit_component()
{
	glDeleteBuffers(4, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void ga_unit_component::update(ga_frame_params* params)
{
	//ga_vec3f direction = get_entity()->get_direction();
	//float angle = acos(direction.dot(previousdirection) / (direction.mag() * previousdirection.mag()));
	//previousdirection = direction;

	//if (!isnan(angle)) {
	//	ga_quatf axis_angle;
	//	axis_angle.make_axis_angle(ga_vec3f::y_vector(), angle);
	//	get_entity()->rotate(axis_angle);
	//}

	ga_static_drawcall draw;
	draw._name = "ga_unit_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw);
	params->_static_drawcall_lock.clear(std::memory_order_release);
}
