#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

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

Model::Model(const char* path, bool isObj) :
    vert_count(0)
{
    if (isObj){
        std::vector< std::vector<float> > v;
        std::vector< std::vector<float> > n;

        FILE* fp;
        fp = fopen(path,"r");
        if (fp == NULL) {
            printf("Error opening %s.\n", path);
            exit(EXIT_FAILURE);
        }

        char line[1024];
        std::vector<float> face;
        while(fgets(line,1024,fp)) {
            char command[50];
            int fieldsRead = sscanf(line, "%s ", command);
            std::string commandStr = command;

            if (fieldsRead < 1) continue;

            if (commandStr == "v"){
                float x, y, z;
                sscanf(line, "v %f %f %f",&x, &y, &z);
                std::vector<float> temp = {x, y, z};
                v.push_back(temp);
            }
            else if (commandStr == "vn"){
                float x, y, z;
                sscanf(line, "vn %f %f %f",&x, &y, &z);
                std::vector<float> temp = {x, y, z};
                n.push_back(temp);
            }
            else if (commandStr == "f"){
                int px, nx, py, ny, pz, nz;
                sscanf(line, "f %i//%i %i//%i %i//%i",&px,&nx, &py,&ny, &pz, &nz);
                face.insert(std::end(face), std::begin(v[px-1]), std::end(v[px-1]));
                face.push_back(0);
                face.push_back(0);
                face.insert(std::end(face), std::begin(n[ny-1]), std::end(n[ny-1]));

                face.insert(std::end(face), std::begin(v[py-1]), std::end(v[py-1]));
                face.push_back(0);
                face.push_back(0);
                face.insert(std::end(face), std::begin(n[nx-1]), std::end(n[nx-1]));

                face.insert(std::end(face), std::begin(v[pz-1]), std::end(v[pz-1]));
                face.push_back(0);
                face.push_back(0);
                face.insert(std::end(face), std::begin(n[nz-1]), std::end(n[nz-1]));
            }
        }
        vert_count = face.size();
        verts = new float[vert_count];
        for(int i = 0;i < face.size();i++){
            verts[i] = face.at(i);
        }
        printf("finished adding obj\n");
        delete fp;
    } else {
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

int ModelPool::AddObj(const char *path)
{
    Model* new_model = new Model(path,true);
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
