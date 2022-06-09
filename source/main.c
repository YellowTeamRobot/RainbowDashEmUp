#include <tonc.h> 				// Tonc library, functionalizes all (or most) hardware stuff.
#include <string.h>				// Stringalingading
// Include Sprites/backgrounds
#include "RainbowdashFB.h"		// RainbowdashFB sprite sheet [header file]
#include "shot.h"				// Projectile sprite 32x16
#include "RainCloud.h"			// Rain cloud sprite sheet
#include "Icons.h"
#include "Numbers.h"
#include "lightning.h"
#include "Shock.h"
#include "TitleV2.h"
// backgrounds
#include "BGback2.h"
#include "BGmid2.h"
#include "BGfront3.h"
#include "Balloon2.h"
// sound stuff
#include "AAS.h"				// Apex Audio System
#include "AAS_Mixer.h"			// Sound Mixer for AAS
#include "../AAS_Data.h"		// The converted sound data made by conv2aas
// more sprites
#include "LevelEnd2.h"
#include "LevelEnd3.h"
#include "Crash.h"
#include "bar.h"

#include <math.h>				// Algebraic
#include <stdlib.h>


extern void StoreValue(u32 data, u32 address);
extern u32 LoadValue(u32 address);

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

// PROTOTYPES
void GameSpriteLoad();
void TitleSpriteLoad();
void CloudAnimation();

// tid = Tile ID, pb = Palette Bank
struct SpriteData {
	int x;
	int y;
	u32 tid; // Tile ID
	u32 pb;  //Palette Bank
	u32 enabled;
	OBJ_ATTR *ptr;
};
// x, y, tid, pb, enabled, animCtr, *ptr, movementPattern, yBase
struct AnimSpriteData {
	int x;
	int y;
	u32 tid;
	u32 pb;
	u32 enabled;
	int animCtr;
	OBJ_ATTR *ptr;
	u32 movementPattern;
	int yBase; // base value of y (for sine movement)
};

// RD holds Rainbowdash's Sprite data (x, y, tid, pb)
struct SpriteData RD = {96, 32, 0, 0, 1};
struct SpriteData crash = {0, 210, 0, 0, 0, &obj_buffer[49]};

// Icon Sprite data
struct SpriteData LifeIcon = {2, 2, 544, 0, 1, &obj_buffer[10]};
struct SpriteData CloudIcon = {187, 2, 552, 0, 1, &obj_buffer[11]};

u32 MainMenuBreak = 0;
// Level End text
struct SpriteData L[3] = {
	{0, 170, 896, 0, 0, &obj_buffer[50]},
	{0, 170, 896, 0, 0, &obj_buffer[51]},
	{0, 170, 896, 0, 0, &obj_buffer[52]}
};
struct SpriteData E[4] = {
	{0, 170, 904, 0, 0, &obj_buffer[53]},
	{0, 170, 904, 0, 0, &obj_buffer[54]},
	{0, 170, 904, 0, 0, &obj_buffer[55]},
	{0, 170, 904, 0, 0, &obj_buffer[56]}
};
struct SpriteData VT =  {0, 170, 912, 0, 0, &obj_buffer[57]};
struct SpriteData CO = {0, 170, 952, 0, 0, &obj_buffer[58]};
struct SpriteData MP = {0, 170, 968, 0, 0, &obj_buffer[59]};
struct SpriteData T =  {0, 170, 920, 0, 0, &obj_buffer[60]};
struct SpriteData AI = {0, 170, 984, 0, 0, &obj_buffer[61]}; // FAI
struct SpriteData D =  {0, 170, 928, 0, 0, &obj_buffer[62]};
struct SpriteData N =  {0, 170, 936, 0, 0, &obj_buffer[63]};
struct SpriteData S =  {0, 170, 944, 0, 0, &obj_buffer[64]};
struct SpriteData S2 =  {0, 170, 944, 0, 0, &obj_buffer[65]};

// OnScreen counters - animCtr used to hold actual number it represents
struct AnimSpriteData LifeNum = {22, 0, 576, 0, 1, 0, &obj_buffer[12]};
struct AnimSpriteData CloudNum[4] = {
	{231, 0, 576, 0, 1, 7, &obj_buffer[13]}, // Ones
	{223, 0, 576, 0, 1, 9, &obj_buffer[14]}, // Tens
	{215, 0, 576, 0, 1, 9, &obj_buffer[15]}, // Hundreds
	{207, 0, 576, 0, 1, 9, &obj_buffer[16]}  // Thousands
};

//  Title sprites data
struct SpriteData Title1A = {27, 0, 0, 0, 1, &obj_buffer[0]};
struct SpriteData Title1B = {91, 0, 128, 0, 1, &obj_buffer[1]};
struct SpriteData Title1C = {155, 0, 256, 0, 1, &obj_buffer[2]};
struct SpriteData Title1D = {27, 64, 384, 0, 1, &obj_buffer[3]};
struct SpriteData Title2A = {91, 64, 512, 0, 1, &obj_buffer[10]};
struct SpriteData Title2B = {155, 64, 640, 0, 1, &obj_buffer[11]};


struct SpriteData bar[8] = {
	{0, 0, 800, 0, 0, &obj_buffer[12]},
	{0, 0, 800, 0, 0, &obj_buffer[13]},
	{0, 0, 800, 0, 0, &obj_buffer[14]},
	{0, 0, 800, 0, 0, &obj_buffer[15]},
	{0, 0, 800, 0, 0, &obj_buffer[16]},
	{0, 0, 800, 0, 0, &obj_buffer[17]},
	{0, 0, 800, 0, 0, &obj_buffer[18]},
	{0, 0, 800, 0, 0, &obj_buffer[19]}
};

// projectile sprites data								
struct SpriteData Shot[3] = {
	{0, 170, 288, 0, 0, &obj_buffer[4]}, 		// Shot[0]
	{0, 170, 288, 0, 0, &obj_buffer[3]},		// Shot[1]
	{0, 170, 288, 0, 0, &obj_buffer[2]}		// Shot[2]
};

struct SpriteData Lightning[3] = {
	{0, 170, 736, 0, 0, &obj_buffer[17]},
	{0, 170, 736, 0, 0, &obj_buffer[18]},
	{0, 170, 736, 0, 0, &obj_buffer[19]}
};

u32 animDelayCloud = 6;
// raincloud sprites data (5 clouds, some get reused) [5*32=160, screen height is 160]
// (x, y, tid, pb, enabled, animCtr, *ptr, movement pattern, yBase)
struct AnimSpriteData Cloud[5] = {
	{0, 210, 320, 0, 0, 0, &obj_buffer[5], 0, 0},		// Cloud[0]
	{0, 210, 320, 0, 0, 0, &obj_buffer[6], 0, 0},		// Cloud[1]
	{0, 210, 320, 0, 0, 0, &obj_buffer[7], 0, 0},		// Cloud[2]
	{0, 210, 320, 0, 0, 0, &obj_buffer[8], 0, 0},		// Cloud[3]
	{0, 210, 320, 0, 0, 0, &obj_buffer[9], 0, 0}		// Cloud[4]
};

// electrocution effect
u32 animDelayShock = 6;
struct AnimSpriteData Shock = {0, 170, 768, 0, 0, 0, &obj_buffer[0]};

struct BGData {
	int x;
	int y;
	int speed;
	int ctr;
};
struct BGData BGback = {0, 0, 30, 0};
struct BGData BGmid = {0, 0, 6, 0};
struct BGData BGfront = {0, 0, 1, 0};
struct BGData BGballoon = {272, 0, 4, 4};
u32 BalloonAppearance = 0;
u32 AppearanceCtr = 0;

s32 iframes = 0; // invincibility frames
u32 music = 1; // 1 music on (On highlighted), 0 music off (Off highlighted)
u32 PhaseCounter; // current phase of level
u32 PhaseDelay = 0;
u32 PhaseActive; // 0 is inactive, 1 is initialization, 2 is active

u32 KCode = 0;
u32 score = 0;

s32 BossHealth, Bounciness, BossLightningFrequency, BLFCTR, BossSpeed;
u32 BossEnabled = 0;

s32 rrange(s32 upper, s32 lower) {
	return (rand() % (upper - lower + 1)) + lower;
}

