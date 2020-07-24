#ifndef CONSOLE_CANVAS_H
#define CONSOLE_CANVAS_H

#include "glm/vec3.hpp"
#include <unistd.h>
#include <string>

class Canvas
{
public:
    Canvas();
    ~Canvas();

    void update_viewport();

    void draw_point(const glm::vec3& pos, char pix);
    
    void draw_line(const glm::vec3& a, const glm::vec3& b, char pix);

    void draw_triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    void draw_square(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d);

    void clear_screen();
    void render();

protected:
    void free_screen();

protected: 
    uint16_t _width;
    uint16_t _height;

    char** _screen_buffer = nullptr;

    uint8_t** _z_buffer = nullptr;

};

#endif