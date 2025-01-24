//
//  sphere.h
//  test
//
//  Created by Nazirul Hasan on 26/9/23.
//

#ifndef sphere_h
#define sphere_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

# define PI 3.1416

using namespace std;

const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

class Parabola
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    // ctor/dtor
    Parabola(float radius = 1.0f, int sectorCount = 36, int stackCount = 18, glm::vec3 amb = glm::vec3(0.98, 0.847, 0.69), glm::vec3 diff = glm::vec3(0.98, 0.847, 0.69), glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f), float shiny = 32.0f) : verticesStride(24)
    {
        set(radius, sectorCount, stackCount, amb, diff, spec, shiny);
        buildCoordinatesAndIndices();
        buildVertices();

        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);

        // create VBO to copy vertex data to VBO
        unsigned int sphereVBO;
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
            this->getVertexSize(), // data size, # of bytes
            this->getVertices(),   // ptr to vertex data
            GL_STATIC_DRAW);                   // usage

        // create EBO to copy index data
        unsigned int sphereEBO;
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
            this->getIndexSize(),             // data size, # of bytes
            this->getIndices(),               // ptr to index data
            GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // set attrib arrays with stride and offset
        int stride = this->getVerticesStride();     // should be 24 bytes
        glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

        // unbind VAO and VBOs
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    ~Parabola() {}

    // getters/setters

    void set(float radius, int sectors, int stacks, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        if (radius > 0)
            this->radius = radius;
        this->sectorCount = sectors;
        if (sectors < MIN_SECTOR_COUNT)
            this->sectorCount = MIN_SECTOR_COUNT;
        this->stackCount = stacks;
        if (stacks < MIN_STACK_COUNT)
            this->stackCount = MIN_STACK_COUNT;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

    void setRadius(float radius)
    {
        if (radius != this->radius)
            set(radius, sectorCount, stackCount, ambient, diffuse, specular, shininess);
    }

    void setSectorCount(int sectors)
    {
        if (sectors != this->sectorCount)
            set(radius, sectors, stackCount, ambient, diffuse, specular, shininess);
    }

    void setStackCount(int stacks)
    {
        if (stacks != this->stackCount)
            set(radius, sectorCount, stacks, ambient, diffuse, specular, shininess);
    }

    // for interleaved vertices
    unsigned int getVertexCount() const
    {
        return (unsigned int)coordinates.size() / 3;     // # of vertices
    }

    unsigned int getVertexSize() const
    {
        return (unsigned int)vertices.size() * sizeof(float);  // # of bytes
    }

    int getVerticesStride() const
    {
        return verticesStride;   // should be 24 bytes
    }
    const float* getVertices() const
    {
        return vertices.data();
    }

    unsigned int getIndexSize() const
    {
        return (unsigned int)indices.size() * sizeof(unsigned int);
    }

    const unsigned int* getIndices() const
    {
        return indices.data();
    }

    unsigned int getIndexCount() const
    {
        return (unsigned int)indices.size();
    }

    // draw in VertexArray mode
    void drawParabola(Shader& lightingShader, glm::mat4 model) const      // draw surface
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        // draw a sphere with VAO
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES,                    // primitive type
            this->getIndexCount(),          // # of indices
            GL_UNSIGNED_INT,                 // data type
            (void*)0);                       // offset to indices

        // unbind VAO
        glBindVertexArray(0);
    }

