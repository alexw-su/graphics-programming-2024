#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/scene/Scene.h>
#include <ituGL/texture/FramebufferObject.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/camera/CameraController.h>
#include <array>
#include <ituGL/asset/ShaderLoader.h>

class Texture2DObject;

class GrassApplication : public Application
{
public:
    GrassApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;

    // Grass Implementation
    void InitializeGrass();

private:
    void InitializeTextures();
    void InitializeMaterials();
    void UpdateGrass();
    void InitializeMeshes();

    void DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix);

    std::shared_ptr<Texture2DObject> CreateDefaultTexture();
    std::shared_ptr<Texture2DObject> CreateHeightMap(unsigned int width, unsigned int height, glm::ivec2 coords);
    std::shared_ptr<Texture2DObject> LoadTexture(const char* path);

    void CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY);

    // Method for making the grass blade
    void CreateGrassMesh(Mesh& mesh, float height);
    std::shared_ptr<Texture2DObject> CreateNoisePattern();

    float deltaTime;
    float lastFrame;
    float time;

private:
    unsigned int m_gridX, m_gridY;

    Camera m_camera;

    ShaderLoader m_vertexShaderLoader;
    ShaderLoader m_fragmentShaderLoader;

    Mesh m_terrainPatch;

    std::shared_ptr<Material> m_defaultMaterial;
    std::shared_ptr<Material> m_terrainMaterial00;
    std::shared_ptr<Material> m_terrainMaterial10;
    std::shared_ptr<Material> m_terrainMaterial01;
    std::shared_ptr<Material> m_terrainMaterial11;
    std::shared_ptr<Material> m_waterMaterial;

    std::shared_ptr<Texture2DObject> m_defaultTexture;
    std::shared_ptr<Texture2DObject> m_heightmapTexture00;
    std::shared_ptr<Texture2DObject> m_heightmapTexture10;
    std::shared_ptr<Texture2DObject> m_heightmapTexture01;
    std::shared_ptr<Texture2DObject> m_heightmapTexture11;
    std::shared_ptr<Texture2DObject> m_dirtTexture;
    std::shared_ptr<Texture2DObject> m_grassTexture;
    std::shared_ptr<Texture2DObject> m_rockTexture;
    std::shared_ptr<Texture2DObject> m_snowTexture;
    std::shared_ptr<Texture2DObject> m_waterTexture;

    // Grass parameters
    Mesh m_grassBlade;
    std::shared_ptr<Material> m_grassMaterial;
    glm::vec4 m_baseGrassColor;
    glm::vec4 m_tipGrassColor;
    glm::vec2 m_windDirection;
    float m_windStrength;
    float m_bladeHeight;
    std::shared_ptr<Texture2DObject> m_noisePattern;
};
