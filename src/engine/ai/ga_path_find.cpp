#include "ga_ai_component.h"
#include "ga_navigation_component.h"

#include <queue>
#include <set>
#include <map>
#include "math\ga_vec2f.h"

#include <iostream>
#include "user_definitions.h"

struct GreaterByDistance
{
	bool operator()(const std::pair<int, ga_vec2f>&  lhs, const std::pair<int, ga_vec2f>& rhs) const
	{
		return lhs.second.x + lhs.second.y > rhs.second.x + rhs.second.y;
	}
};

float distance_between(ga_mesh_triangle* a, ga_mesh_triangle* b)
{
	return (b->get_centroid() - a->get_centroid()).mag();
}

void gather_path(std::vector<int> &ans, std::map<int, int> &pathTo, int &path_end)
{
	int cur = path_end;
	while (cur != -1)
	{
		ans.push_back(cur);
		cur = pathTo.find(cur)->second;
	}
}

bool ga_ai_component::path_find(ga_vec3f* start_loc, ga_vec3f* target_loc)
{
	if (start_loc == nullptr || target_loc == nullptr)
	{
		return false;
	}
	_path.clear();
	int start_id = _nav->get_triangle_of_point(*start_loc);
	int target_id = _nav->get_triangle_of_point(*target_loc);
	if (start_id < 0 || target_id < 0)
	{
		return false;
	}

	ga_mesh_triangle* end_triangle = _nav->get_triangle(target_id);

	// set up data structures
	std::priority_queue<std::pair<int, ga_vec2f>, std::vector<std::pair<int, ga_vec2f>>, GreaterByDistance> fringe;
	std::set<int> visited;
	std::map<int, int> pathTo;
	int path_end = -1;

	// initialize fringe, pathTo, and gcost
	pathTo.insert(std::make_pair(start_id, -1));
	//ga_vec2f cost = create_vec2f(0.0f, distance_between(&start_vec, &target_vec));
	ga_vec2f cost;
	cost.x = 0.0f;
	cost.y = distance_between(_nav->get_triangle(start_id), end_triangle);
	fringe.push(std::make_pair(start_id, cost));

	while (!fringe.empty() && path_end == -1) {
		std::pair<int, ga_vec2f> cur_node = fringe.top();
		int cur_id = cur_node.first;
		ga_vec2f cur_cost = cur_node.second;
		fringe.pop();
		visited.insert(cur_id);
		if (cur_id == target_id)
		{ // found the target
			path_end = cur_id;
		}
		else
		{
			std::vector<int> neighbors = _nav->get_neighbors(cur_id);
			for (int i = 0; i < neighbors.size(); ++i)
			{ // have not visited this triangle
				if (visited.find(neighbors[i]) == visited.end())
				{
					pathTo.insert(std::make_pair(neighbors[i], cur_id));
					ga_vec2f new_cost;
					new_cost.x = cur_cost.x + 1.0f;
					new_cost.y = distance_between(_nav->get_triangle(neighbors[i]), end_triangle);
					fringe.push(std::make_pair(neighbors[i], new_cost));
				}
			}
		}
	}

	std::vector<int> path;
	gather_path(path, pathTo, path_end);
	std::cerr << "path of length " << path.size() << "\n";
	_path.push_back(*target_loc);
#if SMOOTH_PATH
	smooth_path(path, start_loc, target_loc);
#else
	for (int i = 0; i < path.size(); ++i)
		_path.push_back(_nav->get_triangle(path[i])->get_centroid());
#endif

	return path_end != -1;
}

void ga_ai_component::smooth_path(std::vector<int>& path, ga_vec3f * start_loc, ga_vec3f * target_loc)
{
	if (path.size() == 1)
	{
		path.pop_back();
		return; // already in the same triangle
	}
	std::vector<std::pair<ga_vec3f, ga_vec3f>> portals;
	for (int i = path.size() - 1; i > 0; --i)
	{
		ga_mesh_edge* cur_edge = _nav->get_triangle(path[i])->get_edge();
		for (int j = 0; j < 3; ++j)
		{
			if (cur_edge->get_opposite() != nullptr && cur_edge->get_opposite()->get_triangle()->get_id() == path[i - 1])
			{
				portals.push_back(std::make_pair(*cur_edge->get_start_vertex()->get_pos(), *cur_edge->get_end_vertex()->get_pos()));
				break;
			}
			else
			{
				cur_edge = cur_edge->get_next();
			}
		}
	}
	portals.push_back(std::make_pair(*target_loc, *target_loc));
	path.clear();

	std::vector<ga_vec3f> ans;
	ga_vec3f funnel_apex, funnel_left, funnel_right;
	funnel_apex = *start_loc;
	funnel_left = portals[0].first;
	funnel_right = portals[0].second;
	ga_vec3f new_funnel_left, new_funnel_right;
	ga_vec3f new_left_segment, new_right_segment, left_segment, right_segment;
	ga_vec3f vertical = ga_vec3f::y_vector();
	int dt;
	for (int i = 1; i < portals.size(); i += dt)
	{
		dt = 1;
		new_funnel_left = portals[i].first;
		new_funnel_right = portals[i].second;

		left_segment = funnel_left - funnel_apex;
		right_segment = funnel_right - funnel_apex;

		new_left_segment = new_funnel_left - funnel_apex;
		new_right_segment = new_funnel_right- funnel_apex;

		// check right side
		if (ga_vec3f_cross(right_segment, new_right_segment).dot(vertical) > -FLT_EPSILON)
		{ // new r segment is narrowing the funnel
			if (ga_vec3f_cross(new_right_segment, left_segment).dot(vertical) > -FLT_EPSILON)
			{ // new r segment is not crossing l segment
				// thus simply move r segment up
				funnel_right = new_funnel_right;
			}
			else
			{ // new r segment crossed l segment
				funnel_apex = funnel_left;
				ans.push_back(funnel_apex);
				dt = 0;
			}
		}

		// check left side
		if (ga_vec3f_cross(new_left_segment, left_segment).dot(vertical) > -FLT_EPSILON)
		{ // new l segment is narrowing the funnel
			if (ga_vec3f_cross(right_segment, new_left_segment).dot(vertical) > -FLT_EPSILON)
			{ // new r segment is not crossing l segment
				// thus simply move r segment up
				funnel_left = new_funnel_left;
			}
			else
			{ // new r segment crossed l segment
				funnel_apex = funnel_right;
				ans.push_back(funnel_apex);
				dt = 0;
			}
		}
	}

	for (int i = ans.size()-1; i >= 0; --i)
	{
		_path.push_back(ans[i]);
	}
}

