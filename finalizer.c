// GoldenEye XBLA community edition patcher tool
//   $ gcc -O2 -o finalizer finalizer.c
//   $ ./finalizer default.xex defaultCE.xex 0
//   $ ./finalizer default.xex defaultCE-surround.xex 1
// License: The MIT License (MIT)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./patches/xex_patch_restore.h"
#include "./patches/level_entry.h"
#include "./patches/runwaydoorpads.h"
#include "./patches/endcreditspads.h"
#include "./patches/siloicbmview.h"
#include "./patches/aztecshuttlearea.h"
#include "./patches/frigatebg_fixportals.h"
#include "./patches/surfacebg_portaladditions.h"
#include "./patches/surface1cabindoors.h"
#include "./patches/surface2cabindoors.h"
#include "./patches/mpchars_altsandbonus.h"
#include "./patches/mpweaponsets.h"
#include "./patches/mplevellist.h"
#include "./patches/jungle_ai_list_040D.h"
#include "./patches/dam_intro_fix.h"
#include "./patches/egypt_puzzle_glass_door_fix.h"
#include "./patches/armor_fix_code.h"

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(disable:4996)
#endif

// Unused macros helpers
//#define MAX(x, y) (((x) > (y)) ? (x) : (y))
//#define MIN(x, y) (((x) < (y)) ? (x) : (y))
//#define ROUNDADDRUP(x) ((((x) + 0x3) | 0x3) ^ 0x3)
//#define ASCII2HEX(x) (((x) <= '9') ? (x) - '0' : ((x) & 0x7) + 9)
//#define CLAMP(value, min, max) (((value) < (min)) ? (min) : (((value) > (max)) ? (max) : ((value) < (min)) ? (min) : (value)))

#define ARRAYSIZE(array) ((size_t)(sizeof(array)/sizeof(*array)))
#define LINE "__________________________________________________________________"

// Common asm instructions
#define NOOP                        (0x60000000U) // no operation instruction
#define BLR                         (0x4E800020U) // branch to link register (usually to end function)
#define B_JMP(src, dst)             (0x48000000U + (((dst) - (src)) & 0x03FFFFFF)) // calculate offset for unconditional branch jump
#define BL_JMP(src, dst)            (0x48000001U + (((dst) - (src)) & 0x03FFFFFF)) // calculate offset for unconditional branch link jump

// Memory defines
#define RODATARAMOFFSET             (0x82000400U) // rodata segment RAM offset (82000400 - 8207763C)
#define TEXTRAMOFFSET               (0x82090000U) // text segment RAM offset (82090000 - 824102BC)
#define HACKRAMOFFSET               (0x830E0000U) // hacked text segment RAM offset (830E0000 - 830F0000)
#define DATARAMOFFSET               (0x82420000U) // data segment RAM offset (82420000 - 830CC340)
#define RODATABINOFFSET             (0x00003400U) // rodata segment bin offset (00003400 - 0007A63C)
#define TEXTBINOFFSET               (0x0008B000U) // text segment bin offset (0008B000 - 003802BC)
#define HACKBINOFFSET               (0x00F1B000U) // hacked text segment bin offset (00F1B000 - 00F2B000)
#define DATABINOFFSET               (0x00413000U) // data segment bin offset (00413000 - 010BF340)
#define XEXFILESIZEMIN              (15*0x100000L) // minimum XEX filesize supported (15MB)

// Address macros
#define RODATA(address)             ((((address) - RODATARAMOFFSET) & 0x000FFFFF) + RODATABINOFFSET) // convert rodata region to XEX address
#define XEXTEXT(address)            ((((address) -   TEXTRAMOFFSET) & 0x00FFFFFF) +   TEXTBINOFFSET) // convert text code region to XEX address
#define XEXHACK(address)            ((((address) -   HACKRAMOFFSET) & 0x00FFFFFF) +   HACKBINOFFSET) // convert hacked text code region to XEX address
#define DATA(address)               ((((address) -   DATARAMOFFSET) & 0x00FFFFFF) +   DATABINOFFSET) // convert data region to XEX address

#define readarrayint(array, address) \
    ((array[(address)]  << 24) | \
    (array[(address)+1] << 16) | \
    (array[(address)+2] <<  8) | \
    (array[(address)+3]))
#define writearrayint(array, address, value) \
    array[(address)+0] = (((value) & 0xFF000000) >> 24), \
    array[(address)+1] = (((value) & 0x00FF0000) >> 16), \
    array[(address)+2] = (((value) & 0x0000FF00) >>  8), \
    array[(address)+3] =  ((value) & 0x000000FF)

#define xex_read(addr)              (readarrayint(xexbin, addr)) // read 4 byte int from XEX
#define xex_write(addr, val)        (writearrayint(xexbin, addr, val)) // write 4 byte int to XEX
#define text_read(addr)             (readarrayint(xexbin, XEXTEXT(addr))) // read 4 byte int from XEX's text segment
#define text_write(addr, val)       (writearrayint(xexbin, XEXTEXT(addr), val)) // write 4 byte int to XEX's text segment
#define hack_read(addr)             (readarrayint(xexbin, XEXHACK(addr))) // read 4 byte int from XEX's hacked segment
#define hack_write(addr, val)       (writearrayint(xexbin, XEXHACK(addr), val)) // write 4 byte int to XEX's hacked segment
#define rodata_read(addr)           (readarrayint(xexbin, RODATA(addr))) // read 4 byte int from XEX's rodata segment
#define rodata_write(addr, val)     (writearrayint(xexbin, RODATA(addr), val)) // write 4 byte int to XEX's rodata segment
#define data_read(addr)             (readarrayint(xexbin, DATA(addr))) // read 4 byte int from XEX's rodata segment
#define data_write(addr, val)       (writearrayint(xexbin, DATA(addr), val)) // write 4 byte int to XEX's rodata segment

#define text_write_b(addr, dst)     (writearrayint(xexbin, XEXTEXT(addr), B_JMP(addr, dst))) // calculate branch jump and write 4 byte int to XEX's text segment
#define text_write_bl(addr, dst)    (writearrayint(xexbin, XEXTEXT(addr), BL_JMP(addr, dst))) // calculate branch link jump and write 4 byte int to XEX's text segment
#define hack_write_b(addr, dst)     (writearrayint(xexbin, XEXHACK(addr), B_JMP(addr, dst))) // calculate branch jump and write 4 byte int to XEX's hacked segment
#define hack_write_bl(addr, dst)    (writearrayint(xexbin, XEXHACK(addr), BL_JMP(addr, dst))) // calculate branch link jump and write 4 byte int to XEX's hacked segment
#define hack_add(val)               hack_write(hack_cur_addr, val), hack_cur_addr += 4 // add code and increment to next empty slot
#define hack_add_b(addr)            hack_write_b(hack_cur_addr, addr), hack_cur_addr += 4 // b to address and increment to next empty slot
#define hack_add_bl(addr)           hack_write_bl(hack_cur_addr, addr), hack_cur_addr += 4 // bl to address and increment to next empty slot

static unsigned int hack_cur_addr = HACKRAMOFFSET; // add hacks at the start of hacked region

// Fyodorovna hacks
#define DEBUG_BRANCH                (0x82189F28U) // branch if debug memu not active
#define COLLECT_ARMOR_FUNC_END      (0x820B40E0U) // used for the flash hud hack
#define UNEQUAL_CHR_HEIGHT_MASK     (0x820FFA44U) // mask used for saving current MP options
#define UNEQUAL_CHR_HEIGHT_IMM      (0x820FFA4CU) // init mp settings options
#define DEFAULT_MP_OPTIONS          (0x8272B3BCU) // default stored other mp settings options
#define DRONE_ACTIVATE_FLAG         (0x820E59F8U) // drone gun sentry flag set on
#define DRONE_FIRING_RATE           (0x820E7954U) // drone gun sentry firing rate delta
#define XBOX_LIVE_CLICK             (0x820F2838U) // xbox live click
#define XBOX_LIVE_PRINT             (0x820FFC00U) // xbox live print logic
#define MENU_LEADERBOARD_CLICK      (0x820F77D0U) // menu leaderboard click
#define MENU_LEADERBOARD_DISABLE    (0x820EE334U) // dim leaderboard menu entry
#define MENU_MAIN06_SEL_COUNT       (0x820F774CU) // count index for main menu (select logic)
#define MENU_MAIN06_PRINT_COUNT     (0x820F7D00U) // count index for main menu (print logic)
#define WATCH_PRINT_COUNT           (0x820C490CU) // count index for watch sp menu (print logic)
#define WATCH_INTERACT_LEADERBOARD  (0x820C74F8U) // watch interact leaderboard call
#define WATCH_INTERACT_UP           (0x820C7450U) // watch interact move selected item up
#define WATCH_INTERACT_DOWN         (0x820C742CU) // watch interact move selected item down
#define REMOTE_MINE_SFX             (0x820DC824U) // remote mine beep const (SFX ID F3)
#define MP_PAUSE_LEADERBOARD        (0x8213B73CU) // mp pause menu interact leaderboard call
#define FOG_BANK_TABLE_START        (0x82858860U) // fog bank start
#define FOG_BANK_TABLE_END          (0x82859510U) // fog bank end
#define FOG_SMALL_BANK_TABLE_START  (0x82859510U) // fog small bank start
#define FOG_SMALL_BANK_TABLE_END    (0x828595F0U) // fog small bank end

