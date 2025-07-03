#include "Sphere.h"

#include <cmath>

#define M_PI 3.1415926

Sphere::Sphere(int sectors, int stacks)
{
	float radius = 0.5f;

	for (int stack = 0; stack <= stacks; ++stack)
	{
		float phi = M_PI * (float(stack) / stacks); // from 0 to π
		float y = cos(phi);
		float sinPhi = sin(phi);

		for (int sector = 0; sector <= sectors; ++sector)
		{
			float theta = 2.0f * M_PI * (float(sector) / sectors);
			float x = sinPhi * cos(theta);
			float z = sinPhi * sin(theta);
			float u = (float(sector) / sectors);
			float v = (float(stack) / stacks);


			m_Vertices.push_back(radius * x); //x position
			m_Vertices.push_back(radius * y); //y position
			m_Vertices.push_back(radius * z);// z position
			m_Vertices.push_back(x); // Normal  X
			m_Vertices.push_back(y); // Normal  Y
			m_Vertices.push_back(z); // Normal  Z
			m_Vertices.push_back(u); // Texture U
			m_Vertices.push_back(v); // Texture V
		}
	}


	for (int stack = 0; stack < stacks; ++stack)
	{
		int k1 = stack * (sectors + 1); //
		int k2 = k1 + sectors + 1;

		for (int sector = 0; sector < sectors; ++sector, ++k1, ++k2)
		{
			if (stack != 0)
			{
				m_Indices.push_back(k1);
				m_Indices.push_back(k2);
				m_Indices.push_back(k1 + 1);
			}
			if (stack != (stacks - 1))
			{
				m_Indices.push_back(k1 +1);
				m_Indices.push_back(k2);
				m_Indices.push_back(k2 + 1);
			}

		}
	}

	SetupMesh();
}
