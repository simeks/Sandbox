// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERCONTEXT_H__
#define __RENDERCONTEXT_H__

#include <Foundation/IO/MemoryStream.h>

#include "Rendering.h"
#include "RenderBlock.h"

namespace sb
{

	class RenderResource;

	struct Viewport;
	struct ScissorRect;
	struct ShaderContext;

	/// @brief Context for queueing upp render commands that later can be dispatched through the render device.
	/// @remark This class in itself is not thread-safe so consider making one context per-thread rather than sharing one.
	class RenderContext
	{
	public:
		enum
		{
			RC_SET_TARGETS = 0x00, ///< See SetTargetsMsg
			RC_CLEAR_STATE = 0x01, ///< Clears device state
			RC_CLEAR_TARGETS = 0x02, ///< See ClearTargetsMsg

			RC_UPDATE_BUFFER = 0x03, ///< See UpdateBufferMsg

			RC_DRAW = 0x04, ///< See DrawMsg
			RC_DISPATCH = 0x05	///< See DispatchMsg

		};



		struct SetTargetsCmd
		{
			uint32_t render_target[MAX_MULTIPLE_RENDER_TARGETS];
			uint32_t depth_target;

			uint32_t num_viewports; ///! Number of viewports
			uint32_t viewport_offset; ///! Offset to the viewports in the buffer, from the start of this structure.

			uint32_t num_scissor_rects; //! Number of rects
			uint32_t scissor_rect_offset; ///! Offset to the rectangles in the buffer, from the start of this structure.
		};


		struct ClearTargetsCmd
		{
			uint8_t	flags;	//<! Which targets to be cleared, see ClearFlag
			float	color[4];
			float	depth;
			uint8_t	stencil;
		};

		struct UpdateBufferCmd
		{
			RenderResource resource;
			uint32_t data_offset; // Offset from the start of this structure
		};


		// DrawMsg, contains all info and resources needed to execute a draw call.
		// Buffer layout for RenderCmd:
		// DrawMsg				: sizeof(DrawMsg)
		//	IA Resources 
		//		Resources		: sizeof(RenderResource) * Number of IA resources
		//	Shader resources
		//		Resources		: sizeof(RenderResource) * number of resources
		//	Constant buffers
		//		Buffer info		: sizeof(RConstantBuffer) * number of buffers
		//	Instance data
		//		Data			: instance_data_size
		//	Constant buffer data
		//		Data			: Sum of buffer sizes
		struct DrawCmd
		{
			DrawCall draw_call;
			uint32_t ia_resource_count; ///< Number of resources for the input assembler (Vertex buffers, index buffers, etc)
			uint32_t ia_resource_offset; ///< Offset to the resources in the buffer

			uint32_t shader;
			uint32_t shader_resources_count;
			uint32_t shader_resources_offset; ///< Offset in the stream from the top of this struct

			uint32_t constant_buffer_count;
			uint32_t constant_buffer_offset; ///< Constant buffers are right after the shader resources in the buffer

			uint32_t constant_data_offset;

			uint32_t instance_data_size;
			uint32_t instance_data_offset;
		};

		/// DispatchMsg, contains all info needed for dispatching a compute shader.
		// Buffer layout for ComputeCmd:
		// DispatchMsg			: sizeof(DispatchMsg)
		//	Shader resources
		//		Resources		: sizeof(RenderResource) * number of resources
		//	Constant buffers
		//		Buffer info		: sizeof(RConstantBuffer) * number of buffers
		//	Constant buffer data
		//		Data			: Sum of buffer sizes
		struct DispatchCmd
		{
			uint32_t thread_group_count_x;
			uint32_t thread_group_count_y;
			uint32_t thread_group_count_z;

			uint32_t shader;
			uint32_t shader_resources_offset;
			uint32_t shader_resources_count;

			uint32_t constant_buffer_count; ///< Constant buffers are right after the shader resources in the buffer
			uint32_t constant_buffer_offset;

			uint32_t constant_data_offset;
		};


		/// @brief Struct for a sort key
		///
		///	Sort key is a uint64_t used for sorting rendering commands
		struct SortKey
		{
			uint8_t		layer;	// Layer id, 8 bits
			uint16_t	depth;	// Depth, 16 bits
		};

		struct SortCmd
		{
			uint64_t sort_key;
			uint32_t offset;		// Offset in the command buffer
			uint32_t length;		// Length in the command buffer

			vector<uint8_t>* buffer;		// Pointer to the command buffer
		};

		typedef vector<SortCmd> SortCmdList;


	public:
		struct Targets
		{
			RenderResource render_targets[MAX_MULTIPLE_RENDER_TARGETS];
			RenderResource depth_stencil_target;

			uint32_t num_viewports;
			const Viewport* viewports;

			uint32_t num_scissor_rects;
			const ScissorRect* rects;

			Targets();
		};

		RenderContext();
		~RenderContext();

		/// Clears the current device states.
		void ClearState(uint64_t sort_key);

		/// Setups render targets and viewports.
		void SetTargets(uint64_t sort_key, const Targets& targets);

		/// @brief Clear currently bound targets
		/// @param flags See rendering::ClearFlag.
		void ClearTargets(uint64_t sort_key, uint8_t flags, float color[4], float depth = 1.0f, uint8_t stencil = 0);


		/// @brief Queues the given render block
		void Draw(uint64_t sort_key, const RenderBlock& render_block, const ShaderContext& shader_context);

		/// @brief Queues the giveh compute block
		void Dispatch(uint64_t sort_key, const ComputeCall& compute_call, const ShaderContext& shader_context);

		/// @brief Updates the content of the specified hardware buffer.
		void UpdateBuffer(uint64_t sort_key, const RHardwareBuffer& buffer, void* data);

		/// @brief Clears all commands in this context
		void ClearContext();

		const SortCmdList& GetSortCmds() const;
		vector<uint8_t>* GetCmdBuffer();

	private:
		void WriteCommand(uint8_t command, void* data, uint32_t size, uint64_t sort_key);


	private:
		SortCmdList	_sort_cmds;

		vector<uint8_t> _cmd_buffer;
		DynamicMemoryStream	_data_stream; ///< Helper class just to make writing a bit easier

	};

} // namespace sb


#endif // __RENDERCONTEXT_H__