#include <iostream>
#include "console_canvas.h"
#include "glm/vec3.hpp"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <thread>

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

    const glm::vec3 cube_vertices[8] = {
        //front side
        glm::vec3(-.5f, .5f, .5f),
        glm::vec3(.5f, .5f, .5f),
        glm::vec3(.5f, -.5f, .5f),
        glm::vec3(-.5f, -.5f, .5f),
        //back side
        glm::vec3(-.5f, .5f, -.5f),
        glm::vec3(.5f, .5f, -.5f),
        glm::vec3(.5f, -.5f, -.5f),
        glm::vec3(-.5f, -.5f, -.5f),
    };

    //degrees per second
    const float yaw_speed = 10; 
    const float pitch_speed = 1;
    const float roll_speed = 0;

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


        const uint64_t time_for_sleep_ms = (frame_time_ms > prev_frame_time) ?
                                           frame_time_ms - prev_frame_time : 0;

        std::this_thread::sleep_for(std::chrono::duration<uint64_t, std::milli>(time_for_sleep_ms));


        c.clear_screen();
       
        c.draw_string(glm::vec3(-1.f, 1.f, 1.f), "FPS: " + std::to_string(1000.f/prev_frame_time));
        c.draw_string(glm::vec3(-1.f, .9f, 1.f), "Frame time: " + std::to_string(prev_frame_time));

        glm::mat4 transform_matrix(1.f);
        transform_matrix = glm::rotate(transform_matrix, 
                                       glm::radians(yaw), 
                                       glm::vec3(0.f, 0.f, 1.f));

        transform_matrix = glm::rotate(transform_matrix, 
                                       glm::radians(pitch), 
                                       glm::vec3(0.f, 1.f, 0.f));
        
        transform_matrix = glm::rotate(transform_matrix, 
                                       glm::radians(roll), 
                                       glm::vec3(1.f, 0.f, 0.f));
        
        //prepare vertices
        glm::vec3 v[8];
        for(int i = 0; i < (int)(sizeof(v) / sizeof(v[0])); ++i)
        {
            glm::vec4 ext_vertice(cube_vertices[i], 1.f);
            ext_vertice = ext_vertice * transform_matrix;

            v[i] = glm::vec3(ext_vertice);

            c.highlight_point(v[i]);
        }

        c.draw_rectangle(v[0], v[1], v[2], v[3], depth_f);
        c.draw_rectangle(v[0], v[1], v[5], v[4], depth_f);
        c.draw_rectangle(v[4], v[5], v[6], v[7], depth_f);
        c.draw_rectangle(v[3], v[2], v[6], v[7], depth_f);
        c.draw_rectangle(v[0], v[3], v[7], v[4], depth_f);
        c.draw_rectangle(v[1], v[2], v[6], v[5], depth_f);

        char line_char = '*';
        c.draw_line(v[0], v[1], line_char);
        c.draw_line(v[1], v[2], line_char);
        c.draw_line(v[2], v[3], line_char);
        c.draw_line(v[3], v[0], line_char);

        c.draw_line(v[0 + 4], v[1 + 4], line_char);
        c.draw_line(v[1 + 4], v[2 + 4], line_char);
        c.draw_line(v[2 + 4], v[3 + 4], line_char);
        c.draw_line(v[3 + 4], v[0 + 4], line_char);

        c.draw_line(v[0], v[4], line_char);
        c.draw_line(v[1], v[5], line_char);
        c.draw_line(v[2], v[6], line_char);
        c.draw_line(v[3], v[7], line_char);


        
        c.render();

        
        //process movements for next frame
        yaw += yaw_speed * (long int)prev_frame_time / 1000;
        pitch += pitch_speed * (long int)prev_frame_time / 1000;
        roll += roll_speed * (long int)prev_frame_time / 1000;
    }


    return 0;
}
