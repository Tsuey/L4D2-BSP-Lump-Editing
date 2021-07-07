////////////////////////////////////////////////////////////////////////////////////
//
// VARIOUS STRUCTURES FOR READING & MODIFYING *.BSP / *.LMP FILES W/O RECOMPILING
//
////////////////////////////////////////////////////////////////////////////////////
//
// BSP structures and other info are edited/summarized from the Valve Wiki:
//
//	https://developer.valvesoftware.com/wiki/Source_BSP_File_Format
//
// Brush Lump (index 18) and main structures provided by ProdigySim:
//
//	https://github.com/ProdigySim/bsp-structs/blob/main/bsp.h
//
// Game Lump (index 35) and LOTS! of other research and resources provided by Orin:
//
//	https://github.com/Orinuse/BSPStructsL4D2-HexEditNeo
//
// Psychotic "GEOMETRIC" tests and other ramblings by Tsuey.
//
// For HHD Hex Editor Neo.
//
////////////////////////////////////////////////////////////////////////////////////

// JavaScript. Neo Professional edition is required for Structure Viewer, and Neo
// Ultimate edition is required for Scripting. Scripting is extremely worth while
// since it can automate repetitive tasks like Color Scheme creation and Binding,
// noting that "Export Settings" exists, too, but doesn't export Color Schemes.

// Neo requires "javascript" in all lower-case or else it'll throw a "compile error".
// The semicolon after the block seems optional but having it is consistent. Note
// that Color Schemes are a Structure Viewer feature, so this either needs to be a
// "javascript" block inside a "struct" OR a function needs to be declared and then
// called inside that "struct". The "Bind Structure -> Coloring Schemes" UI does not
// get these added, they're only usable by script.

// Useful documentation:
//
//	Structure Viewer | File Associations
//		https://docs.hhdsoftware.com/hex/definitive-guide/structure-viewer/settings-page.html
//
//	Scripting | Color Schemes
//		https://docs.hhdsoftware.com/hex/scripting/reference/parser-object/iparser.html

javascript
{
	function Custom_Color_Scheme()
	{
		// For every clip, prop, or other major asset within each lump, draw it
		// with white text over a dark midnight purple background, black outline,
		// and no round edges to help visually separate them in Structure Viewer.

		parser.add_coloring_scheme( "z_Array",
			{ r:255,	g:255,	b:255,	a:255	},
			{ r:32,		g:0,	b:64,	a:255	},
			{ r:0,		g:0,	b:0,	a:255	}, false );

		// For referential information like fileOfs, or targets of user modification
		// like flags, draw attention to them with black text over a blue background,
		// with a black outline and no round edges.

		parser.add_coloring_scheme( "z_Member",
			{ r:0,		g:0,	b:0,	a:255	},
			{ r:16,		g:116,	b:216,	a:255	},
			{ r:0,		g:0,	b:0,	a:255	}, false );
	}
};

// Custom Data Types for Source Engine.

// Take advantage of Vector to make sure Custom_Color_Scheme() always runs.

// EDIT: Actually, every time a Scripting-made Color Scheme is called, the function
// needs to be called. I tried to "Add" new Color Schemes via the UI, since I
// haven't even tested if "Export Settings" saves them yet... but the ones I "Add"
// vanish after Binding and I can't find them again. So, programmatically setting
// Color Schemes with JavaScript appears to be necessary when I want custom colors.
// Changing the default colors in the UI also resets on close. Scripts only option.

struct Vector
{
	// Custom_Color_Scheme();	// Cute attempt at a hack but this doesn't work

	float x;
	float y;
	float z;
};

struct QAngle
{
	float pitch;
	float yaw;
	float roll;
};

struct color32
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
};

// Resource for showing descriptive content flags. All the below was formatted from
// the Valve Wiki, with a few differences to note. CONTENTS_TEAM2 wasn't commented
// so sourced one. Wiki lists "CONTENTS_MIST = 0x40" but this has also been seen
// as "CONTENTS_BLOCKLOS = 0x40" with the comment "block AI line of sight", which
// on Valve's Wiki is (seemingly) wrongly placed after CONTENTS_OPAQUE, which should
// have the "things that cannot be seen through" comment instead. CONTENTS_TESTFOGVOLUME
// should probably have no comment. The CONTENTS_CURRENT_* flags seem water-related
// but are not of interest for L4D2. While confusing, none of it matters as they'll
// all be tested by trial-and-error anyway.
//
// Exactly how these play together is not known, but some Github repos give a few
// hints: "contents flags are seperate bits", "a given brush can contribute multiple
// content bits", "multiple brushes can be in a single leaf", and "lower bits are
// stronger, and will eat weaker brushes completely".
//
// More flags can be found at various Github repos, such as:
//
//	https://github.com/id-Software/Quake-III-Arena/blob/master/code/game/surfaceflags.h
//
// But this list is suitable enough for the scope of L4D2 fixes/modifications.

enum BrushContents : int
{
	CONTENTS_EMPTY			= 0,		// No contents
	CONTENTS_SOLID			= 0x1,		// an eye is never valid in a solid
	CONTENTS_WINDOW			= 0x2,		// translucent, but not watery (glass)
	CONTENTS_AUX			= 0x4,
	CONTENTS_GRATE			= 0x8,		// alpha-tested "grate" textures. Bullets/sight pass through, but solids don't
	CONTENTS_SLIME			= 0x10,
	CONTENTS_WATER			= 0x20,
	CONTENTS_MIST			= 0x40,
	CONTENTS_OPAQUE			= 0x80,		// block AI line of sight
	CONTENTS_TESTFOGVOLUME		= 0x100,	// things that cannot be seen through (may be non-solid though)
	CONTENTS_UNUSED			= 0x200,	// unused
	CONTENTS_UNUSED6		= 0x400,	// unused
	CONTENTS_TEAM1			= 0x800,	// per team contents used to differentiate collisions between players and objects on different teams
	CONTENTS_TEAM2			= 0x1000,	// between players and objects on different teams
	CONTENTS_IGNORE_NODRAW_OPAQUE	= 0x2000,	// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
	CONTENTS_MOVEABLE		= 0x4000,	// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
	CONTENTS_AREAPORTAL		= 0x8000,	// remaining contents are non-visible, and don't eat brushes
	CONTENTS_PLAYERCLIP		= 0x10000,
	CONTENTS_MONSTERCLIP		= 0x20000,
	CONTENTS_CURRENT_0		= 0x40000,	// currents can be added to any other contents, and may be mixed
	CONTENTS_CURRENT_90		= 0x80000,
	CONTENTS_CURRENT_180		= 0x100000,
	CONTENTS_CURRENT_270		= 0x200000,
	CONTENTS_CURRENT_UP		= 0x400000,
	CONTENTS_CURRENT_DOWN		= 0x800000,
	CONTENTS_ORIGIN			= 0x1000000,	// removed before bsping an entity
	CONTENTS_MONSTER		= 0x2000000,	// should never be on a brush, only in game
	CONTENTS_DEBRIS			= 0x4000000,
	CONTENTS_DETAIL			= 0x8000000,	// brushes to be added after vis leafs & were in func_detail entities before compiling (aka func_detail bud)
	CONTENTS_TRANSLUCENT		= 0x10000000,	// auto set if any surface has transparency
	CONTENTS_LADDER			= 0x20000000,
	CONTENTS_HITBOX			= 0x40000000,	// use accurate hitboxes on trace
};

