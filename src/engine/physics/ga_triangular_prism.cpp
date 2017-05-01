#include "ga_triangular_prism.h"

#include "framework/ga_drawcall.h"
#include "math/ga_math.h"

void ga_triangular_prism::get_corners(std::vector<ga_vec3f>& corners) const
{
	ga_vec3f x_hvec = _half_vectors[0].scale_result(_scale);
	ga_vec3f y_hvec = _half_vectors[1].scale_result(_scale);
	ga_vec3f z_hvec = _half_vectors[2];

	corners.push_back(_center - x_hvec - y_hvec);
	corners.push_back(_center + x_hvec - y_hvec);
	corners.push_back(_center - y_hvec + z_hvec);
	corners.push_back(_center - x_hvec + y_hvec);
	corners.push_back(_center + x_hvec + y_hvec);
	corners.push_back(_center + y_hvec + z_hvec);
}

void ga_triangular_prism::get_debug_draw(const ga_mat4f& transform, ga_dynamic_drawcall* drawcall)
{
	get_corners(drawcall->_positions);
	drawcall->_positions.push_back(ga_vec3f::zero_vector());
	drawcall->_positions.push_back(_half_vectors[0]);
	drawcall->_positions.push_back(_half_vectors[1]);
	drawcall->_positions.push_back(_half_vectors[2]);

	uint32_t indices[] =
	{
		0, 1,
		1, 2,
		2, 0,
		3, 4,
		4, 5,
		5, 3,
		0, 3,
		1, 4,
		2, 5,
	};

	for (int i = 0; i < sizeof(indices) / sizeof(indices[0]); ++i)
	{
		drawcall->_indices.push_back(indices[i]);
	}

	drawcall->_color = { 0.0f, 1.0f, 0.0f };
	drawcall->_draw_mode = GL_LINES;
	drawcall->_transform = transform;
	drawcall->_material = nullptr;
}

void ga_triangular_prism::get_inertia_tensor(ga_mat4f& tensor, float mass)
{
	// unimplemented
}

ga_vec3f ga_triangular_prism::get_offset_to_point(const ga_mat4f& transform, const ga_vec3f& point) const
{
	ga_vec3f center = transform.transform_point(_center);
	return point - center;
}