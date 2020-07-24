#include "console_canvas.h"

#include <sys/ioctl.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include "glm/geometric.hpp"


Canvas::Canvas()
{
    update_viewport();
    clear_screen();
}

Canvas::~Canvas()
{
    free_screen();
}

void Canvas::free_screen()
{
    for(int i = 0; i < _height; ++i)
    {
        delete[] _screen_buffer[i];
    }
    delete[] _screen_buffer;


    for(int i = 0; i < _height; ++i)
    {
        delete[] _z_buffer[i];
    }
    delete[] _z_buffer;
}

glm::vec3 Canvas::viewport_extend(const glm::vec3& vec)
{
    glm::vec3 viewport_pos = vec;
    viewport_pos += 1.f;
    viewport_pos /= 2;

    viewport_pos.x = round(viewport_pos.x * (_width-1));
    viewport_pos.y = _height - round(viewport_pos.y * (_height-1));
    viewport_pos.z = round(viewport_pos.z * 255);

    return viewport_pos;
}

void Canvas::viewport_extend(const glm::vec3& vec, uint16_t& x, uint16_t& y, uint16_t& z)
{
    glm::vec3 dest = viewport_extend(vec);

    x = dest.x;
    y = dest.y;
    z = dest.z;
}

void Canvas::update_viewport()
{
    if(_screen_buffer != nullptr)
    {
        free_screen();
    }

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    _height = w.ws_row;
    _width = w.ws_col;

    _screen_buffer = new char*[_height];
    _z_buffer = new uint8_t*[_height];
    for(int i = 0; i < _height; ++i)
    {
        _screen_buffer[i] = new char[_width];
        _z_buffer[i] = new uint8_t[_width];
    }
}

void Canvas::draw_point(const glm::vec3& pos, char pix)
{
    //pos is normalized device coordinates, -1.f...1.f
    uint16_t x, y, z;
    viewport_extend(pos, x, y, z);

    if(x < 0 || x >= _width ||
       y < 0 || y >= _height||
       z < 0 || z >= 256)
    {
        return;
    }

    if(z <= _z_buffer[y][x])
    {
        return;
    }

    _z_buffer[y][x] = z;

    _screen_buffer[y][x] = pix;    
}

void Canvas::draw_line(const glm::vec3& a, const glm::vec3& b, char pix)
{
    const glm::vec3 line_vec = b-a;
 
    uint16_t view_x, view_y, view_z;
    viewport_extend(line_vec, view_x, view_y, view_z);

    const uint16_t points_count = round(glm::length(glm::vec3(view_x,
                                                              view_y,
                                                              0.f)));

    const float step = 1.f/points_count;
    for(int i = 0; i < points_count; ++i)
    {
        const float line_pos = step * i;

        draw_point(line_vec * line_pos + a, pix);
    }
}

void Canvas::draw_triangle(const glm::vec3& a, 
                           const glm::vec3& b, 
                           const glm::vec3& c, 
                           frag_func_t f)
{
    ;
}

void Canvas::draw_square(const glm::vec3& a, 
                         const glm::vec3& b, 
                         const glm::vec3& c, 
                         const glm::vec3& d, 
                         frag_func_t f)
{
    ;
}

void Canvas::clear_screen()
{
    for(int i = 0 ; i < _height; ++i)
    {
        memset(_screen_buffer[i], ' ', _width);
        memset(_z_buffer[i], 0, _width);
    }
}

void Canvas::render()
{
    for(int i = 0; i < _height; ++i)
    {
        std::cout << _screen_buffer[i];
    }
}