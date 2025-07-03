#pragma once
#include "Mesh.h"

class FullscreenQuad :public Mesh
{
public:
	FullscreenQuad()
	{
		m_Vertices = {
			//positions x,y,z Normals Texture Coordinates

			-1.0f,-1.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f, //bottom left 
			 1.0f,-1.0f,0.0f, 0.0f,0.0f,1.0f, 1.0f,0.0f, //bottom right
			-1.0f, 1.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,1.0f, //Top left
			 1.0f, 1.0f,0.0f, 0.0f,0.0f,1.0f, 1.0f,1.0f, //top right
		};

		m_Indices =
		{
			0,1,2, //triangle one
			1,3,2  //second triangle 
		};

		SetupMesh();
	}
};
