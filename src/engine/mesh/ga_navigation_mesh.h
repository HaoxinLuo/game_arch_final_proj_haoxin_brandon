#pragma once
#include "math\ga_vec3f.h"
#include "mesh\ga_mesh_hash.h"
#include <vector>

struct ga_dynamic_drawcall;

class ga_navigation_mesh {
	typedef std::pair<uint32_t, uint32_t> pixel_coord;
public:
	ga_navigation_mesh(const char* mesh_file);
	
	int get_width() const { return _width; }
	int get_height() const { return _height; }
	struct ga_vec3f* get_pixel(int x, int y);
	ga_mesh_triangle* get_triangle(unsigned int id);
	std::vector<int> get_neighbors(int trig_id);
	int get_triangle_of_point(ga_vec3f& pos);
	int get_triangle_of_point(ga_vec3f &pos, int trig_id); // so nav comp can keep track of location

	void set_triangle(unsigned int id) { _tri_id = id; } // for  debug
	
	bool legal_position(ga_vec3f* pos);

	void get_debug_draw(ga_dynamic_drawcall *draw);

private:
	// mesh helper functions
	ga_mesh_vertex* add_vertex(const ga_vec3f & pos);
	ga_mesh_vertex* get_vertex(int i);
	ga_mesh_triangle* add_triangle(ga_mesh_vertex* a, ga_mesh_vertex* b, ga_mesh_vertex* c);
	void removeTriangle(ga_mesh_triangle* t);
	void mesh_simplification();
	bool is_collapsible_edge(ga_mesh_edge* e);

	// set up helper functions
	bool load_from_file(const char* path);
	void store_data(unsigned char * data);
	void create_mesh();
	
	// navigation helper functions
	bool is_point_inside_triangle(ga_vec3f& pos, ga_mesh_triangle* tri);
	bool is_red(int x, int y);
	bool is_above(int x, int y, int height);
	bool is_on(int x, int y, int height);

	// representation
	int _width, _height;
	std::vector<struct ga_vec3f> _pixels;
	std::vector<ga_mesh_vertex*> _vertices;
	edgeshashtype _edges;
	triangleshashtype _triangles;


	unsigned int _tri_id;
	//int display_triangle;
};
