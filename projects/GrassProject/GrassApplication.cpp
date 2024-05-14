#include "GrassApplication.h"

// For Mesh & Texture
#include <ituGL/geometry/VertexFormat.h>
#include <ituGL/texture/Texture2DObject.h>

#include <glm/gtx/transform.hpp>  // for matrix transformations

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cmath>
#include <iostream>
#include <numbers>  // for PI constant

// For ImGUI
#include <imgui.h>

GrassApplication::GrassApplication()
    : Application(1680, 944, "Stylized Grass")
    , m_gridX(128), m_gridY(128)
    , m_vertexShaderLoader(Shader::Type::VertexShader)
    , m_fragmentShaderLoader(Shader::Type::FragmentShader)
    // Grass
    , m_baseGrassColor(glm::vec4(0.0f, 0.4f, 0.0f, 1.0f))
    , m_tipGrassColor(glm::vec4(0.5f, 1.0f, 0.0f, 1.0f))
    , m_bladeHeight(1.0f)
    , m_windDirection(glm::vec2(0.5f, 0.5f))
    , m_windStrength(0.25f)
    , m_amount(8000)
    , time(0)
    // Camera
    , m_cameraPos(glm::vec3(0.0f, 5.0f, 15.0f))
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(false)
    , m_cameraEnablePressed(false)
    , m_mousePosition(GetMainWindow().GetMousePosition(true))
    // Light
    , m_ambientColor(glm::vec3(0.25f))
    , m_lightColor(glm::vec3(1.0f))
    , m_lightIntensity(1.0f)
    , m_lightPosition(glm::vec3(0.0f, 2.0f, 0.0f))
{
}

void GrassApplication::Update()
{
    Application::Update();

    // Update Camera
    UpdateCamera();

    // Update Grass based on time elapsed
    time += Application::GetDeltaTime();
    UpdateUniforms();
}

void GrassApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Terrain patches
    DrawObject(m_terrainPatch, *m_terrainMaterial, glm::translate(glm::vec3(0.0f, -1.5f, 0.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_terrainMaterial, glm::translate(glm::vec3(-10.0f, -1.5f, 0.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_terrainMaterial, glm::translate(glm::vec3(0.0f, -1.5f, -10.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_terrainMaterial, glm::translate(glm::vec3(-10.0f, -1.5f, -10.0f)) * glm::scale(glm::vec3(10.0f)));

    // Grass Instancing
    DrawObjectInstanced(m_grassBlade, *m_grassMaterial, glm::translate(glm::vec3(0.0f, -1.5f, 0.0f)), m_amount);

    // Render GUI
    RenderGUI();
}

void GrassApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void GrassApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    // Add debug controls for light properties
    ImGui::DragFloat("Grass Height", &m_bladeHeight, 0.01f, 0.0f, 2.0f);
    ImGui::DragInt("Grass Density", &m_amount, 2, 0, 100000);
    ImGui::Separator();
    ImGui::ColorEdit3("Base Color", &m_baseGrassColor[0]);
    ImGui::ColorEdit3("Tip Color", &m_tipGrassColor[0]);
    ImGui::Separator();
    ImGui::DragFloat2("Wind Direction", &m_windDirection[0], 0.01f, -1.0f, 1.0f);
    ImGui::DragFloat("Wind Strength", &m_windStrength, 0.01f, 0.0f, 10.0f);

    ImGui::Separator();
    ImGui::ColorEdit3("Ambient color", &m_ambientColor[0]);
    ImGui::DragFloat3("Light position", &m_lightPosition[0], 0.1f);
    ImGui::ColorEdit3("Light color", &m_lightColor[0]);
    ImGui::DragFloat("Light intensity", &m_lightIntensity, 0.05f, 0.0f, 5.0f);

    m_imGui.EndFrame();
}

void GrassApplication::Initialize()
{
    // Initialize the application window
    Application::Initialize();
    
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    
    InitializeTextures();
    InitializeMaterials();
    InitializeMeshes();

    //Enable depth test
    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
}

void GrassApplication::InitializeCamera()
{
    // Set view matrix, from the camera position looking to the origin
    m_camera.SetViewMatrix(m_cameraPos, glm::vec3(0.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void GrassApplication::InitializeTextures()
{
    // Load terrain textures
    m_dirtTexture = LoadTexture("textures/dirt.png");
    m_grassTexture = LoadTexture("textures/grass.jpg");

    // Create Perlin Gradient Noise texture
    m_noisePattern = CreateNoisePattern();
}

void GrassApplication::InitializeMaterials()
{
    // Setup perlin noise pattern for applying wind
    m_noisePattern = CreateNoisePattern();

    // Setup shader program for grass
    Shader grassVS = m_vertexShaderLoader.Load("shaders/grass.vert");
    Shader grassFS = m_fragmentShaderLoader.Load("shaders/grass.frag");
    std::shared_ptr<ShaderProgram> grassShaderProgram = std::make_shared<ShaderProgram>();
    grassShaderProgram->Build(grassVS, grassFS);

    // Setup Grass Material
    m_grassMaterial = std::make_shared<Material>(grassShaderProgram);

    // Grass Settings
    m_grassMaterial->SetUniformValue("baseColor", m_baseGrassColor);
    m_grassMaterial->SetUniformValue("tipColor", m_tipGrassColor);
    m_grassMaterial->SetUniformValue("bladeHeight", m_bladeHeight);
    m_grassMaterial->SetUniformValue("noise", m_noisePattern);
    m_grassMaterial->SetUniformValue("time", time);
    m_grassMaterial->SetUniformValue("direction", m_windDirection);
    m_grassMaterial->SetUniformValue("strength", m_windStrength);

    // Light Settings
    m_grassMaterial->SetUniformValue("AmbientColor", m_ambientColor);
    m_grassMaterial->SetUniformValue("LightColor", m_lightColor * m_lightIntensity);
    m_grassMaterial->SetUniformValue("LightRange", 5.0f * m_lightIntensity);
    m_grassMaterial->SetUniformValue("LightPosition", m_lightPosition);

    m_grassMaterial->SetUniformValue("AmbientReflection", 1.0f);
    m_grassMaterial->SetUniformValue("DiffuseReflection", 1.0f);

    // Terrain shader
    Shader terrainVS = m_vertexShaderLoader.Load("shaders/terrain.vert");
    Shader terrainFS = m_fragmentShaderLoader.Load("shaders/terrain.frag");
    std::shared_ptr<ShaderProgram> terrainShaderProgram = std::make_shared<ShaderProgram>();
    terrainShaderProgram->Build(terrainVS, terrainFS);

    // Terrain material
    m_terrainMaterial = std::make_shared<Material>(terrainShaderProgram);
    m_terrainMaterial->SetUniformValue("Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_terrainMaterial->SetUniformValue("ColorTexture", m_dirtTexture);
    m_terrainMaterial->SetUniformValue("ColorTextureScale", glm::vec2(0.0625f));

    m_terrainMaterial->SetUniformValue("AmbientColor", m_ambientColor);
    m_terrainMaterial->SetUniformValue("LightColor", m_lightColor * m_lightIntensity);
    m_terrainMaterial->SetUniformValue("LightRange", 5.0f * m_lightIntensity);
    m_terrainMaterial->SetUniformValue("LightPosition", m_lightPosition);
    m_terrainMaterial->SetUniformValue("AmbientReflection", 1.0f);
    m_terrainMaterial->SetUniformValue("DiffuseReflection", 1.0f);

    m_terrainMaterial->SetBlendEquation(Material::BlendEquation::Add);
    m_terrainMaterial->SetBlendParams(Material::BlendParam::SourceAlpha, Material::BlendParam::OneMinusSourceAlpha);
}

void GrassApplication::InitializeMeshes()
{
    CreateTerrainMesh(m_terrainPatch, m_gridX, m_gridY);
    CreateGrassMesh(m_grassBlade, m_bladeHeight);
}

void GrassApplication::UpdateUniforms()
{
    // Update Grass Uniforms
    m_grassMaterial->SetUniformValue("time", time);

    m_grassMaterial->SetUniformValue("baseColor", m_baseGrassColor);
    m_grassMaterial->SetUniformValue("tipColor", m_tipGrassColor);

    m_grassMaterial->SetUniformValue("bladeHeight", m_bladeHeight);
    m_grassMaterial->SetUniformValue("direction", m_windDirection);
    m_grassMaterial->SetUniformValue("strength", m_windStrength);

    m_grassMaterial->SetUniformValue("AmbientColor", m_ambientColor);
    m_grassMaterial->SetUniformValue("LightColor", m_lightColor * m_lightIntensity);
    m_grassMaterial->SetUniformValue("LightRange", 5.0f * m_lightIntensity);
    m_grassMaterial->SetUniformValue("LightPosition", m_lightPosition);

    // Update Terrain Uniforms
    m_terrainMaterial->SetUniformValue("AmbientColor", m_ambientColor);
    m_terrainMaterial->SetUniformValue("LightColor", m_lightColor * m_lightIntensity);
    m_terrainMaterial->SetUniformValue("LightRange", 5.0f * m_lightIntensity);
    m_terrainMaterial->SetUniformValue("LightPosition", m_lightPosition);
}

void GrassApplication::UpdateCamera()
{
    Window& window = GetMainWindow();

    // Update if camera is enabled (controlled by SPACE key)
    {
        bool enablePressed = window.IsKeyPressed(GLFW_KEY_SPACE);
        if (enablePressed && !m_cameraEnablePressed)
        {
            m_cameraEnabled = !m_cameraEnabled;

            window.SetMouseVisible(!m_cameraEnabled);
            m_mousePosition = window.GetMousePosition(true);
        }
        m_cameraEnablePressed = enablePressed;
    }

    if (!m_cameraEnabled)
        return;

    glm::mat4 viewTransposedMatrix = glm::transpose(m_camera.GetViewMatrix());
    glm::vec3 viewRight = viewTransposedMatrix[0];
    glm::vec3 viewForward = -viewTransposedMatrix[2];

    // Update camera translation
    {
        glm::vec2 inputTranslation(0.0f);

        if (window.IsKeyPressed(GLFW_KEY_A))
            inputTranslation.x = -1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_D))
            inputTranslation.x = 1.0f;

        if (window.IsKeyPressed(GLFW_KEY_W))
            inputTranslation.y = 1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_S))
            inputTranslation.y = -1.0f;

        inputTranslation *= m_cameraTranslationSpeed;
        inputTranslation *= GetDeltaTime();

        // Double speed if SHIFT is pressed
        if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            inputTranslation *= 2.0f;

        m_cameraPos += inputTranslation.x * viewRight + inputTranslation.y * viewForward;
    }

    // Update camera rotation
    {
        glm::vec2 mousePosition = window.GetMousePosition(true);
        glm::vec2 deltaMousePosition = mousePosition - m_mousePosition;
        m_mousePosition = mousePosition;

        glm::vec3 inputRotation(-deltaMousePosition.x, deltaMousePosition.y, 0.0f);

        inputRotation *= m_cameraRotationSpeed;

        viewForward = glm::rotate(inputRotation.x, glm::vec3(0, 1, 0)) * glm::rotate(inputRotation.y, glm::vec3(viewRight)) * glm::vec4(viewForward, 0);
    }

    // Update view matrix
    m_camera.SetViewMatrix(m_cameraPos, m_cameraPos + viewForward);
}

// Helper Methods
std::shared_ptr<Texture2DObject> GrassApplication::LoadTexture(const char* path)
{
    std::shared_ptr<Texture2DObject> texture = std::make_shared<Texture2DObject>();

    int width = 0;
    int height = 0;
    int components = 0;

    // Load the texture data here
    unsigned char* data = stbi_load(path, &width, &height, &components, 4);

    texture->Bind();
    texture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA, std::span<const unsigned char>(data, width * height * 4));

    // Generate mipmaps
    texture->GenerateMipmap();

    // Release texture data
    stbi_image_free(data);

    return texture;
}

std::shared_ptr<Texture2DObject> GrassApplication::CreateNoisePattern()
{
    std::shared_ptr<Texture2DObject> noise = std::make_shared<Texture2DObject>();
    float width = 50;
    float height = 50;
    
    std::vector<float> pixels(height * width);
    for (unsigned int j = 0; j < height; ++j)
    {
        for (unsigned int i = 0; i < width; ++i)
        {
            float x = static_cast<float>(i) / (width - 1);
            float y = static_cast<float>(j) / (height - 1);
            pixels[j * width + i] = stb_perlin_fbm_noise3(x, y, 0.0f, 1.9f, 0.5f, 8) * 0.5f;
        }
    }

    noise->Bind();
    noise->SetImage<float>(0, width, height, TextureObject::FormatR, TextureObject::InternalFormatR16F, pixels);
    noise->GenerateMipmap();

    return noise;
}

void GrassApplication::DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix)
{
    material.Use();

    ShaderProgram& shaderProgram = *material.GetShaderProgram();
    ShaderProgram::Location locationWorldMatrix = shaderProgram.GetUniformLocation("WorldMatrix");
    material.GetShaderProgram()->SetUniform(locationWorldMatrix, worldMatrix);
    ShaderProgram::Location locationViewProjMatrix = shaderProgram.GetUniformLocation("ViewProjMatrix");
    material.GetShaderProgram()->SetUniform(locationViewProjMatrix, m_camera.GetViewProjectionMatrix());

    mesh.DrawSubmesh(0);
}

void GrassApplication::DrawObjectInstanced(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix, int instanceCount)
{
    material.Use();

    ShaderProgram& shaderProgram = *material.GetShaderProgram();
    ShaderProgram::Location locationWorldMatrix = shaderProgram.GetUniformLocation("WorldMatrix");
    material.GetShaderProgram()->SetUniform(locationWorldMatrix, worldMatrix);
    ShaderProgram::Location locationViewProjMatrix = shaderProgram.GetUniformLocation("ViewProjMatrix");
    material.GetShaderProgram()->SetUniform(locationViewProjMatrix, m_camera.GetViewProjectionMatrix());

    // Use draw calls for instancing.
    mesh.DrawSubmeshInstanced(0, instanceCount);
}

void GrassApplication::CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY)
{
    // Define the vertex structure
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2 texCoord)
            : position(position), normal(normal), texCoord(texCoord) {}
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(2);

    // List of vertices (VBO)
    std::vector<Vertex> vertices;

    // List of indices (EBO)
    std::vector<unsigned int> indices;

    // Grid scale to convert the entire grid to size 1x1
    glm::vec2 scale(1.0f / (gridX - 1), 1.0f / (gridY - 1));

    // Number of columns and rows
    unsigned int columnCount = gridX;
    unsigned int rowCount = gridY;

    // Iterate over each VERTEX
    for (unsigned int j = 0; j < rowCount; ++j)
    {
        for (unsigned int i = 0; i < columnCount; ++i)
        {
            // Vertex data for this vertex only
            glm::vec3 position(i * scale.x, 0.0f, j * scale.y);
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec2 texCoord(i, j);
            vertices.emplace_back(position, normal, texCoord);

            // Index data for quad formed by previous vertices and current
            if (i > 0 && j > 0)
            {
                unsigned int top_right = j * columnCount + i; // Current vertex
                unsigned int top_left = top_right - 1;
                unsigned int bottom_right = top_right - columnCount;
                unsigned int bottom_left = bottom_right - 1;

                //Triangle 1
                indices.push_back(bottom_left);
                indices.push_back(bottom_right);
                indices.push_back(top_left);

                //Triangle 2
                indices.push_back(bottom_right);
                indices.push_back(top_left);
                indices.push_back(top_right);
            }
        }
    }

    mesh.AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, vertices, indices,
        vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());
}

