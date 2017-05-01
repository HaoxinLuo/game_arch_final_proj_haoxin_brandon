#include "ga_navigation_mesh.h"
#include <stb_image.h>

#include <iostream>

#define GLEW_STATIC

unsigned int ga_mesh_vertex::UNIQUE_VERTEX_ID = 0;
unsigned int ga_mesh_triangle::UNIQUE_TRIANGLE_ID = 0;

ga_navigation_mesh::ga_navigation_mesh(const char* mesh_file)
{
	//display_triangle = 1;
	load_from_file(mesh_file);
	std::cerr << "Loaded mesh file with dimensions " << _width << "x" << _height << "\n";
	create_mesh();
	std::cerr << "Created " << _triangles.size() << " triangles\n";
	mesh_simplification();
}

bool ga_navigation_mesh::load_from_file(const char* path)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	int channels_in_file;
	uint8_t* data = stbi_load(fullpath.c_str(), &_width, &_height, &channels_in_file, 4);
	if (!data)
	{
		std::cerr << "could not load mesh file: " << path << "\n";
		exit(1);
	}

	store_data(data);
	stbi_image_free(data);

	return true;
}


void ga_navigation_mesh::store_data(unsigned char* data)
{
	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			ga_vec3f pixel = ga_vec3f::zero_vector();
			pixel.x = data[x * 4 + y * (_width * 4) + 0];
			pixel.y = data[x * 4 + y * (_width * 4) + 1];
			pixel.z = data[x * 4 + y * (_width * 4) + 2];
			_pixels.push_back(pixel);
		}
	}
}


bool ga_navigation_mesh::is_point_inside_triangle(ga_vec3f & pos, ga_mesh_triangle * tri)
{ // http://blackpawn.com/texts/pointinpoly/
	ga_vec3f A = *(*tri)[0]->get_pos();
	ga_vec3f B = *(*tri)[1]->get_pos();
	ga_vec3f C = *(*tri)[2]->get_pos();
	// Compute vectors        
	ga_vec3f v0 = C - A;
	ga_vec3f v1 = B - A;
	ga_vec3f v2 = pos - A;

	// Compute dot products
	float	dot00 = v0.dot(v0);
	float dot01 = v0.dot(v1);
	float dot02 = v0.dot(v2);
	float dot11 = v1.dot(v1);
	float dot12 = v1.dot(v2);

		// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}

bool ga_navigation_mesh::is_red(int x, int y)
{
	ga_vec3f* pixel = get_pixel(x, y);
	if (pixel != nullptr && pixel->x > 0)
	{
		return true;
	}
	return false;
}

bool ga_navigation_mesh::is_above(int x, int y, int height)
{
	ga_vec3f *pixel = get_pixel(x, y);
	if (pixel != nullptr && pixel->z < height)
	{
		return true;
	}
	return false;
}

bool ga_navigation_mesh::is_on(int x, int y, int height)
{
	ga_vec3f *pixel = get_pixel(x, y);
	if (pixel != nullptr && abs(pixel->z - height) < FLT_EPSILON)
	{
		return true;
	}
	return false;
}

ga_vec3f* ga_navigation_mesh::get_pixel(int x, int y)
{
	if (x >= _width || x < 0 || y >= _height || y < 0 || _pixels.size() < x + y * _width)
	{
		return nullptr;
	}
	return &(_pixels[x + y * _width]);
}

ga_mesh_triangle * ga_navigation_mesh::get_triangle(unsigned int id)
{
	auto itr = _triangles.find(id);
	return (itr == _triangles.end()) ? nullptr : itr->second;
}

std::vector<int> ga_navigation_mesh::get_neighbors(int trig_id)
{
	std::vector<int> ans;
	ga_mesh_triangle* trig = get_triangle(trig_id);
	if (trig != nullptr)
	{
		if (trig->get_edge()->get_opposite() != nullptr)
			ans.push_back(trig->get_edge()->get_opposite()->get_triangle()->get_id());
		if (trig->get_edge()->get_next()->get_opposite() != nullptr)
			ans.push_back(trig->get_edge()->get_next()->get_opposite()->get_triangle()->get_id());
		if (trig->get_edge()->get_next()->get_next()->get_opposite() != nullptr)
			ans.push_back(trig->get_edge()->get_next()->get_next()->get_opposite()->get_triangle()->get_id());
	}
	return ans;
}

int ga_navigation_mesh::get_triangle_of_point(ga_vec3f & pos)
{
	if (pos.x < 0 || pos.x > _width || pos.y < 0 || pos.y > _height)
	{
		return -1;
	}
	for (auto itr = _triangles.begin(); itr != _triangles.end(); itr++)
	{
		if (is_point_inside_triangle(pos, itr->second))
		{
			return itr->second->get_id();
		}
	}
	return -1; //not found
}

int ga_navigation_mesh::get_triangle_of_point(ga_vec3f & pos, int trig_id)
{
	if (pos.x < 0 || pos.x > _width || pos.y < 0 || pos.y > _height)
	{
		_tri_id = -1;
		return -1;
	}
	std::vector<int> neighbors = get_neighbors(trig_id);
	ga_mesh_triangle *cur_tri;
	for (int i = 0; i < neighbors.size(); ++i)
	{
		cur_tri = get_triangle(neighbors[i]);
		if (is_point_inside_triangle(pos, cur_tri))
		{
			_tri_id = cur_tri->get_id();
			return _tri_id;
		}
	}
	return get_triangle_of_point(pos);
}

bool ga_navigation_mesh::legal_position(ga_vec3f* pos)
{
	float x = pos->x;
	float y = pos->z;
	float height = pos->y;
	if ( ((!is_red(x, y) && is_on(x, y, height)) || is_above(x, y, height)) && x > 0 && y > 0)
	{
		return true;
	}
	return false;
}