// Structures that DO NOT appear when "binding" in Neo.

// Where dheader_t defines it as a Valve Source Engine BSP file, with the version,
// and 64 subsections of different kinds of map data. The first 4 bytes are VBSP.
// Structure lump_t contains the offset of the lump (from the beginning of the BSP
// file NOT from the most previous lump) and the total byte length of that lump's
// data block. Version number of the lump's format is usually 0, then fourCC which
// is usually 0, 0, 0, 0 if uncompressed.

// NOTE: Neo supports color schemes to take out some of visual monotony of Hex.
// Color schemes like [color_scheme("Signature")] are identified before the struct's
// member... it's laziest to just use the fine defaults that come with Neo. All
// the most important values to look at are colorized.

struct lump_t
{
	int	version;	// lump format version
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	int	fileOfs;	// offset into file (bytes)
	int	fileLen;	// length of lump (bytes)
	char	fourCC[ 4 ];	// lump ident code
};

struct dheader_t
{
	int	ident;		// BSP file identifier
	int	version;	// BSP file version
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	lump_t	lumps[ 64 ];	// lump directory array
	int	mapRevision;	// the map's revision (iteration, version) number
};

// Brush Lump. Always 12 bytes for each brush definition in 18_LUMP_BRUSHES. Where
// firstSide is an index into 19_LUMP_BRUSHSIDES, numSides which is the number of
// sides making up the brush, and contents which is specifically of interest to make
// brushes non-solid, change PLAYERCLIP/MONSTERCLIP flags, or make solids climbable.

// Uses the "enum BrushContents" defined earlier.

// SIZE :: Always 12 bytes each brush.

struct dbrush_t
{
	int		firstSide;	// first brushside
	int		numSides;	// number of brushsides
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	BrushContents	flags;		// contents flags
};

// Structures that DO appear when "binding".

// Where, in HHD Hex Editor Neo, you "Bind" _BSP_FILE to Byte 0 in the file, to
// then find the fileOfs (Hex Offset) where Lump 18 (or 35) start, then do another
// "Bind" at that Offset (in Decimal) for BSP_EDIT_TEST_brushes (or whatever map
// you're looking at) to then list all the brushes. Note that if you use VBSPINFO
// to extract the *.LMP file directly, the _BSP_FILE step can be skipped and you
// just bind from Byte 20 (skipping the *.LMP file's proprietary header) instead.

public struct _BSP_FILE
{
	dheader_t header;
};

// There is a limit of 8192 brushes per map, in terms of LUMP 18. For LUMP 19, that
// has a limit of 65536 brushsides per map, with a max of 128 sides per brush.

public struct BSP_EDIT_TEST_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 7 ];
};

////////////////////////////////////////////////////////////////////////////////////
//
// Per-map Brush Lump (index 18) number of brushes for array count.
//
// BSP_EDIT_TEST lump 18 is 84 bytes, and at 12 bytes per brush, 84/12 is 7 brushes.
// So that public struct gets an array of 7 so it reads until the end of them all.
//
// Following, is all the official maps and their lump 18 sizes. When using BSPInfo
// (included with BSPSource) to extract these in *.BIN files, their resulting file
// size is identical to struct lump_t's "fileLen" length of the lump (in bytes). So
// getting these values was as simple as logging the *.BIN file sizes. Note that if
// VBSPInfo.exe (included as L4D2 Bin) is used to extract these lumps, they'd all
// have exactly 20-bytes added to them since that's a *.LMP file's fixed header size.
//
// For the first map, bsp_edit_test_version2 lump 18 is 144 bytes large, so 144/12
// is 12 brushes (2 of them func_ladder) -- the size later used for the public struct.
//
// 18_LUMP_BRUSHES.bin (Sizes Per Map)
//
//	144		bsp_edit_test_version2
//
//	46,272		c1m1_hotel
//	25,320		c1m2_streets
//	50,148		c1m3_mall
//	20,748		c1m4_atrium
//	24,636		c2m1_highway
//	41,604		c2m2_fairgrounds
//	53,004		c2m3_coaster
//	21,660		c2m4_barns
//	16,428		c2m5_concert
//	28,068		c3m1_plankcountry
//	14,004		c3m2_swamp
//	28,956		c3m3_shantytown
//	26,664		c3m4_plantation
//	41,604		c4m1_milltown_a
//	29,700		c4m2_sugarmill_a
//	30,312		c4m3_sugarmill_b
//	42,084		c4m4_milltown_b
//	43,596		c4m5_milltown_escape
//	25,908		c5m1_waterfront
//	50,124		c5m1_waterfront_sndscape
//	32,484		c5m2_park
//	39,000		c5m3_cemetery
//	46,380		c5m4_quarter
//	27,336		c5m5_bridge
//	43,596		c6m1_riverbank
//	39,696		c6m2_bedlam
//	31,116		c6m3_port
//	36,444		c7m1_docks
//	35,916		c7m2_barge
//	36,324		c7m3_port
//	30,696		c8m1_apartment
//	53,964		c8m2_subway
//	48,864		c8m3_sewers
//	48,624		c8m4_interior
//	18,024		c8m5_rooftop
//	33,912		c9m1_alleys
//	34,968		c9m2_lots
//	22,764		c10m1_caves
//	49,752		c10m2_drainage
//	46,140		c10m3_ranchhouse
//	76,428		c10m4_mainstreet
//	36,048		c10m5_houseboat
//	31,968		c11m1_greenhouse
//	51,876		c11m2_offices
//	45,912		c11m3_garage
//	66,768		c11m4_terminal
//	15,024		c11m5_runway
//	13,740		c12m1_hilltop
//	32,964		c12m2_traintunnel
//	15,228		c12m3_bridge
//	34,344		c12m4_barn
//	28,356		c12m5_cornfield
//	10,464		c13m1_alpinecreek
//	12,516		c13m2_southpinestream
//	18,288		c13m3_memorialbridge
//	23,856		c13m4_cutthroatcreek
//	41,688		c14m1_junkyard
//	27,156		c14m2_lighthouse
//	120		credits
//	12,756		curling_stadium
//	5,880		tutorial_standards
//	6,372		tutorial_standards_vs
//
// Above lump sizes are unlikely to change since these maps are unlikely to ever get
// recompiled -- but just to ensure they don't change, hashes of BSP's used are at:
//
//	https://github.com/Tsuey/L4D2-BSP-Lump-Editing/blob/main/Lump%20Hashes%20and%20Information.txt
//
// And now, here's a pre-made public struct for each of those to make life easier:
//
////////////////////////////////////////////////////////////////////////////////////

public struct bsp_edit_test_version2_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 12 ];
};

// Official BSP files and their lump 18 "fileLen"'s / 12 to get number of brushes.

public struct c1m1_hotel_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3856 ];
};

public struct c1m2_streets_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2110 ];
};

public struct c1m3_mall_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 4179 ];
};

public struct c1m4_atrium_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1729 ];
};

public struct c2m1_highway_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2053 ];
};

public struct c2m2_fairgrounds_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3467 ];
};

public struct c2m3_coaster_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 4417 ];
};

public struct c2m4_barns_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1805 ];
};

public struct c2m5_concert_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1369 ];
};

public struct c3m1_plankcountry_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2339 ];
};

public struct c3m2_swamp_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1167 ];
};

public struct c3m3_shantytown_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2413 ];
};

