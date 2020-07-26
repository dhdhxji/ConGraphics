#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <thread>

#include "console_canvas.h"
#include "cube.h"
#include "glm/vec3.hpp"

#define MAX_FPS 30

char depth_f(const glm::vec3& pos)
{
    if(pos.z <= -0.34f)
    {
        return '.';
    }
    else if(pos.z <= 0.32)
    {
        return '-';
    }
    else
    {
        return '=';
    }
}

int main()
{
    Canvas c;

    Cube cube(&c, glm::vec3(0.f, 0.f, 0.f), 1.f);

    //degrees per second
    const float yaw_speed = 10; 
    const float pitch_speed = 5;
    const float roll_speed = 2;

    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    const uint64_t frame_time_ms = 1000/MAX_FPS;

    auto time = std::chrono::high_resolution_clock::now();
    while(true)
    {
        //time stuff
        uint64_t prev_frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - time).count();

        time = std::chrono::high_resolution_clock::now();


        c.clear_screen();

        c.draw_string(glm::vec3(-1.f, 1.f, 1.f), "FPS: " + std::to_string(1000.f/prev_frame_time));
        c.draw_string(glm::vec3(-1.f, .9f, 1.f), "Frame time: " + std::to_string(prev_frame_time));


        cube.rotate(glm::vec3(1.f, 0.f, 0.f), pitch);
        cube.rotate(glm::vec3(0.f, 1.f, 0.f), yaw);
        cube.rotate(glm::vec3(0.f, 0.f, 1.f), roll);
        cube.render(depth_f);
        
        c.render();

        
        //process movements for next frame
        yaw = yaw_speed * (long int)prev_frame_time / 1000;
        pitch = pitch_speed * (long int)prev_frame_time / 1000;
        roll = roll_speed * (long int)prev_frame_time / 1000;

        uint64_t code_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::high_resolution_clock::now() - time).count();

        uint64_t sleep_time = ( code_time < frame_time_ms ) ? 
                                frame_time_ms - code_time : 0;

        std::this_thread::sleep_for(std::chrono::duration<uint64_t, std::milli>(sleep_time));
    }


    return 0;
}
