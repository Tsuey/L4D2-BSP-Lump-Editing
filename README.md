# L4D2-BSP-Lump-Editing

Faking BSP recompiles for real this time... maybe.

Map lump extraction used BSPInfo 1.4.0. "Successfully extracted all lumps."

Unlike BSP decompilation which produces errors and guesswork, extracting
lumps is an exact science.

Information below is summarized from:

  https://developer.valvesoftware.com/wiki/Source_BSP_File_Format
  https://developer.valvesoftware.com/wiki/Lump_file_format
  https://developer.valvesoftware.com/wiki/Patching_levels_with_lump_files

See "Lump Hashes and Information.txt" for MD5, SHA1, File Sizes and more.

Keeping everything above-board is critical for this type of project since
the goal is to (indirectly) modify Valve's original map BSP files. This
will allow official updates to accomplish the previous unthinkable, where
recompiling maps is strictly forbidden due to testing costs and risks.

# Procedure

The "normal way" to create *.LMP files is to decompile the original map,
load the resulting *.VMF in Hammer, make the edits, then compile out just
the modified lump -- almost exclusively this is LUMP_ENTITIES at Index 0,
since that's a plaintext lump BSPSource can decompile flawlessly.

But many lumps exist, such as LUMP_BRUSHES at Index 18:

  https://developer.valvesoftware.com/wiki/Source_BSP_File_Format#Brush_and_brushside

Bitflags CONTENTS_PLAYERCLIP (0x10000) and CONTENTS_MONSTERCLIP (0x20000)
exist within Lump 18. It is possible to edit this lump in the BSP directly,
but maintaining *.LMP files is more accountable with smaller file sizes.

The problem with the "normal way" is these are compiled brushes, and using
decompiled VMF's in that case would be very destructive since decompiles
are perfect for LUMP_ENTITIES but guessworked for LUMP_BRUSHES. Instead,
BSPInfo is used to extract all 64 lumps directly from Valve's shipped BSP.

