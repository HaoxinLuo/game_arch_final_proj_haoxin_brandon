#pragma once
#include "ga_entity.h"
#include <math.h>
#include <iostream>

class ga_entity_world
{
public:
	ga_entity_world();
	~ga_entity_world();

	void add_entity(ga_entity* e) { entities.push_back(e); }
	void remove_entity(ga_entity* e) { entities.erase(std::find(entities.begin(), entities.end(), e)); }
	//void addaicomponent(ga_ai_component* a) { aicomponents.push_back(a); }
	//void addnavcomponent(ga_navigation_component* n) { navcomponents.push_back(n); }

	std::vector<ga_entity*> get_entities() { return entities; }
	//std::vector<ga_ai_component*> getaicomponents() { return aicomponents; }

	bool has_entity(int my_self, ga_vec3f t, float radius) {
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i]->get_id() == my_self) continue;
			ga_entity* THEentity = entities[i];
			ga_vec3f t2 = THEentity->get_transform().get_translation();
			float radius2 = sqrt(pow(t.x - t2.x, 2) + pow(t.y - t2.y, 2) + pow(t.z - t2.z, 2));
			if (radius2 < radius) {
				//std::cerr << my_self << " is crashing with " << entities[i]->get_id() << "\n";
				return true;
			}
		}
		return false;
	}

private:
	std::vector<ga_entity*> entities;
	//std::vector<ga_ai_component*> aicomponents;
	//std::vector<ga_navigation_component*> navcomponents;
};