void InputHandling() {
	//Makes Rainbowdash move around the screen
	// move left/right
	if (RD.x >= -2 ||  RD.x <= 208){
		RD.x += 2*key_tri_horz();
	}
	if (RD.x < -2) { RD.x = -2; }
	if (RD.x > 208) {RD.x = 208; }
	// move up/down
	if (RD.y >= -2 || RD.y <= 128 ) {
		RD.y += 2*key_tri_vert();
	}
	// keeps Rainbowdash in screen bounds
	if (RD.y < -2) { RD.y = -2; } 
	if (RD.y > 128) { RD.y = 128; }


	// fire (enables projectile and sets initial position)
	if (key_hit(KEY_A)) {
		if (Shot[0].enabled == 1 && Shot[1].enabled == 1 && Shot[2].enabled == 0) {
			Shot[2].enabled = 1;
			Shot[2].x = RD.x+26;
			Shot[2].y = RD.y+16;
		}
		if (Shot[0].enabled == 1 && Shot[1].enabled == 0) {
			Shot[1].enabled = 1;
			Shot[1].x = RD.x+26;
			Shot[1].y = RD.y+16;
		}
		if (Shot[0].enabled == 0) {
			Shot[0].enabled = 1;
			Shot[0].x = RD.x+26;
			Shot[0].y = RD.y+16;
		}


	}
}
void BackgroundHandling() {
	//BG scrolling
	BGback.ctr++;
	if (BGback.ctr == BGback.speed) {
		BGback.x++;
		REG_BG3HOFS= BGback.x;
		//REG_BG3VOFS= BGback.y;
		BGback.ctr = 0;
	}
	BGmid.ctr++;
	if (BGmid.ctr == BGmid.speed) {
		BGmid.x++;
		REG_BG2HOFS= BGmid.x;
		//REG_BG2VOFS= BGmid.y;
		BGmid.ctr = 0;
	}
	BGfront.ctr++;
	if (BGfront.ctr == BGfront.speed) {
		BGfront.x++;
		REG_BG1HOFS= BGfront.x;
		//REG_BG1VOFS= BGfront.y;
		BGfront.ctr = 0;
	}
	// Y parallax, with character vertical movement
	BGmid.y = 10 + RD.y / 13;
	BGfront.y = RD.y / 4;
	REG_BG2VOFS= BGmid.y;
	REG_BG1VOFS= BGfront.y;


	// Balloon
	BGballoon.ctr++;
	if (AppearanceCtr >= BalloonAppearance && BGballoon.ctr >= BGballoon.speed) {
		BGballoon.x++;
		REG_BG0HOFS= BGballoon.x;
		//BGballoon.y = 20*sin(0.07*BGballoon.x) + 40;
		//REG_BG0VOFS= 20*sin(0.07*BGballoon.x) + 40;
		BGballoon.ctr = 0;
	}
	if (AppearanceCtr >= BalloonAppearance) {
		REG_BG0VOFS = 20*sin(0.07*BGballoon.x) + 40 + RD.y/3;
		if (BGballoon.x > 640) {
			BGballoon.x = 272;
			//REG_BG0HOFS= BGballoon.x;
			AppearanceCtr = 0;
			BalloonAppearance = rrange(10800,1800);
		}
	}
	AppearanceCtr++;
	

}
void ProjectileHandling() {
	u32 j, i;
	for (j=0; j<3; j++) {
		// Moves shot forward when it is enabled
		if (Shot[j].enabled == 1) {
			Shot[j].x += 2;
			// Disabled shot when it goes off screen (and moves it to just below screen)
			if (Shot[j].x >= 240) {
				Shot[j].enabled = 0;
				Shot[j].x = 0;
				Shot[j].y = 170;
			}
			// Collision handling (for when a projectile hits a raincloud)
			for (i=0; i<5; i++){
				if (Cloud[i].enabled == 1 && Shot[j].enabled == 1) {
					if ( ((Shot[j].x+32 >= Cloud[i].x && Shot[j].x+32 <= Cloud[i].x+32) ||  (Shot[j].x+20 <= Cloud[i].x+32 && Shot[j].x+20 >= Cloud[i].x)) && ((Shot[j].y >= Cloud[i].y && Shot[j].y <= Cloud[i].y+32) || (Shot[j].y+16 >= Cloud[i].y && Shot[j].y+16 <= Cloud[i].y+32)) ) {
						if (BossEnabled != 1){
							Cloud[i].enabled = 2; // 0 is disabled, 1 is enabled, 2 is used to play animation before being completely disabled
						}
						else if (BossEnabled == 1){
							BossHealth--;
						}

						Shot[j].enabled = 0;
						Shot[j].x = 0;
						Shot[j].y = 170;

						// play sound
						AAS_SFX_Play(2, 64, 32000, AAS_DATA_SFX_START_CloudPuff,
							AAS_DATA_SFX_END_CloudPuff, AAS_NULL);

						score++;
						// Increment display digit counters: this works like an old flip clock, everytime the n-1 digit wraps around from 9 back to 0 it increments the nth digit (i.e., when Ones digit wraps from 9 back to 0, Tens increases by one)
						CloudNum[0].animCtr++; // increment Ones place
						if (CloudNum[0].animCtr > 9) {CloudNum[0].animCtr = 0; CloudNum[1].animCtr++;} // Increment Tens Place
						if (CloudNum[1].animCtr > 9) {CloudNum[1].animCtr = 0; CloudNum[2].animCtr++;} // Increment Hundreds Place
						if (CloudNum[2].animCtr > 9) {CloudNum[2].animCtr = 0; CloudNum[3].animCtr++;} // Increment Thousands place
						if (CloudNum[3].animCtr > 9) {CloudNum[3].animCtr = 9; CloudNum[2].animCtr = 9; CloudNum[1].animCtr = 9; CloudNum[0].animCtr = 9;} //i.e. score is capped at 9999
					}
				}
			}
			obj_set_pos(Shot[j].ptr, Shot[j].x, Shot[j].y);
		}
	}
}
void LightningHandling() {
	u32 i;
	for(i=0; i<3; i++) {
		if (Lightning[i].enabled == 1) {

			Lightning[i].x -= 2; // move lightning bolt left

			// if lightning goes offscreen
			if (Lightning[i].x <= -32) {
				Lightning[i].x = 0;
				Lightning[i].y = 170;
				Lightning[i].enabled = 0;
			}

			// Collision with rainbowdash
			if (((Lightning[i].x >= RD.x && Lightning[i].x <= RD.x+32) || (Lightning[i].x+32 >= RD.x && Lightning[i].x+32 <= RD.x+32)) && ((Lightning[i].y >= RD.y+3 && Lightning[i].y <= RD.y+30) || (Lightning[i].y+16 >= RD.y+3 && Lightning[i].y+16 <= RD.y+30))  ) {
				if (iframes <= 0) { // if player is not invincible
					LifeNum.animCtr--; // decrease lives
					// Play sound effect
					AAS_SFX_Play(3, 64, 22050, AAS_DATA_SFX_START_Zap2,
						AAS_DATA_SFX_END_Zap2, AAS_NULL);
					// show electrocution animation
					Shock.enabled = 1; Shock.animCtr = 0;
				}
				iframes = 30; // gives 30 frames of invincibility (1/2 second)
				Lightning[i].x = 0;
				Lightning[i].y = 170;
				Lightning[i].enabled = 0;
			}

			obj_set_pos(Lightning[i].ptr, Lightning[i].x, Lightning[i].y); // set lightning bolt position
		}
	}
}
void LevelEndHandling(u32 levelstate, u32 level) {
	u32 b = 0;
	u32 bbx = 0;
	u32 bby = 72;
	if (levelstate != 1){
		// disable any clouds on screen
		for(b=0; b<5; b++) {
			if (Cloud[b].enabled == 1) Cloud[b].enabled = 2;
		}
		tonccpy(&tile_mem[5][384], LevelEnd2Tiles, LevelEnd2TilesLen); //load level end text sprites into memory
		tonccpy(&tile_mem[5][440], LevelEnd3Tiles, LevelEnd3TilesLen);
		tonccpy(&tile_mem[4][0], CrashTiles, CrashTilesLen);
		// ------------------------initialize sprites-------------
		//initialize L
		for (b=0;b<3;b++){
			obj_set_attr(L[b].ptr,
				ATTR0_SQUARE | ATTR0_8BPP,
				ATTR1_SIZE_16x16,
				ATTR2_PALBANK(L[b].pb | L[b].tid) );
			obj_set_pos(L[b].ptr, L[b].x, L[b].y);
			L[b].ptr->attr2= ATTR2_BUILD(L[b].tid, L[b].pb, 0);
		}
		//initialize E
		for (b=0;b<4;b++){
			obj_set_attr(E[b].ptr,
				ATTR0_SQUARE | ATTR0_8BPP,
				ATTR1_SIZE_16x16,
				ATTR2_PALBANK(E[b].pb | E[b].tid) );
			obj_set_pos(E[b].ptr, E[b].x, E[b].y);
			E[b].ptr->attr2= ATTR2_BUILD(E[b].tid, E[b].pb, 0);
		}
		//initialize V
		obj_set_attr(VT.ptr,
			ATTR0_SQUARE | ATTR0_8BPP,
			ATTR1_SIZE_16x16,
			ATTR2_PALBANK(VT.pb | VT.tid) );
		obj_set_pos(VT.ptr, VT.x, VT.y);
		VT.ptr->attr2= ATTR2_BUILD(VT.tid, VT.pb, 0);
		//initialize T
		obj_set_attr(T.ptr,
			ATTR0_SQUARE | ATTR0_8BPP,
			ATTR1_SIZE_16x16,
			ATTR2_PALBANK(T.pb | T.tid) );
		obj_set_pos(T.ptr, T.x, T.y);
		T.ptr->attr2= ATTR2_BUILD(T.tid, T.pb, 0);
		//initialize N
		obj_set_attr(N.ptr,
			ATTR0_SQUARE | ATTR0_8BPP,
			ATTR1_SIZE_16x16,
			ATTR2_PALBANK(N.pb | N.tid) );
		obj_set_pos(N.ptr, N.x, N.y);
		N.ptr->attr2= ATTR2_BUILD(N.tid, N.pb, 0);
		//initialize D
		obj_set_attr(D.ptr,
			ATTR0_SQUARE | ATTR0_8BPP,
			ATTR1_SIZE_16x16,
			ATTR2_PALBANK(D.pb | D.tid) );
		obj_set_pos(D.ptr, D.x, D.y);
		D.ptr->attr2= ATTR2_BUILD(D.tid, D.pb, 0);
		//initialize S
		obj_set_attr(S.ptr,
			ATTR0_SQUARE | ATTR0_8BPP,
			ATTR1_SIZE_16x16,
			ATTR2_PALBANK(S.pb | S.tid) );
		obj_set_pos(S.ptr, S.x, S.y);
		S.ptr->attr2= ATTR2_BUILD(S.tid, S.pb, 0);
		//initialize S2
		obj_set_attr(S2.ptr,
			ATTR0_SQUARE | ATTR0_8BPP,
			ATTR1_SIZE_16x16,
			ATTR2_PALBANK(S2.pb | S2.tid) );
		obj_set_pos(S2.ptr, S2.x, S2.y);
		S2.ptr->attr2= ATTR2_BUILD(S2.tid, S2.pb, 0);
		//initialize CO
		obj_set_attr(CO.ptr,
			ATTR0_WIDE | ATTR0_8BPP,
			ATTR1_SIZE_32x16,
			ATTR2_PALBANK(CO.pb | CO.tid) );
		obj_set_pos(CO.ptr, CO.x, CO.y);
		CO.ptr->attr2= ATTR2_BUILD(CO.tid, CO.pb, 0);
		//initialize MP
		obj_set_attr(MP.ptr,
			ATTR0_WIDE | ATTR0_8BPP,
			ATTR1_SIZE_32x16,
			ATTR2_PALBANK(MP.pb | MP.tid) );
		obj_set_pos(MP.ptr, MP.x, MP.y);
		MP.ptr->attr2= ATTR2_BUILD(MP.tid, MP.pb, 0);
		//initialize AI
		obj_set_attr(AI.ptr,
			ATTR0_WIDE | ATTR0_8BPP,
			ATTR1_SIZE_32x16,
			ATTR2_PALBANK(AI.pb | AI.tid) );
		obj_set_pos(AI.ptr, AI.x, AI.y);
		AI.ptr->attr2= ATTR2_BUILD(AI.tid, AI.pb, 0);
	}

	switch (levelstate) {
		case 1: break; // level is not over
		case 2: // level complete (premade)
			bbx = 34;
			obj_set_pos(L[0].ptr, bbx, bby); 	// L
			obj_set_pos(E[0].ptr, bbx+12, bby); // E
			obj_set_pos(VT.ptr, bbx+24, bby);	// V
			obj_set_pos(E[1].ptr, bbx+36, bby);	// E
			obj_set_pos(L[1].ptr, bbx+48, bby);	// L

			obj_set_pos(CO.ptr, bbx+72, bby);	// CO
			obj_set_pos(MP.ptr, bbx+96, bby);	// MP
			obj_set_pos(L[2].ptr, bbx+126, bby);// L
			obj_set_pos(E[2].ptr, bbx+138, bby);// E
			obj_set_pos(T.ptr, bbx+150, bby);	// T
			obj_set_pos(E[3].ptr, bbx+162, bby);// E
			break;
		case 3: // level failed (premade)
			bbx = 52;
			obj_set_pos(L[0].ptr, bbx, bby); 	// L
			obj_set_pos(E[0].ptr, bbx+12, bby); // E
			obj_set_pos(VT.ptr, bbx+24, bby);	// V
			obj_set_pos(E[1].ptr, bbx+36, bby);	// E
			obj_set_pos(L[1].ptr, bbx+48, bby);	// L

			obj_set_pos(AI.ptr, bbx+72, bby);	// FAI
			obj_set_pos(L[2].ptr, bbx+102, bby);// L
			obj_set_pos(E[3].ptr, bbx+114, bby);// E
			obj_set_pos(D.ptr, bbx+126, bby);	// D
			break;
		case 4: // endless complete (endless)
			bbx = 9;
			obj_set_pos(E[0].ptr, bbx+12, bby);	// E
			obj_set_pos(N.ptr, bbx+24, bby);	// N
			obj_set_pos(D.ptr, bbx+36, bby);	// D
			obj_set_pos(L[0].ptr, bbx+48, bby);	// L
			obj_set_pos(E[1].ptr, bbx+60, bby);	// E
			obj_set_pos(S.ptr, bbx+72, bby);	// S
			obj_set_pos(S2.ptr, bbx+84, bby);	// S

			obj_set_pos(CO.ptr, bbx+108, bby);	// CO
			obj_set_pos(MP.ptr, bbx+132, bby);	// MP
			obj_set_pos(L[2].ptr, bbx+162, bby);// L
			obj_set_pos(E[2].ptr, bbx+174, bby);// E
			obj_set_pos(T.ptr, bbx+186, bby);	// T
			obj_set_pos(E[3].ptr, bbx+198, bby);// E

			break;
		default: break;
	}


	if (levelstate != 1) {
		u32 temphiscore = LoadValue(level);
		// reset shock animation
		Shock.tid = 768;
		Shock.enabled = 0;
		Shock.x = 0;
		Shock.y = 170;
		// check/update hiscore
		if (score > temphiscore) {
				StoreValue(score, level);
			// Play new record voiceline
			AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_NewRecord,
					AAS_DATA_SFX_END_NewRecord, AAS_NULL);
		}
		else if (levelstate == 2) {
			// play one of the normal level complete voicelines
			u32 bloopadoop = rrange(3,1);
			if (bloopadoop == 1) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_StormcloudDefeated,
					AAS_DATA_SFX_END_StormcloudDefeated, AAS_NULL);
			}
			else if (bloopadoop == 2) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_ClearSkies2,
					AAS_DATA_SFX_END_ClearSkies2, AAS_NULL);
			}
			else if (bloopadoop == 3) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_AnotherStormAbated,
					AAS_DATA_SFX_END_AnotherStormAbated, AAS_NULL);
			}
		}
		// wait 8 seconds before returning to main menu
		while (b <= 480) {
			VBlankIntrWait();
			key_poll();

			CloudAnimation(); // animate clouds being destroyed
			ProjectileHandling(); // move any remaining projectiles offscreen and disable
			LightningHandling(); // move any remaining lightning offscreen and disable

			b++;

			if(crash.y < 160) {
				crash.x += 2;
				crash.y++;
			}

			obj_set_pos(crash.ptr, crash.x, crash.y);
			oam_copy(oam_mem, obj_buffer, 128);
		}
		MainMenuBreak = 1;
	}

}
// Defining names for patterns
#if 1

// Formation Patterns
#define V 1
#define LSLANT 2
#define RSLANT 3
#define DIAMOND 4
#define SQUARE 5
#define TWOSINES 6
#define LREG 7
#define LINV 8
#define SIDEWAYSW 9
#define SIDEWAYSM 10
#define VINV 11
#define VSS 12
#define VINVSS 13
#define VSMS 14
#define VINVSMS 15
#define SMILEY 16
#define PENTAGON 17
#define SREG 18
#define SINV 19
#define SINEWAVEFIVE 20
#define ARROW 21
#define WAVYARROW 22
#define TRIANGLE 23
#define MINIBOSSREG 24
#define MINIBOSSSSINE 25
#define MINIBOSSMSINE 26
#define SHIELD 27
#define MPARA 28
#define MPARAINV 29
#define CHECK 30
#define BEAM1 31
#define BEAM2 32

#define SCATTER3SINE 54
#define SCATTER4SINE 55
#define SCATTER5SINE 56
#define SCATTER3 57
#define SCATTER4 58
#define SCATTER5 59

