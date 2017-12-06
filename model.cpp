#include <algorithm>
#include <fstream>

#include "glad/glad.h"

#include "model.hpp"

Model::Model(const char* path) :
    vert_count(0)
{
    //Load Model 1
    std::ifstream model_file;
    model_file.open(path);
    model_file >> vert_count;
    verts = new float[vert_count];
    for (int i = 0; i < vert_count; i++) {
        model_file >> verts[i];
    }
    model_file.close();
}

Model::~Model()
{
    delete[] verts;
}

int Model::VertAttribCount() const
{
    return vert_count;
}

float* Model::VertAttribData() const
{
    return verts;
}

ModelPool::ModelPool() :
    models(0)
{
}

ModelPool::~ModelPool()
{
    for (size_t i = 0; i < models.size(); i++) {
        if (models[i]) {
            delete models[i];
        }
    }
}

int ModelPool::Add(const char *path)
{
    Model* new_model = new Model(path);
    int curr_id = models.size();
    if (curr_id == 0) {
        offsets.push_back(0);
    } else {
        offsets.push_back(offsets.back() + models.back()->VertAttribCount());
    }
    models.push_back(new_model);
    return curr_id;
}

int ModelPool::Offset(int id) const
{
    return offsets[id] / 8; /* This divide-by-8 is specific to our format */
}

int ModelPool::Length(int id) const
{
    return models[id]->VertAttribCount() / 8;
}

void ModelPool::LoadToGPU(GLuint vbo)
{
    size_t i, total_verts = 0;
    for (i = 0; i < models.size(); i++) {
        if (models[i]) {
            total_verts += models[i]->VertAttribCount();
        }
    }

    float* buf = new float[total_verts];
    size_t curr = 0;
    for (i = 0; i < models.size(); i++) {
        const Model* model = models[i];
        if (model) {
            float* data = model->VertAttribData();
            std::copy(data, data + model->VertAttribCount(), buf + curr);
            curr += model->VertAttribCount();
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
    glBufferData(GL_ARRAY_BUFFER, total_verts * sizeof(float), buf, GL_STATIC_DRAW); //upload vertices to vbo
    delete[] buf;
}
