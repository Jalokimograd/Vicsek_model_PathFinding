#pragma once
#include "collision_grid.hpp"
#include "physic_object.hpp"

#include "../engine/common/utils.hpp"
#include "../engine/common/index_vector.hpp"
#include "../thread_pool/thread_pool.hpp"
#include "../engine/common/time_analyzer.hpp"
#include "../engine/common/math.hpp"

#include <SFML/System/Vector2.hpp>

struct PhysicSolver
{
    CIVector<PhysicObject> objects;
    CollisionGrid          grid;
    Vec2                   world_size;

    // Simulation solving pass count
    tp::ThreadPool& thread_pool;

    PhysicSolver(IVec2 size, uint32_t cell_size, tp::ThreadPool& tp)
        : grid{ size.x, size.y, cell_size }
        , world_size{ to<double>(size.x), to<double>(size.y) }
        , thread_pool{ tp }
    {
        grid.clear();
    }


    // Checks if two atoms are colliding and if so create a new contact
    void solveContact(uint32_t atom_1_idx, uint32_t atom_2_idx)
    {
        constexpr double eps = 0.0001;

        PhysicObject& obj_1 = objects.data[atom_1_idx];
        PhysicObject& obj_2 = objects.data[atom_2_idx];

        const Vec2 o2_o1 = obj_1.position - obj_2.position;

        const double sqrDst = o2_o1.x * o2_o1.x + o2_o1.y * o2_o1.y;
        const double dist = sqrt(sqrDst);
        const double view_range = grid.cell_size;

        if (dist < view_range && sqrDst > eps) {
           
            obj_1.nextVelocity += obj_2.velocity;
        }
    }


    void checkBoidsCellDetection(uint32_t atom_idx, const CollisionCell& c)
    {
        for (const auto& element_id : c.objects) {
            solveContact(atom_idx, element_id);
        }
    }

    void processCell(const CollisionCell& c, uint32_t index)
    {
        for (const auto& element_id : c.objects) {
            const uint32_t atom_idx = element_id;
            const uint32_t grid_size = grid.height * grid.width;

            const uint32_t cell_x_pos = index / grid.height;
            const uint32_t cell_y_pos = index % grid.height;

            uint32_t N  = index - 1;
            uint32_t C  = index;
            uint32_t S  = index + 1;

            uint32_t NE = index + grid.height - 1;
            uint32_t E  = index + grid.height;
            uint32_t SE = index + grid.height + 1;

            uint32_t NW = index - grid.height - 1;
            uint32_t W  = index - grid.height;
            uint32_t SW = index - grid.height + 1;

            if (cell_y_pos == 0) {
                N  += grid.height;
                NE += grid.height;
                NW += grid.height;
            }
            else if (cell_y_pos == grid.height - 1) {
                S  -= grid.height;
                SE -= grid.height;
                SW -= grid.height;
            }

            if (cell_x_pos == 0) {
                NW += grid_size;
                W  += grid_size;
                SW += grid_size;
            }
            else if (cell_x_pos == grid.width - 1) {
                NE -= grid_size;
                E  -= grid_size;
                SE -= grid_size;
            }

            checkBoidsCellDetection(atom_idx, grid.data[N]);
            checkBoidsCellDetection(atom_idx, grid.data[C]);
            checkBoidsCellDetection(atom_idx, grid.data[S]);
            checkBoidsCellDetection(atom_idx, grid.data[NE]);
            checkBoidsCellDetection(atom_idx, grid.data[E]);
            checkBoidsCellDetection(atom_idx, grid.data[SE]);
            checkBoidsCellDetection(atom_idx, grid.data[NW]);
            checkBoidsCellDetection(atom_idx, grid.data[W]);
            checkBoidsCellDetection(atom_idx, grid.data[SW]);
        }
    }

    void solveCollisionThreaded(uint32_t i, uint32_t slice_size)
    {
        const uint32_t start = i * slice_size;
        const uint32_t end = (i + 1) * slice_size;
        for (uint32_t idx{ start }; idx < end; ++idx) {
            processCell(grid.data[idx], idx);
        }
    }

    // Find nearby boids
    void solveNeighborhood()
    {
        clock_t time_req = clock();

        // Multi-thread grid
        const uint32_t thread_count = thread_pool.m_thread_count;
        const uint32_t slice_count = thread_count * 2;
        const uint32_t slice_size = (grid.width / slice_count) * grid.height;
        // Find collisions in two passes to avoid data races
        // First collision pass
        for (uint32_t i{ 0 }; i < thread_count; ++i) {
            thread_pool.addTask([this, i, slice_size] {
                solveCollisionThreaded(2 * i, slice_size);
                });
        }
        thread_pool.waitForCompletion();
        // Second collision pass
        for (uint32_t i{ 0 }; i < thread_count; ++i) {
            thread_pool.addTask([this, i, slice_size] {
                solveCollisionThreaded(2 * i + 1, slice_size);
                });
        }
        thread_pool.waitForCompletion();

        TimeAnalyzer::getInstance().collision_time = clock() - time_req;
    }

    // Add a new object to the solver
    uint64_t addObject(const PhysicObject& object)
    {
        return objects.push_back(object);
    }

    // Add a new object to the solver
    uint64_t createObject(Vec2 pos)
    {
        return objects.emplace_back(pos);
    }

    void update(float dt)
    {    
        addObjectsToGrid();          
        solveNeighborhood();
        updateObjects_multi(dt);
    }

    void addObjectsToGrid()
    {
        clock_t time_req = clock();
        //grid.clear();
        
        for (const PhysicObject& obj : objects.data) {
            grid.clear(obj.actual_grid_id);
        }
        
        TimeAnalyzer::getInstance().clear_grid_time = clock() - time_req;

        time_req = clock();
        // Safety border to avoid adding object outside the grid
        uint32_t i{ 0 };
        for (PhysicObject& obj : objects.data) {
            if (obj.position.x > 0.0 && obj.position.x < world_size.x &&
                obj.position.y > 0.0 && obj.position.y < world_size.y) {
                //grid.addAtom(to<int32_t>(obj.position.x), to<int32_t>(obj.position.y), i);
                grid.updateAtomCell(to<int32_t>(obj.position.x), to<int32_t>(obj.position.y), i, obj.actual_grid_id);
            }
            ++i;
        }
        TimeAnalyzer::getInstance().update_grid_time = clock() - time_req;
    }


    void updateObjects_multi(float dt)
    {
        thread_pool.dispatch(to<uint32_t>(objects.size()), [&](uint32_t start, uint32_t end) {
            for (uint32_t i{ start }; i < end; ++i) {
                PhysicObject& obj = objects.data[i];

                obj.update(dt);

                //periodic ownership of the border
                if (obj.position.x > world_size.x) {
                    obj.position.x = obj.position.x - (world_size.x);
                }
                else if (obj.position.x < 0.0) {
                    obj.position.x = (obj.position.x) + (world_size.x);
                }
                if (obj.position.y > world_size.y) {
                    obj.position.y = obj.position.y - (world_size.y);
                }
                else if (obj.position.y < 0.0) {
                    obj.position.y = (obj.position.y) + (world_size.y);
                }
            }
        });
    }
};