The only problem left is that the *.LMP file header is missing, as BSPInfo
is just RAW extracts:

  ![alt text](https://github.com/Tsuey/L4D2-BSP-Lump-Editing/blob/main/Only%20Header%20is%20Missing.jpg)

Software will need to be written that allows selection of the desired lump,
most likely LUMP_ENTITIES (but others would be fun to experiment with), then
adding the *.LMP's fixed-size 20-byte header before the actual lump data.
It contains info about the lump that was in the BSP before extraction.

    struct lumpfileheader_t
    {
      int lumpOffset;   // offset in the file where the lump data begins (should be 0x14)
      int lumpID;       // the lump ID according to the lump table
      int lumpVersion;  // same as "version" in lump_t
      int lumpLength;   // same as "filelen" in lump_t
      int mapRevision;  // same as in dheader_t
    };

From there, further software to facilitate Hex Editing LUMP_BRUSHES is
needed to change the Bitflags to open up new Survivor / Infected pathing.

# Trivia

It is possible to edit BSP files server-side if exclusively LUMP_ENTITIES
is changed. It's also possible to edit an extracted LUMP_ENTITIES with a
plaintext editor, provided the lumpSize field is corrected afterwards.
The third-party tool below could help automate or educate on this process:

  https://archive.org/details/lmpfix

It has been confirmed that editing the LUMP_BRUSHES lump server-side will
fail the client checksum check... it's up to experimentation to see if any
other lumps (besides LUMP_ENTITIES) can differ -- but highly unlikely! It
works for LUMP_ENTITIES since everything in that IS server-side (though
admittedly I haven't accounted for where debris physics props are stored,
such as soda cans and pizza boxes, which DO NOT count toward edict limit
because they are client-side).

# Lump Types

Edited from Wiki to remove multiple Names and only match BSPInfo.

All these should be tested until Source begs for mercy:

	Index	Name					Purpose

	0	LUMP_ENTITIES				Map entities
	1	LUMP_PLANES				Plane array
	2	LUMP_TEXDATA				Index to texture names
	3	LUMP_VERTEXES				Vertex array
	4	LUMP_VISIBILITY				Compressed visibility bit arrays
	5	LUMP_NODES				BSP tree nodes
	6	LUMP_TEXINFO				Face texture array
	7	LUMP_FACES				Face array
	8	LUMP_LIGHTING				Lightmap samples
	9	LUMP_OCCLUSION				Occlusion polygons and vertices
	10	LUMP_LEAFS				BSP tree leaf nodes
	11	LUMP_FACEIDS				Correlates between dfaces and Hammer face IDs. Also used as random seed for detail prop placement.
	12	LUMP_EDGES				Edge array
	13	LUMP_SURFEDGES				Index of edges
	14	LUMP_MODELS				Brush models (geometry of brush entities)
	15	LUMP_WORLDLIGHTS			Internal world lights converted from the entity lump
	16	LUMP_LEAFFACES				Index to faces in each leaf
	17	LUMP_LEAFBRUSHES			Index to brushes in each leaf
	18	LUMP_BRUSHES				Brush array
	19	LUMP_BRUSHSIDES				Brushside array
	20	LUMP_AREAS				Area array
	21	LUMP_AREAPORTALS			Portals between areas
	22	LUMP_PROPCOLLISION			Static props convex hull lists
	23	LUMP_PROPHULLS				Static prop convex hulls
	24	LUMP_PROPHULLVERTS			Static prop collision vertices
	25	LUMP_PROPTRIS				Static prop per hull triangle index start/count
	26	LUMP_DISPINFO				Displacement surface array
	27	LUMP_ORIGINALFACES			Brush faces array before splitting
	28	LUMP_PHYSDISP				Displacement physics collision data
	29	LUMP_PHYSCOLLIDE			Physics collision data
	30	LUMP_VERTNORMALS			Face plane normals
	31	LUMP_VERTNORMALINDICES			Face plane normal index array
	32	LUMP_DISP_LIGHTMAP_ALPHAS		Displacement lightmap alphas (unused/empty since Source 2006)
	33	LUMP_DISP_VERTS				Vertices of displacement surface meshes
	34	LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS	Displacement lightmap sample positions
	35	LUMP_GAME_LUMP				Game-specific data lump
	36	LUMP_LEAFWATERDATA			Data for leaf nodes that are inside water
	37	LUMP_PRIMITIVES				Water polygon data
	38	LUMP_PRIMVERTS				Water polygon vertices
	39	LUMP_PRIMINDICES			Water polygon vertex index array
	40	LUMP_PAKFILE				Embedded uncompressed Zip-format file
	41	LUMP_CLIPPORTALVERTS			Clipped portal polygon vertices
	42	LUMP_CUBEMAPS				env_cubemap location array
	43	LUMP_TEXDATA_STRING_DATA		Texture name data
	44	LUMP_TEXDATA_STRING_TABLE		Index array into texdata string data
	45	LUMP_OVERLAYS				info_overlay data array
	46	LUMP_LEAFMINDISTTOWATER			Distance from leaves to water
	47	LUMP_FACE_MACRO_TEXTURE_INFO		Macro texture info for faces
	48	LUMP_DISP_TRIS				Displacement surface triangles
	49	LUMP_PROP_BLOB				Static prop triangle and string data
	50	LUMP_WATEROVERLAYS			Confirm: info_overlay's on water faces?
	51	LUMP_LEAF_AMBIENT_INDEX_HDR		Index of LUMP_LEAF_AMBIENT_LIGHTING_HDR
	52	LUMP_LEAF_AMBIENT_INDEX			Index of LUMP_LEAF_AMBIENT_LIGHTING
	53	LUMP_LIGHTING_HDR			HDR lightmap samples
	54	LUMP_WORLDLIGHTS_HDR			Internal HDR world lights converted from the entity lump
	55	LUMP_LEAF_AMBIENT_LIGHTING_HDR		Per-leaf ambient light samples (HDR)
	56	LUMP_LEAF_AMBIENT_LIGHTING		Per-leaf ambient light samples (LDR)
	57	LUMP_XZIPPAKFILE			XZip version of pak file for Xbox. Deprecated.
	58	LUMP_FACES_HDR				HDR maps may have different face data
	59	LUMP_MAP_FLAGS				Extended level-wide flags. Not present in all levels.
	60	LUMP_OVERLAY_FADES			Fade distances for overlays
	61	LUMP_OVERLAY_SYSTEM_LEVELS		System level settings (min/max CPU & GPU to render this overlay)
	62	LUMP_PHYSLEVEL				To do
	63	LUMP_DISP_MULTIBLEND			Displacement multiblend info

# Brush Lump

Edited from Wiki. Lump 18 LUMP_BRUSHES contains constructive solid geometry
defined by planes. The lump is an array of 12-byte dbrush_t structures:

	struct dbrush_t
	{
		int	firstside;	// first brushside
		int	numsides;	// number of brushsides
		int	contents;	// contents flags
	};

Which includes info about the sides of the brush and a "contents" Bitflag
that determines the content of the brush. Values are binary-ORed together
and defined in public/bspflags.h (brush array is limited to 8192 entries):

	Name				Value		Notes

	CONTENTS_EMPTY			0		No contents
	CONTENTS_SOLID			0x1		an eye is never valid in a solid
	CONTENTS_WINDOW			0x2		translucent, but not watery (glass)
	CONTENTS_AUX			0x4
	CONTENTS_GRATE			0x8		alpha-tested "grate" textures. Bullets/sight pass through, but solids don't
	CONTENTS_SLIME			0x10
	CONTENTS_WATER			0x20
	CONTENTS_MIST			0x40
	CONTENTS_OPAQUE			0x80		block AI line of sight
	CONTENTS_TESTFOGVOLUME		0x100		things that cannot be seen through (may be non-solid though)
	CONTENTS_UNUSED			0x200		unused
	CONTENTS_UNUSED6		0x400		unused
	CONTENTS_TEAM1			0x800		per team contents used to differentiate collisions between players and objects on different teams
	CONTENTS_TEAM2			0x1000
	CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000		ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
	CONTENTS_MOVEABLE		0x4000		hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
	CONTENTS_AREAPORTAL		0x8000		remaining contents are non-visible, and don't eat brushes
	CONTENTS_PLAYERCLIP		0x10000
	CONTENTS_MONSTERCLIP		0x20000
	CONTENTS_CURRENT_0		0x40000		currents can be added to any other contents, and may be mixed
	CONTENTS_CURRENT_90		0x80000
	CONTENTS_CURRENT_180		0x100000
	CONTENTS_CURRENT_270		0x200000
	CONTENTS_CURRENT_UP		0x400000
	CONTENTS_CURRENT_DOWN		0x800000
	CONTENTS_ORIGIN			0x1000000	removed before bsping an entity
	CONTENTS_MONSTER		0x2000000	should never be on a brush, only in game
	CONTENTS_DEBRIS			0x4000000
	CONTENTS_DETAIL			0x8000000	brushes to be added after vis leafs & were in func_detail entities before compiling
	CONTENTS_TRANSLUCENT		0x10000000	auto set if any surface has trans
	CONTENTS_LADDER			0x20000000
	CONTENTS_HITBOX			0x40000000	use accurate hitboxes on trace

# VBSPINFO

See "_VBSPINFO_GENERATION_DETAILS.TXT" for extensive elaboration on
the process used, and "File Hashes and Information.txt" for the
MD5, SHA1, File Sizes, etc. that resulted from that automated work.

Instead of using BSPSource's bundled BSPInfo GUI, use Valve's
tool instead -- it includes the 20-byte headers. GCFScape by
NemTools can also extract them but stay first-party instead.

  https://developer.valvesoftware.com/wiki/Vbspinfo

Extracting *.LMP files directly from the *.BSP is 100% lossless,
whereas map recompiles significantly change Face array indices etc.

To use it, open CMD.EXE then CD to your maps, then run VBSPINFO,
where -X# is 0-63 (only 64 lumps, will crash if -X64 is specified):

    cd "X:\<<__PATH_HERE__>>\Compiled PC Live BSP's (Again and Identical)"

    "C:\Program Files (x86)\Steam\steamapps\common\Left 4 Dead 2\bin\vbspinfo.exe" -X# cXmX_mapname.bsp

See "_VBSPINFO_DRAG_AND_DROP.BAT" for this script, which prompts
for the desired *.LMP # to extract, then executes:

    set /p ID="Lump #: "

    cd "C:\Program Files (x86)\Steam\steamapps\common\Left 4 Dead 2\bin"

    vbspinfo.exe -X%ID% %1

    pause

This Github repo already has all official 62 map *.LMP extracts driven
by the much larger *.TXT file -- but above is useful for Test Maps.

Optional parameters are also offered, again for all 62 maps their
output is logged in the *.TXT file. Dumps information typically
seen at the end of a Hammer compile log:

    -treeinfo		Dumps BSP tree info

    -worldtexturestats	Dumps list of textures used on Brushes and number of surfaces per use

    -modelstats		Dumps list of studiomodels used and number of times each is used

    -liststaticprops	Dumps prop_static data (only CSGO may support Version 7-10)

    -size			Dumps the worldmodel bounds of the BSP, with and without skybox

    -drawtree		Ignore/deprecated (-treeinfo adds "TREE INFO:" section, otherwise these are identical)

	Does create a "scratch.pad" 1,473-byte file, identical for 2
	different Test Maps, with "E B A E ?" as non-human readable
	text. May have secret use, but out of scope for *.LMP's.

That's all the parameters offered. Note there may be more, but
Valve only offers the source code for VBSP, VRAD, and VVIS:

  https://github.com/ValveSoftware/source-sdk-2013/tree/master/mp/src/utils

VBSPINFO is mostly just a commandline program, the actual lump
extraction (and subsequent READ/WRITE issues with LUMP 35 GAME LUMP)
could be identified in this function, which the -X# parameter calls:

    void WriteLumpToFile( char *filename, int lump )

Defined here:

  https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/utils/common/bsplib.cpp#L2750

# BSPSource Bug

Map decompiles used BSPSource 1.3.18 with "Folders" section settings.

Starting immediately with version 1.3.19, a bug/feature was introduced
that results in L4D2 ladders getting duped -- for every func_ladder in
Hammer, there were exactly two stacked up on top of each other.

Brush counts get messed up from this, so for LUMP editing just use
the older version.

  Good: older version
  https://github.com/ata4/bspsrc/releases/tag/v1.3.18

  Bad: "Brushes flagged as ladders are now written as func_ladder entities"
  https://github.com/ata4/bspsrc/releases/tag/v1.3.19

Concerning issue:

  [L4D2/Any] 07_LUMP_FACES can be zeroed to protect from decompilation
  https://github.com/ata4/bspsrc/issues/131

# Folders

Compiled PC Live BSP's (Extracted Lumps Raw):

    The original 62 BSP map files from the PC version were copied,
    hashed (see *.TXT file), then fed into BSPInfo to extract all
    64 of their lumps. There are 3968 files total, or 62 * 64.
    All are RAW lump data, without the headers needed by *.LMP's.

    See "Lump Hashes and Information.txt" for MD5 and SHA1 hashes
    for the original BSP files -- data integrity is critical and
    we need to know we're all starting from identical BSP's.

    NOTE:

	If an Index # is completely missing, then it was a 0-byte
	lump. Github refuses empty files for some bizarre reason.
	Empty lumps serve no purpose anyway.

	Github also has a File Size limit of 25 MB. A total of 3
	lump extracts were excluded from this repository, the top
	4 largest files overall listed below:

		File Size	Relative Path

		31,379,587	\c4m1_milltown_a\40_LUMP_PAKFILE.bin
		31,639,064	\c4m4_milltown_b\40_LUMP_PAKFILE.bin
		31,357,837	\c4m5_milltown_escape\40_LUMP_PAKFILE.bin
		25,594,088	\c14m1_junkyard\40_LUMP_PAKFILE.bin

	Where c14m1 narrowly fit below 25 MB (26,214,400 bytes).

Compiled PC Live BSP's (Extracted Lumps VBSPINFO):

    VBSPINFO includes all 20-byte *.LMP headers already. The *.BIN
    RAW files lack the 20-byte header but are still useful to analyze
    in cases where the filesize is same as "fileLen".

    See "_VBSPINFO_GENERATION_DETAILS.TXT" for the *.BAT that made
    these and the research (toward 0-byte lumps) that went into it,
    and verbose output for all 62 maps with the optional parameters.

    NOTE:

	Zeroing all *.LMP's works -- except LUMP 35 GAME LUMP,
	which contains its own sub-lumps and "Offset behavior"
	so needs to be either hacked or fixed to use *.LMP files
	to non-destructively modify prop_static.

	LUMP 35 is still included -- as is ~310 MB of lighting
	lumps, but ~700 MB PAKFILES are 100% excluded, and lumps
	that are proven (see *.TXT) to be 0-bytes for all 62 maps
	aren't even part of the VBSPINFO *.BAT script.

Compiled PC Live LMP's:

    All TLS Community Update *.LMP files reside here, including
    a *.LMP.BAK that can be ignored. All Modes post-fix the map
    names with _l_, Scavenge _s_, and Survival _h_ (Holdout,
    not to be confused with the Holdout Mutation).

    These files establish a comparison base for *.LMP headers
    that need to be constructed for the RAW lump extracts.