public struct c3m4_plantation_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2222 ];
};

public struct c4m1_milltown_a_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3467 ];
};

public struct c4m2_sugarmill_a_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2475 ];
};

public struct c4m3_sugarmill_b_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2526 ];
};

public struct c4m4_milltown_b_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3507 ];
};

public struct c4m5_milltown_escape_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3633 ];
};

public struct c5m1_waterfront_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2159 ];
};

public struct c5m1_waterfront_sndscape_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 4177 ];
};

public struct c5m2_park_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2707 ];
};

public struct c5m3_cemetery_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3250 ];
};

public struct c5m4_quarter_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3865 ];
};

public struct c5m5_bridge_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2278 ];
};

public struct c6m1_riverbank_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3633 ];
};

public struct c6m2_bedlam_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3308 ];
};

public struct c6m3_port_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2593 ];
};

public struct c7m1_docks_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3037 ];
};

public struct c7m2_barge_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2993 ];
};

public struct c7m3_port_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3027 ];
};

public struct c8m1_apartment_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2558 ];
};

public struct c8m2_subway_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 4497 ];
};

public struct c8m3_sewers_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 4072 ];
};

public struct c8m4_interior_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 4052 ];
};

public struct c8m5_rooftop_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1502 ];
};

public struct c9m1_alleys_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2826 ];
};

public struct c9m2_lots_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2914 ];
};

public struct c10m1_caves_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1897 ];
};

public struct c10m2_drainage_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 4146 ];
};

public struct c10m3_ranchhouse_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3845 ];
};

public struct c10m4_mainstreet_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 6369 ];
};

public struct c10m5_houseboat_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3004 ];
};

public struct c11m1_greenhouse_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2664 ];
};

public struct c11m2_offices_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 4323 ];
};

public struct c11m3_garage_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3826 ];
};

public struct c11m4_terminal_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 5564 ];
};

public struct c11m5_runway_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1252 ];
};

public struct c12m1_hilltop_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1145 ];
};

public struct c12m2_traintunnel_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2747 ];
};

public struct c12m3_bridge_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1269 ];
};

public struct c12m4_barn_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2862 ];
};

public struct c12m5_cornfield_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2363 ];
};

public struct c13m1_alpinecreek_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 872 ];
};

public struct c13m2_southpinestream_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1043 ];
};

public struct c13m3_memorialbridge_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1524 ];
};

public struct c13m4_cutthroatcreek_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1988 ];
};

public struct c14m1_junkyard_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 3474 ];
};

public struct c14m2_lighthouse_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 2263 ];
};

public struct credits_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 10 ];
};

public struct curling_stadium_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 1063 ];
};

public struct tutorial_standards_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 490 ];
};

public struct tutorial_standards_vs_brushes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrush_t brushes[ 531 ];
};

////////////////////////////////////////////////////////////////////////////////////
//
// Per-map Game Lump (index 35) number of prop_statics for array count.
//
// Prop_Statics are the easiest to change in terms of editing the *.BSP and modifying
// their origin to be under the map. In terms of *.LMP files, they'll be the hardest
// if not impossible due to assumptions (which 100% work for ALL other lumps) the LMP
// file system makes. We're lucky it's even this easy, since L4D2 always has these 4
// lumps 0-bytes / empty (for EVERY map) which simplifies things significantly:
//
//	22	LUMP_PROPCOLLISION	Static props convex hull lists
//	23	LUMP_PROPHULLS		Static prop convex hulls
//	24	LUMP_PROPHULLVERTS	Static prop collision vertices
//	25	LUMP_PROPTRIS		Static prop per hull triangle index start/count
//
// This lump is not as straight-forward. Map bsp_edit_test_version2 has 6 prop_static
// in lump 35 and 1,278 fileLen (lump in byte size). BSP_EDIT_TEST has 0 prop_static
// but its lump is still 60 bytes large. Map c1m1_hotel has lump 35 at 156260 bytes,
// and 1748 prop_statics (counted in the decompiled VMF).
//
// Since this lump starts with a "model dictionary" before the prop_static structure
// (with the origins etc.), this lump has multiple structures -- so it's going to be
// more complicated than the Brush Lump, especially considering that 0 prop_static
// still results in a 60 byte file size, so counting the number of prop_static this
// time will not be as easy as dividing by 12.
//
// For now, following is the size of each Game Lump for all maps:
//
// 35_LUMP_GAME_LUMP.bin (Sizes Per Map)
//
//	60		BSP_EDIT_TEST
//	1,278		bsp_edit_test_version2
//
//	156,260		c1m1_hotel
//	752,780		c1m2_streets
//	335,746		c1m3_mall
//	105,658		c1m4_atrium
//	2,198,812	c2m1_highway
//	193,730		c2m2_fairgrounds
//	1,045,440	c2m3_coaster
//	134,682		c2m4_barns
//	77,810		c2m5_concert
//	2,167,383	c3m1_plankcountry
//	2,693,998	c3m2_swamp
//	2,844,250	c3m3_shantytown
//	2,625,168	c3m4_plantation
//	2,943,774	c4m1_milltown_a
//	3,205,554	c4m2_sugarmill_a
//	885,410		c4m3_sugarmill_b
//	556,282		c4m4_milltown_b
//	553,454		c4m5_milltown_escape
//	319,449		c5m1_waterfront
//	1,806,812	c5m1_waterfront_sndscape
//	1,273,284	c5m2_park
//	2,029,450	c5m3_cemetery
//	828,030		c5m4_quarter
//	248,256		c5m5_bridge
//	597,979		c6m1_riverbank
//	396,885		c6m2_bedlam
//	263,868		c6m3_port
//	396,636		c7m1_docks
//	710,066		c7m2_barge
//	187,868		c7m3_port
//	76,508		c8m1_apartment
//	98,766		c8m2_subway
//	93,382		c8m3_sewers
//	94,104		c8m4_interior
//	36,484		c8m5_rooftop
//	136,494		c9m1_alleys
//	117,968		c9m2_lots
//	1,561,506	c10m1_caves
//	967,140		c10m2_drainage
//	2,589,190	c10m3_ranchhouse
//	488,644		c10m4_mainstreet
//	2,329,562	c10m5_houseboat
//	75,762		c11m1_greenhouse
//	101,954		c11m2_offices
//	392,518		c11m3_garage
//	85,854		c11m4_terminal
//	21,450		c11m5_runway
//	975,184		c12m1_hilltop
//	866,394		c12m2_traintunnel
//	1,595,334	c12m3_bridge
//	1,437,270	c12m4_barn
//	1,342,266	c12m5_cornfield
//	2,427,674	c13m1_alpinecreek
//	172,633		c13m2_southpinestream
//	762,114		c13m3_memorialbridge
//	726,916		c13m4_cutthroatcreek
//	1,888,447	c14m1_junkyard
//	792,710		c14m2_lighthouse
//	60		credits
//	38,424		curling_stadium
//	27,176		tutorial_standards
//	27,464		tutorial_standards_vs
//
// NOTE:
//
//	The above sizes are for reference only. Struct StaticPropBlockLumps_t defines
//	a "int propEntries", where the StaticPropBlock starts with the total number
//	of prop_statics in the map -- so they don't need to be counted with division,
//	where in Brush Lump 18 there's no data for this.
//
//	Similarly, the model dictionary knows this with "int nameEntries".
//
// Game Lump header has its own game-specific lumps within it, where "prps" refers
// to prop_static and "prpd" prop_detail. Structure dgamelump_t defines those lumps,
// in the same way that struct lump_t defines the BSP file's overall 64 main lumps.
//
// NOTE:
//
//	Valve Wiki refers to "prps" as "sprp" instead (still prop_static),
//	and "prpd" as "dprp" instead (still prop_detail). All of Valve's maps
//	for L4D2 will have at least these 2.
//
//	The prop lighting lump (dplt for LDR and dplh for HDR) may also exist,
//	used for prop_detail entities (i.e. grass tufts) automatically emitted
//	by certain textures when placed on displacement surfaces.
//
// It exists to allow extension with new lumps without modifying the main BSP format.
//
// Lump "prps" (for prop_statics) starts with a model dictionary that defines all
// the unique model strings used, then the prop_static structure itself with all the
// propModelIndex to that dictionary and the Origin/Angles for all the props.
//
////////////////////////////////////////////////////////////////////////////////////

