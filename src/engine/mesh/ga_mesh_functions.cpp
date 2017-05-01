#include "ga_navigation_mesh.h"
#include "framework/ga_drawcall.h"

#include <iostream>

#include "user_definitions.h"

ga_vec3f cross_product(ga_vec3f& a, ga_vec3f& b)
{
	ga_vec3f ans;
	ans.x = a.y * b.z - a.z * b.y;
	ans.y = a.z * b.x - a.x * b.z;
	ans.z = a.x * b.y - a.y * b.x;
	return ans;
}

void ga_navigation_mesh::get_debug_draw(ga_dynamic_drawcall * draw)
{	
	for (int i = 0; i < ga_mesh_triangle::get_max_id(); ++i)
	{
		auto tri_itr = _triangles.find(i);
		if (tri_itr == _triangles.end())
		{
			continue;
		}
		ga_mesh_triangle* t = tri_itr->second;
		int prev_size = draw->_positions.size();
		draw->_positions.push_back(*(*t)[0]->get_pos());
		draw->_positions.push_back(*(*t)[1]->get_pos());
		draw->_positions.push_back(*(*t)[2]->get_pos());
		ga_vec3f c = t->is_traversable() ? ga_vec3f::y_vector().scale_result(0.2f) : ga_vec3f::x_vector().scale_result(0.2f);
		for (int j = 0; j < 3; ++j)
		{
			draw->_colorarray.push_back(c);
			draw->_indices.push_back(j + prev_size);
		}
	}

#if EDGE_DEBUG
	ga_mesh_edge* cur_edge;
	ga_vec3f main_dir, side_dir, a, b, c, d;
	int prior_size;
	ga_vec3f vert = ga_vec3f::y_vector().scale_result(0.01f);
	ga_vec3f color;
	ga_mesh_triangle* player_trig = get_triangle(_tri_id);
	ga_mesh_edge* player_edge_1 = (player_trig == nullptr) ? nullptr : player_trig->get_edge();
	ga_mesh_edge* player_edge_2 = (player_trig == nullptr) ? nullptr : player_edge_1->get_next();
	ga_mesh_edge* player_edge_3 = (player_trig == nullptr) ? nullptr : player_edge_1->get_next()->get_next();
	for (auto itr = _edges.begin(); itr != _edges.end(); itr++)
	{
		cur_edge = itr->second;
		main_dir = *cur_edge->get_start_vertex()->get_pos() - *cur_edge->get_end_vertex()->get_pos();
		side_dir = cross_product(ga_vec3f::y_vector(), main_dir);
		if (side_dir.mag() > FLT_EPSILON)
		{
			side_dir.normal();
		}
		side_dir.scale(0.01f);
		a = *cur_edge->get_start_vertex()->get_pos() + vert;
		b = *cur_edge->get_start_vertex()->get_pos() - side_dir + vert;
		c = *cur_edge->get_end_vertex()->get_pos() + vert;
		d = *cur_edge->get_end_vertex()->get_pos() - side_dir + vert;


		if (cur_edge->get_opposite() == nullptr)
			color = ga_vec3f::z_vector() + ga_vec3f::x_vector(); // purple
		else if (cur_edge->is_boundary())
			color = ga_vec3f::one_vector(); // white
		else if (cur_edge == player_edge_1 || cur_edge == player_edge_2 || cur_edge == player_edge_3)
			color = ga_vec3f::x_vector(); // red
		else
			color = ga_vec3f::zero_vector().scale_result(0.7f); //grey
		prior_size = draw->_positions.size();
		draw->_positions.push_back(a);
		draw->_positions.push_back(b);
		draw->_positions.push_back(c);
		draw->_positions.push_back(d);
		draw->_colorarray.push_back(color);
		draw->_colorarray.push_back(color);
		draw->_colorarray.push_back(color);
		draw->_colorarray.push_back(color);
		draw->_indices.push_back(prior_size);
		draw->_indices.push_back(prior_size + 1);
		draw->_indices.push_back(prior_size + 2);
		draw->_indices.push_back(prior_size + 1);
		draw->_indices.push_back(prior_size + 3);
		draw->_indices.push_back(prior_size + 2);
	}
#endif

	draw->_color = { 1.0f, 1.0f, 1.0f };
	draw->_draw_mode = GL_TRIANGLES;
	draw->_material = nullptr;
}