private:
    // member functions
    void buildCoordinatesAndIndices()
    {
        float height = 6.0f;
        float radius = 4.0f;
        int sectorCount = 72;
        int stackCount = 36;
        float sectorStep = 2 * PI / sectorCount;
        float stackStep = height / stackCount;
        float nx, ny, nz;

        // Loop over the stacks (height levels)
        for (int i = 0; i <= stackCount; ++i)
        {
            float stackHeight = height * (i / float(stackCount));
            float radiusAtStack = radius * sqrt(stackHeight / height);

            // Loop over the sectors (around the base)
            for (int j = 0; j <= sectorCount; ++j)
            {
                float sectorAngle = j * sectorStep;
                float x = radiusAtStack * cosf(sectorAngle);
                float z = radiusAtStack * sinf(sectorAngle);

                // Push the vertex position (invert height for downward-facing)
                coordinates.push_back(x);
                coordinates.push_back(-stackHeight); // Invert height for downward face
                coordinates.push_back(z);

                // Normals for a downward paraboloid (pointing radially inward)
                nx = x / sqrt(x * x + z * z);
                ny = 0.0f;  // Normal vector lies in the xz-plane.
                nz = z / sqrt(x * x + z * z);
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
        }

        // Loop to create indices for the side surface
        for (int i = 0; i < stackCount; ++i)
        {
            for (int j = 0; j < sectorCount; ++j)
            {
                int current = i * (sectorCount + 1) + j;
                int next = (i + 1) * (sectorCount + 1) + j;

                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }
    }



    void buildVertices()
    {
        size_t i, j;
        size_t count = coordinates.size();
        for (i = 0, j = 0; i < count; i += 3, j += 2)
        {
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);
        }
    }

    vector<float> computeFaceNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
    {
        const float EPSILON = 0.000001f;

        vector<float> normal(3, 0.0f);     // default return value (0,0,0)
        float nx, ny, nz;

        // find 2 edge vectors: v1-v2, v1-v3
        float ex1 = x2 - x1;
        float ey1 = y2 - y1;
        float ez1 = z2 - z1;
        float ex2 = x3 - x1;
        float ey2 = y3 - y1;
        float ez2 = z3 - z1;

        // cross product: e1 x e2
        nx = ey1 * ez2 - ez1 * ey2;
        ny = ez1 * ex2 - ex1 * ez2;
        nz = ex1 * ey2 - ey1 * ex2;

        // normalize only if the length is > 0
        float length = sqrtf(nx * nx + ny * ny + nz * nz);
        if (length > EPSILON)
        {
            // normalize
            float lengthInv = 1.0f / length;
            normal[0] = nx * lengthInv;
            normal[1] = ny * lengthInv;
            normal[2] = nz * lengthInv;
        }

        return normal;
    }
    class Cone
    {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        // ctor/dtor
        Cone(float radius = 1.0f, int sectorCount = 36, int stackCount = 18, glm::vec3 amb = glm::vec3(0.98, 0.847, 0.69), glm::vec3 diff = glm::vec3(0.98, 0.847, 0.69), glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f), float shiny = 32.0f) : verticesStride(24)
        {
            set(radius, sectorCount, stackCount, amb, diff, spec, shiny);
            buildCoordinatesAndIndices();
            buildVertices();

            glGenVertexArrays(1, &sphereVAO);
            glBindVertexArray(sphereVAO);

            // create VBO to copy vertex data to VBO
            unsigned int sphereVBO;
            glGenBuffers(1, &sphereVBO);
            glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);           // for vertex data
            glBufferData(GL_ARRAY_BUFFER,                   // target
                this->getVertexSize(), // data size, # of bytes
                this->getVertices(),   // ptr to vertex data
                GL_STATIC_DRAW);                   // usage

            // create EBO to copy index data
            unsigned int sphereEBO;
            glGenBuffers(1, &sphereEBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);   // for index data
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
                this->getIndexSize(),             // data size, # of bytes
                this->getIndices(),               // ptr to index data
                GL_STATIC_DRAW);                   // usage

            // activate attrib arrays
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            // set attrib arrays with stride and offset
            int stride = this->getVerticesStride();     // should be 24 bytes
            glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

            // unbind VAO and VBOs
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        ~Cone() {}

        // getters/setters

        void set(float radius, int sectors, int stacks, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
        {
            if (radius > 0)
                this->radius = radius;
            this->sectorCount = sectors;
            if (sectors < MIN_SECTOR_COUNT)
                this->sectorCount = MIN_SECTOR_COUNT;
            this->stackCount = stacks;
            if (stacks < MIN_STACK_COUNT)
                this->stackCount = MIN_STACK_COUNT;
            this->ambient = amb;
            this->diffuse = diff;
            this->specular = spec;
            this->shininess = shiny;
        }

        void setRadius(float radius)
        {
            if (radius != this->radius)
                set(radius, sectorCount, stackCount, ambient, diffuse, specular, shininess);
        }

        void setSectorCount(int sectors)
        {
            if (sectors != this->sectorCount)
                set(radius, sectors, stackCount, ambient, diffuse, specular, shininess);
        }

        void setStackCount(int stacks)
        {
            if (stacks != this->stackCount)
                set(radius, sectorCount, stacks, ambient, diffuse, specular, shininess);
        }

        // for interleaved vertices
        unsigned int getVertexCount() const
        {
            return (unsigned int)coordinates.size() / 3;     // # of vertices
        }

        unsigned int getVertexSize() const
        {
            return (unsigned int)vertices.size() * sizeof(float);  // # of bytes
        }

        int getVerticesStride() const
        {
            return verticesStride;   // should be 24 bytes
        }
        const float* getVertices() const
        {
            return vertices.data();
        }

        unsigned int getIndexSize() const
        {
            return (unsigned int)indices.size() * sizeof(unsigned int);
        }

        const unsigned int* getIndices() const
        {
            return indices.data();
        }

        unsigned int getIndexCount() const
        {
            return (unsigned int)indices.size();
        }

        // draw in VertexArray mode
        void drawCone(Shader& lightingShader, glm::mat4 model) const      // draw surface
        {
            lightingShader.use();

            lightingShader.setVec3("material.ambient", this->ambient);
            lightingShader.setVec3("material.diffuse", this->diffuse);
            lightingShader.setVec3("material.specular", this->specular);
            lightingShader.setFloat("material.shininess", this->shininess);

            lightingShader.setMat4("model", model);

            // draw a sphere with VAO
            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES,                    // primitive type
                this->getIndexCount(),          // # of indices
                GL_UNSIGNED_INT,                 // data type
                (void*)0);                       // offset to indices

            // unbind VAO
            glBindVertexArray(0);
        }

    private:
        // member functions
        void buildCoordinatesAndIndices()
        {
            float height = 2.0;
            float sectorStep = 2 * PI / sectorCount;  // Step size for sector
            float slantHeight = sqrtf(radius * radius + height * height); // Slant height of the cone
            float nx, ny, nz;

            // Base circle vertices and normals
            for (int j = 0; j <= sectorCount; ++j)
            {
                float sectorAngle = j * sectorStep;
                float x = radius * cosf(sectorAngle);
                float z = radius * sinf(sectorAngle);

                // Base vertex position
                coordinates.push_back(x);
                coordinates.push_back(0.0f);
                coordinates.push_back(z);

                // Normal pointing down for base
                normals.push_back(0.0f);
                normals.push_back(-1.0f);
                normals.push_back(0.0f);
            }

            // Apex vertex
            coordinates.push_back(0.0f);
            coordinates.push_back(height);
            coordinates.push_back(0.0f);

            normals.push_back(0.0f);
            normals.push_back(1.0f);
            normals.push_back(0.0f);

            int apexIndex = coordinates.size() / 3 - 1;

            // Side surface normals and indices
            for (int j = 0; j <= sectorCount; ++j)
            {
                float sectorAngle = j * sectorStep;
                float x = radius * cosf(sectorAngle);
                float z = radius * sinf(sectorAngle);

                // Side normal vector components
                nx = x / slantHeight;
                ny = radius / slantHeight;  // Slope of cone's side
                nz = z / slantHeight;

                normals[j * 3 + 0] = nx;
                normals[j * 3 + 1] = ny;
                normals[j * 3 + 2] = nz;
            }

            // Side indices
            for (int j = 0; j < sectorCount; ++j)
            {
                indices.push_back(j);          // Base current vertex
                indices.push_back(apexIndex);  // Apex vertex
                indices.push_back(j + 1);      // Base next vertex
            }

            //// Bottom center vertex
            //coordinates.push_back(0.0f);
            //coordinates.push_back(0.0f);
            //coordinates.push_back(0.0f);

            //normals.push_back(0.0f);
            //normals.push_back(-1.0f);
            //normals.push_back(0.0f);

            //int bottomCenterIndex = coordinates.size() / 3 - 1;

            //// Bottom cap indices
            //for (int j = 0; j < sectorCount; ++j)
            //{
            //    indices.push_back(bottomCenterIndex);
            //    indices.push_back(j + 1);
            //    indices.push_back(j);
            //}
        }

        void buildVertices()
        {
            size_t i, j;
            size_t count = coordinates.size();
            for (i = 0, j = 0; i < count; i += 3, j += 2)
            {
                vertices.push_back(coordinates[i]);
                vertices.push_back(coordinates[i + 1]);
                vertices.push_back(coordinates[i + 2]);

                vertices.push_back(normals[i]);
                vertices.push_back(normals[i + 1]);
                vertices.push_back(normals[i + 2]);
            }
        }

        vector<float> computeFaceNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
        {
            const float EPSILON = 0.000001f;

            vector<float> normal(3, 0.0f);     // default return value (0,0,0)
            float nx, ny, nz;

            // find 2 edge vectors: v1-v2, v1-v3
            float ex1 = x2 - x1;
            float ey1 = y2 - y1;
            float ez1 = z2 - z1;
            float ex2 = x3 - x1;
            float ey2 = y3 - y1;
            float ez2 = z3 - z1;

            // cross product: e1 x e2
            nx = ey1 * ez2 - ez1 * ey2;
            ny = ez1 * ex2 - ex1 * ez2;
            nz = ex1 * ey2 - ey1 * ex2;

            // normalize only if the length is > 0
            float length = sqrtf(nx * nx + ny * ny + nz * nz);
            if (length > EPSILON)
            {
                // normalize
                float lengthInv = 1.0f / length;
                normal[0] = nx * lengthInv;
                normal[1] = ny * lengthInv;
                normal[2] = nz * lengthInv;
            }

            return normal;
        }

        // memeber vars
        unsigned int sphereVAO;
        float radius;
        int sectorCount;                        // longitude, # of slices
        int stackCount;                         // latitude, # of stacks
        vector<float> vertices;
        vector<float> normals;
        vector<unsigned int> indices;
        vector<float> coordinates;
        int verticesStride;                 // # of bytes to hop to the next vertex (should be 24 bytes)

    };



    // memeber vars
    unsigned int sphereVAO;
    float radius;
    int sectorCount;                        // longitude, # of slices
    int stackCount;                         // latitude, # of stacks
    vector<float> vertices;
    vector<float> normals;
    vector<unsigned int> indices;
    vector<float> coordinates;
    int verticesStride;                 // # of bytes to hop to the next vertex (should be 24 bytes)

};


