#pragma once
#include "physics\ga_shape.h"

struct ga_triangular_prism final : ga_shape 
{
	ga_vec3f _center = ga_vec3f::zero_vector();
	ga_vec3f _half_vectors[3];
	float _scale = 0.5f;

	ga_shape_t get_type() const override { return k_shape_triprism; }
	void get_debug_draw(const ga_mat4f& transform, struct ga_dynamic_drawcall* drawcall) override;
	void get_inertia_tensor(ga_mat4f& tensor, float mass) override;
	ga_vec3f get_offset_to_point(const ga_mat4f& transform, const ga_vec3f& point) const override;

	void get_corners(std::vector<ga_vec3f>& corners) const;
};