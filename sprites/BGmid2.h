
//{{BLOCK(BGmid2)

//======================================================================
//
//	BGmid2, 512x256@8, 
//	+ palette 256 entries, not compressed
//	+ 253 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 512 + 16192 + 4096 = 20800
//
//	Time-stamp: 2021-11-02, 19:18:47
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BGMID2_H
#define GRIT_BGMID2_H

#define BGmid2TilesLen 16192
extern const unsigned short BGmid2Tiles[8096];

#define BGmid2MapLen 4096
extern const unsigned short BGmid2Map[2048];

#define BGmid2PalLen 512
extern const unsigned short BGmid2Pal[256];

#endif // GRIT_BGMID2_H

//}}BLOCK(BGmid2)