class Cylinder
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    // ctor/dtor
    Cylinder(float radius = 1.0f, int sectorCount = 36, int stackCount = 18, glm::vec3 amb = glm::vec3(0.922, 0.91, 0.902), glm::vec3 diff = glm::vec3(0.922, 0.91, 0.902), glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f), float shiny = 32.0f) : verticesStride(24)
    {
        set(radius, sectorCount, stackCount, amb, diff, spec, shiny);
        buildCoordinatesAndIndices();
        buildVertices();

        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);

        // create VBO to copy vertex data to VBO
        unsigned int sphereVBO;
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
            this->getVertexSize(), // data size, # of bytes
            this->getVertices(),   // ptr to vertex data
            GL_STATIC_DRAW);                   // usage

        // create EBO to copy index data
        unsigned int sphereEBO;
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
            this->getIndexSize(),             // data size, # of bytes
            this->getIndices(),               // ptr to index data
            GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // set attrib arrays with stride and offset
        int stride = this->getVerticesStride();     // should be 24 bytes
        glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

        // unbind VAO and VBOs
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    ~Cylinder() {}

    // getters/setters

    void set(float radius, int sectors, int stacks, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        if (radius > 0)
            this->radius = radius;
        this->sectorCount = sectors;
        if (sectors < MIN_SECTOR_COUNT)
            this->sectorCount = MIN_SECTOR_COUNT;
        this->stackCount = stacks;
        if (stacks < MIN_STACK_COUNT)
            this->stackCount = MIN_STACK_COUNT;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

    void setRadius(float radius)
    {
        if (radius != this->radius)
            set(radius, sectorCount, stackCount, ambient, diffuse, specular, shininess);
    }

    void setSectorCount(int sectors)
    {
        if (sectors != this->sectorCount)
            set(radius, sectors, stackCount, ambient, diffuse, specular, shininess);
    }

    void setStackCount(int stacks)
    {
        if (stacks != this->stackCount)
            set(radius, sectorCount, stacks, ambient, diffuse, specular, shininess);
    }

    // for interleaved vertices
    unsigned int getVertexCount() const
    {
        return (unsigned int)coordinates.size() / 3;     // # of vertices
    }

    unsigned int getVertexSize() const
    {
        return (unsigned int)vertices.size() * sizeof(float);  // # of bytes
    }

    int getVerticesStride() const
    {
        return verticesStride;   // should be 24 bytes
    }
    const float* getVertices() const
    {
        return vertices.data();
    }

    unsigned int getIndexSize() const
    {
        return (unsigned int)indices.size() * sizeof(unsigned int);
    }

    const unsigned int* getIndices() const
    {
        return indices.data();
    }

    unsigned int getIndexCount() const
    {
        return (unsigned int)indices.size();
    }

    // draw in VertexArray mode
    void drawCylinder(Shader& lightingShader, glm::mat4 model) const      // draw surface
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        // draw a sphere with VAO
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES,                    // primitive type
            this->getIndexCount(),          // # of indices
            GL_UNSIGNED_INT,                 // data type
            (void*)0);                       // offset to indices

        // unbind VAO
        glBindVertexArray(0);
    }

