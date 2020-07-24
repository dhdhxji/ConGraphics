#ifndef CONSOLE_CANVAS_H
#define CONSOLE_CANVAS_H

#include "glm/vec3.hpp"
#include <unistd.h>
#include <string>

typedef char (*frag_func_t)(const glm::vec3& position);

class Canvas
{
public:
    Canvas();
    ~Canvas();

    void update_viewport();

    void draw_point(const glm::vec3& pos, char pix);
    
    void draw_line(const glm::vec3& a, const glm::vec3& b, char pix);

    void draw_triangle(const glm::vec3& a, 
                       const glm::vec3& b, 
                       const glm::vec3& c,
                       frag_func_t f);

    void draw_square(const glm::vec3& a, 
                     const glm::vec3& b, 
                     const glm::vec3& c, 
                     const glm::vec3& d,
                     frag_func_t f);

    void clear_screen();
    void render();

protected:
    void free_screen();
    glm::vec3 viewport_extend(const glm::vec3& vec);
    void viewport_extend(const glm::vec3& vec, uint16_t& x, uint16_t& y, uint16_t& z);

protected: 
    uint16_t _width;
    uint16_t _height;

    char** _screen_buffer = nullptr;

    uint8_t** _z_buffer = nullptr;

};

#endif