// New hacks
#define FOG_FUNC_GET_PLAYER         (0x82117CACU) // load fog for level function
#define X3DAUDIO_INIT_FLAGS         (0x82143D80U) // load flags for X3DAudioInitialize
#define X3DAUDIO_CALC_FLAGS         (0x82143E90U) // load flags for X3DAudioCalculate
#define X3DAUDIO_CURVE_POINTS       (0x82DEE9FCU) // X3DAUDIO_DISTANCE_CURVE_POINT struct (assigned at 83064C88)
#define X3DAUDIO_DISTANCE_HI        (0x8214437CU) // load float for X3DEmitter->CurveDistanceScaler
#define X3DAUDIO_DISTANCE_LO        (0x82144388U) // load float for X3DEmitter->CurveDistanceScaler
#define DOOR_SFX_VOL_MIN            (0x820DD814U) // minimum volume for doors over 60 meters
#define LOGO_STRING                 (0x820ED678U) // string id for goldeneye logo
#define NEAR_CLIP                   (0x82858804U) // near clip global var
#define NEAR_CLIP_INIT              (0x82117E90U) // init near clip to 5.f
#define NEAR_CLIP_WRITE             (0x82117B44U) // write near clip from fog entry to global var
#define STAGE_LOAD_FUNC             (0x8209F5F0U) // for reset internal cheat states hack
#define MENU_GFX_MODE_JMP           (0x82099728U) // intro graphics mode check for menu id
#define BLUR_GRAPHICS_JMP           (0x82188E70U) // original graphics blur check
#define REMOTE_PLAYER_ARMOR_FIX     (0x8216BC1CU) // remote player armor fix function
#define PLAYER_GASP_SFX             (0x820BF264U) // gasp play sfx
#define PLAYER_SLAPPER_SFX          (0x820CDC5CU) // slapper play sfx
#define PLAYER_KNIFE_SFX            (0x820ACF54U) // knife play sfx
#define PLAYER_ITEM_EQUIP_SFX       (0x820AC4D0U) // item equip play sfx
#define PLAYER_REMOTE_GASP_SFX      (0x820BF408U) // remote gasp sfx
#define PLAYER_REMOTE_FIRE_GUN      (0x8216E25CU) // function used to spawn tracers for remote client
#define CALL_MP_KILL_TUNE           (0x820BFB00U) // call to music playback function used for death tune
#define FLOAT_AT_KILL_TUNE_FLAG     (0x8209D88CU) // instruction used to read float that we use for the death tune bypass flag address
#define BYPASS_MP_KILL_TUNE         (0x824203ACU) // new flag used to bypass the mp death tune for single screen mode
#define MP_CHECK_DOOR_TIME          (0x820E4F1CU) // load door open tick value
#define PLAYER_COLLISION_CALC       (0x820CDFA4U) // call to function that calculates player collision radius
#define LVL_ENTRY_INIT_FUNC         (0x8240C368U) // level entry init func
#define LVL_ENTRY_INIT_FUNC_END     (0x8240D130U) // end of level entry init func
#define LVL_ENTRY_INIT_FUNC_CALL    (0x82420030U) // call to level entry init func
#define LVL_ENTRY_TABLE             (0x82857F90U) // level entry table address
#define LVL_ENTRY_TABLE_SLOT(X)     (LVL_ENTRY_TABLE + (0x38 * X)) // new level entry slot
#define GUN_CLIPBOARD_POS           (0x82422744U) // clipboard watch position
#define GUN_CIRCUITBOARD_POS        (0x82ECFF18U) // circuitboard watch position
#define GUN_BOLT_KEY_POS            (0x82422BA4U) // bolt key watch position
#define GUN_YALE_KEY_POS            (0x82422B6CU) // yale key watch position
#define RUNWAY_AILIST_0403          (0x82D5BB6CU) // runway glitched AI list that breaks when spawning hat
#define SURFACE1_AILIST_0421        (0x82D85420U) // surface i glitch AI list (dual klobbs lack akimbo flag)
#define JUNGLE_AILIST_0402          (0x82D43728U) // jungle AI list 0402 (Xenia trigger)
#define JUNGLE_AILIST_040D          (0x82D43BC0U) // jungle AI list 040D (guard idle)
#define STATUE_AILIST_0423          (0x82DAE62CU) // statue AI list 0423 (end cinema guard)
#define STATUE_AILIST_1000          (0x82DAD7F0U) // statue AI list 1000 (Trev spawn)
#define RUNWAY_PADS_DOORS           (0x82D56DE8U) // runway door pads that need fixing
#define EGYPT_MP_AMMO               (0x82CF5AECU) // egypt mp wrong plink for ammo box
#define DAM_GATE_PADLOCK            (0x82D0D124U) // dam padlock flags 1
#define SURFACE1_BOOKSHELF1_FLAG    (0x82D7C554U) // surface i bookshelf 1 flag (glitches game when destroyed!)
#define SURFACE1_BOOKSHELF2_FLAG    (0x82D7C5D4U) // surface i bookshelf 2 flag (glitches game when destroyed!)
#define SURFACE1_PAD_ERROR          (0x82D77DE0U) // surface i pad link error (radar exterior railing)
#define SURFACE2_PAD_ERROR          (0x82D8882CU) // surface ii pad link error (radar exterior railing)
#define CONTROL_DK5_TREV            (0x82CAD4C0U) // control dk5 weapon flags for Trev (left handed)
#define ARCHIVES_MP_AMMO_PAD        (0x82C91238U) // archives mp ammo crate pad position
#define ARCHIVES_MP_AMMO            (0x82DC7A04U) // archives mp ammo crate object
#define TEMPLE_MP_AMMO_PAD          (0x82DDFC20U) // temple mp ammo crate pad position (stacked ammo boxes)
#define TEMPLE_MP_AMMO              (0x82DE05E0U) // temple mp ammo crate object
#define SILO_ARMOR                  (0x82D98BF8U) // silo's armor
#define STATUE_ARMOR_FLAG           (0x82DAAA74U) // statue's armor flags 2
#define JUNGLE_ARMOR_FLAG           (0x82D408E8U) // jungle's armor flags 2
#define FRIGATE_ARMOR               (0x82D27DDCU) // frigate's armor
#define SURFACE2_ARMOR_FLAG         (0x82D8D55CU) // surface ii's armor flags 2
#define SURFACE1_CABIN_DOOR_FIX     (0x82D788ACU) // for surface i's glitched cabin door fix
#define SURFACE2_CABIN_DOOR_FIX     (0x82D89450U) // for surface i's glitched cabin door fix
#define CONTROL_BLAST_DOOR_FIX      (0x82CACD48U) // for control's first floor blast door
#define FACILITY_DOAK_SPAWN1        (0x82CC7D30U) // facility AI list 1004 (spawn Doak logic)
#define FACILITY_DOAK_SPAWN2        (0x82CC7D48U) // facility AI list 1004 (spawn Doak logic)
#define FACILITY_DOAK_SPAWN3        (0x82CC7D60U) // facility AI list 1004 (spawn Doak logic)
#define FACILITY_DOAK_SPAWN4        (0x82CC7D78U) // facility AI list 1004 (spawn Doak logic)
#define FACILITY_DOAK_SPAWN5        (0x82CC7D90U) // facility AI list 1004 (spawn Doak logic)
#define FACILITY_DOAK_SPAWN6        (0x82CC7DA8U) // facility AI list 1004 (spawn Doak logic)
#define FACILITY_DOAK_SPAWN7        (0x82CC7DBCU) // facility AI list 1004 (spawn Doak logic)
#define JUNGLE_NATALYA_CMAG         (0x82CAD418U) // Jungle Natalya's cougar magnum prop
#define CONTROL_NATALYA_CMAG        (0x82D3D2F0U) // Control Natalya's cougar magnum prop
#define DAM_INTRO_COMMAND           (0x82D0E7D8U) // dam intro command camera
#define EGYPT_PUZZLE_GLASS_DOORS    (0x82CF9634U) // egypt's puzzle glass doors
#define CUBA_PADS                   (0x82D444E8U) // cuba setup pads for new ending
#define BASEMENT_FLAG_PAD           (0x82DE24A4U) // basement flag pad position
#define LIBRARY_FLAG_PROP           (0x82DC2D94U) // library flag prop
#define CONTROL_PAD_27BC            (0x82CAB11CU) // control setup pad for glass hit fix
#define CONTROL_PAD_27C9            (0x82CAB1B0U) // control setup pad for glass hit fix
#define CONTROL_PAD_27BE            (0x82CAB244U) // control setup pad for glass hit fix
#define CONTROL_PAD_27BA            (0x82CAB2D8U) // control setup pad for glass hit fix
#define CONTROL_PAD_27BD            (0x82CAB36CU) // control setup pad for glass hit fix
#define CONTROL_PAD_27CA            (0x82CAB400U) // control setup pad for glass hit fix
#define CONTROL_PAD_27BB            (0x82CAB494U) // control setup pad for glass hit fix
#define FACILITY_SP_PAD_2772        (0x82CC0890U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2773        (0x82CC0924U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2775        (0x82CC09B8U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2776        (0x82CC0A4CU) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2777        (0x82CC0AE0U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2778        (0x82CC0B74U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2779        (0x82CC0C08U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_277A        (0x82CC0C9CU) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_277D        (0x82CC0D30U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_277E        (0x82CC0DC4U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_277F        (0x82CC0E58U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2780        (0x82CC0EECU) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2791        (0x82CC0F80U) // facility sp setup pad for glass hit fix
#define FACILITY_SP_PAD_2792        (0x82CC1014U) // facility sp setup pad for glass hit fix
#define FACILITY_MP_PAD_2772        (0x82DCB458U) // facility mp setup pad for glass hit fix
#define FACILITY_MP_PAD_2773        (0x82DCB4ECU) // facility mp setup pad for glass hit fix
#define FACILITY_MP_PAD_2775        (0x82DCB580U) // facility mp setup pad for glass hit fix
#define FACILITY_MP_PAD_2776        (0x82DCB614U) // facility mp setup pad for glass hit fix
#define FACILITY_MP_PAD_2777        (0x82DCB6A8U) // facility mp setup pad for glass hit fix
#define FACILITY_MP_PAD_2778        (0x82DCB73CU) // facility mp setup pad for glass hit fix
#define FACILITY_MP_PAD_2779        (0x82DCB7D0U) // facility mp setup pad for glass hit fix
#define FACILITY_MP_PAD_277A        (0x82DCB864U) // facility mp setup pad for glass hit fix
#define BUNKER2_STAIRS_TILE1        (0x82804B9CU) // bunker ii's stairs stan tile connection
#define BUNKER2_STAIRS_TILE2        (0x82804BECU) // bunker ii's stairs stan tile connection
#define SILO_BG_DATA                (0x82A7A540U) // silo background data (portals/special visibility/etc)
#define AZTEC_BG_DATA               (0x828E8340U) // aztec background data (portals/special visibility/etc)
#define FRIGATE_BG_DATA             (0x829B5CC0U) // frigate background data (portals/special visibility/etc)
#define SURFACE_BG_DATA             (0x82A5DF70U) // surface background data (portals/special visibility/etc)
#define SURFACE_STRING              (0x82006CA8U) // surface internal level string (used for splitting into sf1/sf2)
#define MP_CHARS_UNLOCK1            (0x820EF350U) // multiplayer character's unlocked after completing cradle
#define MP_CHARS_UNLOCK2            (0x82106C54U) // multiplayer character's unlocked after completing cradle (needed for system link)
#define MP_CHARS_STRUCT             (0x8272BA80U) // multiplayer character's struct (for new entries)
#define MP_WEAPON_SETS              (0x82424728U) // multiplayer weapon set struct
#define CHAR_NATALYA_SCALE          (0x82729270U) // natalya's character scale
#define CHAR_NATALYA_MP_POV         (0x8272BA94U) // natalya's pov MP height
#define MP_LEVEL_LIST               (0x8272B918U) // mp level lists
#define SFX_MASTER_VOLUME           (0x82DEEA24U) // sound master volume
#define MUSIC_MASTER_VOLUME         (0x82DEEA28U) // music master volume
#define WATCH_MUSIC_VOL_READ1       (0x820C82C4U) // used to read music volume for slider
#define WATCH_MUSIC_VOL_READ2       (0x820C8324U) // used to read music volume for slider
#define WATCH_MUSIC_VOL_SAVE        (0x820C831CU) // used to save music volume from slider
#define WATCH_SFX_VOL_READ1         (0x820C8154U) // used to read fx volume for slider
#define WATCH_SFX_VOL_READ2         (0x820C81B4U) // used to read fx volume for slider
#define WATCH_SFX_VOL_SAVE          (0x820C81ACU) // used to save fx volume from slider
#define WEAPON_RESPAWN_GG_CHECK     (0x820CF940U) // instruction used to check the current respawning weapon is golden gun, if true don't unset invisible prop flag
#define WATER_REND_GFX_MODE_CHECK   (0x8209ECF4U) // check if original graphics mode is set, and render water
#define MISSION_TIME_FULL           (0x820F9CD4U) // instructions used to round down mission time ticks to seconds

// Small fog bank struct
// 0x00: ID
// 0x04: Sky Colour / Cloud Toggle
// 0x08: Ceiling Height (Float)
// 0x0C: Null
// 0x10: Cloud Red Component (Float)
// 0x14: Cloud Green Component (Float)
// 0x18: Cloud Blue Component (Float)
// 0x1C: Water Toggle
// 0x20: Water Height (Float)
// 0x24: Water Image
// 0x28: Water Red Component (Float)
// 0x2C: Water Green Component (Float)
// 0x30: Water Blue Component (Float)
// 0x34: Null

typedef enum LEVELID
{
    LEVELID_BUNKER1 = 0x9,
    LEVELID_SILO = 0x14,
    LEVELID_STATUE = 0x16,
    LEVELID_CONTROL = 0x17,
    LEVELID_ARCHIVES = 0x18,
    LEVELID_TRAIN = 0x19,
    LEVELID_FRIGATE = 0x1A,
    LEVELID_BUNKER2 = 0x1B,
    LEVELID_AZTEC = 0x1C,
    LEVELID_STREETS = 0x1D,
    LEVELID_DEPOT = 0x1E,
    LEVELID_COMPLEX = 0x1F,
    LEVELID_EGYPT = 0x20,
    LEVELID_DAM = 0x21,
    LEVELID_FACILITY = 0x22,
    LEVELID_RUNWAY = 0x23,
    LEVELID_SURFACE = 0x24,
    LEVELID_JUNGLE = 0x25,
    LEVELID_TEMPLE = 0x26,
    LEVELID_CAVERNS = 0x27,
    LEVELID_CITADEL = 0x28,
    LEVELID_CRADLE = 0x29,
    LEVELID_SHO = 0x2A,
    LEVELID_SURFACE2 = 0x2B,
    LEVELID_ELD = 0x2C,
    LEVELID_BASEMENT = 0x2D,
    LEVELID_STACK = 0x2E,
    LEVELID_LUE = 0x2F,
    LEVELID_LIBRARY = 0x30,
    LEVELID_RIT = 0x31,
    LEVELID_CAVES = 0x32,
    LEVELID_EAR = 0x33,
    LEVELID_LEE = 0x34,
    LEVELID_LIP = 0x35,
    LEVELID_CUBA = 0x36,
    LEVELID_WAX = 0x37,
    LEVELID_PAM = 0x38,
    LEVELID_MAX = 0x39,
    LEVELID_TITLE = 0x5A,
    SP_ALT = 0x64, // fog range offsets
    MP_P2 = 0xC8,
    MP_P3 = 0x12C,
    MP_P4 = 0x190,
    SP_CINEMA = 0x0384 // for cinema only fog
} LEVELID;

static void remove_debug_button(unsigned char *xexbin)
{
    printf("\n  Removing debug button...");

    text_write(DEBUG_BRANCH, 0x2B0B000F); // cmplwi cr6, r11, 0x000F            always skip debug check functions
}