private:
    // member functions
    void buildCoordinatesAndIndices()
    {
        float x, y, z, xz;                              // vertex position
        float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle = -sectorStep;
        float stackAngle = PI / 2 + stackStep;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle -= stackStep;        // starting from pi/2 to -pi/2
            //xz = radius * cosf(stackAngle);
            x = radius * sinf(stackAngle);
            // add (sectorCount+1) vertices per stack
            // first and last vertices have same position and normal, but different tex coords
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle += sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                z = radius * cosf(sectorAngle);
                y = radius * sinf(sectorAngle);
                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                // normalized vertex normal (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
        }

        // generate index list of sphere triangles
        // k1--k1+1
        // |  / |
        // | /  |
        // k2--k2+1

        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks

                    // k1 => k2 => k1+1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                // k1+1 => k2 => k2+1
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);

                // 2 triangles per sector excluding first and last stacks

            }
            // Top center vertex
            float topCenterY = radius * sinf(PI / 2);
            coordinates.push_back(topCenterY);
            coordinates.push_back(0.0f);
            coordinates.push_back(0.0f);

            normals.push_back(1.0f);
            normals.push_back(0.0f);
            normals.push_back(0.0f);

            // Bottom center vertex
            float bottomCenterY = radius * sinf(-PI / 2);
            coordinates.push_back(bottomCenterY);
            coordinates.push_back(0.0f);
            coordinates.push_back(0.0f);

            normals.push_back(-1.0f);
            normals.push_back(0.0f);
            normals.push_back(0.0f);

            int topCenterIndex = coordinates.size() / 3 - 2;
            int bottomCenterIndex = coordinates.size() / 3 - 1;

            // Top Cap
            for (int j = 0; j < sectorCount; ++j)
            {
                indices.push_back(topCenterIndex);
                indices.push_back(j);
                indices.push_back(j + 1);
            }

            // Bottom Cap
            int baseIndex = stackCount * (sectorCount + 1);
            for (int j = 0; j < sectorCount; ++j)
            {
                indices.push_back(bottomCenterIndex);
                indices.push_back(baseIndex + j + 1);
                indices.push_back(baseIndex + j);
            }
        }
    }

    void buildVertices()
    {
        size_t i, j;
        size_t count = coordinates.size();
        for (i = 0, j = 0; i < count; i += 3, j += 2)
        {
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);
        }
    }

    vector<float> computeFaceNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
    {
        const float EPSILON = 0.000001f;

        vector<float> normal(3, 0.0f);     // default return value (0,0,0)
        float nx, ny, nz;

        // find 2 edge vectors: v1-v2, v1-v3
        float ex1 = x2 - x1;
        float ey1 = y2 - y1;
        float ez1 = z2 - z1;
        float ex2 = x3 - x1;
        float ey2 = y3 - y1;
        float ez2 = z3 - z1;

        // cross product: e1 x e2
        nx = ey1 * ez2 - ez1 * ey2;
        ny = ez1 * ex2 - ex1 * ez2;
        nz = ex1 * ey2 - ey1 * ex2;

        // normalize only if the length is > 0
        float length = sqrtf(nx * nx + ny * ny + nz * nz);
        if (length > EPSILON)
        {
            // normalize
            float lengthInv = 1.0f / length;
            normal[0] = nx * lengthInv;
            normal[1] = ny * lengthInv;
            normal[2] = nz * lengthInv;
        }

        return normal;
    }

    // memeber vars
    unsigned int sphereVAO;
    float radius;
    int sectorCount;                        // longitude, # of slices
    int stackCount;                         // latitude, # of stacks
    vector<float> vertices;
    vector<float> normals;
    vector<unsigned int> indices;
    vector<float> coordinates;
    int verticesStride;                 // # of bytes to hop to the next vertex (should be 24 bytes)

};