// Generic Game Lumps directory array (prps & prpd, respectively).

// Valve Wiki Note: "fileOfs is relative to the beginning of the BSP file, not to
// the game lump offset. One exception is the console version of Portal 2, where
// fileOfs is relative to the game lump offset, as one would expect."

struct dgamelump_t
{
	char		type[ 4 ];	// gamelump ID
	unsigned short	flags;		// flags
	unsigned short	version;	// gamelump version
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	int		fileOfs;	// offset to this gamelump (from beginning of file)
	int		fileLen;	// length
};

// Model dictionary. Integer count followed by list of model names used in map.

struct dstaticpropdictname_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	char name[ 128 ];	// Null-padded to this 128 character fixed length.
};

struct StaticPropDictLump_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	int nameEntries;
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dstaticpropdictname_t names[ nameEntries ];	// model name
};

// Valve Wiki Note: "Presumably, this array is used to index into the leaf lump
// to locate the leaves that each prop static is located in. Note that a prop static
// may span several leaves."

struct StaticPropLeafLump_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	int leafEntries;
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	unsigned short leaves[ leafEntries ];
};

// The prop_static Data Block structure.

// L4D1 is V7/8 and L4D2 is V9. This means that members that were exclusive to older
// versions or newer versions are not present. MinDXLevel & MaxDXLevel are V6/7 only.
// An "unsigned int Flags", LightmapResX, and LightmapResY are V7-only. V10-only is
// a "unsigned int FlagsEx", and V11-only is "float UniformScale" (prop scale) -- all
// those are ignored below. In V11 the Xbox 360 flags were removed.

// SIZE :: Always 72 bytes each prop_static, but "propEntries" makes this trivia only.

struct StaticPropLump_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	Vector		origin;			// V4 -- origin
	QAngle		angles;			// V4 -- orientation (pitch yaw roll)
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	unsigned short	propModelIndex;		// V4 -- index into model name dictionary
	unsigned short	firstLeaf;		// V4 -- index into leaf array
	unsigned short	leafCount;		// V4
	unsigned char	solid;			// V4 -- solidity type
	unsigned char	flags;			// V every except V7
	int		skin;			// V4 -- model skin numbers
	float		fadeMinDist;		// V4
	float		fadeMaxDist;		// V4
	Vector		lightingOrigin;		// V4 -- for lighting
	float		forcedFadeScale;	// V5 -- fade distance scale
	unsigned char	minCPULevel;		// V8
	unsigned char	maxCPULevel;		// V8
	unsigned char	minGPULevel;		// V8
	unsigned char	maxGPULevel;		// V8
	color32		diffuseModulation;	// V7 -- per instance color and alpha modulation
	int		disableX360;		// V9/10 -- if true, don't show on XBox 360 (4-bytes long)
};

// Valve kindly lets us know how many prop_statics are in the map!

struct StaticPropBlockLumps_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	int propEntries;
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	StaticPropLump_t prop_static[ propEntries ];
};

// Game Lump Header, where all the above is brought together.

// Renamed from "dgamelumpheader_t".

public struct _lump35_gamelump
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	int lumpCount;		// number of game lumps
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dgamelump_t		gamelump[ lumpCount ];
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	StaticPropDictLump_t	static_prop_namesdict;
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	StaticPropLeafLump_t	static_prop_leavesdict;
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	StaticPropBlockLumps_t	static_prop_blocks;
};

////////////////////////////////////////////////////////////////////////////////////
//
// WORTHLESS LUMPS
//
////////////////////////////////////////////////////////////////////////////////////
//
// Lump 18 for clips and 35 for prop_statics are the big ones.
//
// Unfortunately, worthless lumps range from those which are always 0-bytes for L4D2
// or ones which give access to data already effortlessly available in-game.
//
////////////////////////////////////////////////////////////////////////////////////

// LUMP 14 -- MODELS
////////////////////

// Including the "worldspawn" model at index 0, there is a limit of 1024 "models" per
// map. Not to be confused with "studiomodels" (with the *.mdl extension), these are
// the brushes made for entities which are attached to edicts -- like func_simpleladder,
// func_playerinfected_clip, func_brush, attribute regions, triggers, etc. With the
// apparent exception of brush entities which are point_templated, all of these normally
// have an origin of 0 0 0 -- but DebugDraw still draws them correct with mins/maxs.

// All these models can already be accessed via VScript by method of "cloning": using
// GetModel() to get an entity's model index (cl_/sv_precacheinfo) such as "*45" which
// could be what a func_simpleladder points to, then create a new entity and reference
// that model index for it.

// While this lump is among the easiest to work with, it's immediately obsoleted and
// new Infected ladders (and much more) can already be drawn with "script ShowUpdate()".

// SIZE :: Always 48 bytes each model.

struct dmodel_t
{
	Vector	mins;		// bounding box
	Vector	maxs;
	Vector	origin;		// for sounds or lights
	int	headNode;	// index into node array
	int	firstFace;	// index into face array
	int	numFaces;
};

// Set to the engine's limit for models.

// NOTE: This fails to work if "Binded" within a *.BSP file since it will "over read"
// into irrelevant space... it 100% works fine with *.LMP files, though, since it
// just reads to EOF. Worthless lump anyway so don't bother customizing for each map.

public struct _lump14_models
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dmodel_t model[ 1024 ];
};

////////////////////////////////////////////////////////////////////////////////////
//
// OTHER USEFUL LUMPS
//
////////////////////////////////////////////////////////////////////////////////////
//
// We'll at least experiment with these, and even if viable will be used sparingly.
//
// Listed in the order of discovery (and possible usefulness), rather than # order.
//
////////////////////////////////////////////////////////////////////////////////////

// LUMP 45 -- INFO_OVERLAY
//////////////////////////

// There is a limit of 512 overlays per map. While simpler "infodecal" entities are
// kept in the entity lump, LUMP 45 contains the info_overlays -- which are luckily
// still in an extremely modifiable state.

// Using Dark Carnival 4's missing "THE PEACH PIT" info_overlay as an example, the
// Keyvalues shown in Hammer very closely mirror the structure. The easiest way to
// identify the right info_overlay is to look at the "Overlay Basis Origin" in Hammer
// then use Neo to match that with Vector origin. Similar to LUMP 18 which BSPSource
// maintains the array order of on decompile, the same applies to LUMP 45, only for
// overlays it's easier to identify them by matching the origin. It's #62 for c2m4.

