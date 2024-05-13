#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/scene/Scene.h>
#include <ituGL/texture/FramebufferObject.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/camera/CameraController.h>
#include <array>
#include <ituGL/asset/ShaderLoader.h>

#include <ituGL/utils/DearImGui.h>

class Texture2DObject;

class GrassApplication : public Application
{
public:
    GrassApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

    // Grass Implementation
    void InitializeGrass();

private:
    void InitializeTextures();
    void InitializeMaterials();
    void UpdateGrass();
    void InitializeMeshes();

    void DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix);
    void DrawObjectInstanced(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix, int instanceCount);

    std::shared_ptr<Texture2DObject> LoadTexture(const char* path);

    void CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY);

    // Method for making the grass blade
    void CreateGrassMesh(Mesh& mesh, int amount);
    void SetupInstanceMatrix(Mesh& mesh, int amount);
    std::shared_ptr<Texture2DObject> CreateNoisePattern();

    void RenderGUI();

    float time;

private:
    unsigned int m_gridX, m_gridY;

    ShaderLoader m_vertexShaderLoader;
    ShaderLoader m_fragmentShaderLoader;

    // Helper object for debug GUI
    DearImGui m_imGui;

    // Mouse position for camera controller
    glm::vec2 m_mousePosition;

    // Camera controller parameters
    Camera m_camera;
    glm::vec3 m_cameraPos;

    // Terrain
    Mesh m_terrainPatch;

    std::shared_ptr<Material> m_terrainMaterial;
    std::shared_ptr<Texture2DObject> m_dirtTexture;
    std::shared_ptr<Texture2DObject> m_grassTexture;

    // Grass parameters
    Mesh m_grassBlade;

    std::shared_ptr<Material> m_grassMaterial;

    glm::vec4 m_baseGrassColor;
    glm::vec4 m_tipGrassColor;
    glm::vec2 m_windDirection;
    float m_windStrength;
    float m_bladeHeight;
    int m_amount;

    std::shared_ptr<Texture2DObject> m_noisePattern;
};
