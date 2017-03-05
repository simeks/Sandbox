// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_MESHCOMPONENT_H__
#define __FRAMEWORK_MESHCOMPONENT_H__

#include "RenderComponent.h"

#include <Engine/Rendering/Mesh.h>
#include <Engine/Rendering/RenderBlock.h>


namespace sb
{

	class Material;
	class MaterialManager;
	class RenderContext;
	class Camera;
	class ShaderParameters;
	struct Layer;

	class MeshComponent : public RenderComponent
	{
	public:
		struct SubMesh
		{
			RenderBlock render_block;
			Material* material;
		};


	public:
		MeshComponent();
		~MeshComponent();

		void SetMesh(Mesh* mesh, MaterialManager* material_manager);

		void Render(RenderContext* context, const ShaderParameters* shader_parameters,
			const Camera* camera, const Layer* layer, uint64_t sort_key) const;

		virtual void GetBounds(AABB& aabb) const OVERRIDE;
		virtual uint8_t GetVisibilityFlags() const OVERRIDE;

		MeshData* GetMesh();

	private:
		vector<SubMesh> _sub_meshes;

		MeshData* _mesh;
	};

} // namespace sb

#endif // __FRAMEWORK_MESHCOMPONENT_H__
