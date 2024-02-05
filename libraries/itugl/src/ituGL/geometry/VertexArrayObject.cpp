#include <ituGL/geometry/VertexArrayObject.h>

#include <ituGL/geometry/VertexAttribute.h>

// Create the object initially null, get object handle and generate 1 vertex array
VertexArrayObject::VertexArrayObject() : Object(NullHandle)
{
    Handle& handle = GetHandle();
    
    // (todo) 00.1: Generate 1 vertex array
    glGenVertexArrays(1, &handle);
}

// Get object handle and delete 1 vertex array
VertexArrayObject::~VertexArrayObject()
{
    Handle& handle = GetHandle();

    // (todo) 00.1: Delete 1 vertex array
    if(handle != 0)
        {
            glDeleteVertexArrays(1, &handle);
            handle = 0;
        }
}

// Bind the vertex array handle to the specific target
void VertexArrayObject::Bind() const
{
    Handle handle = GetHandle();
    // (todo) 00.1: Bind vertex array
    
    glBindVertexArray(handle);
}

// Bind the null handle to the specific target
void VertexArrayObject::Unbind()
{
    Handle handle = NullHandle;
    // (todo) 00.1: Bind null vertex array
    
    glBindVertexArray(handle);
}

// Sets the VertexAttribute pointer and enables the VertexAttribute in that location
void VertexArrayObject::SetAttribute(GLuint location, const VertexAttribute& attribute, GLint offset, GLsizei stride)
{
    // Get the attribute properties in OpenGL expected format
    GLint components = attribute.GetComponents();
    GLenum type = static_cast<GLenum>(attribute.GetType());
    GLboolean normalized = attribute.IsNormalized() ? GL_TRUE : GL_FALSE;

    // Compute the attribute pointer
    const unsigned char* pointer = nullptr; // Actual base pointer is in VBO
    //pointer += offset;

    // (todo) 00.1: Set the VertexAttribute pointer in that location
    glVertexAttribPointer(location, components, type, normalized, stride, pointer);

    // (todo) 00.1: Finally, we enable the VertexAttribute in that location
    glEnableVertexAttribArray(location);
}
