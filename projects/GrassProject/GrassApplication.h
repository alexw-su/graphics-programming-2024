#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/geometry/Mesh.h>
#include <ituGL/shader/Material.h>
#include <glm/mat4x4.hpp>
#include <vector>

#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/Model.h>
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

private:
    void InitializeCamera();
    void InitializeMaterials();
    void InitializeTextures();
    void InitializeMeshes();

    void UpdateUniforms();
    void UpdateCamera();

    void DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix);
    void DrawObjectInstanced(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix, int instanceCount);

    std::shared_ptr<Texture2DObject> LoadTexture(const char* path);

    void CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY);
    void CreateGrassMesh(Mesh& mesh, int amount);

    void SetupInstanceMatrix(Mesh& mesh, int location, int amount);
    std::shared_ptr<Texture2DObject> CreateNoisePattern();

    void RenderGUI();

private:
    unsigned int m_gridX, m_gridY;

    ShaderLoader m_vertexShaderLoader;
    ShaderLoader m_fragmentShaderLoader;

    float time;

    // Helper object for debug GUI
    DearImGui m_imGui;

    // Mouse position for camera controller
    glm::vec2 m_mousePosition;

    // Camera
    Camera m_camera;
    glm::vec3 m_cameraPos;
    float m_cameraTranslationSpeed;
    float m_cameraRotationSpeed;
    bool m_cameraEnabled;
    bool m_cameraEnablePressed;

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

    // Add light variables
    glm::vec3 m_ambientColor;
    glm::vec3 m_lightColor;
    float m_lightIntensity;
    glm::vec3 m_lightPosition;
};
