
//{{BLOCK(BGfront3)

//======================================================================
//
//	BGfront3, 512x256@8, 
//	+ palette 256 entries, not compressed
//	+ 192 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 512 + 12288 + 4096 = 16896
//
//	Time-stamp: 2021-11-03, 13:33:13
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BGFRONT3_H
#define GRIT_BGFRONT3_H

#define BGfront3TilesLen 12288
extern const unsigned short BGfront3Tiles[6144];

#define BGfront3MapLen 4096
extern const unsigned short BGfront3Map[2048];

#define BGfront3PalLen 512
extern const unsigned short BGfront3Pal[256];

#endif // GRIT_BGFRONT3_H

//}}BLOCK(BGfront3)