void ga_navigation_mesh::create_mesh()
{
	for (int y = 0; y < _height + 1; ++y)
	{
		ga_vec3f cur_loc;
		for (int x = 0; x < _width + 1; ++x)
		{
			cur_loc.x = x;
			cur_loc.y = 0.0f; // cause engine has y-axis going up
			cur_loc.z = y;
			add_vertex(cur_loc);
		}
	}

	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			add_triangle(get_vertex(x + y * (_width + 1)), get_vertex(x + 1 + y * (_width + 1)), get_vertex(x + 1 + (y + 1) * (_width+1)));
			add_triangle(get_vertex(x + y * (_width + 1)), get_vertex(x + 1 + (y + 1) * (_width + 1)), get_vertex(x + (y + 1) * (_width + 1)));
		}
	}
}


void ga_navigation_mesh::mesh_simplification()
{
	ga_mesh_edge *shortest_edge, *edge_walker;
	ga_vec3f new_vertex_pos;
	float max_triangle_area;
	std::vector<ga_mesh_vertex*> new_triangle_vertexes;
	std::vector<ga_mesh_triangle*> removeable_triangles;

	int num_simplified = 0;
	for (auto itr = _edges.begin(); itr != _edges.end(); itr++)
	{
		shortest_edge = itr->second;
		if (!is_collapsible_edge(shortest_edge))
		{
			continue;
		}

		new_triangle_vertexes.clear();
		removeable_triangles.clear();

		// Create new vertex at the mid point of current edge
		new_vertex_pos = (*shortest_edge->get_end_vertex()->get_pos() +
			*shortest_edge->get_start_vertex()->get_pos()).scale_result(0.5f);

		// walk around edge's end vertex
		edge_walker = shortest_edge->get_next()->get_opposite();
		while (edge_walker != nullptr && edge_walker->get_next()->get_opposite() != shortest_edge)
		{
			removeable_triangles.push_back(edge_walker->get_triangle());
			new_triangle_vertexes.push_back(edge_walker->get_next()->get_next()->get_start_vertex());
			new_triangle_vertexes.push_back(edge_walker->get_next()->get_next()->get_end_vertex());
			edge_walker = edge_walker->get_next()->get_opposite();
		}

		// walk around edge's start vertex
		edge_walker = shortest_edge->get_opposite()->get_next()->get_opposite();
		while (edge_walker != nullptr && edge_walker->get_next() != shortest_edge)
		{
			removeable_triangles.push_back(edge_walker->get_triangle());
			new_triangle_vertexes.push_back(edge_walker->get_next()->get_next()->get_start_vertex());
			new_triangle_vertexes.push_back(edge_walker->get_next()->get_next()->get_end_vertex());
			edge_walker = edge_walker->get_next()->get_opposite();
		}

		// find max area of any new triangle
		max_triangle_area = -1.0f;
		for (int i = 0; i + 1 < new_triangle_vertexes.size(); i += 2)
		{
			ga_vec3f b = *new_triangle_vertexes[i]->get_pos();
			ga_vec3f c = *new_triangle_vertexes[i + 1]->get_pos();
			float cur_triangle_area = ga_vec3f_cross((new_vertex_pos - b), (new_vertex_pos - c)).mag() / 2.0f;
			if (max_triangle_area < cur_triangle_area)
				max_triangle_area = cur_triangle_area;
		}
		// if max new triangle area is less than 1.5, edge collapse
		if (max_triangle_area < 1.5f)
		{
			removeable_triangles.push_back(shortest_edge->get_opposite()->get_triangle());
			removeable_triangles.push_back(shortest_edge->get_triangle());
			// remove all queued triangles
			for (int i = 0; i < removeable_triangles.size(); ++i)
				removeTriangle(removeable_triangles[i]);

			// add the new ones
			ga_mesh_vertex* new_vertex = add_vertex(new_vertex_pos);
			for (int i = 0; i + 1 < new_triangle_vertexes.size(); i += 2)
				add_triangle(new_triangle_vertexes[i + 1], new_vertex, new_triangle_vertexes[i]);

			itr = _edges.begin();
			if (++num_simplified > MAX_TRIANGLES_SIMPLIFIED)
				break;
		}
	}
}