class ConeTex
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    // ctor/dtor
    ConeTex(float radius = 1.5f, int sectorCount = 36, int stackCount = 20, glm::vec3 amb = glm::vec3(0.98, 0.847, 0.69), glm::vec3 diff = glm::vec3(0.98, 0.847, 0.69), glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f), float shiny = 32.0f) : verticesStride(32)
    {
        set(radius, sectorCount, stackCount, amb, diff, spec, shiny);
        buildCoordinatesAndIndices();
        buildVertices();

        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);

        // create VBO to copy vertex data to VBO
        unsigned int sphereVBO;
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
            this->getVertexSize(), // data size, # of bytes
            this->getVertices(),   // ptr to vertex data
            GL_STATIC_DRAW);                   // usage

        // create EBO to copy index data
        unsigned int sphereEBO;
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
            this->getIndexSize(),             // data size, # of bytes
            this->getIndices(),               // ptr to index data
            GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        // set attrib arrays with stride and offset
        int stride = this->getVerticesStride();     // should be 24 bytes
        glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));      // Texture Coordinates

        // unbind VAO and VBOs
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    ~ConeTex() {}

    // getters/setters

    void set(float radius, int sectors, int stacks, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        if (radius > 0)
            this->radius = radius;
        this->sectorCount = sectors;
        if (sectors < MIN_SECTOR_COUNT)
            this->sectorCount = MIN_SECTOR_COUNT;
        this->stackCount = stacks;
        if (stacks < MIN_STACK_COUNT)
            this->stackCount = MIN_STACK_COUNT;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

    void setRadius(float radius)
    {
        if (radius != this->radius)
            set(radius, sectorCount, stackCount, ambient, diffuse, specular, shininess);
    }

    void setSectorCount(int sectors)
    {
        if (sectors != this->sectorCount)
            set(radius, sectors, stackCount, ambient, diffuse, specular, shininess);
    }

    void setStackCount(int stacks)
    {
        if (stacks != this->stackCount)
            set(radius, sectorCount, stacks, ambient, diffuse, specular, shininess);
    }

    // for interleaved vertices
    unsigned int getVertexCount() const
    {
        return (unsigned int)coordinates.size() / 3;     // # of vertices
    }

    unsigned int getVertexSize() const
    {
        return (unsigned int)vertices.size() * sizeof(float);  // # of bytes
    }

    int getVerticesStride() const
    {
        return verticesStride;   // should be 32 bytes
    }
    const float* getVertices() const
    {
        return vertices.data();
    }

    unsigned int getIndexSize() const
    {
        return (unsigned int)indices.size() * sizeof(unsigned int);
    }

    const unsigned int* getIndices() const
    {
        return indices.data();
    }

    unsigned int getIndexCount() const
    {
        return (unsigned int)indices.size();
    }

    // draw in VertexArray mode
    void drawCone(Shader& lightingShader, unsigned int texture, glm::mat4 model) const      // draw surface
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // draw a sphere with VAO
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES,                    // primitive type
            this->getIndexCount(),          // # of indices
            GL_UNSIGNED_INT,                 // data type
            (void*)0);                       // offset to indices

        // unbind VAO
        glBindVertexArray(0);
    }