// Once the desired overlay is found, in-game "mat_surfaceid 1" shows a mysterious
// Hex of 0x1C5A5880 (Decimal 475682944) which I've at least confirmed is not memory
// location (LUMP 7 begins with 0x00 addresses), and "mat_surfaceid 2" shows the face's
// index array which we'll call its ID. In c2m4's case, the ID is 4405 -- this differs
// from Hammer's "Brush faces -> Pick..." option which results in 4006. Finally, to
// fix the overlay, change its faceCount from 0 to 1 then add 4405 for its only face.

// Note there also exists a "mat_surfacemat 1" which draws the material, in c2m4's
// case "concrete_ext_09", onto the same face. There's no "2" setting to get texInfo,
// so it doesn't seem useful to us. Use "mat_wireframe 3" to figure out face splits
// for more complicated geometry.

// faceCount is split into two parts: (1) the lower 14 bits is the number of faces
// the overlay appears on, and (2) the top 2 bits the render order if overlays overlap.
// oFaces (size 64 for "OVERLAY_BSP_FACE_COUNT") are the indices into the face array
// that indicate which map faces the overlay is displayed on. Texture, scale, orientation,
// etc. are other members mirrored in Hammer.

// Similar to how each structure of LUMP 18 Brushes is 12 bytes each, LUMP 45 Overlays
// is 352 bytes each -- so that can be used to customize this for each map, which again
// is only necessary for *.BSP files as *.LMP files simply read to EOF.

// SIZE :: Always 352 bytes each overlay.

struct doverlay_t
{
	int		id;
	short		texInfo;
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	unsigned short	faceCountAndRenderOrder;
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	int		oFaces[ 64 ];
	float		u[ 2 ];
	float		v[ 2 ];
	Vector		uvPoints[ 4 ];
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	Vector		origin;
	Vector		basisNormal;
};

public struct _lump45_overlays
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	doverlay_t overlay[ 512 ];
};

// LUMP 19 -- BRUSHSIDES
////////////////////////

// There is a limit of 65536 brushsides per map (hence the below array limit).

// There is a limit of 128 brushsides on a single brush (don't worry about that).

// An 8-byte structure that complements LUMP 18 BRUSHES. Zeroing either LUMP 18
// or 19 results in the same effect (all world geometry is still visible but it's
// all 100% non-solid except for displacements). The "GROUND CLIP ANOMALY" is the
// only apparent difference, where all "r_DrawClipBrushes 2" clips are aligned
// exactly with the "ground" (that "ground" has been seen on c2m4 at Z = -130 so
// doesn't mean Z = 0, I don't understand why, just how it looks).

// Where "planeNum" is the index into the LUMP 1 PLANES array giving the plane
// corresponding to this brushside, and texInfo / dispInfo references into the
// texture and displacement lumps. Bevel is zero for normal brushsides "but 1 if
// the side is a bevel plane (which seem to be used for collision detection)".

// DEAR DIARY: The Valve Wiki says "unlike the face array, brushsides are not
// culled (removed) where they touch the void". Sides which face the void have
// "their texInfo entry changed to the TOOLS/TOOLSNODRAW texture during the
// compile process"... so texInfo might be useful for removing walls by making
// them invisible. The word "void" scared me though, but this makes optimization
// sense just in case the mapper didn't NODRAW something -- it also may explain
// a sparing amount of seemingly-erroneous NODRAW occurrences, which BSPSource
// would likely replicate during decompile and make them (wrongly) NODRAW.

// Overall, I'm not yet sure where LUMP 19 will be useful... it's here just to
// exhaustively include all the lumps touched upon so far -- even though this
// is the least touched upon one.

// NOTE #1: "There is no way of linking BRUSHES and BRUSHSIDES and the corresponding
// face array entries which are used to render that brush"... whatever that means.
// It may explain technical limitations/problems I encounter later.

// NOTE #2: "BRUSHSIDES are used by the engine to calculate all player physics
// collision with world brushes", where VPhysics objects use 29_LUMP_PHYSCOLLIDE.
// Luckily, zeroing the "GEOMETRIC" LUMPS 3 / 12 / 13 allow players/objects to
// fall through the ground, so LUMP 29 can be ignored when it comes to collision...
// I'd guess that LUMP 29 exists for referential/optimization purposes.

// SIZE :: Always 8 bytes each brushside.

struct dbrushside_t
{
	unsigned short	planeNum;	// facing out of the leaf
	short		texInfo;	// texture info
	short		dispInfo;	// displacement info
	short		bevel;		// is the side a bevel plane?
};

public struct _lump19_brushsides
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dbrushside_t brushSide[ 65536 ];
};

/////////////////////////
/////////////////////////
////                 ////
////    TEXTURING    ////
////                 ////
/////////////////////////
/////////////////////////

// Map texture information is split across a web of lumps.

// When changing a map's textures, the easiest and best way is to modify the
// STRINGDATA and/or STRINGTABLE lumps. There's also relation to LUMP 19 BRUSHSIDES
// which has a "short texInfo" member, which is more work since you first need
// to know which LUMP 18 BRUSH those 6 sides belong to -- they are contiguously
// counted though, from "firstSide" then across "numSides".

// IMPORTANT:

//	Changing existing textures in a map to "TOOLS/TOOLSNODRAW" is possible,
//	but it explicitly changes it to the yellow "NODRAW" texture you see in
//	Hammer -- as tested and confirmed by modifying only texture lumps with
//	a test map that only has 1 wall. Modifying flags to "further specify" it
//	as "NODRAW + NOLIGHT" (to match Valve's existing NODRAW TexInfo's) did
//	not improve or help anything. It absolutely refuses to "invisible" itself.
//
//	Also, LUMP 19 BRUSHSIDES of the test barn door brush on c2m4 was modified
//	to NODRAW, all 6 sides. However, the end result was that "surfaceprop" in
//	console specified it as NODRAW, but it was still the original barn door
//	texture... the only difference is that shooting it didn't bullet hole decal.
//	Afterwards I made it NON-SOLID by editing LUMP 18 BRUSHES, then "surfaceprop"
//	didn't work at all on it -- which was at least to be expected. Just keep in
//	mind that modifying LUMP 19 BRUSHSIDES TexInfo's is UNRELIABLE compared to
//	modifying STRINGDATA / STRINGTABLE... and, at least with NODRAW's case, the
//	reason it failed so miserably is likely because of "residual Face data" still
//	lingering about other lumps -- similar to the reason that zeroing LUMP 7 FACES
//	entirely on c2m4 still resulted in the map functioning un-changed... which
//	later on I realized was NOT the case for a TEST MAP, but I didn't do VIS / VRAD
//	at compile, maybe "Expert"-type compiles add more redundant failsafe data?
//
//	The reason for all of this is simple: you can only use these Neo structures
//	to change a map that has one real texture (like concrete) to another real
//	texture (like grass). When it comes to applying "TOOLS/TOOLSNODRAW" in Hammer,
//	instead of just changing 4 lumps it changes ~10 because while LUMP 3 VERTEXES
//	still has the vertices for them, LUMP 12 EDGES does not make edges, and all
//	lumps related to LUMP 7 FACES (like ORIGINAL FACES, FACEIDS, and MACRO's)
//	all have less entries in their arrays because VBSP literally deletes the side
//	of the brush you apply NODRAW to.
//
//	In short, given the chaotic depth of changes NODRAW textures make at compile,
//	replicating that in a map where there a brush lacks NODRAW requires a LOT MORE
//	modification to replicate what the compiler would've done. Is it theoretically
//	possible to apply all those changes? Yes. But realistic? Overwhelmingly not.

