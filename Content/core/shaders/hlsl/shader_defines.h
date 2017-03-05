#ifndef SHADER_DEFINES_H
#define SHADER_DEFINES_H

#define MAX_LIGHTS_POWER 10
#define MAX_LIGHTS (1<<MAX_LIGHTS_POWER)

// This determines the tile size for light binning and associated tradeoffs
#define COMPUTE_SHADER_TILE_GROUP_DIM 16
#define COMPUTE_SHADER_TILE_GROUP_SIZE (COMPUTE_SHADER_TILE_GROUP_DIM*COMPUTE_SHADER_TILE_GROUP_DIM)


#define SHADOW_MAPPING_SLICE_COUNT 3


#define PCF_FILTER_SIZE_3X3

#if defined(PCF_FILTER_SIZE_3X3)
	#define PCF_FILTER_SIZE 3
#elif defined(PCF_FILTER_SIZE_5X5)
	#define PCF_FILTER_SIZE 5
#elif defined(PCF_FILTER_SIZE_7X7)
	#define PCF_FILTER_SIZE 7
#else
	#define PCF_FILTER_SIZE 2
#endif


#endif
