#pragma once
#include <list>
#include <cstdint>

#include "../engine/common/vec.hpp"
#include "../engine/common/grid.hpp"

struct PhysicObject;

struct CollisionCell
{
	uint32_t objects_count = 0;
	std::list<uint32_t> objects;


	CollisionCell() = default;

	void addAtom(uint32_t id)
	{
		objects.push_back(id);
		objects_count++;
	}

	void clear()
	{
		objects.clear();
		objects_count = 0u;
	}

	void remove(uint32_t id)
	{

		objects.remove_if([id](const uint32_t& element_id) {
			return element_id == id;
		});
	}
};

struct CollisionGrid : public Grid<CollisionCell>
{
	uint32_t cell_size;

	CollisionGrid()
		: Grid<CollisionCell>()
	{}

	// Grid is divided into cells of a specific size
	CollisionGrid(int32_t world_width, int32_t world_height, uint32_t cell_size_)
		: Grid<CollisionCell>(std::ceil((float)world_width / cell_size_), std::ceil((float)world_height / cell_size_)),	//std::ceil to zaokr¹glenie w górê
		cell_size{ cell_size_ }
	{}

	bool updateAtomCell(uint32_t pos_x, uint32_t pos_y, uint32_t atom, uint32_t& atom_cell_id)
	{
		uint32_t cell_x_id = (pos_x / cell_size);
		uint32_t cell_y_id = (pos_y / cell_size);

		const uint32_t id = cell_x_id * height + cell_y_id;

		// Update atom_cell whitch is used in cleaning grid in main loop 
		atom_cell_id = id;
		
		// Add to grid
		data[id].addAtom(atom);
		return true;
	}

	bool addAtom(uint32_t pos_x, uint32_t pos_y, uint32_t atom)
	{
		uint32_t cell_x_id = (pos_x / cell_size);
		uint32_t cell_y_id = (pos_y / cell_size);

		const uint32_t id = cell_x_id * height + cell_y_id;
		// Add to grid
		data[id].addAtom(atom);
		return true;
	}

	void clear()
	{
		for (auto& c : data) {
			c.clear();
		}
	}

	void clear(uint32_t cell_id)
	{
		data[cell_id].clear();
	}
};