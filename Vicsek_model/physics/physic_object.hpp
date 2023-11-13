#pragma once
#include <iostream>
#include <math.h>  

#include "collision_grid.hpp"
#include "../engine/common/utils.hpp"
#include "../engine/common/math.hpp"


struct PhysicObject
{
    Vec2 position = { 0.0, 0.0 };
    Vec2 velocity = { 0.0, 0.0 };
    Vec2 nextVelocity = { 0.0, 0.0 };


    double velocity_module = 50.0;
    double noise_module = 30.0;

    sf::Color color;


    uint32_t actual_grid_id;


    PhysicObject() = default;

    explicit
        PhysicObject(Vec2 position_)
        : position(position_)
    {}

    void setPosition(Vec2 pos)
    {
        position = pos;
    }

    void update(double dt)
    {
        velocity += nextVelocity;

        velocity = normalizeVector(velocity, velocity_module);

        Vec2 noiseVector = { 0.0 , 0.0 };
        noiseVector.x = ((double)rand() / RAND_MAX) - 0.5;
        noiseVector.y = ((double)rand() / RAND_MAX) - 0.5;

        velocity += normalizeVector(noiseVector, noise_module);

        velocity = normalizeVector(velocity, velocity_module);

        const Vec2 new_position = position + velocity * dt;

        position = new_position;

        nextVelocity.x = 0.0;
        nextVelocity.y = 0.0;
    }

    Vec2 normalizeVector(Vec2 originalVector, double vectorLength)
    {
        // Oblicz bie¿¹c¹ d³ugoœæ wektora
        double vector_length = std::sqrt(originalVector.x * originalVector.x + originalVector.y * originalVector.y);
        // Przeskaluj wektor, aby zmieniæ d³ugoœæ
        if (vector_length > 0.0) {
            originalVector.x = (originalVector.x / vector_length) * vectorLength;
            originalVector.y = (originalVector.y / vector_length) * vectorLength;
        }

        return(originalVector);
    }

    void stop()
    {
        velocity.x = 0.0;
        velocity.y = 0.0;
    }

    void slowdown(double ratio)
    {
        velocity = ratio * (velocity);
    }

    [[nodiscard]]
    float getSpeed() const
    {
        return MathVec2::length(velocity);
    }

    [[nodiscard]]
    Vec2 getVelocity() const
    {
        return velocity;
    }

    void addVelocity(Vec2 v)
    {
        velocity += v;
    }

    void move(Vec2 v)
    {
        position += v;
    }

    sf::Color getColor() {
        const Vec2 last_update_move = velocity;
        const double velocity2 = last_update_move.x * last_update_move.x + last_update_move.y * last_update_move.y;
        //const double color_value = (atan(velocity2) * 2 / 3.1416) * 255;
        //color = sf::Color(color_value, 255 - color_value, 255 - color_value, 255);

        return color;
    }
};