#include "ga_floor_component.h"

#include "entity\ga_entity.h"
#include "graphics\ga_material.h"

#include <iostream>
#include "user_definitions.h"

ga_floor_component::ga_floor_component(ga_entity * ent, ga_navigation_mesh * mesh, const char* texture_file) : ga_component(ent)
{
	_mesh = mesh;
	_material = new ga_unlit_texture_material(texture_file);
	_material->init();

	static GLfloat color[] =
	{
		// Top
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};
	static GLfloat vertices[12];
	static GLfloat texcoords[] = {
		// Top
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
	};
	static GLushort indices[] = {
		// Top
		0,  1,  2,
		1,  3,  2,
	};

	int width = _mesh->get_width();
	int height = _mesh->get_height();
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;
	vertices[2] = 0.0f;

	vertices[3] = (float)width;
	vertices[4] = 0.0f;
	vertices[5] = 0.0f;

	vertices[6] = 0.0f;
	vertices[7] = 0.0f;
	vertices[8] = (float)height;

	vertices[9] = (float)width;
	vertices[10] = 0.0f;
	vertices[11] = (float)height;

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

ga_floor_component::~ga_floor_component()
{
	glDeleteBuffers(4, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void ga_floor_component::update(ga_frame_params * params)
{
#if !SHOW_TRIANGLE_FOR_MESH
	ga_static_drawcall draw;
	draw._name = "ga_floor_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw);
	params->_static_drawcall_lock.clear(std::memory_order_release);
#else
	ga_dynamic_drawcall draw;
	draw._transform = get_entity()->get_transform();
	if (params->_button_mask & k_button_x)
		draw._color = ga_vec3f::x_vector();
	else
		draw._color = ga_vec3f::zero_vector();
	_mesh->get_debug_draw(&draw);

	while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_dynamic_drawcalls.push_back(draw);
	params->_dynamic_drawcall_lock.clear(std::memory_order_release);
#endif
}