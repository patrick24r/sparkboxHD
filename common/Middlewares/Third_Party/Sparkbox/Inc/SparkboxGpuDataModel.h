#ifndef INC_SPARKBOXGPUDATAMODEL_H_
#define INC_SPARKBOXGPUDATAMODEL_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#ifdef __cplusplus
}
#endif
/******************************************************************************/
/*                                                                            */
/*                             Sparkbox GPU Data                              */
/*                                                                            */
/******************************************************************************/
/* GPU Data Memory Map
	*  ADDR  |     SIZE (B)     |  DESCRIPTION
	* -------------------------------------------------
	* 0x0000 | 64               | Global config
	* 0x0040 | 6 * 256 = 1536   | User layers metadata
	* 0x0640 | 12 * 256 = 3072  | Fonts metadata
	* 0x1240 | 12 * 32 = 384    | Active layers config
	* 0x13C0 | 64 * 256 = 16384 | User layers palettes
	* 0x53C0 | 1 * 256 = 256    | User layers virtual data
	* 0x54C0 | 1 * 256 = 256    | Fonts virtual data
	* Total Memory Usage: 21920 Bytes
	*
	* User layers metadata
	* Contains the properties for user layers that are immutable
	* without changing the underlying data
	* ------------------------------------------------------------
	* [07:00] layerType - sprite or text | 0 or 1
	* [23:08] height (sprite only) | unsigned integer
	* [39:24] width (sprite only) | unsigned integer
	* [47:40] numberOfFrames (sprite only) | unsigned integer
	* [47:40] textLengthCharacters (textOnly)
	* Total bytes per layer = 6
	*
	* Fonts metadata
	* Contains the properties for fonts that are immutable without
	* changing the underlying data
	* ------------------------------------------------------------
	* [95:00] font name | null terminated ASCII string
	* Total bytes per font = 12
	*
	* Active layers config
	* ------------------------------------------------------------
	* [00:00] layerIsVisible | bool
	* [01:01] iterateSpriteFrames (sprite only) | bool
	* [15:08] layerIndex | unsigned integer
	* [31:16] xPosition | signed integer
	* [47:32] yposition | signed integer
	* [63:48] xVelocity | signed integer
	* [79:64] yVelocity | signed integer
	* [87:80] frame index (sprite) | unsigned integer
	* [87:80] font index (text) | unsigned integer
	* [95:88] text scale factor (text) | unsigned integer
	* Total bytes per active layer = 12
	*
	* User layers palettes
	* ------------------------------------------------------------
	* [15:00] Color 0 | 565RGB (reserved for transparent)
	* [31:16] Color 1 | 565RGB
	* ...
	* ...
	* [511:496] Color 31 | 565RGB
	* Total bytes per user layer = 64
	*
	* User layers virtual data
	* Each of the user layers has a virtual data address. To write
	* layer data, sequentially write to that layer's virtual address
	* ------------------------------------------------------------
	* [7:0] User layer 0 virtual data
	* [15:8] User layer 1 virtual data
	* ...
	* ...
	* [2047:2040] User layer 255 virtual data
	*/

#define MAX_LAYERS (256U)
#define MAX_ACTIVE_LAYERS (32U)
#define MAX_FONTS (256U)
#define MAX_PALETTE_SIZE (32U)

	// Register bit definitions: Layer Metadata Flags
#define LAYERS_META_FLAGS_TYPE_Pos (0UL)
#define LAYERS_META_FLAGS_TYPE_Msk (0x1UL << LAYERS_META_FLAGS_TYPE_Pos)
#define LAYERS_META_FLAGS_TYPE LAYERS_META_FLAGS_TYPE_Msk

// Register bit definitions: Active Layer Flags
#define ACTIVE_LAYERS_FLAGS_VISIBLITY_Pos (0U)
#define ACTIVE_LAYER_FLAGS_VISIBLITY_Msk (0x1UL << ACTIVE_LAYERS_FLAGS_VISIBLITY_Pos)
#define ACTIVE_LAYER_FLAGS_VISIBLITY ACTIVE_LAYER_FLAGS_VISIBLITY_Msk

#define ACTIVE_LAYERS_FLAGS_FRAME_ITERATE_Pos (1U)
#define ACTIVE_LAYERS_FLAGS_FRAME_ITERATE_Msk (0x1UL << ACTIVE_LAYERS_FLAGS_FRAME_ITERATE_Pos)
#define ACTIVE_LAYERS_FLAGS_FRAME_ITERATE ACTIVE_LAYERS_FLAGS_FRAME_ITERATE_Msk
typedef struct 
{
	uint8_t data[64]; // TODO: break up into what the data actually means
} GlobalFlags_TypeDef;

typedef struct
{
	uint8_t flags; /* Layer flags register */
	uint16_t width; /* Layer width register */
	uint16_t height; /* Layer height register */
	union {
		uint8_t numberOfFrames; /* Number of frames (Sprite) */
		uint8_t textLength; /* Text length (text) */
	};
} LayersMeta_TypeDef;

typedef struct {
	char* fontNames[12]; /* null terminated character array */
} FontsMeta_Typedef;

typedef struct {
	uint8_t flags; /* Active layer flags */
	uint8_t layerIndex; /* Index of layer of which this is an instance */
	int16_t xPosition;
	int16_t yPosition; 
	int16_t xVelocity; /* X velocity (pixels per frame) */
	int16_t yVelocity; /* Y velocity (pixels per frame) */
	union {
		uint8_t frameIndex; /* Index of the current frame (Sprite) */
		uint8_t fontIndex; /* Index of the current font (Text) */
	};
	uint8_t textScale;
} ActiveLayers_TypeDef;

typedef struct {
	uint16_t colors[MAX_PALETTE_SIZE];
} Palette_TypeDef;

typedef struct {
	GlobalFlags_TypeDef globalFlags;
	LayersMeta_TypeDef layersMetadata[MAX_LAYERS];
	FontsMeta_Typedef fontsMetadata[MAX_FONTS];
	ActiveLayers_TypeDef activeLayers[MAX_ACTIVE_LAYERS];
	Palette_TypeDef layersPalette[MAX_LAYERS];
	uint8_t virtualLayersData[MAX_LAYERS];
	uint8_t virtualFontsData[MAX_FONTS];
} GpuModel_TypeDef;

#endif /* INC_SPARKBOXGPUDATAMODEL_H_ */