static void flash_health_bars_on_armor_pickup(unsigned char *xexbin)
{
    printf("\n  Adding Health HUD Flash Hack..."); // can use registers r5, r7, r8

    // old code
    // text_write(COLLECT_ARMOR_FUNC_END + 0x00, 0x3D0082F2); // lis r8, 0x82F2 set to player pointer array
    // text_write_b(COLLECT_ARMOR_FUNC_END + 0x04, hack_cur_addr); // b to hack jump to hacked segment function

    // hack_add(0x80E8FA9C); // lwz r7, 0xFA9C (r8)                             load p2 pointer (82F1FA9C) to r7
    // hack_add(0x2F070000); // cmpwi cr6, r7, 0x0000                           compare and store results to cr6
    // hack_add(0x409A0024); // bne cr6, 0x0024                                 branch if not equal to zero (multiplayer active)

    // hack_add(0xE8E8FAA0); // ld r7, 0xFAA0 (r8)                              load p3 and p4 pointers (82F1FAA0-7) to r7
    // hack_add(0x2F270000); // cmpdi cr6, r7, 0x0000                           compare and store results to cr6
    // hack_add(0x409A0018); // bne cr6, 0x0018                                 branch if not equal to zero (multiplayer active)

    text_write(COLLECT_ARMOR_FUNC_END + 0x00, 0x3CE08273); // lis r7, 0x8273    load current screen setting (8272B424)
    text_write_b(COLLECT_ARMOR_FUNC_END + 0x04, hack_cur_addr); // b to hack    jump to hacked segment function

    hack_add(0x80E7B424); // lwz r7, 0xB424 (r7)
    hack_add(0x2F070003); // cmpwi cr6, r7, 0x0003                              compare and store results to cr6
    hack_add(0x409A0018); // bne cr6, 0x0018                                    branch if not equal to full screen mode (0: 4 way, 1: top/bottom, 2: left/right, 3: solo view)

    hack_add(0xD02B01F0); // stfs f1, 0x01F0 (r11)                              save new armor value to last armor value
    hack_add(0x810B01E4); // lwz r8, 0x01E4 (r11)                               load current health
    hack_add(0x910B01EC); // stw r8, 0x01EC (r11)                               save current health to last health value
    hack_add(0x39000000); // li r8, 0x0000                                      set r8 to 0
    hack_add(0x910B0200); // stw r8, 0x0200 (r11)                               reset hud flash counter
    hack_add(BLR); // return
}

static void remove_xbox_live_and_leaderboard(unsigned char *xexbin)
{
    printf("\n  Removing Xbox Live Menu And Leaderboards...");

    text_write_b(XBOX_LIVE_CLICK, 0x820F285C); // b 0x820F285C                  never allow menu to hover over xbox live entry

    text_write(XBOX_LIVE_PRINT + 0x00, NOOP); // don't print xbox live menu in multiplayer
    text_write(XBOX_LIVE_PRINT + 0x04, NOOP);
    text_write(XBOX_LIVE_PRINT + 0x08, NOOP);
    text_write(XBOX_LIVE_PRINT + 0x0C, NOOP);
    text_write(XBOX_LIVE_PRINT + 0x10, NOOP);

    text_write(MENU_LEADERBOARD_CLICK, 0x39600007); // li r11, 0x0007           clickable action
    text_write(MENU_LEADERBOARD_DISABLE, 0x2F030003); // cmpwi cr6, r3, 0x0003  dim entry

    text_write_b(MENU_MAIN06_SEL_COUNT, hack_cur_addr); // b to hack            jump to hacked segment function

    hack_add(0x3BDE0001); // addi r30, r30, 0x0001                              add 1 to entry
    hack_add(0x2F1E0003); // cmpwi cr6, r30, 0x0003                             compare against leaderboard index
    hack_add(0x419A000C); // beq cr6, 0x000C                                    if index == leaderboard, skip
    hack_add(0x2F1E0006); // cmpwi cr6, r30, 0x0006                             compare against buy full game menu item
    hack_add(0x409A0008); // bne cr6, 0x0008                                    if index == buy full game, skip
    hack_add(0x3BDE0001); // addi r30, r30, 0x0001                              increment index (aka skip)
    hack_add_b(MENU_MAIN06_SEL_COUNT + 4); // b from hack                       return

    text_write_b(MENU_MAIN06_PRINT_COUNT, hack_cur_addr); // b to hack          jump to hacked segment function

    hack_add(0x3B5A0001); // addi r26, r26, 0x0001                              add 1 to entry
    hack_add(0x2F1A0003); // cmpwi cr6, r26, 0x0003                             compare against leaderboard index
    hack_add(0x419A000C); // beq cr6, 0x000C                                    if index == leaderboard, skip
    hack_add(0x2F1A0006); // cmpwi cr6, r26, 0x0006                             compare against buy full game menu item
    hack_add(0x409A0008); // bne cr6, 0x0008                                    if index == buy full game, skip
    hack_add(0x3B5A0001); // addi r26, r26, 0x0001                              increment index (aka skip)
    hack_add_b(MENU_MAIN06_PRINT_COUNT + 4); // b from hack                     return

    text_write(WATCH_PRINT_COUNT + 0x0, 0x3BFF0001); // addi r31, r31, 0x0001
    text_write(WATCH_PRINT_COUNT + 0x4, 0x3BBD0004); // addi r29, r29, 0x0004
    text_write_b(WATCH_PRINT_COUNT + 0x8, hack_cur_addr); // b to hack          jump to hacked segment function

    hack_add(0x2F1F0003); // cmpwi cr6, r31, 0x0003                             compare against leaderboard index
    hack_add(0x409A000C); // bne cr6, 0x000C                                    if index != leaderboard item, skip
    hack_add(0x3BFF0001); // addi r31, r31, 0x0001                              increment index (aka skip)
    hack_add(0x3BBD0004); // addi r29, r29, 0x0004                              increment index (aka skip)
    hack_add(0x7F8BE214); // add r28, r11, r28                                  add code that we overwrote with jump
    hack_add_b(WATCH_PRINT_COUNT + 0xC); // b from hack                         return

    text_write(WATCH_INTERACT_LEADERBOARD, NOOP); // disable leaderboard trigger (sp)
    text_write(MP_PAUSE_LEADERBOARD, NOOP); // disable leaderboard trigger (mp)

    text_write_b(WATCH_INTERACT_UP, hack_cur_addr); // b to hack                jump to hacked segment function

    hack_add(0x2F0B0003); // cmpwi cr6, r11, 0x0003                             compare against leaderboard index
    hack_add(0x409A0008); // bne cr6, 0x0008                                    if index != leaderboard item, skip
    hack_add(0x396BFFFF); // addi r11, r11, 0xFFFF                              increment index (aka skip)
    hack_add(0x9149F9BC); // stw r10, 0xF9BC (r9)                               add code that we overwrote with jump
    hack_add_b(WATCH_INTERACT_UP + 4); // b from hack                           return

    text_write_b(WATCH_INTERACT_DOWN, hack_cur_addr); // b to hack              jump to hacked segment function

    hack_add(0x2F0B0003); // cmpwi cr6, r11, 0x0003                             compare against leaderboard index
    hack_add(0x409A0008); // bne cr6, 0x0008                                    if index != leaderboard item, skip
    hack_add(0x396B0001); // addi r11, r11, 0x0001                              increment index (aka skip)
    hack_add(0x9149F9BC); // stw r10, 0xF9BC (r9)                               add code that we overwrote with jump
    hack_add_b(WATCH_INTERACT_DOWN + 4); // b from hack                         return
}

static void set_unequal_heights_on_by_default(unsigned char *xexbin)
{
    printf("\n  Setting Unequal Characters On By Default...");

    text_write(UNEQUAL_CHR_HEIGHT_MASK, 0x716B00E0); // andi. r11, r11, 0x00E0  do not filter out bitmask 0x20 (unequal characters on by default)
    text_write(UNEQUAL_CHR_HEIGHT_IMM, 0x616B0023); // ori r11, r11, 0x23       always set options to 0x23

    data_write(DEFAULT_MP_OPTIONS, 0x8C230101); // set new defaults
}

static void fix_sentry_drones_ignoring_activate_state(unsigned char *xexbin)
{
    printf("\n  Fix Sentry Drones Ignoring Distance And Firing Rate...");

    text_write(DRONE_ACTIVATE_FLAG, 0x556B00C0); // rlwinm r11, r11, 0, 3, 0    mask object bitflags (remove active/been shot flags)
    text_write(DRONE_FIRING_RATE, 0x396B0004); // addi r11, r11, 0x00004        make drone gun fire slower (from 2 ticks to 4 ticks)
}

static void fix_remote_mine_sfx(unsigned char *xexbin)
{
    printf("\n  Change Remote Mine Detonate SFX...");

    text_write(REMOTE_MINE_SFX, 0x388000E5); // set to sound slot E5
}

static void make_mp_always_use_p2_fog(unsigned char *xexbin)
{
    printf("\n  Make Multiplayer Always Use P2 Fog...");

    text_write(FOG_FUNC_GET_PLAYER + 0x0, 0x2F030001); // cmpwi cr6, r3, 0x0001 compare player total results (set on 1 player active)
    text_write_b(FOG_FUNC_GET_PLAYER + 0x4, hack_cur_addr); // b to hack        jump to hacked segment function

    hack_add(0x409A0008); // bne cr6, 0x0008                                    branch if multiple players are active
    hack_add_b(FOG_FUNC_GET_PLAYER + 0x8); // return to li r3, 0x0000           1 player is active, return to original code
    hack_add(0x38600002); // li r3, 0x0002                                      2 or more players are active, always set to value used for 2 player mode and return
    hack_add_b(FOG_FUNC_GET_PLAYER + 0xC); // return
}

static void adjust_fog_bank(unsigned char *xexbin)
{
    printf("\n  Tweaking Fog Settings...");

    for(size_t index = FOG_BANK_TABLE_START; index < FOG_BANK_TABLE_END; index += 0x38) // load each fog bank
    {
        if(data_read(index) == 0) // reached end of list
            break;
        switch((data_read(index) & 0xFFFF0000) >> 16)
        {
            case LEVELID_RUNWAY:
                data_write(index + 0x24, 0x0000D6D8); // set new fog distance to 55000
                data_write(index + 0x28, 0x85ADCA00); // set new fog color to 133, 173, 202
                data_write(index + 0x2C, 0x000124F8); // set far clipping plane to 75000
            case LEVELID_DAM:
            case LEVELID_FACILITY:
            case LEVELID_BUNKER2:
            case LEVELID_EGYPT:
                data_write(index + 0x30, 0x40C00000); // set near clipping to 6
                break;
            case LEVELID_DAM + SP_CINEMA:
                data_write(index + 0x28, 0x85ADCA00); // set new fog color to 133, 173, 202
                break;
            case LEVELID_STREETS:
                data_write(index + 0x08, 0x271003E8); // set max obj vis to 10000
                data_write(index + 0x2C, 0x00003A98); // set far clipping plane to 15000
                data_write(index + 0x30, 0x40C00000); // set near clipping to 6
                break;
            case LEVELID_TRAIN:
                data_write(index + 0x04, 0x11940000); // set original fog distance to 4500
                data_write(index + 0x24, 0x00001194); // set new fog distance to 4500
                data_write(index + 0x30, 0x40C00000); // set near clipping to 6
                break;
            case LEVELID_STATUE:
                data_write(index + 0x30, 0x40C00000); // set near clipping to 6
                data_write(index + 0x24, 0x00001388); // set new fog distance to 5000
                data_write(index + 0x28, 0x09070800); // set new fog color to 9, 7, 8
                data_write(index + 0x2C, 0x00002EE0); // set far clipping plane to 12000
                break;
            case LEVELID_SURFACE2:
            case LEVELID_SURFACE2 + MP_P2:
            case LEVELID_SURFACE2 + MP_P3:
            case LEVELID_SURFACE2 + MP_P4:
                data_write(index + 0x24, 0x00001964); // set new fog distance to 6500
                break;
            case LEVELID_CRADLE:
            case LEVELID_CRADLE + MP_P2:
            case LEVELID_CRADLE + MP_P3:
            case LEVELID_CRADLE + MP_P4:
                data_write(index + 0x28, 0x6E819600); // set new fog color to 110, 129, 150
                data_write(index + 0x30, 0x40C00000); // set near clipping to 6
                break;
            case LEVELID_JUNGLE: // make jungle all misty and shit
                data_write(index + 0x04, 0x157C05DC);
                data_write(index + 0x08, 0x196403E8);
                data_write(index + 0x10, 0x797C7900);
                data_write(index + 0x24, 0x00001194);
                data_write(index + 0x28, 0x797C7900);
                data_write(index + 0x2C, 0x00001388);
                data_write(index + 0x30, 0x40C00000); // set near clipping to 6
                break;
            case LEVELID_ARCHIVES:
            case LEVELID_ARCHIVES + MP_P2:
            case LEVELID_ARCHIVES + MP_P3:
            case LEVELID_ARCHIVES + MP_P4:
                data_write(index + 0x04, 0x11940DAC); // set fog distance to 4500 and near fog to 3500
                data_write(index + 0x24, 0x00001194); // set new fog distance to 4500
                data_write(index + 0x2C, 0x000057E4); // set far clipping to 22500
                break;
            case LEVELID_TEMPLE:
            case LEVELID_TEMPLE + MP_P2:
            case LEVELID_TEMPLE + MP_P3:
            case LEVELID_TEMPLE + MP_P4:
                data_write(index + 0x04, 0x57E40BB8);
                data_write(index + 0x08, 0x0FA00258);
                data_write(index + 0x10, 0x10306001);
                data_write(index + 0x14, 0x138800FF);
                data_write(index + 0x18, 0xFFFF0000);
                data_write(index + 0x1C, 0xEC780000);
                data_write(index + 0x24, 0x000057E4);
                data_write(index + 0x28, 0x10306000);
                data_write(index + 0x2C, 0x000057E4);
                break;
            case LEVELID_CITADEL + MP_P2: // assign unused sky bank to bunker i
                data_write(index + 0x00, 0x0009000A); // set to bunker i
                data_write(index + 0x04, 0x3A9803E8);
                data_write(index + 0x08, 0x4E2002EE);
                data_write(index + 0x0C, 0x03E403E8);
                data_write(index + 0x10, 0x60608001);
                data_write(index + 0x14, 0x271000F0);
                data_write(index + 0x18, 0x781E0000);
                data_write(index + 0x1C, 0xFC180100);
                data_write(index + 0x20, 0xFFFF0700);
                data_write(index + 0x24, 0x0000AFC8);
                data_write(index + 0x28, 0xA4968200);
                data_write(index + 0x2C, 0x0000AFC8);
                data_write(index + 0x30, 0x40000000);
                data_write(index + 0x34, 0xB8D1B717);
                break;
            case LEVELID_CITADEL + MP_P3: // assign unused sky bank to silo
                data_write(index + 0x00, 0x0014000A); // set to silo
                data_write(index + 0x04, 0x271003E8);
                data_write(index + 0x08, 0x3A9802EE);
                data_write(index + 0x0C, 0x03E403E8);
                data_write(index + 0x10, 0x18181801);
                data_write(index + 0x14, 0x27100006);
                data_write(index + 0x18, 0x1B550000);
                data_write(index + 0x1C, 0xFC180100);
                data_write(index + 0x20, 0xFFFF0000);
                data_write(index + 0x24, 0x00002710);
                data_write(index + 0x28, 0x18181800);
                data_write(index + 0x2C, 0x00002710);
                data_write(index + 0x30, 0x40000000);
                data_write(index + 0x34, 0xB8D1B717);
                break;
            default:
                break;
        }
    }
    for(size_t index = FOG_SMALL_BANK_TABLE_START; index < FOG_SMALL_BANK_TABLE_END; index += 0x38) // load each small fog bank
    {
        switch(data_read(index))
        {
            case LEVELID_FRIGATE: // set water height to -253.f
            case LEVELID_FRIGATE + MP_P2:
            case LEVELID_FRIGATE + MP_P3:
            case LEVELID_FRIGATE + MP_P4:
                data_write(index + 0x20, 0xC37D0000);
                break;
            default:
                break;
        }
    }
}

