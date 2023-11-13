#include <iostream>
#include <time.h>


#include "engine/window_context_handler.hpp"
#include "engine/common/number_generator.hpp"
#include "engine/common/color_utils.hpp"

#include "physics/physics.hpp"
#include "thread_pool/thread_pool.hpp"
#include "renderer/renderer.hpp"
#include "engine/common/time_analyzer.hpp"


int main()
{
    srand((unsigned)time(NULL));


    const uint32_t window_width = 1920;
    const uint32_t window_height = 1080;
    WindowContextHandler app("Vicsek Model - MultiThread", sf::Vector2u(window_width, window_height), sf::Style::Default);
    RenderContext& render_context = app.getRenderContext();
    // Initialize solver and renderer


    tp::ThreadPool thread_pool(10);
    const IVec2 world_size{ 300, 300 };
    const uint32_t view_range = 5; // cell size == view range

    PhysicSolver solver{ world_size, view_range, thread_pool };
    Renderer renderer(solver, thread_pool);

    const float margin = 20.0f;
    const auto  zoom = static_cast<float>(window_height - margin) / static_cast<float>(world_size.y);
    render_context.setZoom(zoom);
    render_context.setFocus({ world_size.x * 0.5f, world_size.y * 0.5f });


    bool pasuse = false;
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::P, [&](sfev::CstEv) {
        pasuse = !pasuse;
        });

    constexpr uint32_t fps_cap = 60;
    int32_t target_fps = fps_cap;
    app.getEventManager().addKeyPressedCallback(sf::Keyboard::S, [&](sfev::CstEv) {
        target_fps = target_fps ? 0 : fps_cap;
        app.setFramerateLimit(target_fps);
        });

 
    for (uint32_t i{ 5000 }; i--;) {
        double random_x = ((double)rand() / RAND_MAX) * world_size.x;
        double random_y = ((double)rand() / RAND_MAX) * world_size.y;


        const auto id = solver.createObject({ random_x, random_y});

        solver.objects[id].velocity.x = ((double)rand() / RAND_MAX) * 10 - 5;
        solver.objects[id].velocity.y = ((double)rand() / RAND_MAX) * 10 - 5;
    }


    const float dt = 1.0f / static_cast<float>(fps_cap);

    clock_t time_req;

    // Main loop

    while (app.run()) {
        time_req = clock();

        if (!pasuse) {

            solver.update(dt);
        }
        render_context.clear();
        renderer.render(render_context);
        render_context.display();
        TimeAnalyzer::getInstance().setFPS(1000 / (clock() - time_req));
    }

    return 0;
}