#define AMORSE 60
#define BMORSE 61
#define DMORSE 62
#define HMORSE 63
#define IMORSE 64
#define NMORSE 65
#define OMORSE 66
#define RMORSE 67
#define SMORSE 68
#define WMORSE 69
// coords for random scattering
struct coords {
	u32 x;
	u32 y;
};
struct coords Scatter[16] = {
	{256, 24},  {289, 24},  {322, 24},  {355, 24},
	{256, 56},  {289, 56},  {322, 56},  {355, 56},
	{256, 88},  {289, 88},  {322, 88},  {355, 88},
	{256, 120}, {289, 120}, {322, 120}, {355, 120}
};
struct coords Scatter2[12] = {
	{256, 36},  {289, 36},  {322, 36},  {355, 36},
	{256, 68},  {289, 68},  {322, 68},  {355, 68},
	{256, 100}, {289, 100}, {322, 100}, {355, 100}
};
void scatterCoords() {
	u32 i, j;
	struct coords temp;
	for (i = 0; i<16; i ++) {
		j = (rand()%15)+1;
		temp = Scatter[i];
		Scatter[i] = Scatter[j];
		Scatter[j] = temp;
	}
}
void scatterCoords2() {
	u32 i, j;
	struct coords temp;
	for (i = 0; i<12; i ++) {
		j = (rand()%11)+1;
		temp = Scatter2[i];
		Scatter2[i] = Scatter2[j];
		Scatter2[j] = temp;
	}
}

// Individual cloud movement patterns
// Default = 0
#define SSINE 1 // small sine
#define MSINE 2 // medium sine
#define LSINE 3 // large sine
#define SSINEINV 4 //inverted small sine
#define MSINEINV 5 // inverted medium sine

// Lightning pattern thingy
#define RANDOM1 777
#define RANDOM2 778
#define RANDOM3 779
#define BOSS 2010
#define SCATTER2 56

#endif
void EnableClouds() {
	u32 i;
	for (i=0; i<5; i++) {
		Cloud[i].enabled = 1;
	}
}
void ResetClouds() {
	// reset parameters of all clouds to original/default (i.e. offscreen, disabled, default movement pattern, default tid/animCtr)
	u32 i;
	for (i=0; i<5; i++) {
		Cloud[i].enabled = 0;
		Cloud[i].x = 0;
		Cloud[i].y = 210; // Below screen: out of site, out of mind
		Cloud[i].tid = 320; //this is the base tid of clouds (location in sprite memory of start of first Cloud tile)
		Cloud[i].animCtr = 0; //reset animation so we don't end up with the wrong sprite displayed
		Cloud[i].movementPattern = 0; //reset to default movement Pattern
		Cloud[i].yBase = 0; //set ybase to 0 as well
	}
}
void InitPattern(u32 pattern) {
	ResetClouds();
	EnableClouds();
	u32 i;
	u32 temp = rrange(96, 32); // MORSE Y COORD
	switch (pattern) {
		case V:
					Cloud[0].x = 320; Cloud[0].y = 4;
				Cloud[1].x = 288; Cloud[1].y = 36;
			Cloud[2].x = 256; Cloud[2].y = 68;
				Cloud[3].x = 288; Cloud[3].y = 100;
					Cloud[4].x = 320; Cloud[4].y = 132;
			break;
		case LSLANT:
			for (i=0;i<5;i++) {
				Cloud[i].x = 256 + i*16;
				Cloud[i].y = i*32+4;
			}
			break;
		case RSLANT:
			for (i=0;i<5;i++) {
				Cloud[i].x = 384  - i*16;
				Cloud[i].y = i*32+4;
			}
			break;
		case DIAMOND:
											   Cloud[0].x = 288; Cloud[0].y = 32;
			Cloud[1].x = 256; Cloud[1].y = 64; Cloud[2].x = 288; Cloud[2].y = 64; Cloud[3].x = 320; Cloud[3].y = 64;
											   Cloud[4].x = 288; Cloud[4].y = 96;
			break;
		case SQUARE:
			Cloud[0].x = 256; Cloud[0].y = 32;  Cloud[1].x = 320; Cloud[1].y = 32;
							  Cloud[2].x = 288; Cloud[2].y = 64;
			Cloud[3].x = 256; Cloud[3].y = 96;  Cloud[4].x = 320; Cloud[4].y = 96;
			break;
		case TWOSINES:
			Cloud[0].x = 256; Cloud[0].yBase = 32; Cloud[0].movementPattern = SSINE;			Cloud[2].x = 288; Cloud[2].yBase = 32; Cloud[2].movementPattern = SSINE;
			Cloud[1].x = 256; Cloud[1].yBase = 96; Cloud[1].movementPattern = SSINEINV;			Cloud[3].x = 288; Cloud[3].yBase = 96; Cloud[3].movementPattern = SSINEINV;
			Cloud[4].enabled = 0;
			break;
		case LREG:
			Cloud[0].x = 256; Cloud[0].y = 16;
			Cloud[1].x = 256; Cloud[1].y = 63;
			Cloud[2].x = 256; Cloud[2].y =111;  Cloud[3].x = 296; Cloud[3].y = 111; Cloud[4].x = 336; Cloud[4].y = 111;
			break;
		case LINV:
			Cloud[0].x = 256; Cloud[0].y = 111;
			Cloud[1].x = 256; Cloud[1].y = 63;
			Cloud[2].x = 256; Cloud[2].y =16;  Cloud[3].x = 296; Cloud[3].y = 16; Cloud[4].x = 336; Cloud[4].y = 16;
			break;
		case SIDEWAYSW:
			Cloud[0].x = 256; Cloud[0].y = 4;
					Cloud[1].x = 288; Cloud[1].y = 36;
			Cloud[2].x = 256; Cloud[2].y = 68;
					Cloud[3].x = 288; Cloud[3].y = 100;
			Cloud[4].x = 256; Cloud[4].y = 132;
			break;
		case SIDEWAYSM:
					Cloud[0].x = 288; Cloud[0].y = 4;
			Cloud[1].x = 256; Cloud[1].y = 36;
					Cloud[2].x = 288; Cloud[2].y = 68;
			Cloud[3].x = 256; Cloud[3].y = 100;
					Cloud[4].x = 288; Cloud[4].y = 132;
			break;
		case VINV:
			Cloud[0].x = 256; Cloud[0].y = 4;
				Cloud[1].x = 288; Cloud[1].y = 36;
					Cloud[2].x = 320; Cloud[2].y = 68;
				Cloud[3].x = 288; Cloud[3].y = 100;
			Cloud[4].x = 256; Cloud[4].y = 132;
			break;
		case VSS:
				Cloud[0].x = 288; Cloud[0].yBase = 24; Cloud[0].movementPattern = SSINE;
			Cloud[1].x = 256; Cloud[1].yBase = 56; Cloud[1].movementPattern = SSINEINV;
				Cloud[2].x = 288; Cloud[2].yBase = 104; Cloud[2].movementPattern = SSINE;
			Cloud[3].enabled = 0;
			Cloud[4].enabled = 0;
			break;
		case VINVSS:
			Cloud[0].x = 256; Cloud[0].yBase = 24; Cloud[0].movementPattern = SSINE;
				Cloud[1].x = 288; Cloud[1].yBase = 56; Cloud[1].movementPattern = SSINEINV;
			Cloud[2].x = 256; Cloud[2].yBase = 104; Cloud[2].movementPattern = SSINE;
			Cloud[3].enabled = 0;
			Cloud[4].enabled = 0;
			break;
		case VSMS:
				Cloud[0].x = 288; Cloud[0].yBase = 24; Cloud[0].movementPattern = SSINE;
			Cloud[1].x = 256; Cloud[1].yBase = 56; Cloud[1].movementPattern = MSINE;
				Cloud[2].x = 288; Cloud[2].yBase = 104; Cloud[2].movementPattern = SSINE;
			Cloud[3].enabled = 0;
			Cloud[4].enabled = 0;
			break;
		case VINVSMS:
			Cloud[0].x = 256; Cloud[0].yBase = 24; Cloud[0].movementPattern = SSINE;
				Cloud[1].x = 288; Cloud[1].yBase = 56; Cloud[1].movementPattern = MSINE;
			Cloud[2].x = 256; Cloud[2].yBase = 104; Cloud[2].movementPattern = SSINE;
			Cloud[3].enabled = 0;
			Cloud[4].enabled = 0;
			break;
		case SMILEY:
				Cloud[0].x = 272; Cloud[0].y = 32;						Cloud[1].x = 336; Cloud[1].y = 32;
			Cloud[2].x = 256; Cloud[2].y = 96;								Cloud[3].x = 352; Cloud[3].y = 96;
											Cloud[4].x = 304; Cloud[4].y = 118;
			break;
		case PENTAGON:
										Cloud[0].x = 304; Cloud[0].y = 32;
			Cloud[1].x = 256; Cloud[1].y = 72;						Cloud[2].x = 352; Cloud[2].y = 72;
						Cloud[3].x = 280; Cloud[3].y = 118;	Cloud[4].x = 328; Cloud[4].y = 118;
			break;
		case SREG:
					Cloud[0].x = 288; Cloud[0].y = 4;
			Cloud[1].x = 256; Cloud[1].y = 36;
					Cloud[2].x = 288; Cloud[2].y = 68;  
						Cloud[3].x = 320; Cloud[3].y = 100;
					Cloud[4].x = 288; Cloud[4].y = 132;
			break;
		case SINV:
				Cloud[0].x = 288; Cloud[0].y = 4;
					Cloud[1].x = 320; Cloud[1].y = 36;
				Cloud[2].x = 288; Cloud[2].y = 68;  
			Cloud[3].x = 256; Cloud[3].y = 100;
					Cloud[4].x = 288; Cloud[4].y = 132;
			break;
		case SINEWAVEFIVE:
			for (i=0; i<5;i++){
				Cloud[i].x = i*32 + 256;
				Cloud[i].yBase = 68;
				Cloud[i].movementPattern = MSINE;
			}
			break;
		case ARROW:
				Cloud[1].x = 272; Cloud[1].y = 44;
			Cloud[2].x = 256; Cloud[2].y = 68;		Cloud[0].x = 305; Cloud[0].y = 32;		Cloud[4].x = 305; Cloud[4].y = 100;
				Cloud[3].x = 272; Cloud[3].y = 92;
			break;
		case WAVYARROW:
				Cloud[1].x = 272; Cloud[1].yBase = 44;
			Cloud[2].x = 256; Cloud[2].yBase = 68;		Cloud[0].x = 305; Cloud[0].yBase = 32;		Cloud[4].x = 305; Cloud[4].yBase = 100;
				Cloud[3].x = 272; Cloud[3].yBase = 92;
			for (i=0;i<5;i++) Cloud[i].movementPattern = SSINE;
			break;
		case TRIANGLE:
										Cloud[0].x = 320; Cloud[0].y = 32;
			Cloud[1].x = 256; Cloud[1].y = 132;					Cloud[2].x = 384; Cloud[2].y = 132;
			Cloud[3].enabled = 0; Cloud[4].enabled = 0;
			break;
		case MINIBOSSREG:
			for (i=0; i<5; i++){
				Cloud[i].x = 256; Cloud[i].y =68;
			}
			break;
		case MINIBOSSSSINE:
			for (i=0; i<5; i++){
				Cloud[i].x = 256; Cloud[i].yBase =68; Cloud[i].movementPattern = SSINE;
			}
			break;
		case MINIBOSSMSINE:
			for (i=0; i<5; i++){
				Cloud[i].x = 256; Cloud[i].yBase =68; Cloud[i].movementPattern = MSINE;
			}
			break;
		case AMORSE: //._
			Cloud[0].x = 256; Cloud[0].y = temp;
												Cloud[1].x = 304; Cloud[1].y = temp+32; Cloud[2].x = 342; Cloud[2].y = temp+32;
			Cloud[3].enabled = 0; Cloud[4].enabled = 0;
			break;
		case BMORSE: // _...
			Cloud[2].x = 336; Cloud[2].y = temp; Cloud[3].x = 384; Cloud[3].y = temp; Cloud[4].x = 432; Cloud[4].y = temp;
			Cloud[0].x = 256; Cloud[0].y = temp+32; Cloud[1].x = 288; Cloud[1].y = temp+32;
			break;
		case DMORSE: // _..
			Cloud[2].x = 336; Cloud[2].y = temp; Cloud[3].x = 384; Cloud[3].y = temp;
			Cloud[0].x = 256; Cloud[0].y = temp+32; Cloud[1].x = 288; Cloud[1].y = temp+32;
			Cloud[4].enabled = 0;
			break;
		case HMORSE: // ....
			Cloud[0].x = 256; Cloud[0].y = temp; Cloud[1].x = 304; Cloud[1].y = temp;
			Cloud[2].x = 352; Cloud[2].y = temp; Cloud[3].x = 400; Cloud[3].y = temp;
			Cloud[4].enabled = 0;
			break;
		case IMORSE: // ..
			Cloud[0].x = 256; Cloud[0].y = temp; Cloud[1].x = 304; Cloud[1].y = temp;
			Cloud[2].enabled = 0; Cloud[3].enabled = 0; Cloud[4].enabled = 0;
			break;
		case NMORSE: // _.
			Cloud[0].x = 256; Cloud[0].y = temp+32; Cloud[1].x = 288; Cloud[1].y = temp+32;
			Cloud[2].x = 336; Cloud[2].y = temp;
			Cloud[3].enabled = 0; Cloud[4].enabled = 0;
			break;
		case OMORSE: // _ _ _     [_ _. since only 5, which is G but close enough]
			Cloud[0].x = 256; Cloud[0].y = temp+32; Cloud[1].x = 288; Cloud[1].y = temp+32;
			Cloud[2].x = 336; Cloud[2].y = temp+32; Cloud[3].x = 368; Cloud[3].y = temp+32;
			Cloud[4].x = 416; Cloud[4].y = temp+32;
			break;
		case RMORSE: // ._.
			Cloud[0].x = 256; Cloud[0].y = temp;
			Cloud[1].x = 304; Cloud[1].y = temp+32; Cloud[2].x = 336; Cloud[2].y = temp+32;
			Cloud[3].x = 384; Cloud[3].y = temp;
			Cloud[4].enabled = 0;
			break;
		case SMORSE: // ...
			Cloud[0].x = 256; Cloud[0].y = temp; Cloud[1].x = 304; Cloud[1].y = temp;
			Cloud[2].x = 352; Cloud[2].y = temp;
			Cloud[3].enabled = 0;
			Cloud[4].enabled = 0;
			break;
		case WMORSE: // ._ _
			Cloud[0].x = 256; Cloud[0].y = temp;
			Cloud[1].x = 304; Cloud[1].y = temp+32; Cloud[2].x = 336; Cloud[2].y = temp+32;
			Cloud[3].x = 384; Cloud[3].y = temp+32; Cloud[4].x = 416; Cloud[4].y = temp+32;
			break;
		case SHIELD:
			Cloud[0].x = 256; Cloud[0].y = 40;
			Cloud[1].x = 256; Cloud[1].y = 72;
			Cloud[2].x = 256; Cloud[2].y = 104;
			Cloud[3].x = 304; Cloud[3].y = 56;
			Cloud[4].x = 304; Cloud[4].y = 88;
			break;
		case MPARA:
			Cloud[0].x = 256; Cloud[0].y = 56; Cloud[1].x = 288; Cloud[1].y = 32;
			Cloud[2].x = 336; Cloud[2].y = 114;
			Cloud[3].x = 384; Cloud[3].y = 32; Cloud[4].x = 416; Cloud[4].y = 56;
			break;
		case MPARAINV:
			Cloud[0].x = 256; Cloud[0].y = 114; Cloud[1].x = 288; Cloud[1].y = 82;
			Cloud[2].x = 336; Cloud[2].y = 32;
			Cloud[3].x = 384; Cloud[3].y = 114; Cloud[4].x = 416; Cloud[4].y = 82;
			break;
		case CHECK:
			Cloud[0].x = 256; Cloud[0].y = 95; Cloud[1].x = 288; Cloud[1].y = 127;
			Cloud[2].x = 320; Cloud[2].y = 95; Cloud[3].x = 352; Cloud[3].y = 63; Cloud[4].x = 384; Cloud[4].y = 31; 
			break;

		case BEAM1:
			Cloud[0].x = 256; Cloud[0].yBase = 68; Cloud[0].movementPattern = SSINE;
			Cloud[1].x = 304; Cloud[1].y = 68;
			Cloud[2].x = 352; Cloud[2].yBase = 68; Cloud[2].movementPattern = SSINE;
			Cloud[3].enabled = 0; Cloud[4].enabled = 0;
			break;
		case BEAM2:
			Cloud[0].x = 256; Cloud[0].yBase = 68; Cloud[0].movementPattern = SSINE;
			Cloud[1].x = 304; Cloud[1].yBase = 68; Cloud[1].movementPattern = MSINE;
			Cloud[2].x = 352; Cloud[2].yBase = 68; Cloud[2].movementPattern = SSINE;
			Cloud[3].enabled = 0; Cloud[4].enabled = 0;
			break;

		case SCATTER3SINE:
			scatterCoords2();
			for (i=0; i<3; i++){
				Cloud[i].x = Scatter2[i].x;
				Cloud[i].yBase = Scatter2[i].y;
				Cloud[i].movementPattern = rrange(2,1); // SSINE or MSINE
			}
			Cloud[3].enabled = 0; Cloud[4].enabled = 0;
			break;
		case SCATTER4SINE:
			scatterCoords2();
			for (i=0; i<4; i++){
				Cloud[i].x = Scatter2[i].x;
				Cloud[i].yBase = Scatter2[i].y;
				Cloud[i].movementPattern = rrange(2,1); // SSINE or MSINE
			}
			Cloud[4].enabled = 0;
			break;
		case SCATTER5SINE:
			scatterCoords2();
			for (i=0; i<5; i++){
				Cloud[i].x = Scatter2[i].x;
				Cloud[i].yBase = Scatter2[i].y;
				Cloud[i].movementPattern = rrange(2,1); // SSINE or MSINE
			}
			break;
		case SCATTER3:
			scatterCoords();
			for (i=0; i<3; i++){
				Cloud[i].x = Scatter[i].x;
				Cloud[i].y = Scatter[i].y;
			}
			Cloud[3].enabled = 0; Cloud[4].enabled = 0;
			break;
		case SCATTER4:
			scatterCoords();
			for (i=0; i<4; i++){
				Cloud[i].x = Scatter[i].x;
				Cloud[i].y = Scatter[i].y;
			}
			Cloud[4].enabled = 0;
			break;
		case SCATTER5:
			scatterCoords();
			for (i=0; i<5; i++){
				Cloud[i].x = Scatter[i].x;
				Cloud[i].y = Scatter[i].y;
			}
			break;
		default: break;
	}
}
void InitLightning(u32 pattern, u32 offset) {
	u32 pr[4] = {24, 24, 24, 24};
	u32 i;
	u32 o = offset;
	switch (pattern){
		case 1:	// single in middle of screen
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 72;
			break;
		case 2:	// one on top, one on bottom
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 31;
			Lightning[1].enabled = 1; Lightning[1].x = 256+o; Lightning[1].y = 128;
			break;
		case 3: // < formation
			Lightning[0].enabled = 1; Lightning[0].x = 320+o; Lightning[0].y = 104;
			Lightning[1].enabled = 1; Lightning[1].x = 256+o; Lightning[1].y = 72;
			Lightning[2].enabled = 1; Lightning[2].x = 320+o; Lightning[2].y = 40;
			break;
		case 4: //  /. formation
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 48;
			Lightning[1].enabled = 1; Lightning[1].x = 384+o; Lightning[1].y = 144;
			Lightning[2].enabled = 1; Lightning[2].x = 320+o; Lightning[2].y = 16;
			break;
		case 5: //  \' formation
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 108;
			Lightning[1].enabled = 1; Lightning[1].x = 320+o; Lightning[1].y = 140;
			Lightning[2].enabled = 1; Lightning[2].x = 384+o; Lightning[2].y = 24;
			break;
		case 6: // two in middle
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 56;
			Lightning[1].enabled = 1; Lightning[1].x = 256+o; Lightning[1].y = 88;
			break;
		case 7: // 1 top third
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 45;
			break;
		case 8: // 1 bottom third
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 99;
			break;	
		case 9: // double long in middle
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 72;
			Lightning[1].enabled = 1; Lightning[1].x = 296+o; Lightning[1].y = 72;
			break;
		case 10: // double long top third
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 45;
			Lightning[1].enabled = 1; Lightning[1].x = 296+o; Lightning[1].y = 45;
			break;
		case 11: // double long bottom third
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = 99;
			Lightning[1].enabled = 1; Lightning[1].x = 296+o; Lightning[1].y = 99;
			break;

		case 2880: // These next few are to sort of allow level designated patterns, with pattern being x and offset being y. pattern is 4 digits, first 3 are x, last is Lightning# [2880, is Lightning[0].x=288]
			Lightning[0].enabled = 1; Lightning[0].x = 288; Lightning[0].y = o;
			break;
		case 3280:
			Lightning[0].enabled = 1; Lightning[0].x = 328; Lightning[0].y = o;
			break;
		case 3281:
			Lightning[1].enabled = 1; Lightning[1].x = 328; Lightning[1].y = o;
			break;
		case 3282:
			Lightning[2].enabled = 1; Lightning[2].x = 328; Lightning[2].y = o;
		break;
		case 3681:
			Lightning[1].enabled = 1; Lightning[1].x = 368; Lightning[1].y = o;
			break;
		case 3682:
			Lightning[2].enabled = 1; Lightning[2].x = 368; Lightning[2].y = o;
			break;
		// Randomly generated patterns 
		case RANDOM1:
			Lightning[0].enabled = 1; Lightning[0].x = 256+o; Lightning[0].y = rrange(143, 16);
			break;
		case RANDOM2:
			for(i=0;i<2;i++){
				Lightning[i].enabled = 1; Lightning[i].x = 256+o; Lightning[i].y = rrange(143, 16);
				if (abs(Lightning[i].y - pr[i]) <= 16) {
					i--;
					continue;
				}
				pr[i+1] = Lightning[i].y;
			}
			break;
		case RANDOM3:
			for(i=0;i<3;i++){
				Lightning[i].enabled = 1; Lightning[i].x = 256+o; Lightning[i].y = rrange(143, 16);
				if (abs(Lightning[i].y - pr[i]) <= 16) {
					i--;
					continue;
				}
				if (i>0) {
					if (abs(Lightning[i].y - pr[i-1]) <= 16) {
					i--;
					continue;
					}
				}
				pr[i+1] = Lightning[i].y;
			}
			break;

		case BOSS:
			if (Lightning[0].enabled == 1 && Lightning[1].enabled == 1) {
				Lightning[2].enabled = 1; Lightning[2].x = Cloud[0].x; Lightning[2].y = Cloud[0].y+8;
			}
			else if (Lightning[0].enabled == 1) {
				Lightning[1].enabled = 1; Lightning[1].x = Cloud[0].x; Lightning[1].y = Cloud[0].y+8;
			} else {
				Lightning[0].enabled = 1; Lightning[0].x = Cloud[0].x; Lightning[0].y = Cloud[0].y+8;
			}
			break;

		case SCATTER2:
			scatterCoords();
			for (i=0; i<2; i++){
				Lightning[i].enabled = 1;
				Lightning[i].x = Scatter[i].x;
				Lightning[i].y = Scatter[i].y;
			}
			break;
		case SCATTER3:
			scatterCoords();
			for (i=0; i<3; i++){
				Lightning[i].enabled = 1;
				Lightning[i].x = Scatter[i].x;
				Lightning[i].y = Scatter[i].y;
			}
			break;

		default: 
			Lightning[0].enabled = 0; Lightning[1].enabled = 0; Lightning[2].enabled = 0;
			break;
	}
}