static void set_audio_to_stereo(unsigned char *xexbin)
{
    printf("\n  Set X3DAudio To Stereo...");

    text_write(X3DAUDIO_INIT_FLAGS, 0x38600003); // force game to init as stereo
    text_write(X3DAUDIO_CALC_FLAGS, 0x38C00041); // remove doppler shift factor calculation audio pos function
}

static void extend_audio_distance(unsigned char *xexbin)
{
    printf("\n  Tweak X3DAudio Distance Scale...");

    text_write(X3DAUDIO_DISTANCE_HI, 0x3D4045CB); // lis r10, 0x45CB            set x3daudio CurveDistanceScaler to 6500.f
    text_write(X3DAUDIO_DISTANCE_LO, 0x614A2000); // ori r10, r10, 0x2000

    text_write(X3DAUDIO_DISTANCE_LO + 4, 0x915F005C); // stw r10, 0x005C (r31)  save to CurveDistanceScaler member
}

static void fix_door_volume_clamp(unsigned char *xexbin)
{
    printf("\n  Fixing Door Audio Volume Glitch...");

    text_write(DOOR_SFX_VOL_MIN, 0x38600001); // li r3, 0x0001                  set minimum volume for doors over 60 meters to mute (0 value overflows)
}

static void remove_beta_string_at_logo(unsigned char *xexbin)
{
    printf("\n  Removing ESRB/beta string for GoldenEye Logo...");

    text_write(LOGO_STRING, 0x60639CE3); // replace beta string with empty string
}

static void hardcode_near_clip_to_2(unsigned char *xexbin)
{
    printf("\n  Hardcoding Near Clip Value To 2.0f...");

    data_write(NEAR_CLIP, 0x40000000); // set near clipping to 1.f
    text_write(NEAR_CLIP_INIT, 0xC0070BF4); // set init near clipping to 1.f
    text_write(NEAR_CLIP_WRITE, NOOP); // remove write near clipping in set fog function
}

static void reset_internal_cheat_state_on_stage_load(unsigned char *xexbin)
{
    printf("\n  Adding Stage Load Cheat Reset Fix...");

    text_write_b(STAGE_LOAD_FUNC, hack_cur_addr); // b to hack                  jump to hacked segment function

    hack_add(0x3D608306); // lis r11, 0x8306
    hack_add(0x396B3318); // addi r11, r11, 0x3318                              set r11 to address 83063318 (active_cheat_array)
    hack_add(0x3B8B0051); // addi r28, r11, 0x0051                              set end address
    hack_add(0x3D400000); // lis r10, 0x0000                                    set r10 to 0

    hack_add(0x994B0000); // stb r10, 0x0000 (r11)                              clear byte
    hack_add(0x396B0001); // addi r11, r11, 0x0001                              increment to next byte

    hack_add(0x7F0BE000); // cmpw cr6, r11, r28                                 compare register
    hack_add(0x409AFFF4); // bne cr6, 0xFFF4                                    if not reached end of array, repeat

    hack_add(0x3D608242); // lis r11, 0x8242                                    reset bypass death tune flag (824203AC)
    hack_add(0x914B03AC); // stw r10, 0x03AC (r11)                              save 0 to bypass death tune flag

    hack_add(0x3D608200); // lis r11, 0x8200                                    add code that we overwrote with jump
    hack_add_b(STAGE_LOAD_FUNC + 4); // b from hack                             return

    text_write(FLOAT_AT_KILL_TUNE_FLAG, 0xC02B03A8); // lfs f1, 0x03A8 (r11)    load 5.f from another location (824203A8)

    data_write(BYPASS_MP_KILL_TUNE, 0x00000000); // clear flag
}

static void remove_original_graphics_mode_from_intro(unsigned char *xexbin)
{
    printf("\n  Removing Original Graphics Mode Switch During Intro...");

    //text_write(MENU_GFX_MODE_JMP + 0x00, 0x2F0B000C); // cmpwi cr6, r11, 0x000C   set menu ID to C (in-game)
    //text_write(MENU_GFX_MODE_JMP + 0x04, 0x419A0024); // beq cr6, 0x82099750      branch if in-game
    //text_write(MENU_GFX_MODE_JMP + 0x08, 0x39600000); // li r11, 0x0000           set r11 to 0
    //text_write(MENU_GFX_MODE_JMP + 0x0C, 0x3D4082F1); // lis r10, 0x82F1
    //text_write(MENU_GFX_MODE_JMP + 0x10, 0x996ADF11); // stb r11, 0xDF11 (r10)    hardcode graphics mode to new while not in-game
    //text_write(MENU_GFX_MODE_JMP + 0x14, NOOP); // continue function as expected

    // wreck didn't like the above code so just read from the current graphics flag
    text_write_b(MENU_GFX_MODE_JMP + 0x4, 0x82099750); // just always jump ahead and read the current graphics mode flag
}

static void remove_original_graphics_mode_blur(unsigned char *xexbin)
{
    printf("\n  Removing Original Graphics Blur...");

    text_write(BLUR_GRAPHICS_JMP, NOOP);
}

static void fix_network_armor_bug(unsigned char *xexbin)
{
    printf("\n  Add Network Armor Fix...");

    text_write_b(REMOTE_PLAYER_ARMOR_FIX, hack_cur_addr); // b to hack          jump to hacked segment function

    hack_add(0x917E0064); // stw r12, 0x0064 (r30)                              save prop flag
    hack_add(0x7C6C1B78); // or r12, r3, r3                                     backup r3 value
    hack_add(0x7FC3F378); // or r3, r30, r30                                    move r30 to r3 for function

    for(size_t index = 0; index < ARRAYSIZE(armor_fix_code); index++) // inject armor fix
    {
        hack_add(armor_fix_code[index]);
    }

    hack_add(0x7D836378); // or r3, r12, r12                                    restore r3 value
    hack_add_b(REMOTE_PLAYER_ARMOR_FIX + 4); // return
}

