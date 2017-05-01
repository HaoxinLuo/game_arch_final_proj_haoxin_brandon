#pragma once

// ******************************************************************************
// starter code taken from Prof Barb Cutler's advanced computer graphics class
// ******************************************************************************

// to handle different platforms with different variants of a developing standard
// NOTE: You may need to adjust these depending on your installation
#ifdef __APPLE__
// osx mavericks
#include <unordered_map>
#elif defined(_WIN32)
#include <unordered_map>
#elif defined(__linux__)
#include <unordered_map>
#elif defined(__FreeBSD__)
#include <ext/hash_map>
#else
#error "unknown system"
#endif

#include "ga_mesh_features.h"

#define LARGE_PRIME_A 10007
#define LARGE_PRIME_B 11003


// ===================================================================================
// DIRECTED EDGES are stored in a hash table using a simple hash
// function based on the indices of the start and end vertices
// ===================================================================================

inline unsigned int ordered_two_int_hash(unsigned int a, unsigned int b) {
	return LARGE_PRIME_A * a + LARGE_PRIME_B * b;
}

struct orderedvertexpairhash {
	size_t operator()(std::pair<ga_mesh_vertex*, ga_mesh_vertex*> p) const {
		return ordered_two_int_hash(p.first->get_id(), p.second->get_id());
	}
};

struct orderedsamevertexpair {
	bool operator()(std::pair<ga_mesh_vertex*, ga_mesh_vertex*> p1, std::pair<ga_mesh_vertex*, ga_mesh_vertex*>p2) const {
		if (p1.first->get_id() == p2.first->get_id() && p1.second->get_id() == p2.second->get_id())
			return true;
		return false;
	}
};



// ===================================================================================
// PARENT/CHILD VERTEX relationships (for subdivision) are stored in a
// hash table using a simple hash function based on the indices of the
// parent vertices, smaller index first
// ===================================================================================

inline unsigned int unordered_two_int_hash(unsigned int a, unsigned int b) {
	//assert(a != b);
	if (b < a) {
		return ordered_two_int_hash(b, a);
	}
	else {
		//assert(a < b);
		return ordered_two_int_hash(a, b);
	}
}

struct unorderedvertexpairhash {
	size_t operator()(std::pair<ga_mesh_vertex*, ga_mesh_vertex*> p) const {
		return unordered_two_int_hash(p.first->get_id(), p.second->get_id());
	}
};

struct unorderedsamevertexpair {
	bool operator()(std::pair<ga_mesh_vertex*, ga_mesh_vertex*> p1, std::pair<ga_mesh_vertex*, ga_mesh_vertex*>p2) const {
		if ((p1.first->get_id() == p2.first->get_id() && p1.second->get_id() == p2.second->get_id()) ||
			(p1.first->get_id() == p2.second->get_id() && p1.second->get_id() == p2.first->get_id())) return true;
		return false;
	}
};



// ===================================================================================
// TRIANGLES are stored in a hash table using a simple hash function
// based on the id of the triangle (unique ids are assigned when the
// triangle is constructed)
// ===================================================================================

struct idhash {
	size_t operator()(unsigned int id) const {
		return LARGE_PRIME_A * id;
	}
};

struct sameid {
	bool operator()(unsigned int a, unsigned int b) const {
		if (a == b)
			return true;
		return false;
	}
};



// to handle different platforms with different variants of a developing standard
// NOTE: You may need to adjust these depending on your installation
#ifdef __APPLE__
// osx mavericks
typedef std::unordered_map<std::pair<ga_mesh_vertex*, ga_mesh_vertex*>, ga_mesh_vertex*, unorderedvertexpairhash, unorderedsamevertexpair> vphashtype;
typedef std::unordered_map<std::pair<ga_mesh_vertex*, ga_mesh_vertex*>, ga_mesh_edge*, orderedvertexpairhash, orderedsamevertexpair> edgeshashtype;
typedef std::unordered_map<unsigned int, ga_mesh_triangle*, idhash, sameid> triangleshashtype;
#elif defined(_WIN32)
typedef std::unordered_map<std::pair<ga_mesh_vertex*, ga_mesh_vertex*>, ga_mesh_vertex*, unorderedvertexpairhash, unorderedsamevertexpair> vphashtype;
typedef std::unordered_map<std::pair<ga_mesh_vertex*, ga_mesh_vertex*>, ga_mesh_edge*, orderedvertexpairhash, orderedsamevertexpair> edgeshashtype;
typedef std::unordered_map<unsigned int, ga_mesh_triangle*, idhash, sameid> triangleshashtype;
#elif defined(__linux__)
typedef std::unordered_map<std::pair<ga_mesh_vertex*, ga_mesh_vertex*>, ga_mesh_vertex*, unorderedvertexpairhash, unorderedsamevertexpair> vphashtype;
typedef std::unordered_map<std::pair<ga_mesh_vertex*, ga_mesh_vertex*>, ga_mesh_edge*, orderedvertexpairhash, orderedsamevertexpair> edgeshashtype;
typedef std::unordered_map<unsigned int, ga_mesh_triangle*, idhash, sameid> triangleshashtype;
#elif defined(__FreeBSD__)
typedef __gnu_cxx::hash_map<std::pair<ga_mesh_vertex*, ga_mesh_vertex*>, ga_mesh_vertex*, unorderedvertexpairhash, unorderedsamevertexpair> vphashtype;
typedef __gnu_cxx::hash_map<std::pair<ga_mesh_vertex*, ga_mesh_vertex*>, ga_mesh_edge*, orderedvertexpairhash, orderedsamevertexpair> edgeshashtype;
typedef __gnu_cxx::hash_map<unsigned int, ga_mesh_triangle*, idhash, sameid> triangleshashtype;
#else
#endif
