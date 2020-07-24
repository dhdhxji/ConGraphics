#include "console_canvas.h"

#include <sys/ioctl.h>
#include <cmath>
#include <iostream>
#include <cstring>


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
    glm::vec3 viewport_pos = pos;
    viewport_pos += 1.f;
    viewport_pos /= 2;

    const uint16_t x = round(viewport_pos.x * _width);
    const uint16_t y = round(viewport_pos.y * +_height);
    const uint8_t z = round(viewport_pos.z * 255);

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
    ;
}

void Canvas::draw_triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
    ;
}

void Canvas::draw_square(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
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