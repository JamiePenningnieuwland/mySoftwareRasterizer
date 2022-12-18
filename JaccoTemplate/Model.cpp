#include "precomp.h"
#include "Model.h"
#include "Texture.h"
#include "Renderer.h"
#include <tiny_gltf.h>
#include <filesystem>

Tmpl8::Model::Model(const std::string& filePath)
{
	tinygltf::Model tinyModel;
	tinygltf::TinyGLTF tinyLoader;
	std::string err;
	std::string warn;

	bool ret = tinyLoader.LoadASCIIFromFile(&tinyModel, &err, &warn, filePath);

	if (!warn.empty())
	{
		printf("Warn: %s\n", warn.c_str());
		
	}

	if (!err.empty())
	{
		printf("Err: %s\n", err.c_str());
	
	}

	if (!ret)
	{
		printf("Failed to parse glTF");
		
	}
	//loading textures
	for(auto& texture : tinyModel.textures)
	{
		std::filesystem::path fullPath = filePath;
		std::string fileName = tinyModel.images[texture.source].uri;
		Texture* t = new Texture(fullPath.parent_path().string() + "/" + fileName);
		m_Textures.push_back(t);
	}
	//meshes
	for(auto& mesh : tinyModel.meshes)
	{
		for(auto& primitive : mesh.primitives)
		{
			std::vector<Triangle> triangles;
			std::vector<Vertex> vertices;

			const auto& positionAccessor = tinyModel.accessors[primitive.attributes.at("POSITION")];
			size_t vertexCount = positionAccessor.count;
			vertices.resize(vertexCount);
			//POSITIONS
			{
				int accessorIndex = primitive.attributes.at("POSITION");
				const auto& accessor = tinyModel.accessors[accessorIndex];
				const auto& bufferView = tinyModel.bufferViews[accessor.bufferView];
				const auto& buffer = tinyModel.buffers[bufferView.buffer];
				const int stride = accessor.ByteStride(bufferView);

				for(int i = 0; i <vertexCount; i++)
				{
					size_t index = bufferView.byteOffset + accessor.byteOffset + i * stride;
					memcpy(&vertices[i].position, &buffer.data[index], sizeof(float) * 3);
				}
			}
			//TEXCOORDS
			{
				int accessorIndex = primitive.attributes.at("TEXCOORD_0");
				const auto& accessor = tinyModel.accessors[accessorIndex];
				const auto& bufferView = tinyModel.bufferViews[accessor.bufferView];
				const auto& buffer = tinyModel.buffers[bufferView.buffer];
				const int stride = accessor.ByteStride(bufferView);

				for (int i = 0; i < vertexCount; i++)
				{
					size_t index = bufferView.byteOffset + accessor.byteOffset + i * stride;
					memcpy(&vertices[i].texCoords, &buffer.data[index], sizeof(float) * 2);
				}
			}

			//INDICES
			const auto& accessor = tinyModel.accessors[primitive.indices];
			const auto& bufferView = tinyModel.bufferViews[accessor.bufferView];
			const auto& buffer = tinyModel.buffers[bufferView.buffer];
			const size_t stride = accessor.ByteStride(bufferView);

			triangles.resize(accessor.count / 3);

			for (int i = 0; i < accessor.count; i += 3)
			{
				size_t index = bufferView.byteOffset + accessor.byteOffset + i * stride;
				for(int j = 0; j < 3; j++)
				{
					switch (accessor.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_SHORT:
					{
						short idx = 0;
						memcpy(&idx, &buffer.data[index], sizeof(short));
						triangles[i / 3].indices[j] = static_cast<unsigned int>(idx);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					{
						unsigned short idx = 0;
						memcpy(&idx, &buffer.data[index], sizeof(unsigned short));
						triangles[i / 3].indices[j] = static_cast<unsigned int>(idx);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_INT:
					{
						int idx = 0;
						memcpy(&idx, &buffer.data[index], sizeof(int));
						triangles[i / 3].indices[j] = static_cast<unsigned int>(idx);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
					{
						memcpy(&triangles[i / 3].indices[j], &buffer.data[index], sizeof(unsigned));
						break;
					}
					}
					index += stride;
				}
			}


			m_Meshes.push_back(Mesh(vertices, triangles));
		}
	}
	printf("Loaded model");
}

Tmpl8::Model::~Model()
{
	for (auto& texture : m_Textures)
	{
		delete texture;
	}
}

void Tmpl8::Model::Draw(MathUtil::mat4& transform, Renderer* renderer)
{
	for(auto& mesh: m_Meshes)
	{
		renderer->BindTexture(m_Textures[0]);
		mesh.Draw(transform, renderer);
	}
}

void Tmpl8::Mesh::Draw(MathUtil::mat4& transform, Renderer* renderer)
{
	renderer->Draw(transform, m_Vertices.data(), m_Triangles.data(), m_Vertices.size(), m_Triangles.size());
}
