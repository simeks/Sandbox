// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11STATISTICS_H__
#define __D3D11STATISTICS_H__



namespace sb
{

	/// Structure for keeping statistics about gpu memory usage
	struct D3D11MemoryStatistics
	{
		uint32_t texture_memory; // Number of bytes allocated
		uint32_t render_target_memory; // Number of bytes allocated
		uint32_t vertex_buffer_memory; // Number of bytes allocated
		uint32_t index_buffer_memory; // Number of bytes allocated
		uint32_t constant_buffer_memory; // Number of bytes allocated
		uint32_t raw_buffer_memory; // Number of bytes allocated
	};


} // namespace sb


#endif // __D3D11STATISTICS_H__