// health is # hits needed. bounce range 1-3. frequency is how often shoots lightning (in frames). speed is movement speed of boss 0-3
void InitBoss(u32 health, u32 bounce, u32 frequency, u32 speed) {
	ResetClouds();
	BossHealth = health; Bounciness = bounce; BossLightningFrequency = frequency; BossSpeed = speed;
	BossEnabled = 1; BLFCTR = 0;
	Cloud[0].x = 240; Cloud[0].y = 68;
}
void BossHandling(u32 level, OBJ_ATTR *ptr) {
	if(BossEnabled == 1) {
		// stuff
		if (BossHealth > 0) {
			Cloud[0].enabled = 1;
			// intro
			if (Cloud[0].x > 203) {
				Cloud[0].x--;
				Cloud[0].y = 68;
			}
			// movement
			switch (Bounciness) {
				case 2:
					if(Cloud[0].y <= 32 || Cloud[0].y >= 111) {
						BossSpeed = -BossSpeed;
					}
					break;
				case 3:
					if(Cloud[0].y <= 16 || Cloud[0].y >= 127) {
						BossSpeed = -BossSpeed;
					}
					break;
				default: // 1
					if(Cloud[0].y <= 48 || Cloud[0].y >= 95) {
						BossSpeed = -BossSpeed;
					}
					break;
			}
			Cloud[0].y += BossSpeed;
			// shooting lightning
			if (BLFCTR >= BossLightningFrequency){
				InitLightning(BOSS, 0);
				BLFCTR = 0;
			}
			BLFCTR++;
		}
		else if (BossHealth <= 0) {
			BossEnabled = 0;
			BLFCTR = 0;
			// destroy animation
			Cloud[0].enabled = 2;
			// level complete (unless playing endless mode)
			if (level != 0) { 
				obj_set_pos(ptr, 0, 210);// move Rainbowdash offscreen if level is over (I set up rainbowdash's pointer over a month ago, and it is different from all the others, so I just moved the sprite like this)
				LevelEndHandling(2, level);
			}
			else if (level == 0) {
				// play one of the boss defeated voicelines for endless mode
				u32 bloopadoop = rrange(2,1);
				if (bloopadoop == 1) {
					AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_ThatWasAwesome,
						AAS_DATA_SFX_END_ThatWasAwesome, AAS_NULL);
				}
				else if (bloopadoop == 2) {
					AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_Booyah3,
						AAS_DATA_SFX_END_Booyah3, AAS_NULL);
				}
			}
		}
	}
}
// REMEMBER TO MAKE LIVES DECREASE WHEN THEY GO OFFSCREEN // REMEMBER TO ADD MULTIPLE MOVEMENT TYPES (Have straight, want SINS, Bounce?, LIGHTslantedMOTION.....)

