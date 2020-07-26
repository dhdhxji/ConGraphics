#include "cube.h"

#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

Cube::Cube(Canvas* c, const glm::vec3& pos, float width)
{
    width = width/2;

    _canvas = c;

    //Generate all vertices
    for(int z_dir = 1; z_dir >= -1; z_dir-=2)
        for(int y_dir = 1; y_dir >= -1; y_dir-=2)
            for(int x_dir = -1; x_dir <= 1; x_dir+=2)
            {
                glm::vec3 pos(0.f);
                pos.x = width * x_dir;
                pos.y = width * y_dir;
                pos.z = width * z_dir;

                _vertices.push_back(pos);
            }

    //Fill up line indices
    _lines.push_back({0,1});
    _lines.push_back({0,2});
    _lines.push_back({0,4});

    _lines.push_back({5,1});
    _lines.push_back({5,4});
    _lines.push_back({5,7});

    _lines.push_back({6,2});
    _lines.push_back({6,4});
    _lines.push_back({6,7});

    _lines.push_back({3,1});
    _lines.push_back({3,2});
    _lines.push_back({3,7});

    //Fill up face indices
    _faces.push_back({0,1,3,2});
    _faces.push_back({4,5,7,6});

    _faces.push_back({1,5,7,3});
    _faces.push_back({0,4,6,2});

    _faces.push_back({0,4,5,1});
    _faces.push_back({2,6,7,3});
}

void Cube::move(const glm::vec3& offset)
{
    for(int i = 0; i < (int)_vertices.size(); ++i)
    {
        _vertices[i] += offset;
    }
}

void Cube::set_pos(const glm::vec3& pos)
{
    glm::vec3 current_pos(0.f);
    for(int i = 0; i < (int)_vertices.size(); ++i)
    {
        current_pos += _vertices[i];
    }

    current_pos = current_pos/(float)_vertices.size();

    glm::vec3 offset = pos - current_pos;
    move(offset);
}

void Cube::rotate(const glm::vec3 axis, float angle)
{
    glm::mat4 transform = glm::rotate(glm::mat4(1.f), 
                                      glm::radians(angle),
                                      axis);

    for(int i = 0; i < (int)_vertices.size(); ++i)
    {
        _vertices[i] = transform * glm::vec4(_vertices[i], 1.f);
    }
}

void Cube::render(frag_func_t fragment_shader)
{
    for(auto vertice: _vertices)
    {
        _canvas->highlight_point(vertice);
    }

    for(auto face: _faces)
    {
        _canvas->draw_rectangle(_vertices[face.indices[0]],
                                _vertices[face.indices[1]],
                                _vertices[face.indices[2]],
                                _vertices[face.indices[3]],
                                fragment_shader);
    }

    for(auto line: _lines)
    {
        _canvas->draw_line(_vertices[line.indices[0]],
                           _vertices[line.indices[1]],
                           '*');
    }
}