private:
    // member functions
    void buildCoordinatesAndIndices()
    {
        float height = 1.5;
        float sectorStep = 2 * PI / sectorCount;  // Step size for sector
        float slantHeight = sqrtf(radius * radius + height * height); // Slant height of the cone
        float nx, ny, nz;
        // Texture coordinates
        float texCoordX, texCoordY;

        // Base circle vertices and normals
        for (int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);

            // Base vertex position
            coordinates.push_back(x);
            coordinates.push_back(0.0f);
            coordinates.push_back(z);

            // Normal pointing down for base
            normals.push_back(0.0f);
            normals.push_back(-1.0f);
            normals.push_back(0.0f);

            // Base texture coordinates
            texCoordX = (x / radius + 1.0f) / 2.0f; // Texture X coordinate (normalized)
            texCoordY = (z / radius + 1.0f) / 2.0f; // Texture Y coordinate (normalized)
            texCoords.push_back(texCoordX);
            texCoords.push_back(texCoordY);
        }

        // Apex vertex
        coordinates.push_back(0.0f);
        coordinates.push_back(height);
        coordinates.push_back(0.0f);

        normals.push_back(0.0f);
        normals.push_back(1.0f);
        normals.push_back(0.0f);

        // Apex texture coordinates (at the center of the texture)
        texCoords.push_back(0.5f);
        texCoords.push_back(0.5f);

        int apexIndex = coordinates.size() / 3 - 1;

        // Side surface normals and indices
        for (int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);

            // Side normal vector components
            nx = x / slantHeight;
            ny = radius / slantHeight;  // Slope of cone's side
            nz = z / slantHeight;

            normals[j * 3 + 0] = nx;
            normals[j * 3 + 1] = ny;
            normals[j * 3 + 2] = nz;


            // Side texture coordinates
            texCoordX = (float)j / sectorCount;  // Horizontal texture coordinate (0 to 1)
            texCoordY = 1.0f;  // Vertical texture coordinate for the side (fixed, top of the cone)
            texCoords.push_back(texCoordX);
            texCoords.push_back(texCoordY);
        }

        // Side indices
        for (int j = 0; j < sectorCount; ++j)
        {
            indices.push_back(j);          // Base current vertex
            indices.push_back(apexIndex);  // Apex vertex
            indices.push_back(j + 1);      // Base next vertex
        }

        //// Bottom center vertex
        //coordinates.push_back(0.0f);
        //coordinates.push_back(0.0f);
        //coordinates.push_back(0.0f);

        //normals.push_back(0.0f);
        //normals.push_back(-1.0f);
        //normals.push_back(0.0f);

        //int bottomCenterIndex = coordinates.size() / 3 - 1;

        //// Bottom cap indices
        //for (int j = 0; j < sectorCount; ++j)
        //{
        //    indices.push_back(bottomCenterIndex);
        //    indices.push_back(j + 1);
        //    indices.push_back(j);
        //}
    }

    void buildVertices()
    {
        size_t i, j;
        size_t count = coordinates.size();
        for (i = 0, j = 0; i < count; i += 3, j += 2)
        {
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);

            // Texture coordinate
            vertices.push_back(texCoords[j]);
            vertices.push_back(texCoords[j + 1]);
        }
    }

    vector<float> computeFaceNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
    {
        const float EPSILON = 0.000001f;

        vector<float> normal(3, 0.0f);     // default return value (0,0,0)
        float nx, ny, nz;

        // find 2 edge vectors: v1-v2, v1-v3
        float ex1 = x2 - x1;
        float ey1 = y2 - y1;
        float ez1 = z2 - z1;
        float ex2 = x3 - x1;
        float ey2 = y3 - y1;
        float ez2 = z3 - z1;

        // cross product: e1 x e2
        nx = ey1 * ez2 - ez1 * ey2;
        ny = ez1 * ex2 - ex1 * ez2;
        nz = ex1 * ey2 - ey1 * ex2;

        // normalize only if the length is > 0
        float length = sqrtf(nx * nx + ny * ny + nz * nz);
        if (length > EPSILON)
        {
            // normalize
            float lengthInv = 1.0f / length;
            normal[0] = nx * lengthInv;
            normal[1] = ny * lengthInv;
            normal[2] = nz * lengthInv;
        }

        return normal;
    }

    // memeber vars
    unsigned int sphereVAO;
    float radius;
    int sectorCount;                        // longitude, # of slices
    int stackCount;                         // latitude, # of stacks
    vector<float> vertices;
    vector<float> normals;
    vector<float> texCoords;
    vector<unsigned int> indices;
    vector<float> coordinates;
    int verticesStride;                 // # of bytes to hop to the next vertex (should be 24 bytes)

};