void CloudAnimation() {
	u32 i;
	for (i=0; i<5; i++){
		//animates when cloud is destroyed
		if (Cloud[i].enabled == 2) {
			Cloud[i].animCtr++;
			// animCtr is used to slow down the animation
			if (Cloud[i].animCtr >= 6) {
				Cloud[i].animCtr = 0;
				Cloud[i].tid += 32;
				// Disable cloud after destroy animation is played:  base tid is 320, there are 6 frames. 6*32 = 192. 320+192 = 512 (so when the tid is > 512, the animation is over)
				if (Cloud[i].tid >= 512) {
					Cloud[i].tid = 320;
					Cloud[i].x = 0; Cloud[i].y = 210;
					Cloud[i].ptr->attr2= ATTR2_BUILD(Cloud[i].tid, Cloud[i].pb, 0);
        			obj_set_pos(Cloud[i].ptr, Cloud[i].x, Cloud[i].y);
					Cloud[i].enabled = 0;
					Cloud[i].movementPattern = 0;
				}
			}
		}
		if (Cloud[i].enabled == 1) {
			// calculate cloud y based on movement pattern
			//skips over all the checks if the movement pattern is just default
			if (Cloud[i].movementPattern != 0) {
				Cloud[i].y = Cloud[i].yBase;
				if (Cloud[i].movementPattern == SSINE) {
					Cloud[i].y = 10*sin(0.05*Cloud[i].x) + Cloud[i].yBase;
				} else if (Cloud[i].movementPattern == MSINE) {
					Cloud[i].y = 30*sin(0.04*Cloud[i].x) + Cloud[i].yBase;
				} else if (Cloud[i].movementPattern == SSINEINV) {
					Cloud[i].y = 30*sin(0.05*Cloud[i].x+600) + Cloud[i].yBase;
				} else if (Cloud[i].movementPattern == MSINEINV) {
					Cloud[i].y = 30*sin(0.04*Cloud[i].x+600) + Cloud[i].yBase;
				}
			}
			if (BossEnabled != 1) {
				Cloud[i].x--; // moves cloud left (unless bosscloud, in which case its x coords don't change)
			}

			// When cloud goes offscreen
			if (Cloud[i].x <= -32) {
				Cloud[i].x = 0; Cloud[i].y = 210;
				Cloud[i].ptr->attr2= ATTR2_BUILD(Cloud[i].tid, Cloud[i].pb, 0);
        		obj_set_pos(Cloud[i].ptr, Cloud[i].x, Cloud[i].y);
				Cloud[i].enabled = 0;
				Cloud[i].movementPattern = 0;
				// DECREASE LIVES
				//LifeNum.animCtr--;
			}
		}
		// updates cloud position and stuff if the cloud is enabled
		if (Cloud[i].enabled != 0) {
			Cloud[i].ptr->attr2= ATTR2_BUILD(Cloud[i].tid, Cloud[i].pb, 0);
        	obj_set_pos(Cloud[i].ptr, Cloud[i].x, Cloud[i].y);
		}
	}
}
void CounterAnimation() {
	u32 i;
	for (i=0;i<4;i++) CloudNum[i].ptr->attr2= ATTR2_BUILD((CloudNum[i].tid + CloudNum[i].animCtr * 4), CloudNum[i].pb, 0);
	LifeNum.ptr->attr2= ATTR2_BUILD(LifeNum.tid + LifeNum.animCtr * 4, LifeNum.pb, 0);

}

void LevelHandling(u32 level) {

	
	// initialize phase
	if (PhaseActive == 1) {
		PhaseDelay = 0;
		PhaseActive = 2;
		//--------------------------- endless ---------------------------------------
		if (level == 0) {
			// Normal random clouds 
			if (PhaseCounter < 20) {
				u32 probability = rrange(100,0);
				u32 probability2 = rrange(100,0);
				u32 offset = rrange(80, 20);
				// Choose a random lightning pattern 
				if (probability2 >=60) InitLightning(rrange(11, 1), offset); // 40% chance for predefined pattern
				else if (probability2 >= 33) InitLightning(rrange(RANDOM3, RANDOM1), offset); // 27% chance for random
				else if (probability2 >= 6) InitLightning(rrange(SCATTER3, SCATTER2), offset); // 27% chance for scatter
				else if (probability2 >= 0) InitLightning(0,0); // 6% chance for no lightning
				// Choosing a random cloud pattern
				if (probability >= 50) InitPattern(rrange(32, 1)); // 50% chance for predefined pattern
				else if (probability >= 10) InitPattern(rrange(59, 54)); // 40% chance for scatter pattern
				else if (probability >= 0) InitPattern(rrange(69, 60)); // 10% chance for morse pattern
			}
			else { // Randomized Boss encounter when PhaseCounter gets high enough
				PhaseCounter = 0; // resets PhaseCounter so it goes back to regular patterns
				u32 tempHealth = rrange(8,2)*5; // random boss health from 10, 15, 20....., 40
				u32 tempBounce = rrange(3,1);
				u32 tempFreq = rrange(240,60); // random shooting frequency from 60 - 240  frames (i.e. 1-4 seconds)
				u32 tempSpeed = rrange(0,3); // random speed from 0 (not moving) to 3 pixelsPerFrame
				InitBoss(tempHealth, tempBounce, tempFreq, tempSpeed);
			}
		}
		//------------------------------------------------------------------------------
		// --------------------------------- Level 1 ----------------------------------------
		else if (level == 1) {
			switch (PhaseCounter) {
				case 0:
					InitPattern(V);
				break;
				case 1:
					InitPattern(SQUARE); InitLightning(1, 30);
				break;
				case 2:
					InitPattern(TRIANGLE);
				break;
				case 3:
					InitPattern(CHECK); InitLightning(2880, 56); InitLightning(3281, 32);
				break;
				case 4: 
					InitPattern(SMILEY); InitLightning(1, 30);
				break;
				case 5:
					InitPattern(SQUARE); InitLightning(2, 30);
				break;
				case 6:
					InitPattern(LSLANT);
				break;
				case 7:
					InitPattern(RSLANT);
				break;
				case 8:
					InitPattern(V); InitLightning(1, 30);
				break;
				case 9:
					InitPattern(DIAMOND); InitLightning(1, 30);
				break;
				case 10:
					InitPattern(VINV); InitLightning(1, 30);
				break;
				case 11:
					InitPattern(SMILEY); InitLightning(2, 30);
				break;
				case 12:
					InitPattern(TRIANGLE); InitLightning(6, 30);
				break;
				case 13:
					InitPattern(LREG); InitLightning(8, 30);
				break;
				case 14:
					InitPattern(LINV); InitLightning(7, 30);
				break;
				case 15:
					InitPattern(V);
				break;
				case 16:
					InitPattern(LSLANT); InitLightning(2, 30);
				break;
				case 17:
					InitPattern(SQUARE); InitLightning(1, 30);
				break;
				case 18:
					InitPattern(RSLANT); InitLightning(2, 30);
				break;
				case 19:
					InitBoss(15, 2, 210, 1);
				break;
				default:
				break;
			}
		}
		//------------------------------------------------------------------------------------
		// -------------- Level 2 ----------------------
		else if(level == 2){
			switch (PhaseCounter) {
				case 0:
					InitPattern(V);
				break;
				case 1:
					InitPattern(RMORSE); InitLightning(1, 10);
				break;
				case 2:
					InitPattern(AMORSE); InitLightning(7, 10);
				break;
				case 3:
					InitPattern(IMORSE); InitLightning(1, 10);
				break;
				case 4:
					InitPattern(NMORSE); InitLightning(8, 10);
				break;
				case 5:
					InitPattern(BMORSE); InitLightning(1,10);
				break;
				case 6:
					InitPattern(OMORSE); InitLightning(7,10);
				break;
				case 7:
					InitPattern(WMORSE); InitLightning(1,10);
				break;
				case 8:
					InitPattern(DMORSE); InitLightning(8, 10);
				break;
				case 9:
					InitPattern(AMORSE); InitLightning(1, 10);
				break;
				case 10:
					InitPattern(SMORSE); InitLightning(7,10);
				break;
				case 11:
					InitPattern(HMORSE); InitLightning(1, 10);
				break;
				case 12:
					InitPattern(VINV);
				break;
				case 13:
					InitPattern(TWOSINES); InitLightning(6, 40);
				break;
				case 14:
					InitPattern(SQUARE); InitLightning(2, 40);
				break;
				case 15:
					InitPattern(MINIBOSSREG); InitLightning(1, 10);
				break;
				case 16:
					InitPattern(SHIELD); InitLightning(9, 20);
				break;
				case 17:
					InitPattern(SINEWAVEFIVE); InitLightning(2, 30);
				break;
				case 18:
					InitPattern(CHECK); InitLightning(8, 30);
				break;
				case 19:
					InitPattern(MPARA); InitLightning(9, 20);
				break;
				case 20:
					InitPattern(SMILEY); InitLightning(1, 30);
				break;
				case 21:
					InitPattern(BEAM1); InitLightning(2, 20);
				break;
				case 22:
					InitPattern(SREG);
				break;
				case 23:
					InitPattern(ARROW); InitLightning(3, 0);
				break;
				case 24:
					InitPattern(PENTAGON); InitLightning(5, 30);
				break;
				case 25:
					InitPattern(TWOSINES); InitLightning(7, 30);
				break;
				case 26:
					InitPattern(TWOSINES); InitLightning(8, 30);
				break;
				case 27:
					InitPattern(VSS); InitLightning(2, 30);
				break;
				case 28:
					InitPattern(DIAMOND); InitLightning(6,0);
				break;
				case 29:
					InitBoss(25, 2, 168, 1);
				break;
				default:
				break;
			}
		}
		//----------------------------------------------
		//--------------------------Level 3----------------------
		else if(level == 3) {
			switch(PhaseCounter) {
				case 0:
					InitPattern(SINEWAVEFIVE);
				break;
				case 1:
					InitPattern(BEAM2); InitLightning(3, 20);
				break;
				case 2:
					InitPattern(V); InitLightning(6, 30);
				break;
				case 3:
					InitPattern(TRIANGLE); InitLightning(2, 30);
				break;
				case 4:
					InitPattern(VSS); InitLightning(9, 20);
				break;
				case 5:
					InitPattern(SCATTER4); InitLightning(RANDOM2, 30);
				break;
				case 6:
					InitPattern(SCATTER3); InitLightning(RANDOM2, 30);
				break;
				case 7:
					InitPattern(SCATTER4); InitLightning(RANDOM2, 30);
				break;
				case 8:
					InitPattern(SCATTER3); InitLightning(RANDOM2, 30);
				break;
				case 9:
					InitPattern(SIDEWAYSW); InitLightning(3, 30);
				break;
				case 10:
					InitPattern(MINIBOSSREG); InitLightning(1, 30);
				break;
				case 11:
					InitPattern(WAVYARROW); InitLightning(2, 30);
				break;
				case 12:
					InitPattern(BEAM2); InitLightning(1, 30);
				break;
				case 13:
					InitPattern(SCATTER3); InitLightning(9, 20);
				break;
				case 14:
					InitPattern(VINVSS); InitLightning(3, 10);
				break;
				case 15:
					InitPattern(SIDEWAYSM); InitLightning(6, 30);
				break;
				case 16:
					InitPattern(V); InitLightning(3682, 72); InitLightning(3281, 40); InitLightning(3280, 104);
				break;
				case 17:
					InitPattern(VINV); InitLightning(3, 20);
				break;
				case 18:
					InitPattern(SINEWAVEFIVE); InitLightning(1, 30);
				break;
				case 19:
					InitPattern(MINIBOSSSSINE); InitLightning(1, 30);
				break;
				case 20:
					InitPattern(SINV); InitLightning(2, 20);
				break;
				case 21:
					InitPattern(SCATTER4SINE); InitLightning(RANDOM2, 30);
				break;
				case 22:
					InitPattern(SCATTER3SINE); InitLightning(RANDOM2, 30);
				break;
				case 23:
					InitPattern(SCATTER4SINE); InitLightning(RANDOM2, 30);
				break;
				case 24:
					InitPattern(SCATTER3SINE); InitLightning(RANDOM2, 30);
				break;
				case 25:
					InitPattern(PENTAGON); InitLightning(6, 20);
				break;
				case 26:
					InitPattern(CHECK); InitLightning(10, 40); InitLightning(3682, 45);
				break;
				case 27:
					InitPattern(RMORSE); InitLightning(4, 10);
				break;
				case 28:
					InitPattern(SMILEY); InitLightning(9, 30);
				break;
				case 29:
					InitPattern(MINIBOSSMSINE); InitLightning(1, 30);
				break;
				case 30:
					InitPattern(VSS); InitLightning(6, 20);
				break;
				case 31:
					InitPattern(BEAM1); InitLightning(1, 10);
				break;
				case 32:
					InitPattern(TWOSINES); InitLightning(6, 30);
				break;
				case 33:
					InitPattern(DIAMOND); InitLightning(1, 60);
				break;
				case 34:
					InitPattern(SREG);
				break;
				case 35:
					InitPattern(MPARA); InitLightning(7, 30);
				break;
				case 36:
					InitPattern(MPARAINV); InitLightning(8, 30);
				break;
				case 37:
					InitPattern(SCATTER5); InitLightning(RANDOM1, 20);
				break;
				case 38:
					InitPattern(SCATTER5SINE); InitLightning(RANDOM1, 20);
				break;
				case 39:
					InitBoss(35, 1, 180, 1);
				break;
				default:
				break;
			}
		}
		//-------------------------------------------------------
		//----------- Level 4 --------------

		//----------------------------------
		// ------------------------------------- Level 5 -------------------------------------
		else if(level == 5) {
			switch(PhaseCounter) {
				case 0:
					InitPattern(SINEWAVEFIVE);
				break;
				case 1:
					InitPattern(TWOSINES); InitLightning(2, 50); InitLightning(3682, 0);
				break;
				case 2:
					InitPattern(V); InitLightning(3, 30);
				break;
				case 3:
					InitPattern(SCATTER5SINE); InitLightning(2880, 72); InitLightning(3281, 72); InitLightning(3682, 72);
				break;
				case 4:
					InitPattern(RSLANT); InitLightning(5, 40);
				break;
				case 5:
					InitPattern(LSLANT); InitLightning(4, 40);
				break;
				case 6:
					InitPattern(SHIELD); InitLightning(3, 40);
				break;
				case 7:
					InitPattern(CHECK); InitLightning(2880, 45); InitLightning(3281, 90); InitLightning(3682, 135);
				break;
				case 8:
					InitPattern(VINV); InitLightning(3280, 37); InitLightning(3281, 107); InitLightning(3682, 72);
				break;
				case 9:
					InitPattern(BEAM2); InitLightning(3280, 19); InitLightning(3281, 61); InitLightning(3282, 132);
				break;
				case 10:
					InitPattern(BEAM2); InitLightning(3280, 124); InitLightning(3281, 82); InitLightning(3282, 11);
				break;
				default:
				break;
			}
		}
		//------------------------------------------------------------------------------------
	}
	if (PhaseActive == 0) {
		PhaseDelay++;
		// wait one second before starting next phase
		if(PhaseDelay >= 60) {
			PhaseActive = 1;
			PhaseCounter++;
		}
	}

	// phase is not active if all clouds are disabled (i.e. phase ends when all clouds are destroyed/go offscreen)
	if (Cloud[0].enabled == 0 && Cloud[1].enabled == 0 && Cloud[2].enabled == 0 && Cloud[3].enabled == 0 && Cloud[4].enabled == 0 && BossEnabled == 0 && PhaseActive == 2) {
		PhaseActive = 0;
		PhaseDelay = 0;
	}

}
void KonamiCode() {
	if (KCode == 0 && key_hit(KEY_UP)) KCode++;
	else if (KCode == 1 && key_hit(KEY_UP)) KCode++;
	else if (KCode == 2 && key_hit(KEY_DOWN)) KCode++;
	else if (KCode == 3 && key_hit(KEY_DOWN)) KCode++;
	else if (KCode == 4 && key_hit(KEY_LEFT)) KCode++;
	else if (KCode == 5 && key_hit(KEY_RIGHT)) KCode++;
	else if (KCode == 6 && key_hit(KEY_LEFT)) KCode++;
	else if (KCode == 7 && key_hit(KEY_RIGHT)) KCode++;
	else if (KCode == 8 && key_hit(KEY_B)) KCode++;
	else if (KCode == 9 && key_hit(KEY_A)) KCode++;
	else if (KCode == 10 && key_hit(KEY_START)) {
		u32 bloopadoop = rrange (3,1);
		if (bloopadoop == 1) {
			AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_NotKonamiGame,
					AAS_DATA_SFX_END_NotKonamiGame, AAS_NULL);
		}
		else if (bloopadoop == 2) {
			AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_BeepBopBoop,
					AAS_DATA_SFX_END_BeepBopBoop, AAS_NULL);
		}
		else if (bloopadoop == 3) {
			AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_SuperExtra,
					AAS_DATA_SFX_END_SuperExtra, AAS_NULL);
		}
	}
	else if (key_hit(KEY_ANY)) KCode = 0;
}