static void set_mp_sfx_to_use_player_location(unsigned char *xexbin)
{
    printf("\n  Set Various Sound Effects To Use 3D Location...");

    text_write_bl(PLAYER_GASP_SFX, hack_cur_addr);
    text_write_bl(PLAYER_SLAPPER_SFX, hack_cur_addr);
    text_write_bl(PLAYER_KNIFE_SFX, hack_cur_addr);
    text_write_bl(PLAYER_ITEM_EQUIP_SFX, hack_cur_addr);

    hack_add(0x7D8802A6); // mfspr r12, lr                                      move link register to r12
    hack_add(0x9181FFF8); // stw r12, 0xFFF8 (r1)                               save r12 to stack
    hack_add(0x9421FFA0); // stwu r1, 0xFFA0 (r1)                               save stack pointer

    hack_add(0x3D600000); // lis r11, 0x0000                                    set r11 to 0
    hack_add(0x3C608242); // lis r3, 0x8242                                     load bypass death tune flag address (824203AC)
    hack_add(0x916303AC); // stw r11, 0x03AC (r3)                               disable bypass death tune flag

    const unsigned int play_sfx_with_3d_location = hack_cur_addr;
    hack_add_bl(0x82144920); // bl 0x82144920                                   sfx play function
    hack_add(0x2B030000); // cmplwi cr6, r3, 0x0000                             if sound buffer is null, set cr6
    hack_add(0x419A004C); // beq cr6, 0x004C                                    if sound buffer is null, don't set location

    hack_add(0x3C808273); // lis r4, 0x8273                                     load current screen setting (8272B424)
    hack_add(0x8084B424); // lwz r4, 0xB424 (r4)
    hack_add(0x2F040003); // cmpwi cr6, r4, 0x0003                              compare and store results to cr6
    hack_add(0x409A003C); // bne cr6, 0x003C                                    branch if not equal to full screen mode (0: 4 way, 1: top/bottom, 2: left/right, 3: solo view)

    hack_add(0x3C8082F2); // lis r4, 0x82F2                                     set to player pointer array
    hack_add(0x8164FA9C); // lwz r11, 0xFA9C (r4)                               load p2 pointer (82F1FA9C) to r11
    hack_add(0x2F0B0000); // cmpwi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x409A0010); // bne cr6, 0x0010                                    branch if not equal to zero (multiplayer is active)

    hack_add(0xE964FAA0); // ld r11, 0xFAA0 (r4)                                load p3 and p4 pointers (82F1FAA0-7) to r11
    hack_add(0x2F2B0000); // cmpdi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x419A0020); // beq cr6, 0x0020                                    branch if equal to zero (singleplayer is active)

    hack_add(0x8084FAAC); // lwz r4, 0xFAAC (r4)                                load current player (82F1FAAC)
    hack_add(0x81640904); // lwz r11, 0x0904 (r4)                               load current viewport size (x+y)
    hack_add(0x2F0B0000); // cmpwi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x419A0010); // beq cr6, 0x0010                                    branch if equal to zero (current player is the active viewport - don't use 3d audio position)

    hack_add(0x808401AC); // lwz r4, 0x01AC (r4)                                load coord struct pointer
    hack_add(0x3884000C); // addi r4, r4, 0x000C                                offset to player location
    hack_add_bl(0x821448F8); // bl 0x821448F8                                   sfx set location function

    hack_add(0x38210060); // addi r1, r1, 0x0060                                restore stack pointer
    hack_add(0x8181FFF8); // lwz r12, 0x0008 (r1)                               restore link register
    hack_add(0x7D8803A6); // mtspr lr, r12                                      move r12 to link register
    hack_add(BLR); // return

    printf("\n  Trigger Gasps If Local Player Damaged Else Trigger Argh Sound Effect...");

    text_write_bl(PLAYER_REMOTE_GASP_SFX, hack_cur_addr);

    hack_add(0x7D8802A6); // mfspr r12, lr                                      move link register to r12
    hack_add(0x9181FFF8); // stw r12, 0xFFF8 (r1)                               save r12 to stack
    hack_add(0x9421FFA0); // stwu r1, 0xFFA0 (r1)                               save stack pointer

    hack_add(0x3CA08273); // lis r5, 0x8273                                     load current screen setting (8272B424)
    hack_add(0x80A5B424); // lwz r5, 0xB424 (r5)
    hack_add(0x2F050003); // cmpwi cr6, r5, 0x0003                              compare and store results to cr6
    hack_add(0x409A00B4); // bne cr6, 0x00B4                                    branch if not equal to full screen mode (0: 4 way, 1: top/bottom, 2: left/right, 3: solo view)

    hack_add(0x3CA082F2); // lis r5, 0x82F2                                     set to player pointer array
    hack_add(0x8165FA9C); // lwz r11, 0xFA9C (r5)                               load p2 pointer (82F1FA9C) to r11
    hack_add(0x2F0B0000); // cmpwi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x409A0010); // bne cr6, 0x0010                                    branch if not equal to zero (multiplayer is active)

    hack_add(0xE965FAA0); // ld r11, 0xFAA0 (r5)                                load p3 and p4 pointers (82F1FAA0-7) to r11
    hack_add(0x2F2B0000); // cmpdi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x419A0098); // beq cr6, 0x0098                                    branch if equal to zero (singleplayer is active - trigger gasp)

    hack_add(0x80A5FAAC); // lwz r5, 0xFAAC (r5)                                load current player (82F1FAAC)
    hack_add(0x81650904); // lwz r11, 0x0904 (r5)                               load current viewport size (x+y)
    hack_add(0x2F0B0000); // cmpwi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x419A0088); // beq cr6, 0x0088                                    branch if equal to zero (current player is the active viewport - trigger gasp)

    hack_add(0x816501AC); // lwz r11, 0x01AC (r5)                               load current player chr pointer
    hack_add(0x816B0008); // lwz r11, 0x0008 (r11)
    hack_add(0x2F0B0000); // cmpwi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x419A0078); // beq cr6, 0x0078                                    branch if equal to zero (model ptr is null - trigger gasp)

    hack_add(0x38A00001); // li r5, 0x0001                                      set r5 as true
    hack_add(0x3C608242); // lis r3, 0x8242                                     load bypass death tune flag address (824203AC)
    hack_add(0x90A303AC); // stw r5, 0x03AC (r3)                                enable bypass death tune flag
    hack_add(0x88AB000F); // lbz r5, 0x000F (r11)                               load chr->bodynum
    hack_add(0x1CA50024); // mulli r5, r5, 0x0024                               multiply to size of body info slot
    hack_add(0x3D608273); // lis r11, 0x8273                                    load body info array (82729020)
    hack_add(0x396B9020); // addi r11, r11, 0x9020
    hack_add(0x7D6B2A14); // add r11, r11, r5                                   add index to body array ptr
    hack_add(0x896B0018); // lbz r11, 0x0018 (r11)                              load gender for body slot
    hack_add(0x38A00000); // li r5, 0x0000                                      clear r5 for sfx function
    hack_add(0x2F0B0000); // cmpwi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x3D608306); // lis r11, 0x8306                                    load upper argh addresses (83062BF4/83062BF8)
    hack_add(0x419A0024); // beq cr6, 0x0024                                    branch if equal to zero (female)

    hack_add(0x808B2BF8); // lwz r4, 0x2BF8 (r11)                               load current male argh index
    hack_add(0x38840001); // addi r4, r4, 0x0001                                add 1 to current index
    hack_add(0x2F040018); // cmpwi cr6, r4, 0x0018                              compare and store results to cr6
    hack_add(0x40990008); // ble cr6, 0x0008                                    branch if less than or equal to 0x18 (overflowed)
    hack_add(0x38800000); // li r4, 0x0000                                      reset counter
    hack_add(0x908B2BF8); // stw r4, 0x2BF8 (r11)                               save current male argh index
    hack_add(0x38840086); // addi r4, r4, 0x0086                                offset to male argh range
    hack_add_b(play_sfx_with_3d_location); // jump to play sfx from player position

    hack_add(0x808B2BF4); // lwz r4, 0x2BF4 (r11)                               load current female argh index
    hack_add(0x38840001); // addi r4, r4, 0x0001                                add 1 to current index
    hack_add(0x2F040002); // cmpwi cr6, r4, 0x0002                              compare and store results to cr6
    hack_add(0x40990008); // ble cr6, 0x0008                                    branch if less than or equal to 0x02 (overflowed)
    hack_add(0x38800000); // li r4, 0x0000                                      reset counter
    hack_add(0x908B2BF4); // stw r4, 0x2BF4 (r11)                               save current female argh index
    hack_add(0x3884000D); // addi r4, r4, 0x000D                                offset to female argh range
    hack_add_b(play_sfx_with_3d_location); // jump to play sfx from player position

    hack_add(0x38A00000); // li r5, 0x0000                                      clear r5 for sfx function
    hack_add(0x3C608242); // lis r3, 0x8242                                     load bypass death tune flag address (824203AC)
    hack_add(0x90A303AC); // stw r5, 0x03AC (r3)                                disable bypass death tune flag
    hack_add_bl(0x82144920); // bl 0x82144920                                   sfx play function (and don't use player location)

    hack_add(0x38210060); // addi r1, r1, 0x0060                                restore stack pointer
    hack_add(0x8181FFF8); // lwz r12, 0x0008 (r1)                               restore link register
    hack_add(0x7D8803A6); // mtspr lr, r12                                      move r12 to link register
    hack_add(BLR); // return

    // recreate 821220dc for remote player gasp
    // female 83062bf4 (0-2)
    // male 83062bf8 (0-18)

    // body struct list 82729020
    // *void 0x00
    // pchar 0x04
    // *void 0x08
    // *void 0x0C
    // f32   0x10
    // f32   0x14
    // u8    0x18 gender
    // u8    0x19 contains/is head
    // u16   0x1A
    // u32   0x1C
    // u32   0x20

    // get chr ptr in bonddata
    // 1AC->8>
}

static void only_trigger_mp_death_tune_for_your_kills_and_yourself(unsigned char *xexbin)
{
    printf("\n  Only Trigger Death Tune For Your Deaths In Multiplayer...");

    text_write(CALL_MP_KILL_TUNE, 0x3C608242); // lis r3, 0x8242                load bypass death tune flag address (824203AC)
    text_write_bl(CALL_MP_KILL_TUNE + 4, hack_cur_addr); // bl to hack          jump to hack segment function

    hack_add(0x806303AC); // lwz r3, 0x03AC (r3)
    hack_add(0x2F030000); // cmpwi cr6, r3, 0x0000                              compare and store results to cr6
    hack_add(0x409A000C); // bne cr6, 0x000C                                    branch if not equal to zero (bypass flag == true)
    hack_add(0x38600006); // li r3, 0x0006                                      set argument for play music function
    hack_add_b(0x82144C60); // b 0x82144C60                                     play music function (trigger death tune)
    hack_add_b(CALL_MP_KILL_TUNE + 8); // return
}

