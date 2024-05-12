#include "GrassApplication.h"

// Mesh, Texture and terrain
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

// Lights
#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ituGL/lighting/DirectionalLight.h>
#include <ituGL/lighting/PointLight.h>

// Skybox and Scene
#include <ituGL/asset/TextureCubemapLoader.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>
#include <ituGL/scene/SceneLight.h>

#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/scene/SceneModel.h>

#include <ituGL/renderer/SkyboxRenderPass.h>
#include <ituGL/renderer/ForwardRenderPass.h>
#include <ituGL/scene/RendererSceneVisitor.h>


GrassApplication::GrassApplication()
    : Application(1024, 1024, "Stylized Grass")
    , m_gridX(128), m_gridY(128)
    , m_vertexShaderLoader(Shader::Type::VertexShader)
    , m_fragmentShaderLoader(Shader::Type::FragmentShader)
    , m_baseGrassColor(glm::vec4(0.0f, 0.4f, 0.0f, 1.0f))
    , m_tipGrassColor(glm::vec4(0.5f, 1.0f, 0.0f, 1.0f))
    , m_bladeHeight(5.0f)
    , m_windDirection(glm::vec2(0.5f, 0.5f))
    , m_windStrength(1.0f)
    , time(0)
{
}

void GrassApplication::Update()
{
    Application::Update();

    const Window& window = GetMainWindow();

    glm::vec2 mousePosition = window.GetMousePosition(true);
    m_camera.SetViewMatrix(glm::vec3(0.0f, 15.0f, 15.0f), glm::vec3(mousePosition, 0.0f));

    int width, height;
    window.GetDimensions(width, height);
    float aspectRatio = static_cast<float>(width) / height;
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 100.0f);

    // Update Grass based on time elapsed
    time += Application::GetDeltaTime();
    UpdateGrass();
}

void GrassApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Terrain patches
    DrawObject(m_terrainPatch, *m_terrainMaterial00, glm::translate(glm::vec3(0.0f,   -5.0f, 0.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_terrainMaterial10, glm::translate(glm::vec3(-10.0f, -5.0f, 0.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_terrainMaterial01, glm::translate(glm::vec3(0.0f,   -5.0f, -10.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_terrainMaterial11, glm::translate(glm::vec3(-10.0f, -5.0f, -10.0f)) * glm::scale(glm::vec3(10.0f)));

    // Water patches
    DrawObject(m_terrainPatch, *m_waterMaterial, glm::translate(glm::vec3(0.0f, -1.5f, 0.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_waterMaterial, glm::translate(glm::vec3(-10.0f, -1.5f, 0.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_waterMaterial, glm::translate(glm::vec3(0.0f, -1.5f, -10.0f)) * glm::scale(glm::vec3(10.0f)));
    DrawObject(m_terrainPatch, *m_waterMaterial, glm::translate(glm::vec3(-10.0f, -1.5f, -10.0f)) * glm::scale(glm::vec3(10.0f)));

    // Add Grass Field
    DrawObject(m_grassBlade, *m_grassMaterial, glm::scale(glm::vec3(5.0f)));
}


void GrassApplication::Initialize(){

    m_baseGrassColor = glm::vec4(0.0f, 0.4f, 0.0f, 1.0f);
    m_tipGrassColor  = glm::vec4(0.5f, 1.0f, 0.0f, 1.0f);
    m_bladeHeight = 1.0f;
    m_windDirection = glm::vec2(0.0f, 1.0f);

    // Initialize the application window
    Application::Initialize();

    // Build textures and keep them in a list
    InitializeTextures();

    // Build materials and keep them in a list
    InitializeMaterials();

    InitializeGrass();

    // Build meshes and keep them in a list
    InitializeMeshes();

    //Enable depth test
    GetDevice().EnableFeature(GL_DEPTH_TEST);

    //Enable wireframe
    //GetDevice().SetWireframeEnabled(true);
}

void GrassApplication::InitializeTextures()
{
    m_defaultTexture = CreateDefaultTexture();

    // Load terrain textures
    m_dirtTexture = LoadTexture("textures/dirt.png");
    m_grassTexture = LoadTexture("textures/grass.jpg");
    m_rockTexture = LoadTexture("textures/rock.jpg");
    m_snowTexture = LoadTexture("textures/snow.jpg");

    m_heightmapTexture00 = CreateHeightMap(m_gridX, m_gridY, glm::ivec2(0, 0));
    m_heightmapTexture10 = CreateHeightMap(m_gridX, m_gridY, glm::ivec2(-1, 0));
    m_heightmapTexture01 = CreateHeightMap(m_gridX, m_gridY, glm::ivec2(0, -1));
    m_heightmapTexture11 = CreateHeightMap(m_gridX, m_gridY, glm::ivec2(-1, -1));

    // Load water texture here
    m_waterTexture = LoadTexture("textures/water.png");
}

void GrassApplication::InitializeGrass()
{
    // Setup offset for each instance of blade
    std::vector<glm::vec3> translations;
    int index = 0;
    float offset = 0.1f;
    for (int z = -10; z < 10; z += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            glm::vec3 translation;
            translation.x = (float) x / 10.0f + offset;
            translation.y = 0;
            translation.z = (float) z / 10.0f + offset;
            translations.emplace_back(translation);
        }
    }

    // Setup perlin noise pattern for moving the grass
    m_noisePattern = CreateNoisePattern();

    // Setup shader program for grass
    Shader grassVS = m_vertexShaderLoader.Load("shaders/grass.vert");
    Shader grassFS = m_fragmentShaderLoader.Load("shaders/grass.frag");
    std::shared_ptr<ShaderProgram> grassShaderProgram = std::make_shared<ShaderProgram>();
    grassShaderProgram->Build(grassVS, grassFS);

    // Setup Grass Material
    m_grassMaterial = std::make_shared<Material>(grassShaderProgram);

    // Setting Uniform properties of Grass Material
    m_grassMaterial->SetUniformValue("baseColor", m_baseGrassColor);
    m_grassMaterial->SetUniformValue("tipColor", m_tipGrassColor);
    m_grassMaterial->SetUniformValue("bladeHeight", m_bladeHeight);
    m_grassMaterial->SetUniformValue("noise", m_noisePattern);
    m_grassMaterial->SetUniformValue("time", time);
    m_grassMaterial->SetUniformValue("direction", glm::vec2(0.5f, 0.5f));
    m_grassMaterial->SetUniformValue("strength", 1.0f);

    m_grassMaterial->SetUniformValue("WindFrequency", glm::vec4(0.025, 0.025, 0, 0));
    m_grassMaterial->SetUniformValue("WindDistortionMapScaleOffset", glm::vec4(0.01f, 0.01f, 0.0f, 0.0f));
    //m_grassMaterial->SetUniformValue("offset", translations);
}

void GrassApplication::InitializeMaterials()
{
    // Default shader program
    Shader defaultVS = m_vertexShaderLoader.Load("shaders/default.vert");
    Shader defaultFS = m_fragmentShaderLoader.Load("shaders/default.frag");
    std::shared_ptr<ShaderProgram> defaultShaderProgram = std::make_shared<ShaderProgram>();
    defaultShaderProgram->Build(defaultVS, defaultFS);

    // Default material
    m_defaultMaterial = std::make_shared<Material>(defaultShaderProgram);
    m_defaultMaterial->SetUniformValue("Color", glm::vec4(1.0f));

    // Terrain shader program
    Shader terrainVS = m_vertexShaderLoader.Load("shaders/terrain.vert");
    Shader terrainFS = m_fragmentShaderLoader.Load("shaders/terrain.frag");
    std::shared_ptr<ShaderProgram> terrainShaderProgram = std::make_shared<ShaderProgram>();
    terrainShaderProgram->Build(terrainVS, terrainFS);

    // Terrain materials
    m_terrainMaterial00 = std::make_shared<Material>(terrainShaderProgram);
    m_terrainMaterial00->SetUniformValue("Color", glm::vec4(1.0f));
    m_terrainMaterial00->SetUniformValue("Heightmap", m_heightmapTexture00);
    m_terrainMaterial00->SetUniformValue("ColorTexture0", m_dirtTexture);
    m_terrainMaterial00->SetUniformValue("ColorTexture1", m_grassTexture);
    m_terrainMaterial00->SetUniformValue("ColorTexture2", m_rockTexture);
    m_terrainMaterial00->SetUniformValue("ColorTexture3", m_snowTexture);
    m_terrainMaterial00->SetUniformValue("ColorTextureRange01", glm::vec2(-0.2f, 0.0f));
    m_terrainMaterial00->SetUniformValue("ColorTextureRange12", glm::vec2(0.1f, 0.2f));
    m_terrainMaterial00->SetUniformValue("ColorTextureRange23", glm::vec2(0.25f, 0.3f));
    m_terrainMaterial00->SetUniformValue("ColorTextureScale", glm::vec2(0.125f));

    m_terrainMaterial10 = std::make_shared<Material>(*m_terrainMaterial00);
    m_terrainMaterial10->SetUniformValue("Heightmap", m_heightmapTexture10);

    m_terrainMaterial01 = std::make_shared<Material>(*m_terrainMaterial00);
    m_terrainMaterial01->SetUniformValue("Heightmap", m_heightmapTexture01);

    m_terrainMaterial11 = std::make_shared<Material>(*m_terrainMaterial00);
    m_terrainMaterial11->SetUniformValue("Heightmap", m_heightmapTexture11);

    // Water shader
    Shader waterVS = m_vertexShaderLoader.Load("shaders/water.vert");
    Shader waterFS = m_fragmentShaderLoader.Load("shaders/water.frag");
    std::shared_ptr<ShaderProgram> waterShaderProgram = std::make_shared<ShaderProgram>();
    waterShaderProgram->Build(waterVS, waterFS);

    // Water material
    m_waterMaterial = std::make_shared<Material>(waterShaderProgram);
    m_waterMaterial->SetUniformValue("Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_waterMaterial->SetUniformValue("ColorTexture", m_grassTexture);
    m_waterMaterial->SetUniformValue("ColorTextureScale", glm::vec2(0.0625f));
    m_waterMaterial->SetBlendEquation(Material::BlendEquation::Add);
    m_waterMaterial->SetBlendParams(Material::BlendParam::SourceAlpha, Material::BlendParam::OneMinusSourceAlpha);
}

void GrassApplication::UpdateGrass()
{
    m_grassMaterial->SetUniformValue("time", time);
}


void GrassApplication::InitializeMeshes()
{
    CreateTerrainMesh(m_terrainPatch, m_gridX, m_gridY);
    CreateGrassMesh(m_grassBlade, m_bladeHeight);
}

std::shared_ptr<Texture2DObject> GrassApplication::CreateDefaultTexture()
{
    std::shared_ptr<Texture2DObject> texture = std::make_shared<Texture2DObject>();

    int width = 4;
    int height = 4;
    std::vector<float> pixels;
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            pixels.push_back(1.0f);
            pixels.push_back(0.0f);
            pixels.push_back(1.0f);
            pixels.push_back(1.0f);
        }
    }

    texture->Bind();
    texture->SetImage<float>(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA, pixels);
    texture->GenerateMipmap();

    return texture;
}

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

std::shared_ptr<Texture2DObject> GrassApplication::CreateHeightMap(unsigned int width, unsigned int height, glm::ivec2 coords)
{
    std::shared_ptr<Texture2DObject> heightmap = std::make_shared<Texture2DObject>();

    std::vector<float> pixels(height * width);
    for (unsigned int j = 0; j < height; ++j)
    {
        for (unsigned int i = 0; i < width; ++i)
        {
            float x = static_cast<float>(i) / (width - 1) + coords.x;
            float y = static_cast<float>(j) / (height - 1) + coords.y;
            pixels[j * width + i] = stb_perlin_fbm_noise3(x, y, 0.0f, 1.9f, 0.5f, 8) * 0.5f;
        }
    }

    heightmap->Bind();
    heightmap->SetImage<float>(0, width, height, TextureObject::FormatR, TextureObject::InternalFormatR16F, pixels);
    heightmap->GenerateMipmap();

    return heightmap;
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

void GrassApplication::CreateGrassMesh(Mesh& mesh, float height)
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
}