// While NODRAW is out of the question, visualizing the focus here is visualizing
// the "workflow" for modifying STRINGDATA / STRINGTABLE to change textures, and
// observing that LUMP 19 BRUSHSIDES "short texInfo" member can also be changed --
// but first, you need to "reverse" several other lumps to find an existing texture
// that already has the U/V's you want to then apply that to new brushes:

//	Vertexes -> Edges -> Surfedges -> (Faces) -> Sides -> TexInfo -> Brushes
//
//	... as Planes "direct traffic" for Faces & Sides orientation in the world
//	... and the visibility and collision (and lightmap?) calculations that result.
//
//	... and Faces doesn't matter (since for L4D2 the lump can be zeroed) but
//	... does matter for matching LUMP 7 planeNum's to LUMP 19 BRUSHSIDES to
//	... then (by trial-and-error) identify the LUMP 18 BRUSH.

// Bitflag values for LUMP 6 TEXINFO (the "parent" of all those other lumps).
// Derived from texture's *.VMT file contents and specify special properties.

enum TextureSurfaces : int
{
	SURF_LIGHT		= 0x1,		// value will hold the light strength
	SURF_SKY2D		= 0x2,		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
	SURF_SKY		= 0x4,		// don't draw, but add to skybox
	SURF_WARP		= 0x8,		// turbulent water warp
	SURF_TRANS		= 0x10,		// texture is translucent
	SURF_NOPORTAL		= 0x20,		// the surface can not have a portal placed on it
	SURF_TRIGGER		= 0x40,		// FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
	SURF_NODRAW		= 0x80,		// don't bother referencing the texture
	SURF_HINT		= 0x100,	// make a primary bsp splitter
	SURF_SKIP		= 0x200,	// completely ignore, allowing non-closed brushes
	SURF_NOLIGHT		= 0x400,	// Don't calculate light
	SURF_BUMPLIGHT		= 0x800,	// calculate three lightmaps for the surface for bumpmapping
	SURF_NOSHADOWS		= 0x1000,	// Don't receive shadows
	SURF_NODECALS		= 0x2000,	// Don't receive decals
	SURF_NOCHOP		= 0x4000,	// Don't subdivide patches on this surface
	SURF_HITBOX		= 0x8000,	// surface is part of a hitbox
};

// TEXTURES -- LUMPS 2 | 6 | 43 | 44 | 47
/////////////////////////////////////////

// 2 -- LUMP_TEXDATA -- Index to texture names
// -------------------------------------------
//
//	Array limit of 2048 per map.
//
//	The size of the texture, pointing to texture name in LUMP 44 STRING TABLE.
//
//	Also has an RGB Vector that corresponds to texture reflectivity as derived
//	from the material's *.VTF file, probably used for radiosity calculations
//	of what light bounces off its surface.
//
// 6 -- LUMP_TEXINFO -- Face texture array
// ---------------------------------------
//
//	Array limit of 12288 per map.
//
//	Face-specific texture/lightmap x/y/z offsets and Surface flags, pointing
//	to LUMP 2 TEXDATA to get the size and texture name. The limit is higher
//	because offsets may flexibly differ for each face a texture is applied to.
//
//	TexInfo's are called by many other lumps and are the actual thing you use
//	"off the shelf", provided you have the index for it and know the texture
//	it references. In regards to the Vector arrays for 2D U/V orientation and
//	scaling, in terms of L4D2 those are way out of scope and Propper would be
//	better for anything requiring that extent of work.
//
//	If a TexInfo index (from like LUMP 19 BRUSHSIDES) is specified as -1, this
//	means that no texture info is associated with that face, which means on
//	compile it had SKIP / CLIP / INVISIBLE textures in the editor.
//
// 43 -- LUMP_TEXDATA_STRING_DATA -- Texture name data
// ---------------------------------------------------
//
//	Limit of up to 256000 bytes in the TexdataStringData data block. Limit is
//	different from other limits which are instead based around array size.
//
//	A "data block" of "concatenated null-terminated strings" giving the texture
//	names. LUMP 35 GAME LUMP has a "model dictionary"; this a "texture dictionary".
//
// 44 -- LUMP_TEXDATA_STRING_TABLE -- Index array into texdata string data
// -----------------------------------------------------------------------
//
//	Array limit of 65536 per map.
//
//	Actual array that holds texture name pointers. Integers that are offsets
//	into LUMP 43 STRINGDATA. Does not have the strings themselves, it just
//	points to them.
//
// 47 -- LUMP_FACE_MACRO_TEXTURE_INFO -- Macro texture info for faces
// ------------------------------------------------------------------
//
//	Array limit of 65536 per map (Valve Wiki only states it's a "short array").
//
//	Won't be used for L4D2 map edits... but here anyway, to be complete.
//
//	Should match LUMP 7 FACES index count. Looks up into LUMP 44 STRINGTABLE,
//	which looks up into LUMP 43 STRINGDATA, and specifies 0xFFFF (-1) if the
//	face has no "macro" texture. If it's not -1 then it is an index of a texture
//	in LUMP 4 STRINGTABLE. Short array that matches number of map faces.
//
//	Valve Wiki: "In VRAD, the corresponding texture is mapped onto the world
//	extents, and used to modulate the lightmaps of that face. There is also a
//	base macro texture (located at materials/macro/mapname/base.vtf) that is
//	applied to all faces if found. Only maps in VTMB seem to make any use of
//	macro textures"... in other words, ignore this !!
//
// For other limits, TEXTURE_NAME_LENGTH is 128 and MAX_MAP_TEXTURES is 1024.

// NOTE:

//	Neo cannot interface between lumps, so data must be manually cross-referenced
//	and each require their own array. For a standalone "BSP Viewer", only TexInfo
//	would require an array. Such a feature would've been especially handy here
//	given how tightly-woven these lumps are.

// LUMP 2 -- TEXDATA
////////////////////

// SIZE :: Always 32 bytes each.

struct dtexdata_t
{
	Vector	reflectivity;		// RGB reflectivity
	int	nameStringTableID;	// index into TexdataStringTable array
	int	width;			// source image
	int	height;
	int	view_width;
	int	view_height;
};

public struct _lump2_texdatas
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dtexdata_t texData[ 2048 ];
};

// LUMP 6 -- TEXINFO
////////////////////

// SIZE :: Always 72 bytes each.

// Use the "enum TextureSurfaces" from earlier for this int.

// The 2 floats are actually "*Vecs[ 2 ][ 4 ]" but they're simplified to just
// the "[ 8 ]" (4 arrays of a 2-float array) since adjusting U/V is way out of
// scope for L4D2. Just preserve data allocation to read the other members,
// since C / Neo gives an error for double-arrays.

struct dtexinfo_t
{
	float		textureVecs[ 8 ];	// [s/t][xyz offset] - AKA "textureVecsTexelsPerWorldUnits"
	float		lightmapVecs[ 8 ];	// [s/t][xyz offset] - length is in units of texels/area
	TextureSurfaces	flags;			// miptex flags <VDC had accidental tab here> overrides
	int		texData;		// Pointer to texture name, size, etc.
};

public struct _lump6_texinfos
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dtexinfo_t texInfo[ 12288 ];
};

