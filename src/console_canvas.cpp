#include "console_canvas.h"

#include <sys/ioctl.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <sstream>
#include <string>
#include <iomanip>

#include "glm/geometric.hpp"
#include "glm/vec2.hpp"

//math stuff
static glm::vec3 barometric(const glm::vec2& a,
                            const glm::vec2& b,
                            const glm::vec2& c,
                            const glm::vec2& p)
{
    const glm::vec3 v_x(b.x - a.x, c.x - a.x, a.x - p.x);
    const glm::vec3 v_y(b.y - a.y, c.y - a.y, a.y - p.y);
     
    const glm::vec3 u = glm::cross(v_x, v_y);
    
    return glm::vec3(1.f - (u.x + u.y)/u.z,
                     u.x/u.z,
                     u.y/u.z);
}

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
    viewport_pos.y = (_height-1) - round(viewport_pos.y * (_height-1));
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
    draw_viewport_point(viewport_extend(pos), pix); 
}

void Canvas::draw_point(const glm::vec3& pos, frag_func_t f)
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

    _screen_buffer[y][x] = f(pos);
}

void Canvas::draw_viewport_point(const glm::vec3& pos, char pix)
{
    const int x = pos.x;
    const int y = pos.y;
    const int z = pos.z;

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
    const glm::vec3 vp_a = viewport_extend(a);
    const glm::vec3 vp_b = viewport_extend(b);
    const glm::vec3 vp_c = viewport_extend(c);

    glm::vec2 box_min(vp_a.x, vp_a.y);
    glm::vec2 box_max(vp_a.x, vp_a.y);

    box_min.x = std::min(box_min.x, vp_b.x);
    box_min.x = std::min(box_min.x, vp_c.x);
    box_min.y = std::min(box_min.y, vp_b.y);
    box_min.y = std::min(box_min.y, vp_c.y);

    box_max.x = std::max(box_max.x, vp_b.x);
    box_max.x = std::max(box_max.x, vp_c.x);
    box_max.y = std::max(box_max.y, vp_b.y);
    box_max.y = std::max(box_max.y, vp_c.y);

    for(int y = box_min.y; y < box_max.y; ++y)
    {
        for(int x = box_min.x; x < box_max.x; ++x)
        {
            glm::vec3 baro = barometric(vp_a, vp_b, vp_c, glm::vec2(x, y));
            
            if(baro.x < 0.f || baro.y < 0.f || baro.z < 0.f)
            {
                continue;
            }
            else
            {
                const glm::vec3 res = a*baro.x + b*baro.y + c*baro.z;

                draw_point(res, f);
            }
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

void Canvas::highlight_point(const glm::vec3& p)
{
    const glm::vec3 p_front(p.x, p.y, 1.f);
    const glm::vec3 vp_coords = viewport_extend(p_front);
    

    std::stringstream ss;
    ss << std::setprecision(3) << std::fixed << "(" << p.x << ";" << p.y << ";" << p.z << ")";

    std::string point_info = ss.str();
    
    const glm::vec3 l_vec(point_info.length() + 2, 0.f, 0.f);

    const glm::vec3 l1_start((float)vp_coords.x+1, vp_coords.y-1, vp_coords.z);
    const glm::vec3 l2_start((float)vp_coords.x+1, vp_coords.y+1, vp_coords.z);

    for(int i = 0; i < (int)point_info.length() + 2; ++i)
    {
        draw_viewport_point(l1_start + glm::vec3(i, 0.f, 0.f), ' ');
        draw_viewport_point(l2_start + glm::vec3(i, 0.f, 0.f), ' ');
    }

    draw_viewport_point(glm::vec3(vp_coords.x + 1, vp_coords.y, 255), ' ');
    draw_viewport_point(glm::vec3(vp_coords.x + 2 + point_info.length(), vp_coords.y, 255), ' ');

    glm::vec3 text_base_pos(vp_coords.x + 2, vp_coords.y, 255);
    for(int i = 0; i < (int)(point_info.length()); ++i)
    {
        glm::vec3 text_offset(i, 0.f, 0.f);

        draw_viewport_point(text_base_pos + text_offset, point_info[i]);
    }            
}

void Canvas::draw_string(const glm::vec3& pos, const std::string& str)
{
    const glm::vec3 vp_pos = viewport_extend(pos);

    for(int i = 0; i < (int)str.length(); ++i)
    {
        draw_viewport_point(vp_pos + glm::vec3(vp_pos.x + i, 0.f, 0.f), str[i]);
    }
}

void Canvas::clear_screen()
{
    std::cout << "\e[0;0H";

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