class CylinderTex
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    // ctor/dtor
    CylinderTex(float radius = 1.0f, int sectorCount = 36, int stackCount = 18, glm::vec3 amb = glm::vec3(0.922, 0.91, 0.902), glm::vec3 diff = glm::vec3(0.922, 0.91, 0.902), glm::vec3 spec = glm::vec3(0.1f, 0.1f, 0.1f), float shiny = 8.0f) : verticesStride(32)
    {
        set(radius, sectorCount, stackCount, amb, diff, spec, shiny);
        buildCoordinatesAndIndices();
        buildVertices();

        glGenVertexArrays(1, &sphereVAO);
        glBindVertexArray(sphereVAO);

        // create VBO to copy vertex data to VBO
        unsigned int sphereVBO;
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
            this->getVertexSize(), // data size, # of bytes
            this->getVertices(),   // ptr to vertex data
            GL_STATIC_DRAW);                   // usage

        // create EBO to copy index data
        unsigned int sphereEBO;
        glGenBuffers(1, &sphereEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
            this->getIndexSize(),             // data size, # of bytes
            this->getIndices(),               // ptr to index data
            GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        // set attrib arrays with stride and offset
        int stride = this->getVerticesStride();     // should be 24 bytes
        glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));      // Texture Coordinates

        // unbind VAO and VBOs
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    ~CylinderTex() {}

    // getters/setters

    void set(float radius, int sectors, int stacks, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        if (radius > 0)
            this->radius = radius;
        this->sectorCount = sectors;
        if (sectors < MIN_SECTOR_COUNT)
            this->sectorCount = MIN_SECTOR_COUNT;
        this->stackCount = stacks;
        if (stacks < MIN_STACK_COUNT)
            this->stackCount = MIN_STACK_COUNT;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

    // for interleaved vertices
    unsigned int getVertexCount() const
    {
        return (unsigned int)coordinates.size() / 3;     // # of vertices
    }

    unsigned int getVertexSize() const
    {
        return (unsigned int)vertices.size() * sizeof(float);  // # of bytes
    }

    int getVerticesStride() const
    {
        return verticesStride;   // should be 32 bytes
    }
    const float* getVertices() const
    {
        return vertices.data();
    }

    unsigned int getIndexSize() const
    {
        return (unsigned int)indices.size() * sizeof(unsigned int);
    }

    const unsigned int* getIndices() const
    {
        return indices.data();
    }

    unsigned int getIndexCount() const
    {
        return (unsigned int)indices.size();
    }

    // draw in VertexArray mode
    void drawCylinder(Shader& lightingShader, unsigned int texture, glm::mat4 model) const      // draw surface
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // draw a sphere with VAO
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES,                    // primitive type
            this->getIndexCount(),          // # of indices
            GL_UNSIGNED_INT,                 // data type
            (void*)0);                       // offset to indices

        // unbind VAO
        glBindVertexArray(0);
    }

