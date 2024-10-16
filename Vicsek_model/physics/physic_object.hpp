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
    char nextRole = 'S';
    char role = 'S';
    double annealingTime = 10.0;


    double velocity_module = 50.0;
    double noise_module = 10.0;

    sf::Color color;


    uint32_t actual_grid_id;


    PhysicObject() = default;

    explicit
        PhysicObject(Vec2 position_, char role_)
        : position(position_),
        nextRole(role_)
    {
        color = sf::Color(0, 0, 0, 255);
    }

    void setPosition(Vec2 pos)
    {
        position = pos;
    }

    void update(double dt)
    {
        if (role != 'O') {

            
            if (annealingTime < 0)
                nextRole = 'S';

            if (role != 'S')
                annealingTime -= 0.001;
            

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
        if (nextRole != role)
            roleChange(nextRole);
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

    void roleChange(char newRole) {
        annealingTime = 1.0;

        if (newRole == 'C') {
            role = 'C';
            color = sf::Color(255, 0, 0, 255);
            velocity = -velocity;
        }
        else if (newRole == 'Z') {
            role = 'Z';
            color = sf::Color(0, 255, 0, 255);
            velocity = -velocity;
        }
        else if (newRole == 'S') {
            role = 'S';
            color = sf::Color(0, 0, 0, 255);
            velocity = -velocity;
        }
        else if (newRole == 'O') {
            role = 'O';
            color = sf::Color(255, 255, 255, 255);
        }
    }
};

/*
struct Obstacle : PhysicObject {
    char role = 'O';

    sf::Color color = sf::Color(255, 255, 255, 255);

    Obstacle()
    {
        // Inicjalizacje domyœlne, jeœli s¹ potrzebne
    }

    explicit Obstacle(Vec2 position_)
        : PhysicObject(position_)  // Wywo³anie konstruktora klasy bazowej
    {
        color = sf::Color(255, 255, 255, 255);
    }

    void update(double dt) 
    {
    }
};
*/