#pragma once
#include "Tests.h"
#include "../Mesh/GeometryFactory.h"
#include "../Mesh/Mesh.h"
#include "../Shader.h"
#include "../Renderer.h"

namespace test
{
    class testGeometryFactories : public Tests
    {
    public:
        testGeometryFactories();
        ~testGeometryFactories();

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        enum GeometryType {
            TRIANGLE = 0,
            QUAD = 1,
            CUBE = 2,
            SPHERE = 3,
            FULLSCREEN_QUAD = 4,
            CYLINDER = 5,
            PLANE = 6
        };

        Mesh* m_CurrentMesh;
        Shader* m_Shader;
        Renderer* m_Renderer;

        GeometryType m_CurrentGeometry;
        bool m_ShowWireframe;
        float m_RotationSpeed;
        float m_CurrentRotation;
        float m_Scale;

        void SwitchGeometry(GeometryType type);
        void UpdateMesh();
    };
}