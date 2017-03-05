// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11INSTANCEMERGER_H__
#define __D3D11INSTANCEMERGER_H__

#include <Engine/Rendering/RenderContext.h>
#include <Engine/Rendering/RRawBuffer.h>



namespace sb
{

	class D3D11RenderDevice;
	class D3D11DeviceContext;

	/// Helper class for batching object instances
	class D3D11InstanceMerger
	{
	public:
		D3D11InstanceMerger(D3D11RenderDevice* device);
		~D3D11InstanceMerger();

		/// Adds an instance to the merger
		///		Assumes all commands to be added in order, meaning all instances of the same type should follow each other.
		void Add(RenderContext::DrawCmd* cmd, const uint64_t& sort_key);

		void Dispatch(D3D11DeviceContext* context);

		/// Returns the number of batches currently in the merger
		uint32_t Size() const;

	private:
		enum { INITIAL_INSTANCE_BUFFER_SIZE = 128 };

		struct Batch
		{
			uint64_t sort_key;
			vector<RenderContext::DrawCmd*> cmds;

			Batch() {}
		};

		/// Tries to find a batch with the specified key and uid, creates a new one if none is found.
		Batch& GetBatch(const uint64_t& sort_key);

		D3D11RenderDevice* _device;

		vector<Batch> _batches;

		vector<uint8_t> _instance_data;
		RRawBuffer _instance_data_buffer;
		RawBufferDesc _instance_data_buffer_desc;
	};

} // namespace sb

#endif // __D3D11INSTANCEMERGER_H__