ga_mesh_vertex * ga_navigation_mesh::add_vertex(const ga_vec3f & pos)
{
	ga_mesh_vertex* vertex = new ga_mesh_vertex(pos);
	_vertices.push_back(vertex);
	return vertex;
}

ga_mesh_vertex * ga_navigation_mesh::get_vertex(int i)
{
	return _vertices.at(i);
}

ga_mesh_triangle * ga_navigation_mesh::add_triangle(ga_mesh_vertex * a, ga_mesh_vertex * b, ga_mesh_vertex * c)
{
	ga_mesh_triangle* triangle = new ga_mesh_triangle();
	ga_mesh_edge* ab = new ga_mesh_edge(a, b, triangle);
	ga_mesh_edge* bc = new ga_mesh_edge(b, c, triangle);
	ga_mesh_edge* ca = new ga_mesh_edge(c, a, triangle);
	triangle->set_edge(ab);
	ab->set_next(bc);
	bc->set_next(ca);
	ca->set_next(ab);
	ga_vec3f avg_pos = (*a->get_pos() + *b->get_pos() + *c->get_pos()).scale_result(1.0f / 3.0f);
	triangle->set_traversable(!is_red(avg_pos.x, avg_pos.z));
	_edges[std::make_pair(a, b)] = ab;
	_edges[std::make_pair(b, c)] = bc;
	_edges[std::make_pair(c, a)] = ca;
	edgeshashtype::iterator ab_op = _edges.find(std::make_pair(b, a));
	edgeshashtype::iterator bc_op = _edges.find(std::make_pair(c, b));
	edgeshashtype::iterator ca_op = _edges.find(std::make_pair(a, c));
	if (ab_op != _edges.end()) { ab_op->second->set_opposite(ab); }
	if (bc_op != _edges.end()) { bc_op->second->set_opposite(bc); }
	if (ca_op != _edges.end()) { ca_op->second->set_opposite(ca); }
	_triangles[triangle->get_id()] = triangle;
	return triangle;
}

void ga_navigation_mesh::removeTriangle(ga_mesh_triangle * t)
{
	ga_mesh_edge* ab = t->get_edge();
	ga_mesh_edge* bc = ab->get_next();
	ga_mesh_edge* ca = bc->get_next();
	ga_mesh_vertex* a = (*t)[0];
	ga_mesh_vertex* b = (*t)[1];
	ga_mesh_vertex* c = (*t)[2];
	_edges.erase(std::make_pair(a, b));
	_edges.erase(std::make_pair(b, c));
	_edges.erase(std::make_pair(c, a));
	_triangles.erase(t->get_id());
	delete ab;
	delete bc;
	delete ca;
	delete t;
}

bool ga_navigation_mesh::is_collapsible_edge(ga_mesh_edge* shortest_edge)
{
	if (!shortest_edge->is_boundary())
	{ // if this edge is not a boundary, check if it would affect boundary edge
		ga_mesh_edge* e_walk = shortest_edge->get_next()->get_opposite();

		while (e_walk != nullptr && e_walk != shortest_edge)
		{ // walk around shortest edge's end vertex
			if (e_walk->is_boundary())
			{
				e_walk = nullptr;
			}
			else
			{
				e_walk = e_walk->get_next()->get_opposite();
			}
		}
		if (e_walk == nullptr)
		{ // if we found an boundary or nullptr "connected" to vertex e, we can't cut this edge
			return false;
		}
		ga_mesh_edge* s_walk = shortest_edge->get_opposite()->get_next()->get_opposite();
		while (s_walk != nullptr && s_walk->get_next() != shortest_edge)
		{
			if (s_walk->is_boundary())
			{
				s_walk = nullptr;
			}
			else
			{
				s_walk = s_walk->get_next()->get_opposite();
			}
		}
		if (s_walk == nullptr)
		{ // if we found an boundary or nullptr "connected" to vertex s, we can't cut this edge
			return false;
		}
		return true;
	}
	return false;
}