void gameloop(u32 level) {

	// Load in the game sprites (replacing Title sprites in tile memory)
	GameSpriteLoad();

	// Set balloon as background0			(replaces text from the main menu)
	tonccpy(&tile_mem[0][0], Balloon2Tiles, Balloon2TilesLen);
    tonccpy(&se_mem[22][0], Balloon2Map, Balloon2MapLen);
	REG_BG0CNT= BG_CBB(0) | BG_SBB(22) | BG_8BPP | BG_REG_64x32;
	pal_bg_bank[0][15]= 0x6236; //text in menu messes up this palette entry, so I manually set it to what it should be here (the purple color used for spike), also faster than recopying the entire palette
	//reset BG0 position (when coming back from menu)
	REG_BG0HOFS= BGballoon.x;
	REG_BG0VOFS = 20*sin(0.07*BGballoon.x) + 40 + RD.y/3;

	// Rainbowdash animation related variables
	u32 AnimSlower = 0, BlinkCounter = 0, RandBlinkInterval = 9;
	u32 i; //reusable loop variable
	score = 0; // reset score at beginning of level

// Sprite setup
#if 1

	// Setup sprite "objects" (not OOP objects) ---------------------------------------------------------------
	// Rainbowdash
	RD.x = 96; RD.y = 32;
    OBJ_ATTR *RainbowdashFB= &obj_buffer[1];
    obj_set_attr(RainbowdashFB, 
        0x2000,              				// 0x2000 means 256 color mode (single palette) [same as ATTR0_8BPP]
        ATTR1_SIZE_32,              		// 32x32p, 
        ATTR2_PALBANK(RD.pb | RD.tid) );   	// palbank 0, tile 0

	obj_set_attr(crash.ptr,
		0x2000,
		ATTR1_SIZE_32,              		// 32x32p, 
        ATTR2_PALBANK(RD.pb | RD.tid) );
	crash.ptr->attr2= ATTR2_BUILD(crash.tid, crash.pb, 0);
	obj_set_pos(crash.ptr, 0, 210);

	// ------------ Shot sprites -------------------------------------
	// set attributes for Shot[0-2]
	for (i=0;i<3;i++) {
		obj_set_attr(Shot[i].ptr,
			ATTR0_WIDE | ATTR0_8BPP,
			ATTR1_SIZE_32x16,
			ATTR2_PALBANK(Shot[i].pb | Shot[i].tid) );

		obj_set_pos(Shot[i].ptr, Shot[i].x, Shot[i].y);
		Shot[i].ptr->attr2= ATTR2_BUILD(Shot[i].tid, Shot[i].pb, 0);

	}
		// 0x6100 makes hidden. (maybe 0x6200, gotta check whether 100 or 200 is hidden flag)

	// ----------------------------------------------------------------

	// ------------------ electrocution effect ------------------------------
	obj_set_attr(Shock.ptr,
		ATTR0_8BPP,
		ATTR1_SIZE_32,
		ATTR2_PALBANK(Shock.pb | Shock.tid) );
	obj_set_pos(Shock.ptr, Shock.x, Shock.y);
	Shock.ptr->attr2= ATTR2_BUILD(Shock.tid, Shock.pb, 0);
	//-----------------------------------------------------------------------

	// --------------- Cloud Sprites -----------------------------------
	// set attributes for Cloud[0-4]
	for (i=0;i<5;i++) {
		obj_set_attr(Cloud[i].ptr,
			ATTR0_8BPP,
			ATTR1_SIZE_32x32,
			ATTR2_PALBANK(Cloud[i].pb | Cloud[i].tid) );

		obj_set_pos(Cloud[i].ptr, Cloud[i].x, Cloud[i].y);
		Cloud[i].ptr->attr2= ATTR2_BUILD(Cloud[i].tid, Cloud[i].pb, 0);
	}
	// -----------------------------------------------------------------

	// ----------------- Icon Sprites ----------------------------
	obj_set_attr(LifeIcon.ptr,
			ATTR0_8BPP,
			ATTR1_SIZE_16x16,
			ATTR2_PALBANK(LifeIcon.tid) );
	obj_set_pos(LifeIcon.ptr, LifeIcon.x, LifeIcon.y);
	LifeIcon.ptr->attr2= ATTR2_BUILD(LifeIcon.tid, LifeIcon.pb, 0);		

	obj_set_attr(CloudIcon.ptr,
			ATTR0_8BPP,
			ATTR1_SIZE_16x16,
			ATTR2_PALBANK(CloudIcon.tid) );
	obj_set_pos(CloudIcon.ptr, CloudIcon.x, CloudIcon.y);
	CloudIcon.ptr->attr2= ATTR2_BUILD(CloudIcon.tid, CloudIcon.pb, 0);	
	//------------------------------------------------------------

	// ---------------- Number Sprites -----------------------------
	obj_set_attr(LifeNum.ptr,
		ATTR0_TALL | ATTR0_8BPP,
		ATTR1_SIZE_8x16,
		ATTR2_PALBANK(LifeNum.tid) );
	obj_set_pos(LifeNum.ptr, LifeNum.x, LifeNum.y);
	LifeNum.ptr->attr2= ATTR2_BUILD(LifeNum.tid, LifeNum.pb, 0);	

	for (i=0;i<4;i++) {
		obj_set_attr(CloudNum[i].ptr,
			ATTR0_TALL | ATTR0_8BPP,
			ATTR1_SIZE_8x16,
			ATTR2_PALBANK(CloudNum[i].pb | CloudNum[i].tid) );

		obj_set_pos(CloudNum[i].ptr, CloudNum[i].x, CloudNum[i].y);
		CloudNum[i].ptr->attr2= ATTR2_BUILD(CloudNum[i].tid, CloudNum[i].pb, 0);
	}
	// --------------------------------------------------------------

	// ----------------- Lightning Sprites -------------------------
	for (i=0;i<3;i++) {
		obj_set_attr(Lightning[i].ptr,
			ATTR0_WIDE | ATTR0_8BPP,
			ATTR1_SIZE_32x16,
			ATTR2_PALBANK(Lightning[i].pb | Lightning[i].tid) );

		obj_set_pos(Lightning[i].ptr, Lightning[i].x, Lightning[i].y);
		Lightning[i].ptr->attr2= ATTR2_BUILD(Lightning[i].tid, Lightning[i].pb, 0);

	}

	
	//---------------------------------------------------------------------------------------------------------
#endif

    // (4) position sprite (redundant here; the _real_ position
    // is set further down
    obj_set_pos(RainbowdashFB, RD.x, RD.y);

	// Set lives at start of round, and set all cloud destroyed counter digits to zero
	LifeNum.animCtr = 3;
	for (i=0;i<4;i++) CloudNum[i].animCtr = 0;

	PhaseCounter = 0; // used to indicate what phase current level is on.
	PhaseDelay = 0;
	PhaseActive = 1; // enable first phase
	u32 levelstate = 1; // 1: active. 2: gameover level. 3: level complete. 4: endless complete.
	BossEnabled = 0;

	// TEMPORARY TO TEST PATTERNS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//InitPattern(TWOSINES);
	//InitBoss(20, 3, 240, 2);
	MainMenuBreak = 0;

    while(1)
    {
		VBlankIntrWait();
        key_poll();

		//if(key_hit(KEY_R)) break; //temp main menu reset, MAKE SURE TO MOVE ALL SPRITES OFFSCREEN (and disable) BEFORE RETURNING TO MENU
		//if(key_hit(KEY_L)) InitPattern(rrange(32,31)); // temp spawn more clouds of random pattern
		//if(key_hit(KEY_B)) InitLightning(rrange(779,777), 50);

		if (MainMenuBreak == 1) break;

		//decrement iframes if it is active
		if (iframes > 0) iframes--;

		BackgroundHandling();

		InputHandling();

		BossHandling(level, RainbowdashFB);
		ProjectileHandling();

		LightningHandling();

		CloudAnimation();

		CounterAnimation();

		LevelHandling(level);

		#if 1
		if (LifeNum.animCtr <= 0) {
			crash.x = RD.x; crash.y = RD.y;
			obj_set_pos(RainbowdashFB, 0, 210);// move Rainbowdash offscreen if level is over
			obj_set_pos(crash.ptr, crash.x, crash.y);
			if (level == 0) levelstate = 4;
			else if (level != 0) levelstate = 3;
			// play one of the defeat voicelines
			u32 bloopadoop = rrange(3,1);
			if (bloopadoop == 1) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_Drats,
					AAS_DATA_SFX_END_Drats, AAS_NULL);
			}
			else if (bloopadoop == 2) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_aagh,
					AAS_DATA_SFX_END_aagh, AAS_NULL);
			}
			else if (bloopadoop == 3) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_DangIt,
					AAS_DATA_SFX_END_DangIt, AAS_NULL);
			}
		}
		#endif
		// Call level end handling
		LevelEndHandling(levelstate, level);
		

        // ---------------------------------------------Rainbowdash Animation -------------------------------------------
		// Only update animation every nth frame
		// 1st part (if) is regular animation cycle (4 frames flying, eyes open), the 2nd part (else if) goes through blink animation once (first 4 regular frames, them 5 frames with blink), 
		// then resets blink counter, randomizes interval till next blink, and continues regular flying animation cycle
		if (AnimSlower == 6 && BlinkCounter != RandBlinkInterval) {
			//Flying animation
			RD.tid = (RD.tid+32) % 128; 	// +32 means displays next frame, the %128 is so it loops on the 4th frame (32*4 = 128)
			AnimSlower = 0;
			if (RD.tid == 0) {BlinkCounter++;}
		} else if (AnimSlower == 6 && BlinkCounter == RandBlinkInterval) {
			RD.tid = (RD.tid+32) % 288;
			AnimSlower = 0;
			if (RD.tid == 0) {
				BlinkCounter=0; 
				RandBlinkInterval = rrange(20, 7);
				RD.tid = 32; // blink is 5 frames (last frame of blink has wings up like first frame of regular), so we start at the second frame (tid = 32) of the normal flying animation so the wings aren't up 2 animation frames in a row
				}
		}
		AnimSlower++;
		// ---------------------------------------------------------------------------------------------------------------

		// --------------------Electrocution Animation------------------
		if (Shock.enabled == 1){
			if (Shock.animCtr >= animDelayShock) {
				Shock.tid = (Shock.tid+32);
				Shock.x = RD.x; Shock.y = RD.y;
				if (Shock.tid == 896) {
					Shock.tid = 768;
					Shock.enabled = 0;
					Shock.x = 0;
					Shock.y = 170;
				}
				Shock.animCtr = 0;
				Shock.ptr->attr2= ATTR2_BUILD(Shock.tid, Shock.pb, 0);
			}
			obj_set_pos(Shock.ptr, Shock.x, Shock.y);
			Shock.animCtr++;
		}
		//--------------------------------------------------------------

		KonamiCode();

        // updates Rainbowdash's position and frame
        RainbowdashFB->attr2= ATTR2_BUILD(RD.tid, RD.pb, 0);
        obj_set_pos(RainbowdashFB, RD.x, RD.y);

        oam_copy(oam_mem, obj_buffer, 128);   // (6) Update OAM (effectively draw sprites 0-127 [all possible sprites])

		// PAUSE
		if (key_hit(KEY_START)) {
			while (1) {
				VBlankIntrWait();
       			key_poll();
				if (key_hit(KEY_START)) break;
			}
		}

    }
}

