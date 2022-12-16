#pragma once
#include <string>
#include <vector>



namespace Tmpl8
{
	class Texture;
	class Renderer;
	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles) :m_Vertices(vertices), m_Triangles(triangles) {}
		void Draw(MathUtil::mat4& transform, Renderer* renderer);
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<Triangle> m_Triangles;
	};

	struct Node
	{
		Node() = default;
		Node(MathUtil::mat4& aTransform) : transform(aTransform){}
		MathUtil::mat4 transform;
		std::vector<Node> nodes;
		int textureIndex = -1;
		std::vector<int> meshIndexes;
	};

	class Model
	{
	public:
		Model() = default;
		~Model()
		{
			
			for (auto& texture : m_Textures)
			{
				delete texture;
			}
		}
		Model(const std::string&);
		void Draw(MathUtil::mat4& transform, Renderer* renderer);
	private:
		std::vector<Mesh> m_Meshes;
		std::vector<Texture*> m_Textures;
		std::vector<Node> m_Nodes;
	};


}
