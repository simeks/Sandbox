// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11HARDWAREBUFFERMANAGER_H__
#define __D3D11HARDWAREBUFFERMANAGER_H__



namespace sb
{

	class D3D11RenderDevice;
	class D3D11DeviceContext;
	struct D3D11VertexBuffer;
	struct D3D11IndexBuffer;
	struct D3D11ConstantBuffer;
	struct D3D11RawBuffer;
	struct D3D11MemoryStatistics;
	struct VertexBufferDesc;
	struct IndexBufferDesc;
	struct RawBufferDesc;

	class D3D11HardwareBufferManager
	{
	public:
		D3D11HardwareBufferManager(D3D11RenderDevice* render_device, D3D11MemoryStatistics* statistics);
		~D3D11HardwareBufferManager();

		/// @brief Creates a D3D11 vertex buffer from the specified buffer description
		/// @param d3d_buffer This will hold the created buffer
		/// @param buffer This holds the description for the new buffer
		void CreateVertexBuffer(D3D11VertexBuffer* d3d_buffer, const VertexBufferDesc& desc, void* initial_data = 0);
		void UpdateVertexBuffer(D3D11DeviceContext* context, D3D11VertexBuffer* d3d_buffer, void* buffer_data);

		/// @brief Destroys the specified vertex buffer
		void DestroyVertexBuffer(D3D11VertexBuffer* d3d_buffer);


		/// @brief Creates a D3D11 index buffer from the specified buffer description
		/// @param d3d_buffer This will hold the created buffer
		/// @param buffer This holds the description for the new buffer
		void CreateIndexBuffer(D3D11IndexBuffer* d3d_buffer, const IndexBufferDesc& desc, void* initial_data = 0);
		void UpdateIndexBuffer(D3D11DeviceContext* context, D3D11IndexBuffer* d3d_buffer, void* buffer_data);

		/// @brief Destroys the specified index buffer
		void DestroyIndexBuffer(D3D11IndexBuffer* d3d_buffer);


		/// @brief Creates a D3D11 constant buffer from the specified buffer description
		/// @param d3d_buffer This will hold the created buffer
		/// @param buffer This holds the description for the new buffer
		void CreateConstantBuffer(D3D11ConstantBuffer* d3d_buffer, uint32_t size, void* initial_data = 0);
		void UpdateConstantBuffer(D3D11DeviceContext* context, D3D11ConstantBuffer* d3d_buffer, void* buffer_data);

		/// @brief Destroys the specified constant buffer
		void DestroyConstantBuffer(D3D11ConstantBuffer* d3d_buffer);

		/// @brief Creates a new buffer for raw data
		void CreateRawBuffer(D3D11RawBuffer* d3d_buffer, const RawBufferDesc& desc, void* initial_data = 0);
		void UpdateRawBuffer(D3D11DeviceContext* context, D3D11RawBuffer* d3d_buffer, void* buffer_data);
		void DestroyRawBuffer(D3D11RawBuffer* d3d_buffer);


	private:
		D3D11RenderDevice* _render_device;
		D3D11MemoryStatistics* _statistics;
	};

} // namespace sb


#endif // __D3D11HARDWAREBUFFERMANAGER_H__

