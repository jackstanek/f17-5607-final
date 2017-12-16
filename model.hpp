#ifndef MODEL_HPP_
#define MODEL_HPP_

#include <vector>

#include "glad/glad.h"

class Model {
public:
    Model(const char* path);
    Model(const char* path, bool isObj);
    ~Model();

    int VertAttribCount() const;
    float* VertAttribData() const;

private:
    float* verts;
    int vert_count;
};

class ModelPool {
public:
    ModelPool();
    ~ModelPool();

    int Add(const char* path);
    int AddObj(const char* path);
    void LoadToGPU(GLuint vbo);

    int Offset(int id) const;
    int Length(int id) const;

private:
    std::vector<const Model*> models;
    std::vector<int> offsets;

};

#endif
