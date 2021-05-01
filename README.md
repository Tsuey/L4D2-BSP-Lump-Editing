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

The normal way to create *.LMP files is to decompile the original map,
load the resulting *.VMF in Hammer, make the edits, then compile out just
the modified lump -- almost exclusively this is LUMP_ENTITIES at Index 0.

But many lumps exist, such as LUMP_BRUSHES at Index 18:

  https://developer.valvesoftware.com/wiki/Source_BSP_File_Format#Brush_and_brushside

Bitflags CONTENTS_PLAYERCLIP (0x10000) and CONTENTS_MONSTERCLIP (0x20000)
exist within Lump 18. It is possible to edit this lump in the BSP directly,
but maintaining *.LMP files is more accountable with smaller file sizes.

The problem with the "normal way" is these are compiled brushes, and using
decompiled VMF's in that case would be very destructive since decompiles
are perfect for LUMP_ENTITIES but guessworked for LUMP_BRUSHES. Instead,
BSPInfo is used to extract all 64 lumps directly.

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

Trivia: It is possible to edit BSP files server-side if only LUMP_ENTITIES
is changed. It's also possible to edit an extracted LUMP_ENTITIES with a
plain text editor, provided the lumpSize field is corrected afterwards.
The third-party tool below could help automate or educate on this process:

  https://archive.org/details/lmpfix

# Folders

Compiled PC Live BSP's (Extracted Lumps Raw):

    The original 62 BSP map files from the PC version were copied,
    hashed (see *.TXT file), then fed into BSPInfo to extract all
    64 of their lumps. There are 3968 files total, or 62 * 64.
    All are RAW lump data, without the headers needed by *.LMP's.

    See "Lump Hashes and Information.txt" for MD5 and SHA1 hashes
    for the original BSP files -- data integrity is critical and
    we need to know we're all starting from identical BSP's.

Compiled PC Live LMP's:

    All TLS Community Update *.LMP files reside here, including
    a *.LMP.BAK that can be ignored. All Modes post-fix the map
    names with _l_, Scavenge _s_, and Survival _h_ (Holdout,
    not to be confused with the Holdout Mutation).

    These files establish a comparison base for *.LMP headers
    that need to be constructed for the RAW lump extracts.