void ResetGameSprites() {
	//reset projectiles
	u32 i;
	for (i=0;i<3;i++) {
		Shot[i].enabled = 0;
		Shot[i].x = 0;
		Shot[i].y = 170;
		obj_set_pos(Shot[i].ptr, Shot[i].x, Shot[i].y);
	}

	//reset counters
	for (i=0;i<4;i++) {
		obj_set_pos(CloudNum[i].ptr, 0, 170);
	}
	obj_set_pos(LifeNum.ptr, 0, 170);

	//reset level end text
	for(i=0;i<3;i++) obj_set_pos(L[i].ptr, 0, 170);
	for(i=0;i<4;i++) obj_set_pos(E[i].ptr, 0, 170);
	obj_set_pos(VT.ptr, VT.x, VT.y);
	obj_set_pos(T.ptr, T.x, T.y);
	obj_set_pos(N.ptr, N.x, N.y);
	obj_set_pos(D.ptr, D.x, D.y);
	obj_set_pos(S.ptr, S.x, S.y);
	obj_set_pos(S2.ptr, S2.x, S2.y);
	obj_set_pos(CO.ptr, CO.x, CO.y);
	obj_set_pos(MP.ptr, MP.x, MP.y);
	obj_set_pos(AI.ptr, AI.x, AI.y);

}
void GameSpriteLoad() {
	//Initialize RainbowdashFB sprite
	tonccpy(&tile_mem[4][0], RainbowdashFBTiles, RainbowdashFBTilesLen);

	//Initialize other sprites
	// ---
	tonccpy(&tile_mem[4][288], shotTiles, shotTilesLen); // Projectile sprites
	tonccpy(&tile_mem[4][320], RainCloudTiles, RainCloudTilesLen); // cloud sprites
	tonccpy(&tile_mem[5][32], IconsTiles, IconsTilesLen); // Icon sprites
	tonccpy(&tile_mem[5][64], NumbersTiles, NumbersTilesLen); // sprites for numbers 0-9
	tonccpy(&tile_mem[5][224], lightningTiles, lightningTilesLen); // lightning bolt 736
	tonccpy(&tile_mem[5][256], ShockTiles, ShockTilesLen); // electrocution effect 768
}
void TitleSpriteLoad() {
	//tonccpy(&tile_mem[4][0], Title1Tiles, Title1TilesLen); // load first part of title sprites (3 64x64px)
	//tonccpy(&tile_mem[4][384], Title2Tiles, Title2TilesLen); // second part of title sprites (1 32x32px)
	tonccpy(&tile_mem[4][0], TitleV2Tiles, TitleV2TilesLen);
	tonccpy(&tile_mem[5][288], barTiles, barTilesLen);
	
	// Build Title1A
	obj_set_attr(Title1A.ptr,
			ATTR0_8BPP,
			ATTR1_SIZE_64x64,
			ATTR2_PALBANK(Title1A.pb | Title1A.tid) );
	Title1A.ptr->attr2= ATTR2_BUILD(Title1A.tid, Title1A.pb, 0);
	obj_set_pos(Title1A.ptr, Title1A.x, Title1A.y);
	// Build Title1B
	obj_set_attr(Title1B.ptr,
		ATTR0_8BPP,
		ATTR1_SIZE_64x64,
		ATTR2_PALBANK(Title1B.pb | Title1B.tid) );
	Title1B.ptr->attr2= ATTR2_BUILD(Title1B.tid, Title1B.pb, 0);
	obj_set_pos(Title1B.ptr, Title1B.x, Title1B.y);
	// Build Title1C
	obj_set_attr(Title1C.ptr,
		ATTR0_8BPP,
		ATTR1_SIZE_64x64,
		ATTR2_PALBANK(Title1C.pb | Title1C.tid) );
	Title1C.ptr->attr2= ATTR2_BUILD(Title1C.tid, Title1C.pb, 0);
	obj_set_pos(Title1C.ptr, Title1C.x, Title1C.y);
	// Build Title1D
	obj_set_attr(Title1D.ptr,
		ATTR0_8BPP,
		ATTR1_SIZE_64x64,
		ATTR2_PALBANK(Title1D.pb | Title1D.tid) );
	Title1D.ptr->attr2= ATTR2_BUILD(Title1D.tid, Title1D.pb, 0);
	obj_set_pos(Title1D.ptr, Title1D.x, Title1D.y);
	//Build Title2A
	obj_set_attr(Title2A.ptr,
		ATTR0_8BPP,
		ATTR1_SIZE_64x64,
		ATTR2_PALBANK(Title2A.pb | Title2A.tid) );
	Title2A.ptr->attr2= ATTR2_BUILD(Title2A.tid, Title2A.pb, 0);
	obj_set_pos(Title2A.ptr, Title2A.x, Title2A.y);
	//Build Title2B
	obj_set_attr(Title2B.ptr,
		ATTR0_8BPP,
		ATTR1_SIZE_64x64,
		ATTR2_PALBANK(Title2B.pb | Title2B.tid) );
	Title2B.ptr->attr2= ATTR2_BUILD(Title2B.tid, Title2B.pb, 0);
	obj_set_pos(Title2B.ptr, Title2B.x, Title2B.y);

	//Build credit bars
	u32 i;
	for(i=0;i<8;i++) {
		obj_set_attr(bar[i].ptr,
			ATTR0_WIDE | ATTR0_8BPP,
			ATTR1_SIZE_64x32,
			ATTR2_PALBANK(bar[i].pb | bar[i].tid));
		bar[i].ptr->attr2= ATTR2_BUILD(bar[i].tid, bar[i].pb, 0);
		obj_set_pos(bar[i].ptr, 0, 170);
	}
	
}

