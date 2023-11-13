#include "renderer.hpp"
#include "../engine/common/time_analyzer.hpp"


Renderer::Renderer(PhysicSolver& solver_, tp::ThreadPool& tp)
    : solver{ solver_ }
    , world_va{ sf::Quads, 4 }
    , objects_va{ sf::Triangles }
    , thread_pool{ tp }
{
    initializeWorldVA();
}

void Renderer::render(RenderContext& context)
{
    context.draw(world_va);

    sf::RenderStates states;
    context.draw(world_va, states);
    // Boids
    updateParticlesVA();
    context.draw(objects_va, states);
    renderHUD(context);
}

void Renderer::initializeWorldVA()
{
    world_va[0].position = { 0.0f               , 0.0f };
    world_va[1].position = { (float)solver.world_size.x, 0.0f };
    world_va[2].position = { (float)solver.world_size.x, (float)solver.world_size.y };
    world_va[3].position = { 0.0f               , (float)solver.world_size.y };

    const uint8_t level = 50;
    const sf::Color background_color{ level, level, level };
    world_va[0].color = background_color;
    world_va[1].color = background_color;
    world_va[2].color = background_color;
    world_va[3].color = background_color;
}

void Renderer::updateParticlesVA()
{
    objects_va.resize(solver.objects.size() * 3);

    const float texture_size = 1024.0f;
    const float radius = 0.5f;

    thread_pool.dispatch(to<uint32_t>(solver.objects.size()), [&](uint32_t start, uint32_t end)   {
        for (uint32_t i{ start }; i < end; ++i) {
            PhysicObject& object = solver.objects.data[i];
            const uint32_t idx = i * 3;


            objects_va[idx + 0].position = FVec2{ (float)(object.velocity.x / object.velocity_module) , (float)(object.velocity.y / object.velocity_module) };
            objects_va[idx + 1].position = FVec2{ -(float)(object.velocity.y / (2 * object.velocity_module)) , (float)(object.velocity.x / (2 * object.velocity_module)) };
            objects_va[idx + 2].position = FVec2{ (float)(object.velocity.y / (2 * object.velocity_module)) , -(float)(object.velocity.x / (2 * object.velocity_module)) };

            for (uint32_t j{ idx }; j < idx + 3; ++j) {
                objects_va[j].position.x += (float)object.position.x;
                objects_va[j].position.y += (float)object.position.y;
            }

            objects_va[idx + 0].texCoords = { 0.0f        , 0.0f };
            objects_va[idx + 1].texCoords = { texture_size, 0.0f };
            objects_va[idx + 2].texCoords = { texture_size/2, texture_size };

            const sf::Color color = object.getColor();
            objects_va[idx + 0].color = color;
            objects_va[idx + 1].color = color;
            objects_va[idx + 2].color = color;
        }
    });
}

void Renderer::renderHUD(RenderContext& context)
{
    sf::Font font;
    font.loadFromFile("../res/adventpro-regular.ttf");


    const float margin = 20.0f;
    const float shift  = 40.0f;
    float       current_y = margin;
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::White);
    //text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    text.setString("Version: 1.0");
    text.setPosition({ margin, current_y });
    current_y += shift;
    context.renderToHUD(text);

    text.setString("Objects: " + toString(solver.objects.size()));
    text.setPosition({ margin, current_y });
    current_y += shift;
    context.renderToHUD(text);

    text.setString("Simulation FPS: " + toString(TimeAnalyzer::getInstance().getFPS()) + " FPS");
    text.setPosition({ margin, current_y });
    current_y += shift;
    context.renderToHUD(text);

    text.setString("Simulation Time: " + toString((int)((clock() - TimeAnalyzer::getInstance().simulation_start_time))/1000) + " s");
    text.setPosition({ margin, current_y });
    current_y += shift;
    context.renderToHUD(text);

    text.setString("Clear grid time: " + toString(TimeAnalyzer::getInstance().clear_grid_time) + " ms");
    text.setPosition({ margin, current_y });
    current_y += shift;
    context.renderToHUD(text);

    text.setString("Update grid time: " + toString(TimeAnalyzer::getInstance().update_grid_time) + " ms");
    text.setPosition({ margin, current_y });
    current_y += shift;
    context.renderToHUD(text);

    text.setString("Velocity Vector calc time: " + toString(TimeAnalyzer::getInstance().collision_time) + " ms");
    text.setPosition({ margin, current_y });
    current_y += shift;
    context.renderToHUD(text);

    text.setString("Zoom: " + toString(context.getZoom()));
    text.setPosition({ margin, current_y });
    current_y += shift;
    context.renderToHUD(text);
}