static void add_sfx_to_remote_player_weapons(unsigned char *xexbin)
{
    printf("\n  Implement Weapon Firing Sound Effects For Network Players...");

    const unsigned int sfx_hack_func = hack_cur_addr;

    hack_add(0x7D8802A6); // mfspr r12, lr                                      move link register to r12
    hack_add_bl(0x823ED100); // bl 0x823ED100                                   save general registers (r14-r31)
    hack_add(0xF961FF60); // std r11, 0xFF60 (r1)                               save r11 to stack
    hack_add(0xF941FF58); // std r10, 0xFF58 (r1)                               save r10 to stack
    hack_add(0xF8A1FF50); // std r5, 0xFF50 (r1)                                save r5 to stack
    hack_add(0xF881FF48); // std r4, 0xFF48 (r1)                                save r4 to stack
    hack_add(0xF861FF40); // std r3, 0xFF40 (r1)                                save r3 to stack
    hack_add(0xF821FF38); // std r1, 0xFF38 (r1)                                save r1 to stack
    hack_add(0xD9C1FF30); // stfd f14, 0xFF30 (r1)                              save f14 to stack
    hack_add(0xD9A1FF28); // stfd f13, 0xFF28 (r1)                              save f13 to stack
    hack_add(0xD981FF20); // stfd f12, 0xFF20 (r1)                              save f12 to stack
    hack_add(0xD961FF18); // stfd f11, 0xFF18 (r1)                              save f11 to stack
    hack_add(0xD941FF10); // stfd f10, 0xFF10 (r1)                              save f10 to stack
    hack_add(0xD921FF08); // stfd f9, 0xFF08 (r1)                               save f9 to stack
    hack_add(0xD901FF00); // stfd f8, 0xFF00 (r1)                               save f8 to stack
    hack_add(0xD8E1FEF8); // stfd f7, 0xFEF8 (r1)                               save f7 to stack
    hack_add(0xD8C1FEF0); // stfd f6, 0xFEF0 (r1)                               save f6 to stack
    hack_add(0xD8A1FEE8); // stfd f5, 0xFEE8 (r1)                               save f5 to stack
    hack_add(0xD881FEE0); // stfd f4, 0xFEE0 (r1)                               save f4 to stack
    hack_add(0xD861FED8); // stfd f3, 0xFED8 (r1)                               save f3 to stack
    hack_add(0xD841FED0); // stfd f2, 0xFED0 (r1)                               save f2 to stack
    hack_add(0xD821FEC8); // stfd f1, 0xFEC8 (r1)                               save f1 to stack
    hack_add(0xD801FEC0); // stfd f10, 0xFEC0 (r1)                              save f0 to stack
    hack_add(0xF9A1FEB8); // ld r13, 0xFEB8 (r1)                                save r13 to stack
    hack_add(0xF981FEB0); // ld r12, 0xFEB0 (r1)                                save r12 to stack
    hack_add(0xF921FEA8); // ld r9, 0xFEA8 (r1)                                 save r9 to stack
    hack_add(0xF901FEA0); // ld r8, 0xFEA0 (r1)                                 save r8 to stack
    hack_add(0xF8E1FE98); // ld r7, 0xFE98 (r1)                                 save r7 to stack
    hack_add(0xF8C1FE90); // ld r6, 0xFE90 (r1)                                 save r6 to stack
    hack_add(0xF841FE88); // ld r2, 0xFE88 (r1)                                 save r2 to stack
    hack_add(0xF801FE80); // ld r0, 0xFE80 (r1)                                 save r0 to stack
    hack_add(0x3981FE80); // subi r12, r1, 0xFE80                               offset stack pointer
    hack_add_bl(0x823ED514); // bl 0x823ED514                                   save floating point registers
    hack_add(0x9421FC70); // stwu r1, 0xFC70 (r1)                               save stack pointer
    hack_add(0x7D7F5B78); // or r31, r11, r11                                   move r11 register (player pointer)

    hack_add(0x3D208306); // lis r9, 0x8306                                     load pause game flag (830633EC)
    hack_add(0x812933EC); // lwz r9, 0x33EC (r9)
    hack_add(0x2F090000); // cmpwi cr6, r9, 0x0000                              compare and store results to cr6
    hack_add(0x409A00DC); // bne cr6, 0x00DC                                    branch if not equal to zero (game paused)

    hack_add(0x815F2044); // lwz r10, 0x2044 (r31)                              load unknown player offset 0x2044
    hack_add(0x2F0A0000); // cmpwi cr6, r10, 0x0000                             compare and store results to cr6
    hack_add(0x409A00D0); // bne cr6, 0x00D0                                    branch if not equal to zero

    hack_add(0x3BA0145F); // li r29, 0x145F                                     set channel id (unsure, but set to this regardless)
    hack_add(0x3BDF0AFC); // addi r30, r31, 0x0AFC                              set sfx buffer used to free'd address
    hack_add(0x807F0AFC); // lwz r3, 0x0AFC (r31)                               load sound buffer 1
    hack_add(0x2F030000); // cmpwi cr6, r3, 0x0000                              compare and store results to cr6
    hack_add(0x419A0018); // beq cr6, 0x0018                                    branch if not null (skip to deactivate sound buffer 2)
    hack_add_bl(0x82144970); // bl 0x82144970                                   unknown function but this is needed for deactivating sfx slot
    hack_add(0x2F030000); // cmpwi cr6, r3, 0x0000                              compare and store results to cr6
    hack_add(0x419A000C); // beq cr6, 0x000C                                    branch if not null (skip to deactivate sound buffer 2)
    hack_add(0x807F0AFC); // lwz r3, 0x0AFC (r31)                               load sound buffer 1
    hack_add_bl(0x82144A08); // bl 0x82144A08                                   sfx deactivate func

    hack_add(0x807F0B00); // lwz r3, 0x0B00 (r31)                               load sound buffer 2
    hack_add(0x2F030000); // cmpwi cr6, r3, 0x0000                              compare and store results to cr6
    hack_add(0x419A0020); // beq cr6, 0x0020                                    branch if not null (don't deactivate sound buffer 2)
    hack_add_bl(0x82144970); // bl 0x82144970                                   unknown function but this is needed for deactivating sfx slot
    hack_add(0x2F030000); // cmpwi cr6, r3, 0x0000                              compare and store results to cr6
    hack_add(0x419A0014); // beq cr6, 0x0014                                    branch if not null (don't deactivate sound buffer 2)
    hack_add(0x807F0B00); // lwz r3, 0x0B00 (r31)                               load sound buffer 2
    hack_add_bl(0x82144A08); // bl 0x82144A08                                   sfx deactivate func
    hack_add(0x3BDF0B00); // addi r30, r31, 0x0B00                              set sfx buffer used to free'd address
    hack_add(0x3BA01461); // li r29, 0x1461                                     set channel id (unsure, but set to this regardless)

    hack_add(0x815F0928); // lwz r10, 0x0928 (r31)                              load current weapon
    hack_add(0x1D4A0038); // mulli r10, r10, 0x0038                             multiply to size of item stats entry
    hack_add(0x3D208242); // lis r9, 0x8242                                     set to weapon stats array (82421968)
    hack_add(0x39291968); // addi r9, r9, 0x1968
    hack_add(0x7D2A4A14); // add r9, r10, r9                                    add item offset entry to weapon stats array pointer
    hack_add(0x81490008); // lwz r10, 0x0008 (r9)                               load model flag from weapon entry
    hack_add(0x2F0A0000); // cmpwi cr6, r10, 0x0000                             compare and store results to cr6
    hack_add(0x409A0060); // bne cr6, 0x0060                                    branch if not equal to zero (no model associated with item, skip)

    hack_add(0x8149000C); // lwz r10, 0x000C (r9)                               load weapon stats ptr for weapon entry
    hack_add(0x2F0A0000); // cmpwi cr6, r10, 0x0000                             compare and store results to cr6
    hack_add(0x419A0054); // beq cr6, 0x0054                                    branch if ptr is null
    
    hack_add(0xA08A0026); // lhz r4, 0x0026 (r10)                               load sound effect id for weapon
    hack_add(0x2F040105); // cmpwi cr6, r4, 0x0105                              compare and store results to cr6
    hack_add(0x41990048); // bgt cr6, 0x0048                                    branch if sound id range is illegal

    hack_add(0x3D008306); // lis r8, 0x8306                                     load upper address for r3/r6 (83064DE0/83064DE8)
    hack_add(0x80684DE0); // lwz r3, 0x4DE0 (r8)
    hack_add(0x80C84DE8); // lwz r6, 0x4DE8 (r8)
    hack_add(0x7FC5F378); // or r5, r30, r30                                    move r30 register (available sound buffer)
    hack_add(0x3CE08200); // lis r7, 0x8200                                     set r7 to assert string address (820036A8)
    hack_add(0x38E736A8); // addi r7, r7, 0x36A8
    hack_add(0x7FA8EB78); // or r8, r29, r29                                    move r29 register (unknown argument)
    hack_add_bl(0x82144920); // bl 0x82144920                                   play sfx function
    hack_add(0x2B030000); // cmplwi cr6, r3, 0x0000                             if sound buffer is null, set cr6
    hack_add(0x419A0020); // beq cr6, 0x0020                                    if sound buffer is null, don't set location

    hack_add(0x3C808273); // lis r4, 0x8273                                     load current screen setting (8272B424)
    hack_add(0x8084B424); // lwz r4, 0xB424 (r4)
    hack_add(0x2F040003); // cmpwi cr6, r4, 0x0003                              compare and store results to cr6
    hack_add(0x409A0010); // bne cr6, 0x0010                                    branch if equal to full screen mode (0: 4 way, 1: top/bottom, 2: left/right, 3: solo view)

    hack_add(0x809F01AC); // lwz r4, 0x01AC (r31)                               load coord struct pointer
    hack_add(0x3884000C); // addi r4, r4, 0x000C                                offset to player location
    hack_add_bl(0x821448F8); // bl 0x821448F8                                   sfx set location function

    hack_add(0x38210390); // addi r1, r1, 0x0390                                offset stack pointer
    hack_add(0x3981FE80); // subi r12, r1, 0xFE80                               offset stack pointer
    hack_add_bl(0x823ED560); // bl 0x823ED560                                   load floating point registers
    hack_add(0xE961FF60); // ld r11, 0xFF60 (r1)                                load r11 from stack
    hack_add(0xE941FF58); // ld r10, 0xFF58 (r1)                                load r10 from stack
    hack_add(0xE8A1FF50); // ld r5, 0xFF50 (r1)                                 load r5 from stack
    hack_add(0xE881FF48); // ld r4, 0xFF48 (r1)                                 load r4 from stack
    hack_add(0xE861FF40); // ld r3, 0xFF40 (r1)                                 load r3 from stack
    hack_add(0xE821FF38); // ld r1, 0xFF38 (r1)                                 load r1 from stack
    hack_add(0xC9C1FF30); // lfd f14, 0xFF30 (r1)                               load f14 from stack
    hack_add(0xC9A1FF28); // lfd f13, 0xFF28 (r1)                               load f13 from stack
    hack_add(0xC981FF20); // lfd f12, 0xFF20 (r1)                               load f12 from stack
    hack_add(0xC961FF18); // lfd f11, 0xFF18 (r1)                               load f11 from stack
    hack_add(0xC941FF10); // lfd f10, 0xFF10 (r1)                               load f10 from stack
    hack_add(0xC921FF08); // lfd f9, 0xFF08 (r1)                                load f9 from stack
    hack_add(0xC901FF00); // lfd f8, 0xFF00 (r1)                                load f8 from stack
    hack_add(0xC8E1FEF8); // lfd f7, 0xFEF8 (r1)                                load f7 from stack
    hack_add(0xC8C1FEF0); // lfd f6, 0xFEF0 (r1)                                load f6 from stack
    hack_add(0xC8A1FEE8); // lfd f5, 0xFEE8 (r1)                                load f5 from stack
    hack_add(0xC881FEE0); // lfd f4, 0xFEE0 (r1)                                load f4 from stack
    hack_add(0xC861FED8); // lfd f3, 0xFED8 (r1)                                load f3 from stack
    hack_add(0xC841FED0); // lfd f2, 0xFED0 (r1)                                load f2 from stack
    hack_add(0xC821FEC8); // lfd f1, 0xFEC8 (r1)                                load f1 from stack
    hack_add(0xC801FEC0); // lfd f0, 0xFEC0 (r1)                                load f0 from stack
    hack_add(0xE9A1FEB8); // ld r13, 0xFEB8 (r1)                                load r13 from stack
    hack_add(0xE981FEB0); // ld r12, 0xFEB0 (r1)                                load r12 from stack
    hack_add(0xE921FEA8); // ld r9, 0xFEA8 (r1)                                 load r9 from stack
    hack_add(0xE901FEA0); // ld r8, 0xFEA0 (r1)                                 load r8 from stack
    hack_add(0xE8E1FE98); // ld r7, 0xFE98 (r1)                                 load r7 from stack
    hack_add(0xE8C1FE90); // ld r6, 0xFE90 (r1)                                 load r6 from stack
    hack_add(0xE841FE88); // ld r2, 0xFE88 (r1)                                 load r2 from stack
    hack_add(0xE801FE80); // ld r0, 0xFE80 (r1)                                 load r0 from stack
    hack_add_b(0x823ED150); // b 0x823ED150                                     load general registers (r14-r31) and return

    text_write_b(PLAYER_REMOTE_FIRE_GUN, hack_cur_addr); // b to hack           jump to hack segment function

    hack_add_bl(sfx_hack_func); // bl to hack                                   jump to hack segment function
    hack_add(0x7D6A5A14); // add r11, r10, r11
    hack_add_b(PLAYER_REMOTE_FIRE_GUN + 4); // return

    // r11 contains pointer of remote player firing
    // player offset 928 = current weapon
    // item database 82421968, entry size 38. offset 8 != 0 means use default stats (no sfx). if not using default stats check offset c for item stats ptr (null == no sfx)
    // item stats ptr offset 26 == sound id
}

static void disable_doors_autoclosing_on_mp(unsigned char *xexbin)
{
    printf("\n  Disable Doors From Auto-Closing For Network Multiplayer...");

    text_write_b(MP_CHECK_DOOR_TIME, hack_cur_addr); // b to hack               jump to hacked segment function

    hack_add(0x3D60830D); // lis r11, 0x830D                                    load current network multiplayer flag (830CAEA0)
    hack_add(0x896BAEA0); // lbz r11, 0xAEA0 (r11)
    hack_add(0x2F0B0000); // cmpwi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x409A000C); // bne cr6, 0x000C                                    branch if not equal to zero (network session active)

    hack_add(0x817E00E8); // lwz r11, 0x00E8 (r30)                              load tick value for door
    hack_add_b(MP_CHECK_DOOR_TIME + 4); // return
    hack_add(0x39600000); // li r11, 0x0000                                     set r11 to 0 (never autoclose door)
    hack_add_b(MP_CHECK_DOOR_TIME + 4); // return
}

static void disable_player_collisions_for_network_mp(unsigned char *xexbin)
{
    printf("\n  Disable Player Collisions For Network Multiplayer...");

    text_write_b(PLAYER_COLLISION_CALC, hack_cur_addr); // b to hack            jump to hacked segment function

    hack_add(0x3D60830D); // lis r11, 0x830D                                    load current network multiplayer flag (830CAEA0)
    hack_add(0x896BAEA0); // lbz r11, 0xAEA0 (r11)
    hack_add(0x2F0B0000); // cmpwi cr6, r11, 0x0000                             compare and store results to cr6
    hack_add(0x409A0008); // bne cr6, 0x0008                                    branch if not equal to zero (network session active)

    hack_add_b(0x820B3E90); // b 0x820B3E90                                     calculate player collision (only for local splitscreen multiplayer)

    hack_add(BLR); // return
}

static void fix_watch_volume_sliders_range(unsigned char *xexbin)
{
    printf("\n  Increase Watch Volume Sliders Range...");

    text_write_bl(WATCH_MUSIC_VOL_READ1, hack_cur_addr); // bl to hack
    text_write_bl(WATCH_MUSIC_VOL_READ2, hack_cur_addr); // bl to hack

    hack_add(0x88630295); // lbz r3, 0x0295 (r3)                                load music volume
    hack_add(0x7C630E70); // srawi r3, r3, 0x0001                               divided loaded volume value by 2
    hack_add(BLR); // return

    text_write_bl(WATCH_MUSIC_VOL_SAVE, hack_cur_addr); // bl to hack

    hack_add(0x7C842214); // add r4, r4, r4                                     double music volume argument
    hack_add_b(0x82184E18); // b 0x82184E18                                     jump to save music volume function

    text_write_bl(WATCH_SFX_VOL_READ1, hack_cur_addr); // bl to hack
    text_write_bl(WATCH_SFX_VOL_READ2, hack_cur_addr); // bl to hack

    hack_add(0x88630294); // lbz r3, 0x0294 (r3)                                load fx volume
    hack_add(0x7C630E70); // srawi r3, r3, 0x0001                               divided loaded volume value by 2
    hack_add(BLR); // return

    text_write_bl(WATCH_SFX_VOL_SAVE, hack_cur_addr); // bl to hack

    hack_add(0x7C842214); // add r4, r4, r4                                     double fx volume argument
    hack_add_b(0x82184E48); // b 0x82184E48                                     jump to save fx volume function
}

static void fix_golden_gun_respawn_visiblity_flag(unsigned char *xexbin)
{
    printf("\n  Fix Golden Gun Respawn Not Becoming Visible...");

    text_write(WEAPON_RESPAWN_GG_CHECK, 0x3C6082F6); // lis r3, 0x82F6          load current multiplayer scenario (82F61084)
    text_write_bl(WEAPON_RESPAWN_GG_CHECK + 0x4, hack_cur_addr); // bl to hack

    hack_add(0x80631084); // lwz r3, 0x1084 (r3)
    hack_add(0x2F030003); // cmpwi cr6, r3, 0x0003                              compare if scenario is equal to 3 (MWTGG)
    hack_add(0x419A0008); // beq cr6, 0x0008                                    if index == 3, continue
    hack_add_b(WEAPON_RESPAWN_GG_CHECK + 0xC); // return

    hack_add(0x2F0B0013); // cmpwi cr6, r11, 0x0013                             compare if current weapon respawning is golden gun
    hack_add(0x419A0008); // beq cr6, 0x0008                                    if weapon id == 13, continue
    hack_add_b(WEAPON_RESPAWN_GG_CHECK + 0xC); // return

    hack_add_b(WEAPON_RESPAWN_GG_CHECK + 0x8); // return
}

static void fix_water_rendering_for_new_graphics(unsigned char *xexbin)
{
    printf("\n  Fix Water Rendering For New Graphics Mode...");

    text_write(WATER_REND_GFX_MODE_CHECK, NOOP); // don't check graphics mode for water rendering
}

static void new_level_entry_init(unsigned char *xexbin)
{
    printf("\n  Creating Setup Pointer List For Modders...");

    data_write(LVL_ENTRY_INIT_FUNC_CALL, 0); // remove call to level entry function (set to NULL)

    for(size_t index = LVL_ENTRY_INIT_FUNC; index < LVL_ENTRY_INIT_FUNC_END; index += 4) // clear rest of function
    {
        text_write(index, 0);
    }

    for(size_t index = 0; index < ARRAYSIZE(level_entry); index++) // populate level entry table with valid data (as init function has been removed)
    {
        data_write(LVL_ENTRY_TABLE + (index * 4), level_entry[index]);
    }
}

