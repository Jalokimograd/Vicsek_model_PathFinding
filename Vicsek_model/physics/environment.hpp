#pragma once
#include <iostream>
#include <math.h>  
#include <time.h>

#include "collision_grid.hpp"
#include "physic_object.hpp"

struct Environment
{
public:
    const Vec2 greenBasePos = { 280.0, 150.0 };
    const Vec2 redBasePos = { 150.0, 150.0 };
    const double baseRadius = 5.0;



    static Environment& getInstance() {
        static Environment instance; // Jedna instancja zostanie utworzona tylko raz.
        return instance;
    }

    void solveContact(PhysicObject& obj_1, PhysicObject& obj_2, double cell_size)
    {
        constexpr double eps = 0.0001;


        const Vec2 o2_o1 = obj_1.position - obj_2.position;

        const double sqrDst = o2_o1.x * o2_o1.x + o2_o1.y * o2_o1.y;
        const double dist = sqrt(sqrDst);
        const double view_range = cell_size;

        if (dist < view_range && sqrDst > eps) {
            if (obj_1.role == 'S') {
                if (obj_2.role == 'O') {
                    obj_1.nextVelocity += o2_o1 * (1000 / sqrDst);
                }
                else if (obj_2.role == 'S') {
                    //obj_1.nextVelocity += obj_2.velocity;
                }
            }
            else if (obj_1.role == 'Z') {
                if (obj_2.role == 'O') {
                    obj_1.nextVelocity += o2_o1 * (1000 / sqrDst);
                }
                else if (obj_2.role == 'C') {
                    obj_1.nextVelocity -= obj_2.velocity * obj_2.annealingTime;
                }
                else if (obj_2.role == 'Z') {
                    //obj_1.nextVelocity += obj_2.velocity;
                }
            }
            else if (obj_1.role == 'C') {
                if (obj_2.role == 'O') {
                    obj_1.nextVelocity += o2_o1 * (1000 / sqrDst);
                }
                else if (obj_2.role == 'Z') {
                    obj_1.nextVelocity -= obj_2.velocity * obj_2.annealingTime;
                }
                else if (obj_2.role == 'C') {
                   // obj_1.nextVelocity += obj_2.velocity;
                }
            }
            
        }
    }

    void reachingTheBaseDetection(PhysicObject& obj_1)
    {
        Vec2 o2_b = obj_1.position - greenBasePos;
        double sqrDst = sqrt(o2_b.x * o2_b.x + o2_b.y * o2_b.y);

        if (sqrDst <= baseRadius) {
            obj_1.nextRole = 'C';
            return;
        }

        o2_b = obj_1.position - redBasePos;
        sqrDst = sqrt(o2_b.x * o2_b.x + o2_b.y * o2_b.y);

        if (sqrDst <= baseRadius) {
            obj_1.nextRole = 'Z';
            return;
        }
    }


private:
    // Prywatny konstruktor, aby uniemo¿liwiæ tworzenie obiektów spoza klasy.
    Environment()
    {
    }
    // Prywatny destruktor, aby zapewniæ kontrolê nad cyklem ¿ycia obiektu.
    ~Environment() {}
    // Prywatny konstruktor kopiuj¹cy i operator przypisania, aby uniemo¿liwiæ kopiowanie.
    Environment(const Environment&) = delete;

    Environment& operator=(const Environment&) = delete;
};