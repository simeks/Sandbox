// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "MeshComponent.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "RenderWorld.h"
#include "Rendering/Layer.h"

#include <Engine/Rendering/Material.h>
#include <Engine/Rendering/MaterialManager.h>
#include <Engine/Rendering/ShaderParameters.h>
#include <Engine/Rendering/RenderContext.h>
#include <Engine/Rendering/RShader.h>


namespace sb
{

	MeshComponent::MeshComponent()
		: RenderComponent(MESH),
		_mesh(nullptr)
	{
	}
	MeshComponent::~MeshComponent()
	{
	}

	void MeshComponent::SetMesh(Mesh* mesh, MaterialManager* material_manager)
	{
		_mesh = mesh->GetData();

		// Initialize sub meshes
		uint32_t sub_mesh_count = (uint32_t)_mesh->submeshes.size();
		_sub_meshes.resize(sub_mesh_count);
		for (uint32_t i = 0; i < sub_mesh_count; ++i)
		{
			SubMesh& sub_mesh = _sub_meshes[i];
			const MeshData::SubMesh& sub_mesh_data = _mesh->submeshes[i];

			StringId32 material = _mesh->materials[sub_mesh_data.material_index];

			sub_mesh.material = material_manager->GetMaterial(material);
			Assert(sub_mesh.material != nullptr);

			sub_mesh.render_block.vertex_buffer = _mesh->vertex_buffer;
			sub_mesh.render_block.index_buffer = _mesh->index_buffer;
			sub_mesh.render_block.vertex_declaration = _mesh->vertex_declaration;
			sub_mesh.render_block.draw_call.index_count = sub_mesh_data.size;
			sub_mesh.render_block.draw_call.index_offset = sub_mesh_data.offset;
			sub_mesh.render_block.draw_call.vertex_count = _mesh->vertex_buffer.GetDesc().vertex_count;
			sub_mesh.render_block.draw_call.vertex_offset = 0;
			sub_mesh.render_block.draw_call.prim_type = DrawCall::PRIMITIVE_TRIANGLELIST;
			sub_mesh.render_block.draw_call.instance_count = 0;

		}

	}

	void MeshComponent::Render(RenderContext* context, const ShaderParameters* shader_parameters,
		const Camera* camera, const Layer* layer, uint64_t sort_key) const
	{
		Mat4x4f world = Mat4x4f::CreateIdentity();
		if (_game_object)
		{
			world = _transform.GetWorld();
		}

		// Calculate depth
		Vec3f pos_vs = camera->GetViewMatrix() * world.GetTranslation();
		float depth = math::Fabs(pos_vs.z / camera->GetFarRange());
		depth = Min(depth, 1.0f); // Clamp

		uint16_t sort_depth = uint16_t(depth * 0xFFFF);
		if (layer->depth_sort == render_sorting::BACK_TO_FRONT)
			sort_depth = 0xFFFF - sort_depth;

		sort_key |= uint64_t(sort_depth) << render_sorting::DEPTH_BIT;

		for (auto& submesh : _sub_meshes)
		{
			Shader* shader = submesh.material->GetShader();
			ShaderContext* shader_context = submesh.material->GetShaderContext();
			ShaderResourceBinder& resource_binder = shader->GetShaderResourceBinder();

			// Bind per object variables
			resource_binder.BindAutoVariables(shader_context->resources, world);
			// Bind external parameters
			resource_binder.Bind(shader_context->resources, *shader_parameters);
			// Bind material parameters
			resource_binder.Bind(shader_context->resources, submesh.material->GetShaderParams());

			uint64_t block_key = sort_key;
			// Sort by material to try minimize state changes
			block_key |= (uint64_t(submesh.material->GetName().GetId()) & 0xFFFFFF) << render_sorting::USER_DATA_BIT;

			const ShaderData* shader_data = shader->GetData();
			for (uint32_t p = 0; p < shader_data->passes.size(); ++p)
			{
				if (shader_data->passes[p].technique == layer->technique)
				{
					if (shader_data->passes[p].instanced)
					{
						block_key |= 1 << render_sorting::INSTANCE_BIT;
					}
					block_key |= uint64_t(p) << render_sorting::SHADER_PASS_BIT;
					context->Draw(block_key, submesh.render_block, *shader_context);
				}
			}
		}

	}

	void MeshComponent::GetBounds(AABB& aabb) const
	{
		if (_mesh)
		{
			aabb = _mesh->bounding_box;
		}
		else
		{
			aabb = AABB(Vec3f::ZERO, Vec3f::ZERO);
		}
	}
	uint8_t MeshComponent::GetVisibilityFlags() const
	{
		return 0;
	}

	MeshData* MeshComponent::GetMesh()
	{
		return _mesh;
	}

} // namespace sb