static void fix_item_watch_positions(unsigned char *xexbin)
{
    printf("\n  Fixing Item Watch Position...");

    data_write(GUN_CLIPBOARD_POS + 0x00, 0x41800000); // fix clipboard watch position
    data_write(GUN_CLIPBOARD_POS + 0x08, 0x43ED0000);
    data_write(GUN_CLIPBOARD_POS + 0x1C, 0xC0C00000);
    data_write(GUN_CLIPBOARD_POS + 0x20, 0x43AF0000);

    data_write(GUN_CIRCUITBOARD_POS + 0x00, 0x41F29AEA); // fix circuitboard watch position
    data_write(GUN_CIRCUITBOARD_POS + 0x04, 0x3F3A5311);
    data_write(GUN_CIRCUITBOARD_POS + 0x08, 0x42C43EDF);

    data_write(GUN_BOLT_KEY_POS + 0x00, 0xC2D20000); // fix bolt key watch position
    data_write(GUN_BOLT_KEY_POS + 0x04, 0xC3E10000);
    data_write(GUN_BOLT_KEY_POS + 0x08, 0x43C80000);
    data_write(GUN_BOLT_KEY_POS + 0x18, 0xC3E10000);
    data_write(GUN_BOLT_KEY_POS + 0x1C, 0xC2FA0000);
    data_write(GUN_BOLT_KEY_POS + 0x20, 0x43C80000);

    data_write(GUN_YALE_KEY_POS + 0x00, 0x43F30000); // fix yale key watch position
    data_write(GUN_YALE_KEY_POS + 0x04, 0xC3F60000);
    data_write(GUN_YALE_KEY_POS + 0x08, 0x43020000);
    data_write(GUN_YALE_KEY_POS + 0x18, 0xC3F00000);
    data_write(GUN_YALE_KEY_POS + 0x1C, 0x43EB0000);
    data_write(GUN_YALE_KEY_POS + 0x20, 0x43960000);
}

static void fix_setups(unsigned char *xexbin)
{
    printf("\n  Applying Setup Fixes...");

    data_write(RUNWAY_AILIST_0403, 0x00002902); // fix runway guards not spawning with weapons

    for(size_t index = 0; index < ARRAYSIZE(runwaydoorpads); index++) // fix runway door pads floating off ground
    {
        xexbin[DATA(RUNWAY_PADS_DOORS + index)] = runwaydoorpads[index];
    }

    data_write(SURFACE1_AILIST_0421 + 0x00, 0x00C10780); // fix surface i guards not spawning with dual flagged klobbs
    data_write(SURFACE1_AILIST_0421 + 0x10, 0xC1079000);

    data_write(JUNGLE_AILIST_0402 + 0x00, 0x9D00000A); // fix Xenia triggering earlier thanks to new fog settings
    data_write(JUNGLE_AILIST_0402 + 0x04, 0x00020803);
    data_write(JUNGLE_AILIST_0402 + 0x08, 0x55005C2C);
    data_write(JUNGLE_AILIST_0402 + 0x0C, 0xF8FD2C3C);
    data_write(JUNGLE_AILIST_0402 + 0x10, 0x2C010802);

    for(size_t index = 0; index < ARRAYSIZE(jungle_ai_list_040D); index++) // fix guards detecting Bond during start
    {
        xexbin[DATA(JUNGLE_AILIST_040D + index)] = jungle_ai_list_040D[index];
    }

    data_write(STATUE_AILIST_0423, 0xFFFF0710); // fix guard in end cinema cutscene holding weapon incorrectly (set mirror flag for animation)

    data_write(STATUE_AILIST_1000 + 0x00, 0x4A003B23); // fix Trev spawning thanks to new fog settings (force spawn flag)
    data_write(STATUE_AILIST_1000 + 0x14, 0x1023AD54);
    data_write(STATUE_AILIST_1000 + 0x3C, 0x00001123);
    data_write(STATUE_AILIST_1000 + 0x4C, 0x00001123);
    data_write(STATUE_AILIST_1000 + 0x5C, 0x00001123);
    data_write(STATUE_AILIST_1000 + 0x6C, 0x00001123);

    data_write(EGYPT_MP_AMMO, 0x8200B7C8); // fix ammo box pad plink (sets to p470a)

    data_write(DAM_GATE_PADLOCK, 0x01000009); // make padlock drop after destroying

    data_write(SURFACE1_BOOKSHELF1_FLAG, 0x000201E1); // make bookshelves invincible like archives/surface ii (game glitches out when destroyed)
    data_write(SURFACE1_BOOKSHELF2_FLAG, 0x000201E1);

    data_write(SURFACE1_PAD_ERROR + 0x00, 0xC59B4000); // fix surface i/ii pad links for exterior radar railing
    data_write(SURFACE2_PAD_ERROR + 0x00, 0xC59B4000);
    data_write(SURFACE1_PAD_ERROR + 0x1C, 0x82012798);
    data_write(SURFACE2_PAD_ERROR + 0x1C, 0x82012798);

    data_write(CONTROL_DK5_TREV, 0x00004000); // set Trev's DK5 as right hand held

    data_write(ARCHIVES_MP_AMMO_PAD, 0x43988000); // shift pad over
    data_write(ARCHIVES_MP_AMMO, 0x000527FC); // set to pad 27FC

    data_write(TEMPLE_MP_AMMO_PAD + 0x00, 0x43730000); // shift pad over
    data_write(TEMPLE_MP_AMMO_PAD + 0x08, 0x43490000);
    data_write(TEMPLE_MP_AMMO_PAD + 0x10, 0x3F800000);
    data_write(TEMPLE_MP_AMMO_PAD + 0x18, 0xBF800000);
    data_write(TEMPLE_MP_AMMO_PAD + 0x20, 0x00000000);
    data_write(TEMPLE_MP_AMMO + 0x0000, 0x0005001C);
    data_write(TEMPLE_MP_AMMO + 0x00B4, 0x0005000E);
    data_write(TEMPLE_MP_AMMO + 0x01F4, 0x0005001F);
    data_write(TEMPLE_MP_AMMO + 0x02A8, 0x00050020);
    data_write(TEMPLE_MP_AMMO + 0x03E8, 0x00050013);
    data_write(TEMPLE_MP_AMMO + 0x049C, 0x00050014);
    data_write(TEMPLE_MP_AMMO + 0x0A78, 0x00050010);

    data_write(SILO_ARMOR + 0x00, 0x007400E3); // changed armor model to match amount given in Silo
    data_write(SILO_ARMOR + 0x08, 0x00000000); // set half armor in Silo to also load on 007 diff
    data_write(STATUE_ARMOR_FLAG, 0x00000000); // set armor in Statue to also load on 007 diff
    data_write(JUNGLE_ARMOR_FLAG, 0x00000000); // set armor in Jungle to also load on 007 diff
    data_write(FRIGATE_ARMOR, 0x00732745); // moved hidden armor in Frigate
    data_write(SURFACE2_ARMOR_FLAG, 0x000000C0); // update half armor in Surface ii to be in SA diff

    for(size_t index = 0; index < ARRAYSIZE(surface1cabindoors); index++) // fix surface i cabin door from not opening
    {
        xexbin[DATA(SURFACE1_CABIN_DOOR_FIX + index)] = surface1cabindoors[index];
    }
    for(size_t index = 0; index < ARRAYSIZE(surface2cabindoors); index++) // fix surface ii cabin door from not opening
    {
        xexbin[DATA(SURFACE2_CABIN_DOOR_FIX + index)] = surface2cabindoors[index];
    }

    data_write(CONTROL_BLAST_DOOR_FIX, 0x00040004); // set blast door on first floor flag to squish on open

    data_write(FACILITY_DOAK_SPAWN1, 0x233300EE); // set Doak spawn to use Dave head (33)
    data_write(FACILITY_DOAK_SPAWN2, 0x233300EF);
    data_write(FACILITY_DOAK_SPAWN3, 0x233300F0);
    data_write(FACILITY_DOAK_SPAWN4, 0x233300F1);
    data_write(FACILITY_DOAK_SPAWN5, 0x233300F2);
    data_write(FACILITY_DOAK_SPAWN6, 0x233300F3);
    data_write(FACILITY_DOAK_SPAWN7, 0xBD233300);

    data_write(JUNGLE_NATALYA_CMAG, 0x00004000); // set Natalya to drop magnum on death (Jungle)
    data_write(CONTROL_NATALYA_CMAG, 0x00004000); // set Natalya to drop magnum on death (Control)

    for(size_t index = 0; index < ARRAYSIZE(dam_intro_fix); index++) // fix dam intro camera
    {
        xexbin[DATA(DAM_INTRO_COMMAND + index)] = dam_intro_fix[index];
    }

    for(size_t index = 0; index < ARRAYSIZE(egypt_puzzle_glass_door_fix); index++) // fix egypt's puzzle glass doors
    {
        xexbin[DATA(EGYPT_PUZZLE_GLASS_DOORS + index)] = egypt_puzzle_glass_door_fix[index];
    }

    data_write(BASEMENT_FLAG_PAD + 0x00, 0xC5198000); // fix pad position for basement flag
    data_write(BASEMENT_FLAG_PAD + 0x10, 0xBF800000);
    data_write(BASEMENT_FLAG_PAD + 0x18, 0x00000000);
    data_write(BASEMENT_FLAG_PAD + 0x1C, 0x820154A0);

    data_write(LIBRARY_FLAG_PROP, 0x014D0029); // set library flag to use pad 0029

    data_write(CONTROL_PAD_27BC + 0, 0x10000B62); // fix glass bullet holes for control setup
    data_write(CONTROL_PAD_27BC + 4, 0x00304000);
    data_write(CONTROL_PAD_27C9 + 0, 0x10000B62);
    data_write(CONTROL_PAD_27C9 + 4, 0x00304000);
    data_write(CONTROL_PAD_27BE + 0, 0x10000B62);
    data_write(CONTROL_PAD_27BE + 4, 0x00304000);
    data_write(CONTROL_PAD_27BA + 0, 0x10000B62);
    data_write(CONTROL_PAD_27BA + 4, 0x00304000);
    data_write(CONTROL_PAD_27BD + 0, 0x10000B62);
    data_write(CONTROL_PAD_27BD + 4, 0x00304000);
    data_write(CONTROL_PAD_27CA + 0, 0x10000B62);
    data_write(CONTROL_PAD_27CA + 4, 0x00304000);
    data_write(CONTROL_PAD_27BB + 0, 0x10000B62);
    data_write(CONTROL_PAD_27BB + 4, 0x00304000);

    data_write(FACILITY_SP_PAD_2772 + 0, 0x10000262); // fix glass bullet holes for sp facility setup
    data_write(FACILITY_SP_PAD_2772 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2773 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2773 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2775 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2775 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2776 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2776 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2777 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2777 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2778 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2778 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2779 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2779 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_277A + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_277A + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_277D + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_277D + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_277E + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_277E + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_277F + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_277F + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2780 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2780 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2791 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2791 + 4, 0x00304000);
    data_write(FACILITY_SP_PAD_2792 + 0, 0x10000262);
    data_write(FACILITY_SP_PAD_2792 + 4, 0x00304000);

    data_write(FACILITY_MP_PAD_2772 + 0, 0x10000262); // fix glass bullet holes for mp facility setup
    data_write(FACILITY_MP_PAD_2772 + 4, 0x00304000);
    data_write(FACILITY_MP_PAD_2773 + 0, 0x10000262);
    data_write(FACILITY_MP_PAD_2773 + 4, 0x00304000);
    data_write(FACILITY_MP_PAD_2775 + 0, 0x10000262);
    data_write(FACILITY_MP_PAD_2775 + 4, 0x00304000);
    data_write(FACILITY_MP_PAD_2776 + 0, 0x10000262);
    data_write(FACILITY_MP_PAD_2776 + 4, 0x00304000);
    data_write(FACILITY_MP_PAD_2777 + 0, 0x10000262);
    data_write(FACILITY_MP_PAD_2777 + 4, 0x00304000);
    data_write(FACILITY_MP_PAD_2778 + 0, 0x10000262);
    data_write(FACILITY_MP_PAD_2778 + 4, 0x00304000);
    data_write(FACILITY_MP_PAD_2779 + 0, 0x10000262);
    data_write(FACILITY_MP_PAD_2779 + 4, 0x00304000);
    data_write(FACILITY_MP_PAD_277A + 0, 0x10000262);
    data_write(FACILITY_MP_PAD_277A + 4, 0x00304000);

    data_write(BUNKER2_STAIRS_TILE1, 0x07550CCD); // fix bunker ii's weird nook on stairs
    data_write(BUNKER2_STAIRS_TILE2, 0x075B0CC4);
}

static void split_surface_string(unsigned char *xexbin)
{
    printf("\n  Splitting Surface String Into sf1/sf2...");

    rodata_write(SURFACE_STRING + 0, 0x73663100); // "sf1/0"
    rodata_write(SURFACE_STRING + 4, 0x73663200); // "sf2/0"

    data_write(LVL_ENTRY_TABLE_SLOT(0x17) + 0x4, data_read(LVL_ENTRY_TABLE_SLOT(0x17) + 0x4) + 0x4); // set surface ii to second substring
}

