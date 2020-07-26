#ifndef CUBE_H
#define CUBE_H

#include "console_canvas.h"
#include "glm/vec3.hpp"
#include <vector>

struct Face
{
    uint8_t indices[4];
};

struct Line
{
    uint8_t indices[2];
};

class Cube
{
public:
    Cube(Canvas* c, const glm::vec3& pos, float width);

    void move(const glm::vec3& offset);
    void set_pos(const glm::vec3& pos);
    void rotate(const glm::vec3 axis, float angle);

    void render(frag_func_t fragment_shader);

protected:
    std::vector<glm::vec3> _vertices;

    std::vector<Face> _faces;
    std::vector<Line> _lines;

    Canvas* _canvas;
};

#endif