private:
    // member functions
    void buildCoordinatesAndIndices()
    {
        float x, y, z, xz;
        float nx, ny, nz, lengthInv = 1.0f / radius;
        float s, t;

        // Introduce noise or sine wave to make the trunk look more organic
        float trunkNoiseFactor = 0.05f; // Controls the distortion intensity

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle = -sectorStep;
        float stackAngle = PI / 2 + stackStep;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle -= stackStep;
            x = radius * sinf(stackAngle);

            // Apply distortion to the trunk for each stack
            float distortion = trunkNoiseFactor * sinf(stackAngle * 5.0f);  // Vary distortion based on the angle
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle += sectorStep;

                // Apply distortion to radius for the tree bark effect
                xz = radius + distortion + trunkNoiseFactor * (rand() % 1000 / 1000.0f);  // Random noise distortion

                z = xz * cosf(sectorAngle);
                y = xz * sinf(sectorAngle);
                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);

                s = (float)j / sectorCount; // U-coordinate
                t = (float)i / stackCount; // V-coordinate
                texCoords.push_back(s);
                texCoords.push_back(t);
            }
        }

        // Generate index list for the tree trunk's triangles
        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);
            k2 = k1 + sectorCount + 1;

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    

    }

    void buildVertices()
    {
        size_t i, j;
        size_t count = coordinates.size();
        for (i = 0, j = 0; i < count; i += 3, j += 2)
        {
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);

            size_t texIndex = i / 3 * 2;
            vertices.push_back(texCoords[texIndex]);
            vertices.push_back(texCoords[texIndex + 1]);
        }
    }

    vector<float> computeFaceNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
    {
        const float EPSILON = 0.000001f;

        vector<float> normal(3, 0.0f);     // default return value (0,0,0)
        float nx, ny, nz;

        // find 2 edge vectors: v1-v2, v1-v3
        float ex1 = x2 - x1;
        float ey1 = y2 - y1;
        float ez1 = z2 - z1;
        float ex2 = x3 - x1;
        float ey2 = y3 - y1;
        float ez2 = z3 - z1;

        // cross product: e1 x e2
        nx = ey1 * ez2 - ez1 * ey2;
        ny = ez1 * ex2 - ex1 * ez2;
        nz = ex1 * ey2 - ey1 * ex2;

        // normalize only if the length is > 0
        float length = sqrtf(nx * nx + ny * ny + nz * nz);
        if (length > EPSILON)
        {
            // normalize
            float lengthInv = 1.0f / length;
            normal[0] = nx * lengthInv;
            normal[1] = ny * lengthInv;
            normal[2] = nz * lengthInv;
        }

        return normal;
    }

    // memeber vars
    unsigned int sphereVAO;
    float radius;
    int sectorCount;                        // longitude, # of slices
    int stackCount;                         // latitude, # of stacks
    vector<float> vertices;
    vector<float> normals;
    vector<float> texCoords;
    vector<unsigned int> indices;
    vector<float> coordinates;
    int verticesStride;                 // # of bytes to hop to the next vertex (should be 24 bytes)

};

#endif /* sphere_h */


