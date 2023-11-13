#pragma once
#include <SFML/Graphics.hpp>

class TimeAnalyzer {

public:

    static TimeAnalyzer& getInstance() {
        static TimeAnalyzer instance; // Jedna instancja zostanie utworzona tylko raz.
        return instance;
    }

    // obliczanie œredniej liczby FPS po 8 ostatnich pomiarach
    uint16_t getFPS() {
        uint16_t mean = 0;
        for (uint16_t i{ 0 }; i < 8; i++) {
            mean += average_fps[i];
        }
        return mean / 8;
    }

    void setFPS(uint16_t actualFPS) {
        average_fps[fps_id] = actualFPS;
        fps_id++;
        if (fps_id >= 8)
            fps_id = 0;
    }

private:
    // Prywatny konstruktor, aby uniemo¿liwiæ tworzenie obiektów spoza klasy.
    TimeAnalyzer() 
    {
        simulation_start_time = clock();
    }
    // Prywatny destruktor, aby zapewniæ kontrolê nad cyklem ¿ycia obiektu.
    ~TimeAnalyzer() {}
    // Prywatny konstruktor kopiuj¹cy i operator przypisania, aby uniemo¿liwiæ kopiowanie.
    TimeAnalyzer(const TimeAnalyzer&) = delete;

    TimeAnalyzer& operator=(const TimeAnalyzer&) = delete;

private:
    uint16_t average_fps[8];
    uint16_t fps_id = 0;

public:
    clock_t simulation_start_time;
    float render_time = 0;
    float collision_time = 0;
    float clear_grid_time = 0;
    float update_grid_time = 0;
};
