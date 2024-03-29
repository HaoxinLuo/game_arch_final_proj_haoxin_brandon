/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_camera.h"
#include "framework/ga_compiler_defines.h"
#include "framework/ga_input.h"
#include "framework/ga_sim.h"
#include "framework/ga_output.h"
#include "jobs/ga_job.h"

#include "entity/ga_entity.h"

#include "graphics/ga_cube_component.h"
#include "graphics/ga_program.h"

#include "physics/ga_intersection.tests.h"
#include "physics/ga_physics_component.h"
#include "physics/ga_physics_world.h"
#include "physics/ga_rigid_body.h"
#include "physics/ga_shape.h"

// Mine includes
#include "mesh/ga_navigation_mesh.h"
#include "physics/ga_triangular_prism.h"
#include "entity/ga_unit_component.h"
#include "ai\ga_ai_component.h"
#include "graphics\ga_floor_component.h"

#include "user_definitions.h"
// ends mine includes

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

static void set_root_path(const char* exepath);
static void run_unit_tests();

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);
	//std::string INPUT_MAP_FILE = "data/maps/small_square.png";
	//std::string mesh_file = "data/maps/center_box_big.png";
	//std::string mesh_file = "data/maps/nested_cube.bmp"; // too many grids i guess
	//std::string mesh_file = "data/maps/four_four.png";

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_physics_world* world = new ga_physics_world();
	ga_output* output = new ga_output(input->get_window());

	// Create camera.
	ga_camera* camera = new ga_camera({ 5.0f,10.0f, 10.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(65.0f));
	camera->rotate(rotation);

	ga_navigation_mesh *mesh = new ga_navigation_mesh(INPUT_MAP_FILE);
	camera->set_mesh(mesh);

	ga_entity_world entworld = ga_entity_world();

	ga_entity test_1_unit;
	test_1_unit.translate({ 0.5f, 0.0f, 0.25f });
	ga_unit_component test_1_model(&test_1_unit, "data/textures/rpi.png");
	ga_navigation_component test_1_nav(&test_1_unit, mesh);
	test_1_nav.set_speed(2.0f);
	ga_ai_component test_1_control(&test_1_unit, &test_1_nav, &entworld);
	sim->add_entity(&test_1_unit);

#if TEST_MULTI_AGENT
	ga_entity test_2_unit;
	test_2_unit.translate({ 2.0f, 0.0f, 1.0f });
	ga_unit_component test_2_model(&test_2_unit, "data/textures/rpi.png");
	
	ga_navigation_component test_2_nav(&test_2_unit, mesh);
	test_2_nav.set_speed(1.5f);
	ga_ai_component test_2_control(&test_2_unit, &test_2_nav, &entworld);
	sim->add_entity(&test_2_unit);
#endif

	ga_entity floor;
	ga_floor_component floor_model(&floor, mesh, INPUT_MAP_FILE);
	sim->add_entity(&floor);

	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// Step the physics world.
		world->step(&params);

		// Perform the late update.
		sim->late_update(&params);

		// Draw to screen.
		output->update(&params);
	}

	delete output;
	delete world;
	delete sim;
	delete input;
	delete camera;

	ga_job::shutdown();

	return 0;
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(GA_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(GA_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}

void run_unit_tests()
{
	ga_intersection_utility_unit_tests();
	ga_intersection_unit_tests();
}
