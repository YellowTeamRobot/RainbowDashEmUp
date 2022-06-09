
//{{BLOCK(Balloon)

//======================================================================
//
//	Balloon, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 227 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 14528 + 2048 = 17088
//
//	Time-stamp: 2021-11-02, 19:38:20
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BALLOON_H
#define GRIT_BALLOON_H

#define BalloonTilesLen 14528
extern const unsigned short BalloonTiles[7264];

#define BalloonMapLen 2048
extern const unsigned short BalloonMap[1024];

#define BalloonPalLen 512
extern const unsigned short BalloonPal[256];

#endif // GRIT_BALLOON_H

//}}BLOCK(Balloon)