static void increase_mp_characters(unsigned char *xexbin)
{
    printf("\n  Increasing MP Characters Total And Changing Dev Characters...");

    text_write(MP_CHARS_UNLOCK1, 0x39600032); // li r11, 0x0032                 set character's total to 32 after completing cradle on 00A
    text_write(MP_CHARS_UNLOCK2, 0x39600032); // li r11, 0x0032

    for(size_t index = 0; index < ARRAYSIZE(mpchars_altsandbonus); index++) // biker uses the brown Siberian Guard jacket (most similar to film), and Terrorist has green camo. Also adjusted the POV for Cuba Natalya, since she is shorter
    {
        xexbin[DATA(MP_CHARS_STRUCT + index)] = mpchars_altsandbonus[index];
    }
}

static void fix_mp_weapon_sizes(unsigned char *xexbin)
{
    printf("\n  Tweaked MP Weapon Set Sizes For Props...");

    for(size_t index = 0; index < ARRAYSIZE(mpweaponsets); index++)
    {
        xexbin[DATA(MP_WEAPON_SETS + index)] = mpweaponsets[index];
    }
}

static void tweak_natalya_scale(unsigned char *xexbin)
{
    printf("\n  Tweaked Natalya's Scale And POV...");

    data_write(CHAR_NATALYA_SCALE, 0x3F733333); // set natalya's character scale to 0.9499999881
    data_write(CHAR_NATALYA_MP_POV, 0x3F6EB1C4); // set natalya's pov MP height to 0.9323999882
}

static void tweak_mp_level_names(unsigned char *xexbin)
{
    printf("\n  Tweaked MP Level Names...");

    for(size_t index = 0; index < ARRAYSIZE(mplevellist); index++)
    {
        xexbin[DATA(MP_LEVEL_LIST + index)] = mplevellist[index];
    }
}

static void silo_aztec_fix_shuttle_culling(unsigned char *xexbin)
{
    printf("\n  Fixing Silo/Aztec Shuttle Portal Error...");

    for(size_t index = 0; index < ARRAYSIZE(siloicbmview); index++)
    {
        xexbin[DATA(SILO_BG_DATA + index)] = siloicbmview[index];
    }

    for(size_t index = 0; index < ARRAYSIZE(aztecshuttlearea); index++)
    {
        xexbin[DATA(AZTEC_BG_DATA + index)] = aztecshuttlearea[index];
    }
}

static void fix_level_portals_bg_data(unsigned char *xexbin)
{
    printf("\n  Fixing Level Portals/BG Data...");

    for(size_t index = 0; index < ARRAYSIZE(frigatebg_fixportals); index++)
    {
        xexbin[DATA(FRIGATE_BG_DATA + index)] = frigatebg_fixportals[index];
    }

    for(size_t index = 0; index < ARRAYSIZE(surfacebg_portaladditions); index++)
    {
        xexbin[DATA(SURFACE_BG_DATA + index)] = surfacebg_portaladditions[index];
    }
}

static void replace_cuba_with_jungle(unsigned char *xexbin)
{
    printf("\n  Replacing Cuba With Jungle...");

    data_write(LVL_ENTRY_TABLE_SLOT(0x22) + 0x14, data_read(LVL_ENTRY_TABLE_SLOT(0x11) + 0x14)); // set cuba's stan to jungle
    data_write(LVL_ENTRY_TABLE_SLOT(0x22) + 0x20, data_read(LVL_ENTRY_TABLE_SLOT(0x11) + 0x20)); // set cuba's bg to jungle

    for(size_t index = FOG_BANK_TABLE_START; index < FOG_BANK_TABLE_END; index += 0x38) // load each fog bank
    {
        if((data_read(index) == 0) || (data_read(index) == -1)) // reached end of list
            break;
        switch((data_read(index) & 0xFFFF0000) >> 16)
        {
            case LEVELID_CITADEL: // used for new cuba->jungle ending
                data_write(index + 0x00, 0x0036000A);
                data_write(index + 0x04, 0x177005DC);
                data_write(index + 0x08, 0x177003E8);
                data_write(index + 0x10, 0x30401000);
                data_write(index + 0x18, 0xFFFF0000);
                data_write(index + 0x1C, 0x00000000);
                data_write(index + 0x20, 0x00000000);
                data_write(index + 0x24, 0x00001770);
                data_write(index + 0x28, 0x30401000);
                data_write(index + 0x2C, 0x00001770);
            default:
                break;
        }
    }
    for(size_t index = FOG_SMALL_BANK_TABLE_START; index < FOG_SMALL_BANK_TABLE_END; index += 0x38) // load each small fog bank
    {
        switch(data_read(index))
        {
            case LEVELID_CUBA: // used for new cuba->jungle ending (note: this is a small sky struct format, fine to replace the ID like this)
                data_write(index + 0x00, 0x00000037);
                break;
            default:
                break;
        }
    }

    for(size_t index = 0; index < ARRAYSIZE(endcreditspads); index++) // inject new pad positions for cuba ending
    {
        xexbin[DATA(CUBA_PADS + index)] = endcreditspads[index];
    }
}

static void set_surface_to_use_same_stan(unsigned char *xexbin)
{
    printf("\n  Set Surface I/II To Use Same Stan File...");

    data_write(LVL_ENTRY_TABLE_SLOT(0x17) + 0x14, data_read(LVL_ENTRY_TABLE_SLOT(0x10) + 0x14)); // set surface ii's stan to surface i
}

static void make_mission_time_full_precision(unsigned char *xexbin)
{
    printf("\n  Replace end mission timer display...");

    text_write(MISSION_TIME_FULL + 0x00, 0x3D4082F2); // lis r10, 0x82F2        load mission time (82F1F41C)

    text_write(MISSION_TIME_FULL + 0x08, 0x80CAF41C); // lwz r6, 0xF41C (r10)
    text_write(MISSION_TIME_FULL + 0x0C, 0x7CC607B4); // extsw r6, r6           extend sign
    text_write(MISSION_TIME_FULL + 0x10, 0xF8C10070); // std r6, 0x0070 (r1)    store double word int
    text_write(MISSION_TIME_FULL + 0x14, 0xC8010070); // lfd f0, 0x0070 (r1)    load double word
    text_write(MISSION_TIME_FULL + 0x18, 0xFC00069C); // fcfid f0, f0           convert int to double
    text_write(MISSION_TIME_FULL + 0x1C, 0xFC000018); // frsp f0, f0            convert double to floating point
    text_write(MISSION_TIME_FULL + 0x20, 0xC025D36C); // lfs f1, 0xD36C (r5)    load 60.f (82003688)
    text_write(MISSION_TIME_FULL + 0x24, 0xEC000824); // fdivs f0, f0, f1       divide ticks by 60
    text_write(MISSION_TIME_FULL + 0x28, 0xD8010070); // stfd f0, 0x0070 (r1)   store float to stack

    text_write(MISSION_TIME_FULL + 0x2C, 0xE8C10070); // ld r6, 0x0070 (r1)     load float as argument
    text_write(MISSION_TIME_FULL + 0x30, 0x38A5AC1C); // addi r5, r5, 0xAC1C    offset sprintf string argument to float display (%f - 82000F38)
}

static void do_hack_tasks(unsigned char *xexbin, const int stereo_flag)
{
    //xexbin[0x71E5B3] = LEVELID_COMPLEX; // change sp facility level id (for testing reasons)
    remove_debug_button(xexbin);
    flash_health_bars_on_armor_pickup(xexbin);
    remove_xbox_live_and_leaderboard(xexbin);
    set_unequal_heights_on_by_default(xexbin);
    fix_sentry_drones_ignoring_activate_state(xexbin);
    fix_remote_mine_sfx(xexbin);

    make_mp_always_use_p2_fog(xexbin);
    adjust_fog_bank(xexbin);
    if(stereo_flag)
        set_audio_to_stereo(xexbin);
    extend_audio_distance(xexbin);
    fix_door_volume_clamp(xexbin);
    remove_beta_string_at_logo(xexbin);
    hardcode_near_clip_to_2(xexbin);
    reset_internal_cheat_state_on_stage_load(xexbin);
    remove_original_graphics_mode_from_intro(xexbin);
    remove_original_graphics_mode_blur(xexbin);
    fix_network_armor_bug(xexbin);
    set_mp_sfx_to_use_player_location(xexbin);
    only_trigger_mp_death_tune_for_your_kills_and_yourself(xexbin);
    add_sfx_to_remote_player_weapons(xexbin);
    disable_doors_autoclosing_on_mp(xexbin);
    disable_player_collisions_for_network_mp(xexbin);
    fix_watch_volume_sliders_range(xexbin);
    fix_golden_gun_respawn_visiblity_flag(xexbin);
    fix_water_rendering_for_new_graphics(xexbin);

    new_level_entry_init(xexbin);
    fix_item_watch_positions(xexbin);
    fix_setups(xexbin);
    split_surface_string(xexbin);
    increase_mp_characters(xexbin);
    fix_mp_weapon_sizes(xexbin);
    tweak_natalya_scale(xexbin);
    tweak_mp_level_names(xexbin);
    silo_aztec_fix_shuttle_culling(xexbin);
    fix_level_portals_bg_data(xexbin);
    replace_cuba_with_jungle(xexbin);
    set_surface_to_use_same_stan(xexbin);

    //make_mission_time_full_precision(xexbin);
}

char patch_fyodorovna_xex(unsigned char *xexbin, const long int xexsize, const unsigned char *patch, const long int patch_size)
{
    for(size_t patch_offset = 5; patch_offset < patch_size;)
    {
        size_t offset = (patch[patch_offset+0] << 0x10U) | (patch[patch_offset+1] << 0x8U) | patch[patch_offset+2];
        if(offset == 0x454F46U) // IPS end of file
            break;
        patch_offset += 3;

        unsigned short size = (patch[patch_offset+0] << 8) | patch[patch_offset+1];
        patch_offset += 2;
        if(!size) // RLE mode
        {
            unsigned short rle_size = (patch[patch_offset+0] << 8) | patch[patch_offset+1];
            patch_offset += 2;
            const unsigned char value = patch[patch_offset];
            patch_offset += 1;
            while(rle_size--)
            {
                if(offset >= xexsize || patch_offset >= patch_size)
                    return 1;
                xexbin[offset] = value;
                offset++;
            }
        }
        else // Copy mode
        {
            while(size--)
            {
                if(offset >= xexsize || patch_offset >= patch_size)
                    return 1;
                xexbin[offset] = patch[patch_offset];
                patch_offset++;
                offset++;
            }
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    printf("\n XEX Finalizer for GoldenEye XBLA Bean (%s)\n%s\n", __DATE__, LINE);
    if(argc != 4) // no file provided or too many arguments
    {
        printf("\n  About: Inject hacks into GoldenEye XBLA for community edition\n\n  Syntax: %s \"input XEX\" \"output XEX\" surround_flag\n  Example: %s \"default.xex\" \"defaultCE.xex\" 0\n", argv[0], argv[0]);
        goto exit;
    }
    FILE *xexinput = fopen(argv[1], "rb"); // load XEX from argument
    if(xexinput == NULL)
    {
        printf("\n  Error: Aborted, XEX cannot be opened");
        goto exit;
    }
    if(fseek(xexinput, 0, SEEK_END) != 0)
    {
        printf("\n  Error: Aborted, could not find end to XEX file");
        goto error_xex;
    }
    const long int filesize = ftell(xexinput); // get XEX filesize
    if(filesize == -1)
    {
        printf("\n  Error: Aborted, could not find length to XEX file");
        goto error_xex;
    }
    if(filesize < XEXFILESIZEMIN) // if XEX filesize is under supported filesize, abort
    {
        printf("\n  Error: Aborted, XEX is invalid");
        goto error_xex;
    }
    if(fseek(xexinput, 0, SEEK_SET) != 0)
    {
        printf("\n  Error: Aborted, could not find beginning of XEX file");
        goto error_xex;
    }
    printf("\n  Input XEX: %s\n", argv[1]);

    unsigned char *xexbin = malloc(filesize);
    if(xexbin == NULL)
    {
        printf("\n  Error: Aborted, not enough memory to load XEX");
        goto error_xex;
    }
    fread(xexbin, filesize, 1, xexinput); // read XEX to file buffer

    if(patch_fyodorovna_xex(xexbin, filesize, xex_patch_restore, ARRAYSIZE(xex_patch_restore)))
    {
        printf("\n  Error: Aborted, input was not expected XEX");
        goto error_output;
    }

    printf("\n  Executing Hacks:");
    do_hack_tasks(xexbin, !(argv[3][0] == '1'));

    FILE *xexoutput = fopen(argv[2], "wb"); // save XEX
    if(xexoutput == NULL)
    {
        printf("\n  Error: Aborted, could not save XEX output");
        goto error_output;
    }
    fwrite(xexbin, 1, filesize, xexoutput);
    fclose(xexoutput);

    printf("\n\n  Finished Successfully!\n  Output XEX:\t%s", argv[2]);

error_output:
    free(xexbin);
error_xex:
    fclose(xexinput);
exit:
    printf("\n%s\n", LINE);
    return 0;
}