// LUMP 43 -- STRINGDATA
////////////////////////

// SIZE :: The "data block" has a limit of 256000 bytes. Since TEXTURE_NAME_LENGTH
// is 128 and MAX_MAP_TEXTURES is 1024, 128*1024 = 131072... if that limit is
// referring to the maximum number of textures that can be called upon by a single
// map, then the limit may be impossible to reach.

// Like "model dictionary" of LUMP 35 GAME LUMP, except that provides "nameEntries"
// but this "texture dictionary" does not.

// BROKEN:

//	The "model dictionary" of LUMP 35 is always "null-padded" to be at a fixed
//	128 character length -- but STRINGDATA's dictionary is "null-terminated".
//	This means there's no (reasonable?) way for Neo to discern when a string ends.
//
//	LUMP 0 ENTITIES is in a similar situation, as both of these are like plaintext
//	globs. Thus, when it comes to modifying this lump in Neo, modify the Text
//	and ignore the Hex, and definitely ignore the array because it's wrong (yet
//	the 128 chars still visually "newlines" the structure, but it's worthless).

struct dstringdata_t
{
	char name[ 128 ];	// Null-padded (wrong) to this 128 char length (BROKEN)
};

public struct _lump43_stringdatas
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dstringdata_t stringData[ 1024 ];
};

// LUMP 44 -- STRINGTABLE
/////////////////////////

// SIZE :: Always 65536 bytes each. Is an actual array, unlike LUMP 43 STRINGDATA.

// Caution that given the *.LMP file's 20-byte header, whatever the "offset" is
// here you need to +20 to it to find the actual start of the texture name string
// in LUMP 43 STRINGDATA. Every string is "null-terminated" (with a 00 after it),
// so every texture name starts exactly at its first letter.

struct dstringtable_t
{
	int offset;	// Index pointer into LUMP 43 STRINGDATA's actual strings
};

public struct _lump44_stringtables
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dstringtable_t stringTable[ 65536 ];
};

// LUMP 47 -- FACEMACRO
///////////////////////

// SIZE :: Always 65536 bytes each.

// Ignore for L4D2.

struct dfacemacro_t
{
	unsigned short macroTexNameID;
};

public struct _lump47_facemacros
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dfacemacro_t faceMacro[ 65536 ];
};

////////////////////////////////////////////////////////////////////////////////////
//
// WIP | EXPERIMENTAL | GEOMETRIC
//
////////////////////////////////////////////////////////////////////////////////////
//
// Worldspawn lumps that may prove to be worthless, too complex, or too risky.
//
////////////////////////////////////////////////////////////////////////////////////

// LUMP 1 -- PLANES
///////////////////

// There is a limit of 65536 planes per map.

// IMPORTANT:

//	If a map's *.LMP file for planes is zeroed (except for the 20-byte header),
//	effectively moving all planes to the map's "0 0 0" origin, the map turns
//	completely unwalkable and "r_DrawPortals 2" verifies everything in the map
//	to be rendered at all times. Shooting walls leaves no hole decals so even
//	though always stuck, collision on worldspawn geometry seems gone. Darkened
//	prop_statics some pitchblack. No geometry congestion at map origin, since
//	LUMP 3 VERTEXES (and/or LUMP 12/13 EDGES/SURFEDGES) also need to be moved
//	for all geometry to appear at origin. Some geometry like c2m4 barns rooftops
//	and walls don't render but I don't think they're deleted just visibility is
//	confused. Also "r_DrawClipBrushes 2" shows some but not all missing (the
//	only way to get all of them to disappear is to zero LUMP 18 BRUSHES as well,
//	under the assumption that some planes don't care if zeroed because maybe
//	they started out with mostly zeroes -- this is just guessing). Majority of
//	displacements are visibly gone but oddly not all.
//
//	Overall, it's guessed that PLANES are responsible for lightmaps and lighting
//	and visibility / portal calculations... the lighting has already been baked-in
//	to LUMP 8 LIGHTING and is fixed, but "r_DrawPortals 2" are in real-time and
//	are successfully deleted which removes 100% of all culling. Thus, to fix
//	issues where you're standing inside of a deleted wall and the world around you
//	is black, zeroing the exact right plane could fix that.

// Planes define the basis of BSP geometry and split surfaces across the BSP tree
// structure. There's 20 bytes per plane, so the entire lump should be a multiple
// of 20. The Normal Vector (length 1.0) is perpendicular to the plane's surface,
// where dist is the distance from the map origin to nearest point on the plane.

// SIZE :: Always 20 bytes each plane.

struct dplane_t
{
	Vector	normal;		// normal vector
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	float	dist;		// distance from origin
	int	type;		// plane axis identifier ("usually not used", but varies for L4D2)
};

public struct _lump1_planes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dplane_t plane[ 65536 ];
};

// LUMP 7 -- FACES
//////////////////

// There is a limit of 65536 faces per map.

// IMPORTANT:

//	It has been triple++ confirmed that if a map's *.LMP file for faces is zeroed,
//	the L4D2 engine (or Source Engine in general) WILL NOT CARE and will render
//	and have everything working as normal. Only BSPSource will care, which when
//	the *.BSP is decompiled some wall textures/materials fail to work (can't predict
//	which because I don't know why to begin with), but more important 100% of all
//	info_overlays and displacements are broken.
//
//	To make matters even more confusing, "mat_SurfaceID 2" will still show face ID's
//	even with LUMP 7 zeroed -- but there's plenty of other arrays or lumps to keep
//	track of this info. However, this lump cannot be completely ignored, because the
//	command can be used, to find "planeNum", which is the index into LUMP 1 PLANES
//	array that aligns with this face in the world.
//
//	I haven't needed to understand "side" yet, but the Valve Wiki states that it's
//	"zero if this plane faces in the same direction as the face (i.e. 'out' of the face)
//	or non-zero otherwise".
//
//	On one hand we're lucky that LUMP 7 FACES can be zeroed for the map to still work
//	since it's a get-out-of-jail-free card from needing to understand one of the most
//	complex structures... on the other hand, it's unfortunate this doesn't have an
//	"isEnabled" off-switch member or something to quickly disable faces, since these
//	indices are what "mat_SurfaceID 2" will show. Instead, LUMPS 3 / 12 / 13 are needed.
//
//	Overall, interpret Valve Wiki's word "contains" as being literal: this lump is
//	mostly referential, it's not the geometry itself. See LUMPS 3 / 12 / 13 for that.

// Contains the major map geometry, used by the game engine to render the viewpoint
// of the player. Recall the c2m4 info_overlay example where in Hammer "Brush sides"
// of 4006 resulted in an actual lump face of 4405... the Valve Wiki states that
// "the face lump contains faces after they have undergone the BSP splitting process;
// they therefore do not directly correspond to the faces of brushes created in Hammer".

// Even though origFace refers to the original face it was split from, note that even
// still this did not correspond to Hammer's "Brush sides" of 4006 for that overlay.
// In fact, c2m4's working face of 4405 shows an origFace of 3458 in LUMP 7. Confusing!

// While LUMP 18 and LUMP 45 exist for BSPSource to very accurately reverse VBSP compiles,
// essentially acting as historical references for what the VMF had, when it comes to
// planes and faces they are chaotic in comparison. LUMP 27 (the ORIGINAL FACE lump)
// likely helps BSPSource a bit, since that contains all the faces before the splitting
// process (matching to origFace) and therefore has less of them, but overall faces,
// displacements, and visibility are amongst the most complexly volatile BSP structures.

