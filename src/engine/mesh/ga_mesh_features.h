#pragma once
#include "math\ga_vec3f.h"
#include <assert.h>

class ga_mesh_vertex
{
public:
	ga_mesh_vertex(const ga_vec3f &pos)
	{
		_pos.x = pos.x;
		_pos.y = pos.y;
		_pos.z = pos.z;
		_id = UNIQUE_VERTEX_ID++;
	}
	int get_id() const { return _id; }
	float get_x() const { return _pos.x; }
	float get_y() const { return _pos.y; }
	float get_z() const { return _pos.z; }
	ga_vec3f *get_pos() {	return &_pos; }

private:
	int _id;
	ga_vec3f _pos;

	static unsigned int UNIQUE_VERTEX_ID;
};


class ga_mesh_triangle;
class ga_mesh_edge
{
public:
	ga_mesh_edge(ga_mesh_vertex* a, ga_mesh_vertex* b, ga_mesh_triangle* t)
	{
		if (a == nullptr || b == nullptr || t == nullptr) assert(false);
		_start = a;
		_end = b;
		_tri = t;
		_next = _opposite = nullptr;
	}
	~ga_mesh_edge()
	{
		if (_opposite != nullptr)
			_opposite->_opposite = nullptr;
	}

	ga_mesh_vertex* get_start_vertex() { return _start; }
	ga_mesh_vertex* get_end_vertex() { return _end; }
	ga_mesh_edge* get_next() { return _next; }
	ga_mesh_edge* get_opposite() { return _opposite; }
	ga_mesh_triangle* get_triangle() { return _tri; }

	bool is_boundary() const;
	float get_length() const { return (*_end->get_pos() - *_start->get_pos()).mag(); }

	void set_opposite(ga_mesh_edge* o);
	void set_next(ga_mesh_edge* e) { _next = e; }
	void clear_opposite() 
	{ 
		if (_opposite == nullptr) return;
		_opposite->_opposite = nullptr;
		_opposite = nullptr;
	}

private:
	ga_mesh_vertex* _start, *_end;
	ga_mesh_triangle* _tri;
	ga_mesh_edge* _next, *_opposite;
};


class ga_mesh_triangle
{
public:
	ga_mesh_triangle()
	{
		_edge = nullptr;
		_id = UNIQUE_TRIANGLE_ID++;
	}

	ga_mesh_vertex* operator[](int i) const
	{
		if (i == 0) return _edge->get_start_vertex();
		if (i == 1) return _edge->get_next()->get_start_vertex();
		if (i == 2) return _edge->get_next()->get_next()->get_start_vertex();
	}

	int get_id() const { return _id; }
	ga_mesh_edge* get_edge() const { return _edge; }
	bool is_traversable() const	{	return _traversable; }
	ga_vec3f get_centroid() const;

	void set_traversable(bool b) { _traversable = b; }
	void set_edge(ga_mesh_edge* e) { _edge = e; }
	static const unsigned int get_max_id() { return UNIQUE_TRIANGLE_ID; }

private:
	ga_mesh_edge* _edge;
	int _id;
	bool _traversable;
	static unsigned int UNIQUE_TRIANGLE_ID;
};