void GrassApplication::CreateGrassMesh(Mesh& mesh, int amount)
{
    // Define the vertex structure
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& position, const glm::vec3& normal)
            : position(position), normal(normal) {}
        glm::vec3 position;
        glm::vec3 normal;
    };

    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(3);

    // List of vertices (VBO)
    std::vector<Vertex> vertices;

    // List of indices (EBO)
    std::vector<unsigned int> indices;

    // Vertex data for this vertex only
    glm::vec3 position(0.03f, 0.0f, 0.0f);
    glm::vec3 normal(1.0f, 0.0f, 0.0f);

    glm::vec3 position1(-0.03f, 0.0f, 0.0f);
    glm::vec3 normal1(-1.0f, 0.0f, 0.0f);

    glm::vec3 position2(0.0f, 1.0f, 0.0f);
    glm::vec3 normal2(0.0f, 1.0f, 0.0f);

    vertices.emplace_back(position, normal);
    vertices.emplace_back(position1, normal1);
    vertices.emplace_back(position2, normal2);

    //Triangle
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    
    mesh.AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Triangles, vertices, indices,
        vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());

    // Creates 100000 instances of mesh at 2nd vertex attribute location
    SetupInstanceMatrix(mesh, 2, 100000);
}

void GrassApplication::SetupInstanceMatrix(Mesh& mesh, int location, int amount)
{
    // Attributes can take 4 components max in OpenGL. A 4x4 matrix will take 4 attribute locations
    VertexAttribute columnAttribute(Data::Type::Float, 4);
    
    std::vector<glm::mat4> matrices;
    // Fill out matrices

    srand(glfwGetTime());
    for(unsigned int matrix_id = 0; matrix_id < amount; matrix_id++)
    { 
        float pos_x = (float) (rand() % 200 - 100) * 0.1f ;
        float pos_z = (float) (rand() % 200 - 100) * 0.1f ;
        float scale = (float) (rand() % 10) * 0.1f + 0.5f;

        glm::mat4 mat{
            1.0f , 0.0f , 0.0f , 0.0f,
            0.0f , scale, 0.0f , 0.0f,
            0.0f , 0.0f , 1.0f , 0.0f,
            pos_x, 0.0f , pos_z, 1.0f
        };

        matrices.emplace_back(mat);
    }

    // Adding your vertex data in a new VBO
    int vboIndex = mesh.AddVertexData<glm::mat4>(matrices);
    VertexBufferObject& vbo = mesh.GetVertexBuffer(vboIndex);

    // Get the existing VAO for your submesh
    int vaoIndex = mesh.GetSubmesh(0).vaoIndex; // Assuming only one submesh
    VertexArrayObject& vao = mesh.GetVertexArray(vaoIndex);

    vao.Bind();
    vbo.Bind();
 
    int offset = 0; // Data starts at the beginning of the buffer
    int stride = 4 * columnAttribute.GetSize(); // Each vertex data is one matrix apart from each other
    for (int i = 0; i < 4; ++i)
    {
        vao.SetAttribute(location, columnAttribute, offset, stride);
        glVertexAttribDivisor(location, 1);

        location++;
        offset += columnAttribute.GetSize();
    }
}