int main() {
	//-------------------------- INITIAL SETUP --------------------------------


	// load palette for all sprites (since I don't use more than 256 unique colors for all sprites, they all use the same palette, which I just load once at the beginning)
	//tonccpy(pal_obj_mem, RainCloudPal, RainCloudPalLen); // sprite palette (raincloud was last added sprite, so has most complete palette)
	tonccpy(pal_obj_mem, lightningPal, lightningPalLen); // lightning palette is most complete palette, shared by all sprites

	// INITIALIZE ALL SPRITES
	oam_init(obj_buffer, 128);

	// ------------------------------------------------------------------------------------------------

	// Initialize backgrounds
	// Furthest Background
	tonccpy(&tile_mem[3][0], BGback2Tiles, BGback2TilesLen);
    tonccpy(&se_mem[30][0], BGback2Map, BGback2MapLen);
	REG_BG3CNT= BG_CBB(3) | BG_SBB(30) | BG_8BPP | BG_REG_64x32;

	// Mid background
	tonccpy(&tile_mem[1][0], BGmid2Tiles, BGmid2TilesLen);
    tonccpy(&se_mem[27][0], BGmid2Map, BGmid2MapLen);
	REG_BG2CNT= BG_CBB(1) | BG_SBB(27) | BG_8BPP | BG_REG_64x32;

	// Front (cloud) background
	//tonccpy(pal_bg_mem,BGfront3Pal,BGfront3PalLen); // Will get replaced with bg palette of the balloon, which will have palette info for everything
	tonccpy(&tile_mem[2][0], BGfront3Tiles, BGfront3TilesLen);
    tonccpy(&se_mem[25][0], BGfront3Map, BGfront3MapLen);
	REG_BG1CNT= BG_CBB(2) | BG_SBB(25) | BG_8BPP | BG_REG_64x32;

	// Balloon moved to gameloop, since bg0 is used for text in the menu
	tonccpy(pal_bg_mem,Balloon2Pal,Balloon2PalLen); // sets palette for backgrounds
	
	//Set screenmode to 0 [regular sprite]. Set background to background
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D;
	
	// Initialise AAS
	AAS_SetConfig(AAS_CONFIG_MIX_32KHZ, AAS_CONFIG_CHANS_8,
		AAS_CONFIG_SPATIAL_MONO, AAS_CONFIG_DYNAMIC_OFF);

	// Initializes interrupts, very helpful
	irq_init(NULL);
	irq_enable(II_VBLANK);	// Enables VBlank interrupt, that means I can call vsync with VBlankIntrWait();   [i.e. best method of vsync]
	// set timer 1 to AAS_Timer1InterruptHandler()   [note aas uses timers 0 and 1, so for non-sound uses, make sure to initialize and use timer 2 or 3 if needed]
	irq_add(II_TIMER1, AAS_Timer1InterruptHandler);

	// Start playing sound (menu theme)
	AAS_SFX_Play(0, 32, 16000, AAS_DATA_SFX_START_MLPthemeIntro,
		AAS_DATA_SFX_END_MLPthemeLoop, AAS_DATA_SFX_START_MLPthemeLoop);


	//--------------------------------------------------------------------------

//15589
//21887
	tte_init_con();				// initializes console so tte_printf works (effectively black magic, but it works)
	pal_bg_bank[4][9]= 23054;


	u32 selection = 0; // 0 is on level select, 1 is on options, 2 is on credits
	s32 level = 0; // 0 is endless, 1 is lvl 1, 2 is lvl 2, 3 is lvl 3, etc...

	u32 hiscore = LoadValue(0);
	// if no save file exists, set hiscores to 0
	if (hiscore == -1) {
		hiscore = 0;
		u32 k;
		for(k=0; k<20; k++) {StoreValue(0, k);}
	} 

	s32 option = 0; // 0 just displays "<options>", 1 displays "<Music:On/Off>", 2 displays "<Reset Highscore>"
	
	//u32 TxtClrSet = 0; // whether color for text has been set or not
	u32 txtX = 64, txtY = 128;
	// Darker blue for text is 27235 (dec), lighter blue for text is 31603 (dec)
	u32 titleInit = 0;
	u32 seed = 0;
	u32 seedInit = 0;
	u32 scrollctr = 0;
	while (1) {
		// MAIN MENU ---------------------------------------------------------------------------------------------------------
		VBlankIntrWait();
		key_poll();

		if (seedInit == 0) seed++; //if seed not initialized, increment it each frame.

		// Initial one-time setup for menu when starting game or going back to menu
		if (titleInit == 0) {
			ResetGameSprites(); // IMPORTANT: do before TitleSpriteLoad or else Title sprites won't load correctly
			TitleSpriteLoad();
			titleInit = 1;
			
			ResetClouds(); // reset clouds so we don't have weird sprite artifacts in the menu
			u32 i;
			for(i=0; i<5; i++) {
				Cloud[i].ptr->attr2= ATTR2_BUILD(Cloud[i].tid, Cloud[i].pb, 0); // Build clouds after they are reset so they *actually* get reset
				obj_set_pos(Cloud[i].ptr, Cloud[i].x, Cloud[i].y); // and update their position so they get moved offscreen as well, which is probably the most important thing
			}

			tte_init_se(
				0,                      // Background number (BG 0)
				BG_CBB(0)|BG_SBB(22),   // BG control (for REG_BGxCNT)
				0,                      // Tile offset (special cattr)
				15589,             		// 15589 is value for the background palette color
				8,                     // BitUnpack offset (on-pixel = 9)
				NULL,                   // Default font (sys8) 
				NULL);                  // Default renderer (se_drawg_s)
			REG_BG0HOFS= 0;
			REG_BG0VOFS= 0;	
			tte_printf("#{er:0,0,240,160}"); // erased the background (removes balloon background, otherwise balloon would be loaded but pointing to text tiles, looks cool, but not wanted)
		}
		// voiceline easter eggs
		if (key_is_down(KEY_B)){
			if(key_hit(KEY_LEFT)) AAS_SFX_Play(1, 64, 32000, AAS_DATA_SFX_START_Applejack, AAS_DATA_SFX_END_Applejack, AAS_NULL);
			else if(key_hit(KEY_UP)) AAS_SFX_Play(1, 64, 32000, AAS_DATA_SFX_START_Twilight, AAS_DATA_SFX_END_Twilight, AAS_NULL);
			else if(key_hit(KEY_RIGHT)) AAS_SFX_Play(1, 64, 32000, AAS_DATA_SFX_START_Fluttershy, AAS_DATA_SFX_END_Fluttershy, AAS_NULL);
			else if(key_hit(KEY_DOWN)) AAS_SFX_Play(1, 64, 32000, AAS_DATA_SFX_START_Pinkie, AAS_DATA_SFX_END_Pinkie, AAS_NULL);
			else if(key_hit(KEY_L)) AAS_SFX_Play(1, 64, 32000, AAS_DATA_SFX_START_Spike, AAS_DATA_SFX_END_Spike, AAS_NULL);
			else if(key_hit(KEY_R)) AAS_SFX_Play(1, 64, 32000, AAS_DATA_SFX_START_Rarity, AAS_DATA_SFX_END_Rarity, AAS_NULL);
		}
		// Moving through the menu
		if (key_hit(KEY_DOWN)) selection++;
		if (key_hit(KEY_UP)) selection--;
		if (selection > 2) selection = 0; // makes selection wrap
		if (key_hit(KEY_RIGHT)) {
			if (selection == 0) {
				level++;
				if( level >3 ) level = 3; // don't go above max level [at the moment, level 3], I plan on adding a few more levels in the future, but that is less about coding, and more just tedium
				hiscore = LoadValue((unsigned int)level);
			}
			if (selection == 1) option++;
		}
		if (key_hit(KEY_LEFT)) {
			if (selection == 0) {
				level--;
				if (level < 0) level = 0;
				hiscore = LoadValue((unsigned int)level);
			}
			if (selection == 1) option--;
			
		}
		//insert something to make sure level select never goes above # of actual levels [first need to figure out how many total levels there will be]
		if (option > 2) option = 0; // makes option wrap
		if (option < 0) option = 2;

		if (selection == 0) {
			tte_set_pos(txtX, txtY);
			if (level == 0) tte_printf("#{el; cx:0}Play <endless>\n");
			else tte_printf("#{el; cx:0}Play Level <%d>\n", level);
			//tte_set_ink(0x1000);
			tte_printf("#{el; cx:0x4000; X:72}Highscore:%d\n", hiscore);
			tte_printf("#{el; X:80}<options>\n");
			tte_printf("#{X:88}Credits");

		}
		if (selection == 1) {
			tte_set_pos(txtX, txtY);
			if (level == 0) tte_printf("#{cx:0x4000}Play <endless>\n");
			else tte_printf("#{el; cx:0x4000}Play Level <%d>\n", level);
			tte_printf("#{el; X:72}Highscore:%d\n", hiscore);
			if (option == 0) tte_printf("#{el; cx:0; X:80}<options>\n");
			else if (option == 1) {
				if (music == 1){
					tte_printf("#{cx:0; X:60}<Music:On#{cx:0x4000}/Off#{cx:0}>\n");
					if (key_hit(KEY_A)) { 
						music = 0;
						AAS_SFX_Stop(0); // stop currently playing menu music
					}
				}
				else if (music == 0) {
					tte_printf("#{cx:0; X:60}<Music:#{cx:0x4000}On/#{cx:0}Off>\n");
					if (key_hit(KEY_A)) { 
						music = 1;
						AAS_SFX_Resume(0); // resume playing menu music
					}
				}
			}
			else if (option == 2) {
				tte_printf("#{el; cx:0; X:56}<reset hiscore>\n");
				if (key_hit(KEY_A)) {
					hiscore = 0;
					u32 k;
					for (k=0; k<20; k++) {StoreValue(0, k);}
				}
			}
			tte_printf("#{cx:0x4000; X:88}Credits");
		}
		else option = 0;
		if (selection == 2) { // credits highlighted
			tte_set_pos(txtX, txtY);
			if (level == 0) tte_printf("#{cx:0x4000}Play <endless>\n");
			else tte_printf("#{el; cx:0x4000}Play Level <%d>\n", level);
			tte_printf("#{el; X:72}Highscore:%d\n", hiscore);
			tte_printf("#{el; X:80}<options>\n");
			tte_printf("#{cx:0; X:88}Credits");
			// Plays credits if A button is pressed while credits button is highlighted
			if (key_hit(KEY_A)) {
				// move logo offscreen
				obj_set_pos(Title1A.ptr, 0, 170);
				obj_set_pos(Title1B.ptr, 0, 170);
				obj_set_pos(Title1C.ptr, 0, 170);
				obj_set_pos(Title1D.ptr, 0, 170);
				obj_set_pos(Title2A.ptr, 0, 170);
				obj_set_pos(Title2B.ptr, 0, 170);
				// add bars on top and bottom to hide text being replaced
				obj_set_pos(bar[0].ptr, 0, -16); obj_set_pos(bar[1].ptr, 64, -16); obj_set_pos(bar[2].ptr, 128, -16); obj_set_pos(bar[3].ptr, 192, -16);
				obj_set_pos(bar[4].ptr, 0, 143); obj_set_pos(bar[5].ptr, 64, 143); obj_set_pos(bar[6].ptr, 128, 143); obj_set_pos(bar[7].ptr, 192, 143);
				oam_copy(oam_mem, obj_buffer, 128);
				REG_BG0VOFS = -16;
				REG_BG0HOFS = -1;
				// set up text
				tte_set_pos(0, 128);
				tte_printf("#{er:0,0,240,256}"); // erase menu text
				tte_printf("#{cx:0}\nProgramming by\n");
				tte_printf("#{cx:0x4000}Ian Rudy\n\n\n");
				tte_printf("#{cx:0}Original Soundtrack by\n");
				tte_printf("#{cx:0x4000}Stephen Pender\n\n");
				tte_printf("#{cx:0}Menu Music courtesy of\n");
				tte_printf("#{cx:0x4000}Arturo E.\n\n");
				tte_printf("#{cx:0}Voicelines generated by\n");
				tte_printf("#{cx:0x4000}15.ai (v24.2.1)\n\n");
				tte_printf("#{cx:0}Apex Audio System courtesy of\n");
				tte_printf("#{cx:0x4000}Apex Designs & James Daniels\n\n\n");
				//
				
				//
				
				for (scrollctr = 0; scrollctr < 128; scrollctr++) {
					VBlankIntrWait(); VBlankIntrWait(); VBlankIntrWait();
					key_poll();
					REG_BG0VOFS = scrollctr-16;
				}
				//tte_printf("#{er:0,0,240,256}"); // erase background
				tte_set_pos(0, 0);
				tte_printf("#{cx:0}\nCharacter art courtesy of\n");
				tte_printf("#{cx:0x4000}Ponygauntlet (CC0)\n\n");
				tte_printf("#{cx:0}Additional art by\n");
				tte_printf("#{cx:0x4000}Ian Rudy\n\n\n");
				tte_printf("#{cx:0}Special Thanks to -\n");
				tte_printf("#{cx:0x4000}Coronac/Tonc\n");
				tte_printf("#{cx:0x4000}GBATEK\n");
				tte_printf("#{cx:0x4000}exelotl & circleseverywhere\n");
				tte_printf("#{cx:0x4000}GBAdev.org\n");
				tte_printf("#{cx:0x4000}Kustren\n");
				tte_printf("#{cx:0x4000}Hojin Chang\n");
				tte_printf("#{cx:0x4000}gbaguy @ patater.com\n");
				

				for (scrollctr = 128; scrollctr < 256; scrollctr++) {
					VBlankIntrWait(); VBlankIntrWait(); VBlankIntrWait();
					key_poll();
					REG_BG0VOFS = scrollctr-16;
				}
				tte_printf("#{er:0,128,240,256}"); // erase background
				//tte_printf("#{er:0,0,240,16}");
				tte_set_pos(0, 128);
				tte_printf("#{cx:0x4000}devkitPro\n\n\n\n");
				tte_printf("#{cx:0x4000}This is a fan project,\n");
				tte_printf("#{cx:0x4000}developed out of both\n");
				tte_printf("#{cx:0x4000}personal interest and for\n");
				tte_printf("#{cx:0x4000}use as an academic project.\n");
				tte_printf("#{cx:0x4000}No copyright infringement\n");
				tte_printf("#{cx:0x4000}is intended.\n");
				tte_printf("#{cx:0x1000}My Little Pony #{cx:0x4000} is a\n");
				tte_printf("#{cx:0x4000}registered trademark of\n");
				tte_printf("#{cx:0x1000}Hasbro, Inc.\n");
				tte_printf("#{cx:0x4000}All #{cx:0x1000}My Little Pony #{cx:0x4000}names,\n");
				tte_printf("#{cx:0x4000}characters, images, \n");
				tte_printf("#{cx:0x4000}trademarks, and logos are\n");
				
				for (scrollctr = 0; scrollctr < 128; scrollctr++) {
					VBlankIntrWait(); VBlankIntrWait(); VBlankIntrWait();
					key_poll();
					REG_BG0VOFS = scrollctr-16;
				}
				tte_printf("#{er:0,0,240,128}"); // erase background
				tte_set_pos(0, 0);
				tte_printf("#{cx:0x4000}protected by trademark\n");
				tte_printf("#{cx:0x4000}rights, copyrights, and\n");
				tte_printf("#{cx:0x4000}other rights owned by #{cx:0x1000}Hasbro\n");
				tte_printf("#{cx:0x4000}or #{cx:0x1000}Hasbro's #{cx:0x4000}licensors,\n");
				tte_printf("#{cx:0x4000}licensees, suppliers,\n");
				tte_printf("#{cx:0x4000}and accounts.\n\n");
				tte_printf("#{cx:0x4000}2021");
				for (scrollctr = 128; scrollctr < 256; scrollctr++) {
					VBlankIntrWait(); VBlankIntrWait(); VBlankIntrWait();
					key_poll();
					REG_BG0VOFS = scrollctr-16;
				}
				tte_printf("#{er:0,128,240,256}"); // erase background
				for (scrollctr = 0; scrollctr < 128; scrollctr++) {
					VBlankIntrWait(); VBlankIntrWait(); VBlankIntrWait();
					key_poll();
					REG_BG0VOFS = scrollctr-16;
				}
				REG_BG0VOFS = 0;
				tte_printf("#{er:0,0,240,256}"); // erase background
				// move logo back onscreen
				obj_set_pos(Title1A.ptr, Title1A.x, Title1A.y);
				obj_set_pos(Title1B.ptr, Title1B.x, Title1B.y);
				obj_set_pos(Title1C.ptr, Title1C.x, Title1C.y);
				obj_set_pos(Title1D.ptr, Title1D.x, Title1D.y);
				obj_set_pos(Title2A.ptr, Title2A.x, Title2A.y);
				obj_set_pos(Title2B.ptr, Title2B.x, Title2B.y);
				// remove bars
				obj_set_pos(bar[0].ptr, 0, 170); obj_set_pos(bar[1].ptr, 64, 170); obj_set_pos(bar[2].ptr, 128, 170); obj_set_pos(bar[3].ptr, 192, 170);
				obj_set_pos(bar[4].ptr, 0, 170); obj_set_pos(bar[5].ptr, 64, 170); obj_set_pos(bar[6].ptr, 128, 170); obj_set_pos(bar[7].ptr, 192, 170);
				oam_copy(oam_mem, obj_buffer, 128);
			}
		}
		


		// starts game when player chooses to play
		if (key_hit(KEY_A) && selection == 0) {
			if (seedInit == 0) {
				seedInit = 1;
				srand(seed); // seeds the random based on how many frames passed since game startup and playing clicking play for the first time (does not reseed when returned to menu and playing again)
			}

			// play random choice of intro message
			u32 bloopadoop = rrange(4,1);
			if (bloopadoop == 1) {
			AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_LetsDoThis1,
				AAS_DATA_SFX_END_LetsDoThis1, AAS_NULL);
			}
			else if (bloopadoop == 2) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_LetsDoThis2,
				AAS_DATA_SFX_END_LetsDoThis2, AAS_NULL);
			}
			else if (bloopadoop == 3) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_LetsDoThis3,
				AAS_DATA_SFX_END_LetsDoThis3, AAS_NULL);
			}
			else if (bloopadoop == 4) {
				AAS_SFX_Play(1, 64, 44100, AAS_DATA_SFX_START_LetsDoThis4,
				AAS_DATA_SFX_END_LetsDoThis4, AAS_NULL);
			}

			gameloop(level); // start the game
			selection = 0; // sets default selection to 0 when returning to menu
			titleInit = 0;
			PhaseCounter = 0; // resets phasecounter
			hiscore = LoadValue(level);
			}
		oam_copy(oam_mem, obj_buffer, 128);
	}
	// Infinite loop because programs stop on power off, return of main function is undefined on gba
	while (1) {}
	
}
