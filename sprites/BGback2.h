
//{{BLOCK(BGback2)

//======================================================================
//
//	BGback2, 512x256@8, 
//	+ palette 256 entries, not compressed
//	+ 32 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 512 + 2048 + 4096 = 6656
//
//	Time-stamp: 2021-11-02, 22:20:39
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BGBACK2_H
#define GRIT_BGBACK2_H

#define BGback2TilesLen 2048
extern const unsigned short BGback2Tiles[1024];

#define BGback2MapLen 4096
extern const unsigned short BGback2Map[2048];

#define BGback2PalLen 512
extern const unsigned short BGback2Pal[256];

#endif // GRIT_BGBACK2_H

//}}BLOCK(BGback2)