// Faces are always flat, convex polygons, though they can contain edges that are co-linear.
// Each structure is 56 bytes long. Valve Wiki uses "byte", which is converted to "char".

// SIZE :: Always 56 bytes each face.

struct dface_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	unsigned short	planeNum;			// the plane number
	char		side;				// faces opposite to the node's plane direction
	char		onNode;				// 1 of on node, 0 if in leaf
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	int		firstEdge;			// index into surfedges
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	short		numEdges;			// number of surfedges
	short		texInfo;			// texture info
	short		dispInfo;			// displacement info
	short		surfaceFogVolumeID;		// ????
	char		styles[ 4 ];			// switchable lighting info
	int		lightOfs;			// offset into lightmap lump
	float		area;				// face area in units^2
	int		lightmapTexMinsInLuxels[ 2 ];	// texture lighting info
	int		lightmapTexSizeInLuxels[ 2 ];	// texture lighting info
	int		origFace;			// original face this was split from
	unsigned short	numPrims;			// primitives
	unsigned short	firstPrimID;			// ????
	unsigned int	smoothingGroups;		// lightmap smoothing group
};

public struct _lump7_faces
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dface_t face[ 65536 ];
};

/////////////////////////
/////////////////////////
////                 ////
////    GEOMETRIC    ////
////                 ////
/////////////////////////
/////////////////////////

////////////////////////////////////////////////////////////////////////////////////
//
// Lumps 3 / 12 / 13 are the beating heart of worldspawn. If LUMP 3 VERTEXES is zeroed
// the map remains 95% playable since all wall/roof collision remains, but ALL of the
// visible world is gone. All lighting and prop_statics are completely normal. Walls
// are invisible but still have collision from LUMP 18 BRUSHES so they still make hole
// decals, stop bullets, and create particles. The only hazard is falling into the
// spaces below displacements, since "r_DrawDisp 1" (default setting) shows they're all
// deleted (my short-word for "moved to the map origin", where they can all still be
// seen congested and "mat_SurfaceMat 1" will even DebugDrawText their material paths).
// Also "r_DrawPortals 1" shows they're still present and culling as normal (contrast
// this with zeroing LUMP 1 PLANES and they'll all be gone with no culling). Finally,
// with "r_DrawClipBrushes 2" all invisible walls are present with collision as normal.
//
// If LUMP 12 EDGES is zeroed (and all other 63/64 lumps unchanged), the result is 100%
// identical to when LUMP 3 VERTEXES is zeroed. This makes sense because LUMP 3 is the
// vertices that LUMP 12 connects... if either is zeroed, both have identical effect.
//
// If LUMP 13 SURFEDGES is zeroed, it's 100% identical to LUMP 12 getting zeroed. This
// illustrates a "dependency tree" where all 3 of these are CRITICALLY CO-DEPENDENT,
// but usually it's only SURFEDGES (not EDGES) which are interacted with since they
// carry the additional +/- sign to say which direction they're traced in.
//
// LUMPS 12 and 13 are analogous to LUMP 18 BRUSHES and LUMP 19 BRUSHSIDES: zeroing
// either LUMP 18 or 19 results in the same effect (all world geometry still appears
// but is 100% non-solid except for displacements) because they're co-dependent...
// ignoring the "GROUND CLIP ANOMALY" that results from zeroing LUMP 19 by itself as
// that's the only apparent difference.
//
// Overall, LUMPS 3 and 12 do most of the work... but 13 steals most of the glory.
//
////////////////////////////////////////////////////////////////////////////////////

// LUMP 3 -- VERTEXES
/////////////////////

// There is a limit of 65536 faces per map.

// Simple and direct: an array of coordinates of all the vertices (corners) of
// brushes in the map geometry. Each vertex is a Vector of 3 floats (x/y/z) for
// a total of 12 bytes per vertex -- so the *.LMP file's total size, minus the
// 20-byte header, should be divisible by / a multiple of 12.

// NOTE: Vertices can be shared between faces, if the vertices coincide exactly.

// DEAR DIARY: A vertex is literally a single x/y/z point out in space. Originally
// I had this as an array "point[ 3 ]" which consumed 12*3 bytes, as mentally I
// was thinking of this in terms of "polygons" (which would require 3 points).
// It literally is as freaking simple as the below. I got scared when I noticed
// I was only seeing 1/3rd of the array indices I was supposed to, which LUMP 12
// did not agree with at all!!

// SIZE :: Always 12 bytes each vertex.

struct dvertex_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	Vector	point;
};

public struct _lump3_vertexes
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dvertex_t vertex[ 65536 ];
};

// LUMP 12 -- EDGES
///////////////////

// There is a limit of 256000 edges per map.

// Each edge is a pair of vertex indices (which index into the vertex lump array).
// The edge is defined as the straight line between the two vertices.

// NOTE #1: This lump is immediately abstracted by LUMP 13 SURFEDGES, so usually
// this array is instead referenced by the SURFEDGE array.

// NOTE #2: "Edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face".

// NOTE #3: Like vertices, edges can be shared between adjacent faces.

// SIZE :: Always 4 bytes each edge.

struct dedge_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	unsigned short	vertex[ 2 ];	// vertex indices
};

public struct _lump12_edges
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dedge_t edge[ 256000 ];
};

// LUMP 13 -- SURFEDGES
///////////////////////

// There is a limit of 512000 surfedges per map.

// Short for "surface edge" is an array of signed integers. The absolute value of
// "edge" refers to the index into the EDGE array. If positive (+) it means the
// edge is defined from the 1st to 2nd vertex; if negative (-) it's from 2nd to 1st.
// Again, the absolute value of SURFEDGES reference the index of the EDGE array.

// The purpose of this is to allow EDGES to be referenced for a specific direction.
// Looking at LUMP 7's "struct dface_t", it has an int of "firstEdge" (index into
// SURFEDGES array) and a short of "numEdges" (number of SURFEDGES). This means that
// "firstEdge" is the first SURFEDGE array index where the face starts... then count
// from there by "numEdges" to find the others that belong to that face.

// The +/- "tracing" is necessitated because, as quoted from the Valve Wiki: "the
// vertices which make up the face are thus referenced in clockwise order; when looking
// toward the face, each edge is traced in a clockwise direction" which "makes rendering
// the faces easier, and allows quick culling of faces that face away from the viewpoint".
// I WILL NOT PRETEND TO UNDERSTAND THAT, given that I've proven that modifying a *.BSP
// file to completely zero LUMP 7 FACES (minus the 20-byte header) doesn't break the
// game (only BSPSource decompiles for some wall textures / info_overlays / and completely
// deleted displacements). But that's what the Valve Wiki states.

// NOTE #1: The limit is double that of EDGES since the number of SURFEDGES is not
// necessarily the same as the number of EDGES in the map.

// NOTE #2: "Edge 0 is never used"... (see quote for LUMP 12)

// SIZE :: Always 4 bytes each surfedge.

struct dsurfedge_t
{
	Custom_Color_Scheme(); [color_scheme("z_Member")]
	signed int	edge;		// references LUMP 12 edge with + or - sign
};

public struct _lump13_surfedges
{
	Custom_Color_Scheme(); [color_scheme("z_Array")]
	dsurfedge_t surfEdge[ 512000 ];
};