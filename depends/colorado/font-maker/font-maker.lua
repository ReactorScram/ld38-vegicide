local ffi = require "ffi"

local stb = ffi.load "./font-maker.so"

ffi.cdef [[
typedef struct
{
   unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
   float xoff,yoff,xadvance;
} stbtt_bakedchar;

typedef struct
{
   unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
   float xoff,yoff,xadvance;
   float xoff2,yoff2;
} stbtt_packedchar;

extern int stbtt_BakeFontBitmap(const unsigned char *data, int offset,  // font location (use offset=0 for plain .ttf)
                                float pixel_height,                     // height of font in pixels
                                unsigned char *pixels, int pw, int ph,  // bitmap to be filled in
                                int first_char, int num_chars,          // characters to bake
                                stbtt_bakedchar *chardata);             // you allocate this, it's num_chars long
// if return is positive, the first unused row of the bitmap
// if return is negative, returns the negative of the number of characters that fit
// if return is 0, no characters fit and no rows were used
// This uses a very crappy packing.

typedef struct
{
   float x0,y0,s0,t0; // top-left
   float x1,y1,s1,t1; // bottom-right
} stbtt_aligned_quad;

extern void stbtt_GetBakedQuad(stbtt_bakedchar *chardata, int pw, int ph,  // same data as above
                               int char_index,             // character to display
                               float *xpos, float *ypos,   // pointers to current position in screen pixel space
                               stbtt_aligned_quad *q,      // output: quad to draw
                               int opengl_fillrule);       // true if opengl fill rule; false if DX9 or earlier

typedef struct {
   void *user_allocator_context;
   void *pack_info;
   int   width;
   int   height;
   int   stride_in_bytes;
   int   padding;
   unsigned int   h_oversample, v_oversample;
   unsigned char *pixels;
   void  *nodes;
} stbtt_pack_context;

extern int stbtt_PackBegin(stbtt_pack_context *spc, unsigned char *pixels, int width, int height, int stride_in_bytes, int padding, void *alloc_context);

extern void stbtt_PackEnd  (stbtt_pack_context *spc);
// Cleans up the packing context and frees all memory.

extern int  stbtt_PackFontRange(stbtt_pack_context *spc, const unsigned char *fontdata, int font_index, float font_size,
                                int first_unicode_char_in_range, int num_chars_in_range, stbtt_packedchar *chardata_for_range);

typedef struct
{
   float font_size;
   int first_unicode_char_in_range;
   int num_chars_in_range;
   stbtt_packedchar *chardata_for_range; // output
} stbtt_pack_range;

extern int  stbtt_PackFontRanges(stbtt_pack_context *spc, unsigned char *fontdata, int font_index, stbtt_pack_range *ranges, int num_ranges);


extern void stbtt_PackSetOversampling(stbtt_pack_context *spc, unsigned int h_oversample, unsigned int v_oversample);
// Oversampling a font increases the quality by allowing higher-quality subpixel
// positioning, and is especially valuable at smaller text sizes.
//
// This function sets the amount of oversampling for all following calls to
// stbtt_PackFontRange(s). The default (no oversampling) is achieved by
// h_oversample=1, v_oversample=1. The total number of pixels required is
// h_oversample*v_oversample larger than the default; for example, 2x2
// oversampling requires 4x the storage of 1x1. For best results, render
// oversampled textures with bilinear filtering. Look at the readme in
// stb/tests/oversample for information about oversampled fonts

extern void stbtt_GetPackedQuad(stbtt_packedchar *chardata, int pw, int ph,  // same data as above
                               int char_index,             // character to display
                               float *xpos, float *ypos,   // pointers to current position in screen pixel space
                               stbtt_aligned_quad *q,      // output: quad to draw
                               int align_to_integer);
]]

ffi.cdef [[
extern int stbi_write_png(char const *filename, int w, int h, int comp, const void  *data, int stride_in_bytes);
]]

local width = 256
local height = 256

local pixels = ffi.new ("unsigned char [?]", width * height)

local fontBuffer = (function ()
	local f = io.open ("/usr/share/fonts/TTF/VeraSe.ttf", "r")
	return f:read ("*all")
end) ()

-- Inclusive
local startChar = 32
local endChar = 128
local charCount = endChar - startChar
--local bakedChars = ffi.new ("stbtt_bakedchar [?]", charCount)
local packedChars = ffi.new ("stbtt_packedchar [?]", charCount)

local fontSize = 20

local padding = 4

local packContext = ffi.new ("stbtt_pack_context")
local beginRc = stb.stbtt_PackBegin (packContext, pixels, width, height, 0, padding, nil)
stb.stbtt_PackFontRange (packContext, fontBuffer, 0, fontSize, startChar, charCount, packedChars)
stb.stbtt_PackEnd (packContext)

--local unusedRow = stb.stbtt_BakeFontBitmap (fontBuffer, 0, 24, pixels, width, height, startChar, charCount, bakedChars)

--print ("unusedRow", unusedRow)
--print ("Pixels used:", unusedRow * width)

stb.stbi_write_png ("font.png", width, height, 1, pixels, width)

-- I actually can't recall if LuaJIT has sizeof or something
local function writeBakedChars (bakedChars, charCount)
	local t = {}
	local function add (...)
		table.insert (t, string.format (...))
	end
	
	add "return {\n"
		
		for i = 0, charCount - 1 do
			local c = bakedChars [i]
			add ("{ x0 = %i, y0 = %i, x1 = %i, y1 = %i, xoff = %f, yoff = %f, xadvance = %f },\n",
				c.x0, c.y0, c.x1, c.y1, c.xoff, c.yoff, c.xadvance)
		end
		
	add "}"
	
	return table.concat (t)
end

do
	local f = io.open ("baked-chars.lua", "w")
	f:write (writeBakedChars (packedChars, charCount))
end
