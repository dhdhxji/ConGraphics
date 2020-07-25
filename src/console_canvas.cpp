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

    if(z < _z_buffer[y][x])
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
    const glm::vec3 ab_vec = b-a;
    const glm::vec3 ac_vec = c-a;

    glm::vec3 view_ab_vec = viewport_extend(ab_vec);
    glm::vec3 view_ac_vec = viewport_extend(ac_vec);
    view_ab_vec.z = 0;
    view_ac_vec.z = 0;

    const uint16_t ab_point_cnt = std::max(view_ab_vec.x, view_ab_vec.y);
    const uint16_t ac_point_max_cnt = std::max(view_ac_vec.x, view_ac_vec.y);

    const float ab_step = 1.f/ab_point_cnt;
    const float ac_step = 1.f/ac_point_max_cnt;

    for(int ab = 0; ab < ab_point_cnt; ++ab)
    {
        uint16_t ac_point_cnt = ac_point_max_cnt * (ab_point_cnt - ab) / ab_point_cnt;
        for(int ac = 0; ac < ac_point_cnt; ++ac)
        {
            const float ab_coef = ab_step * ab;
            const float ac_coef = ac_step * ac;
            
            const glm::vec3 ab_pos = ab_vec * ab_coef;
            const glm::vec3 ac_pos = ac_vec * ac_coef;

            const glm::vec3 result_vec = (ab_pos + ac_pos) + a;
            draw_point(result_vec, f(result_vec));
        }
    }
}

void Canvas::draw_rectangle(const glm::vec3& a, 
                         const glm::vec3& b, 
                         const glm::vec3& c, 
                         const glm::vec3& d, 
                         frag_func_t f)
{
    draw_triangle(a, b, c, f);
    draw_triangle(a, c, d, f);
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