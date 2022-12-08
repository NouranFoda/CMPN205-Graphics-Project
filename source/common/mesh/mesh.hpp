#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
    public:

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            //TODO: (Req 2) Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc

            //create vertex array object and bind it 
            glGenVertexArrays(1, &VAO);
            //1 is the number of vertex array objects to be generated
            glBindVertexArray(VAO);
            
            // create vertex buffer object and bind it
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            // copy vertex data to VRAM
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);
            // GL_ARRAY_BUFFER is a type of buffer object used to store vertex attributes, 
            //such as vertex coordinates, texture coordinates, vertex normals, vertex colors.
            //vertices.size() * sizeof(Vertex) is the size of the buffer object's new data in bytes.
            //&vertices.front() is a pointer to the data that will be copied into the data store for initialization.
            //GL_STATIC_DRAW is how a buffer object's data store will be accessed. It means that the data store contents will be modified once and used many times.

            // create element buffer object and bind it
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            // copy element data to VRAM
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), &elements.front(), GL_STATIC_DRAW);
            // GL_ELEMENT_ARRAY_BUFFER is a type of buffer object used to store element indices.

            // set the vertex attribute pointers
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
            // ATTRIB_LOC_POSITION is the index of the generic vertex attribute to be modified.
            // 3 is the number of components per generic vertex attribute.
            // GL_FLOAT is the data type of each component in the array.
            // GL_FALSE is whether fixed-point data values should be normalized (GL_TRUE) or converted directly as fixed-point values (GL_FALSE) when they are accessed.
            // sizeof(Vertex) is the byte offset between consecutive generic vertex attributes.
            // (void*)offsetof(Vertex, position) is offset of the attribute in the buffer

            //SIMILARLY FOR OTHER ATTRIBUTES
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            

            // enable the vertex attribute arrays
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);

            // unbind the VAO
            glBindVertexArray(0);

            // unbind the VBO
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // unbind the EBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            elementCount = (GLsizei)elements.size();

        }

        // this function should render the mesh
        void draw() 
        {
            //TODO: (Req 2) Write this function
            // bind the VAO
            glBindVertexArray(VAO);

            // bind the EBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            // draw the triangles
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
            // GL_TRIANGLES is the type of primitives to render. It can be GL_POINTS, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, etc.
            // elementCount is the number of elements to be rendered.
            // GL_UNSIGNED_INT is the data type of the indices in the element array buffer.

            // unbind the VAO
            glBindVertexArray(0);

        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 2) Write this function

            // delete the VBO , EBO and VAO
            glDeleteBuffers(1, &VBO);
            //1 is the number of buffer objects to be deleted.
            //&VBO is a pointer to the buffer objects to be deleted.
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);

        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}