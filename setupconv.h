// GoldenEye XBLA setup converter
// License: The MIT License (MIT)

struct coord
{
    float x;
    float y;
    float z;
};

struct bbox
{
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
};

struct waypoint
{
    int32_t id;
    uint32_t pad;
    int32_t group;
    uint32_t unk;
};

struct waygroup
{
    uint32_t pad1;
    uint32_t pad2;
    uint32_t unk;
};

struct ailist
{
    uint32_t list;
    int32_t id;
};

struct patrol
{
    uint32_t list;
    uint16_t id;
    uint16_t count; // calculated in-game
};

struct pad
{
    struct coord pos;
    struct coord up;
    struct coord look;
    uint32_t plink;
    uint32_t unk;
};

struct padadv
{
    struct coord pos;
    struct coord up;
    struct coord look;
    uint32_t plink;
    uint32_t unk;
    struct bbox bbox;
};

typedef enum OBJECT_TYPE
{
    OBJTYPE_NULL = 0,
    OBJTYPE_DOOR = 1,
    OBJTYPE_DOOR_SCALE = 2,
    OBJTYPE_PROP = 3,
    OBJTYPE_KEY = 4,
    OBJTYPE_ALARM = 5,
    OBJTYPE_CCTV = 6,
    OBJTYPE_AMMO_MAG = 7,
    OBJTYPE_WEAPON = 8,
    OBJTYPE_CHR = 9,
    OBJTYPE_MONITOR = 10,
    OBJTYPE_MULMONITOR = 11,
    OBJTYPE_RACK = 12,
    OBJTYPE_DRONE = 13,
    OBJTYPE_LINK_ITEMS = 14,
    OBJTYPE_15 = 15,
    OBJTYPE_16 = 16,
    OBJTYPE_HAT = 17,
    OBJTYPE_CHR_GRENADE_RNG = 18,
    OBJTYPE_LINK_PROPS = 19,
    OBJTYPE_AMMO_BOX = 20,
    OBJTYPE_ARMOR = 21,
    OBJTYPE_TAG = 22,
    OBJTYPE_OBJECTIVE_START = 23,
    OBJTYPE_OBJECTIVE_END = 24,
    OBJTYPE_OBJECTIVE_DESTROY = 25,
    OBJTYPE_OBJECTIVE_BIT_TRUE = 26,
    OBJTYPE_OBJECTIVE_BIT_FALSE = 27,
    OBJTYPE_OBJECTIVE_COLLECT = 28,
    OBJTYPE_OBJECTIVE_THROW = 29,
    OBJTYPE_OBJECTIVE_PHOTO = 30,
    OBJTYPE_OBJECTIVE_UNKNOWN = 31,
    OBJTYPE_OBJECTIVE_ENTER_ROOM = 32,
    OBJTYPE_OBJECTIVE_THROW_IN_ROOM = 33,
    OBJTYPE_OBJECTIVE_KEY_ANALYZER = 34,
    OBJTYPE_BRIEFING = 35,
    OBJTYPE_GAS_PROP = 36,
    OBJTYPE_RENAME = 37,
    OBJTYPE_LOCK = 38,
    OBJTYPE_VEHICLE = 39,
    OBJTYPE_AIRCRAFT = 40,
    OBJTYPE_41 = 41,
    OBJTYPE_GLASS = 42,
    OBJTYPE_SAFE = 43,
    OBJTYPE_ITEM_IN_SAFE = 44,
    OBJTYPE_TANK = 45,
    OBJTYPE_CUTSCENE_CAM = 46,
    OBJTYPE_GLASS_TINTED = 47,
    OBJTYPE_END = 48,
    OBJTYPE_MAX
} OBJECT_TYPE;

const char *OBJ_DESCR[] =
{
    "OBJTYPE_NULL",
    "OBJTYPE_DOOR",
    "OBJTYPE_DOOR_SCALE",
    "OBJTYPE_PROP",
    "OBJTYPE_KEY",
    "OBJTYPE_ALARM",
    "OBJTYPE_CCTV",
    "OBJTYPE_AMMO_MAG",
    "OBJTYPE_WEAPON",
    "OBJTYPE_CHR",
    "OBJTYPE_MONITOR",
    "OBJTYPE_MULMONITOR",
    "OBJTYPE_RACK",
    "OBJTYPE_DRONE",
    "OBJTYPE_LINK_ITEMS",
    "OBJTYPE_15",
    "OBJTYPE_16",
    "OBJTYPE_HAT",
    "OBJTYPE_CHR_GRENADE_RNG",
    "OBJTYPE_LINK_PROPS",
    "OBJTYPE_AMMO_BOX",
    "OBJTYPE_ARMOR",
    "OBJTYPE_TAG",
    "OBJTYPE_OBJECTIVE_START",
    "OBJTYPE_OBJECTIVE_END",
    "OBJTYPE_OBJECTIVE_DESTROY",
    "OBJTYPE_OBJECTIVE_BIT_TRUE",
    "OBJTYPE_OBJECTIVE_BIT_FALSE",
    "OBJTYPE_OBJECTIVE_COLLECT",
    "OBJTYPE_OBJECTIVE_THROW",
    "OBJTYPE_OBJECTIVE_PHOTO",
    "OBJTYPE_OBJECTIVE_UNKNOWN",
    "OBJTYPE_OBJECTIVE_ENTER_ROOM",
    "OBJTYPE_OBJECTIVE_THROW_IN_ROOM",
    "OBJTYPE_OBJECTIVE_KEY_ANALYZER",
    "OBJTYPE_BRIEFING",
    "OBJTYPE_GAS_PROP",
    "OBJTYPE_RENAME",
    "OBJTYPE_LOCK",
    "OBJTYPE_VEHICLE",
    "OBJTYPE_AIRCRAFT",
    "OBJTYPE_41",
    "OBJTYPE_GLASS",
    "OBJTYPE_SAFE",
    "OBJTYPE_ITEM_IN_SAFE",
    "OBJTYPE_TANK",
    "OBJTYPE_CUTSCENE_CAM",
    "OBJTYPE_GLASS_TINTED",
    "OBJTYPE_END"
};

typedef enum INTRO_TYPE
{
    INTROTYPE_SPAWN = 0,
    INTROTYPE_ITEM = 1,
    INTROTYPE_AMMO = 2,
    INTROTYPE_SWIRL = 3,
    INTROTYPE_ANIM = 4,
    INTROTYPE_CUFF = 5,
    INTROTYPE_CAMERA = 6,
    INTROTYPE_WATCH = 7,
    INTROTYPE_CREDITS = 8,
    INTROTYPE_END = 9,
    INTROTYPE_MAX
} INTRO_TYPE;

const char *INTRO_DESCR[] =
{
    "INTROTYPE_SPAWN",
    "INTROTYPE_ITEM",
    "INTROTYPE_AMMO",
    "INTROTYPE_SWIRL",
    "INTROTYPE_ANIM",
    "INTROTYPE_CUFF",
    "INTROTYPE_CAMERA",
    "INTROTYPE_WATCH",
    "INTROTYPE_CREDITS",
    "INTROTYPE_END"
};

typedef enum ITEM_IDS
{
    ITEM_NULL = 0,
    ITEM_FIST = 1,
    ITEM_KNIFE = 2,
    ITEM_THROWKNIFE = 3,
    ITEM_WPPK = 4,
    ITEM_WPPKSIL = 5,
    ITEM_TT33 = 6,
    ITEM_SKORPION = 7,
    ITEM_AK47 = 8,
    ITEM_UZI = 9,
    ITEM_MP5K = 10,
    ITEM_MP5KSIL = 11,
    ITEM_SPECTRE = 12,
    ITEM_M16 = 13,
    ITEM_FNP90 = 14,
    ITEM_SHOTGUN = 15,
    ITEM_AUTOSHOT = 16,
    ITEM_SNIPERRIFLE = 17,
    ITEM_RUGER = 18,
    ITEM_GOLDENGUN = 19,
    ITEM_SILVERWPPK = 20,
    ITEM_GOLDWPPK = 21,
    ITEM_LASER = 22,
    ITEM_WATCHLASER = 23,
    ITEM_GRENADELAUNCH = 24,
    ITEM_ROCKETLAUNCH = 25,
    ITEM_GRENADE = 26,
    ITEM_TIMEDMINE = 27,
    ITEM_PROXIMITYMINE = 28,
    ITEM_REMOTEMINE = 29,
    ITEM_TRIGGER = 30,
    ITEM_TASER = 31,
    ITEM_TANKSHELLS = 32,
    ITEM_BOMBCASE = 33,
    ITEM_PLASTIQUE = 34,
    ITEM_FLAREPISTOL = 35,
    ITEM_PITONGUN = 36,
    ITEM_BUNGEE = 37,
    ITEM_DOORDECODER = 38,
    ITEM_BOMBDEFUSER = 39,
    ITEM_CAMERA = 40,
    ITEM_LOCKEXPLODER = 41,
    ITEM_DOOREXPLODER = 42,
    ITEM_BRIEFCASE = 43,
    ITEM_WEAPONCASE = 44,
    ITEM_SAFECRACKERCASE = 45,
    ITEM_KEYANALYSERCASE = 46,
    ITEM_BUG = 47,
    ITEM_MICROCAMERA = 48,
    ITEM_BUGDETECTOR = 49,
    ITEM_EXPLOSIVEFLOPPY = 50,
    ITEM_POLARIZEDGLASSES = 51,
    ITEM_DARKGLASSES = 52,
    ITEM_CREDITCARD = 53,
    ITEM_GASKEYRING = 54,
    ITEM_DATATHIEF = 55,
    ITEM_WATCHIDENTIFIER = 56,
    ITEM_WATCHCOMMUNICATOR = 57,
    ITEM_WATCHGEIGERCOUNTER = 58,
    ITEM_WATCHMAGNETREPEL = 59,
    ITEM_WATCHMAGNETATTRACT = 60,
    ITEM_GOLDENEYEKEY = 61,
    ITEM_BLACKBOX = 62,
    ITEM_CIRCUITBOARD = 63,
    ITEM_CLIPBOARD = 64,
    ITEM_STAFFLIST = 65,
    ITEM_DOSSIERRED = 66,
    ITEM_PLANS = 67,
    ITEM_SPYFILE = 68,
    ITEM_BLUEPRINTS = 69,
    ITEM_MAP = 70,
    ITEM_AUDIOTAPE = 71,
    ITEM_VIDEOTAPE = 72,
    ITEM_DATTAPE = 73,
    ITEM_SPOOLTAPE = 74,
    ITEM_MICROFILM = 75,
    ITEM_MICROCODE = 76,
    ITEM_LECTRE = 77,
    ITEM_MONEY = 78,
    ITEM_GOLDBAR = 79,
    ITEM_HEROIN = 80,
    ITEM_KEYCARD = 81,
    ITEM_KEYYALE = 82,
    ITEM_KEYBOLT = 83,
    ITEM_SUIT_LF_HAND = 84,
    ITEM_JOYPAD = 85,
    ITEM_NULL86 = 86,
    ITEM_NULL87 = 87,
    ITEM_TOKEN = 88,
    ITEM_MAX
} ITEM_IDS;

const char *ITEM_DESCR[] =
{
    "ITEM_NULL",
    "ITEM_FIST",
    "ITEM_KNIFE",
    "ITEM_THROWKNIFE",
    "ITEM_WPPK",
    "ITEM_WPPKSIL",
    "ITEM_TT33",
    "ITEM_SKORPION",
    "ITEM_AK47",
    "ITEM_UZI",
    "ITEM_MP5K",
    "ITEM_MP5KSIL",
    "ITEM_SPECTRE",
    "ITEM_M16",
    "ITEM_FNP90",
    "ITEM_SHOTGUN",
    "ITEM_AUTOSHOT",
    "ITEM_SNIPERRIFLE",
    "ITEM_RUGER",
    "ITEM_GOLDENGUN",
    "ITEM_SILVERWPPK",
    "ITEM_GOLDWPPK",
    "ITEM_LASER",
    "ITEM_WATCHLASER",
    "ITEM_GRENADELAUNCH",
    "ITEM_ROCKETLAUNCH",
    "ITEM_GRENADE",
    "ITEM_TIMEDMINE",
    "ITEM_PROXIMITYMINE",
    "ITEM_REMOTEMINE",
    "ITEM_TRIGGER",
    "ITEM_TASER",
    "ITEM_TANKSHELLS",
    "ITEM_BOMBCASE",
    "ITEM_PLASTIQUE",
    "ITEM_FLAREPISTOL",
    "ITEM_PITONGUN",
    "ITEM_BUNGEE",
    "ITEM_DOORDECODER",
    "ITEM_BOMBDEFUSER",
    "ITEM_CAMERA",
    "ITEM_LOCKEXPLODER",
    "ITEM_DOOREXPLODER",
    "ITEM_BRIEFCASE",
    "ITEM_WEAPONCASE",
    "ITEM_SAFECRACKERCASE",
    "ITEM_KEYANALYSERCASE",
    "ITEM_BUG",
    "ITEM_MICROCAMERA",
    "ITEM_BUGDETECTOR",
    "ITEM_EXPLOSIVEFLOPPY",
    "ITEM_POLARIZEDGLASSES",
    "ITEM_DARKGLASSES",
    "ITEM_CREDITCARD",
    "ITEM_GASKEYRING",
    "ITEM_DATATHIEF",
    "ITEM_WATCHIDENTIFIER",
    "ITEM_WATCHCOMMUNICATOR",
    "ITEM_WATCHGEIGERCOUNTER",
    "ITEM_WATCHMAGNETREPEL",
    "ITEM_WATCHMAGNETATTRACT",
    "ITEM_GOLDENEYEKEY",
    "ITEM_BLACKBOX",
    "ITEM_CIRCUITBOARD",
    "ITEM_CLIPBOARD",
    "ITEM_STAFFLIST",
    "ITEM_DOSSIERRED",
    "ITEM_PLANS",
    "ITEM_SPYFILE",
    "ITEM_BLUEPRINTS",
    "ITEM_MAP",
    "ITEM_AUDIOTAPE",
    "ITEM_VIDEOTAPE",
    "ITEM_DATTAPE",
    "ITEM_SPOOLTAPE",
    "ITEM_MICROFILM",
    "ITEM_MICROCODE",
    "ITEM_LECTRE", 
    "ITEM_MONEY",
    "ITEM_GOLDBAR",
    "ITEM_HEROIN",
    "ITEM_KEYBOLT",
    "ITEM_KEYCARD",
    "ITEM_KEYYALE",
    "ITEM_SUIT_LF_HAND",
    "ITEM_JOYPAD",
    "ITEM_NULL86",
    "ITEM_NULL87",
    "ITEM_TOKEN"
};

typedef enum PROP_IDS
{
    PROP_alarm1               = 0, /* Beta Alarm / Default Multi Weapon */
    PROP_alarm2               = 1, /* Alarm */
    PROP_explosionbit           = 2, /* White Pyramid (Explosion Bit) */
    PROP_ammo_crate1             = 3, /* Ammo Crate (Brown w/ Edge Brace, 6x240 Black) */
    PROP_ammo_crate2             = 4, /* Ammo Crate (Brown w/ Center Brace, 12x8 Black) */
    PROP_ammo_crate3             = 5, /* Ammo Crate (Green w/ Center Brace, 12x8 Brown) */
    PROP_ammo_crate4             = 6, /* Ammo Crate (Green w/ Edge Brace, 6x8 White) */
    PROP_ammo_crate5             = 7, /* Ammo Crate (Green w/ Double Brace, 24x60 Black) */
    PROP_bin1                   = 8, /* Rusted Trash Bin */
    PROP_blotter1               = 9, /* Desk Blotter */
    PROP_book1                = 10, /* Red Book */
    PROP_bookshelf1          = 11, /* Bookshelf */
    PROP_bridge_console1a      = 12, /* Bridge Console w/ Monitor, Navigation + Keyboard 1A */
    PROP_bridge_console1b      = 13, /* Bridge Console w/ Navigation 1B */
    PROP_bridge_console2a      = 14, /* Bridge Console w/ Navigation, Monitor + Keyboard 2A */
    PROP_bridge_console2b      = 15, /* Bridge Console w/ Various Controls 2B */
    PROP_bridge_console3a      = 16, /* Bridge Console w/ Monitor, Navigation + Keyboard 3A */
    PROP_bridge_console3b      = 17, /* Bridge Console w/ Monitor, Keyboard + Navigation 3B */
    PROP_card_box1            = 18, /* Cardboard Box, Kapto|Enb */
    PROP_card_box2            = 19, /* Cardboard Box, Red Arrow, Bahko */
    PROP_card_box3            = 20, /* Cardboard Box, Scrawled Text, Bahah */
    PROP_card_box4_lg          = 21, /* Cardboard Box, Three Seams */
    PROP_card_box5_lg          = 22, /* Cardboard Box, Two Seams, Bahah */
    PROP_card_box6_lg          = 23, /* Cardboard Box, Bahko */
    PROP_cctv                  = 24, /* Surveillance Camera */
    PROP_console1              = 25, /* Double Screen Consoles w/ Keyboards */
    PROP_console2              = 26, /* Double Screen Consoles w/ Left Keyboard */
    PROP_console3              = 27, /* Double Screen Consoles w/ Right Keyboard */
    PROP_console_seva          = 28, /* Console w/ Keyboard */
    PROP_console_sevb          = 29, /* Console w/ Monitor + Keyboard */
    PROP_console_sevc          = 30, /* Console w/ Switches */
    PROP_console_sevd          = 31, /* Console w/ Five Gauges */
    PROP_console_sev2a        = 32, /* Console w/ Four Faders */
    PROP_console_sev2b        = 33, /* Console w/ Monitor, Keyboard + Switches */
    PROP_console_sev2c        = 34, /* Console w/ Three Gauges */
    PROP_console_sev2d        = 35, /* Console w/ Pressure Gauge */
    PROP_console_sev_GEa        = 36, /* Console w/ GoldenEye Key Slot */
    PROP_console_sev_GEb        = 37, /* Console w/ Faders + Pressure Gauge */
    PROP_desk1                = 38, /* Desk w/ Kickplate */
    PROP_desk2                = 39, /* Desk */
    PROP_desk_lamp2          = 40, /* Desk Lamp */
    PROP_disc_reader            = 41, /* External Hard Drive */
    PROP_disk_drive1            = 42, /* Floppy Disc Drive */
    PROP_filing_cabinet1        = 43, /* Filing Cabinet */
    PROP_jerry_can1          = 44, /* Jerrycan (Fuel Container) */
    PROP_keyboard1            = 45, /* Computer Keyboard */
    PROP_kit_units1          = 46, /* Kitchen Cabinets */
    PROP_letter_tray1          = 47, /* Letter Tray */
    PROP_mainframe1          = 48, /* Mainframe, Basic */
    PROP_mainframe2          = 49, /* Mainframe, Advanced */
    PROP_metal_chair1          = 50, /* Chair (Metal) */
    PROP_metal_crate1          = 51, /* Metal Crate, 6 Top Corner */
    PROP_metal_crate2          = 52, /* Metal Crate, 6 Bottom Corner */
    PROP_metal_crate3          = 53, /* Metal Crate, Toxic Materials */
    PROP_metal_crate4          = 54, /* Metal Crate, Double Stripe - Class D1 Hazard */
    PROP_missile_rack          = 55, /* Naval Harpoon Missile in Containment Rack */
    PROP_missile_rack2        = 56, /* Naval Harpoon Missiles in Containment Racks x4 */
    PROP_oil_drum1            = 57, /* Oil Drum, Single Stripe, Ribbed */
    PROP_oil_drum2            = 58, /* Oil Drum, Single Stripe, Ribbed - Class D1 Hazard */
    PROP_oil_drum3            = 59, /* Oil Drum, Single Stripe, Ribbed - Toxic Materials */
    PROP_oil_drum5            = 60, /* Oil Drum, Double Stripe - Toxic Materials */
    PROP_oil_drum6            = 61, /* Oil Drum - Toxic Materials */
    PROP_oil_drum7            = 62, /* Oil Drum, Double Dashes - Class D1 Hazard */
    PROP_padlock                = 63, /* Padlock */
    PROP_phone1              = 64, /* Telephone */
    PROP_radio_unit1            = 65, /* Radio Tuner w/ 1 Knob + 2 Gauges */
    PROP_radio_unit2            = 66, /* Radio Tuner w/ 1 Knob + 5 Gauges */
    PROP_radio_unit3            = 67, /* Radio Tuner w/ 3 Knobs + 5 Gauges */
    PROP_radio_unit4            = 68, /* Radio Tuner w/ 3 Knobs + 2 Gauges */
    PROP_sat1_reflect          = 69, /* GoldenEye Satellite */
    PROP_satdish                = 70, /* Satellite Dish (Arkangelsk) */
    PROP_satbox              = 71, /* Uplink Box */
    PROP_stool1              = 72, /* Wooden Stool */
    PROP_swivel_chair1        = 73, /* Swivel Chair */
    PROP_torpedo_rack          = 74, /* Naval Torpedo Rack x3 */
    PROP_tv1                    = 75, /* Television Monitor */
    PROP_tv_holder            = 76, /* Hanging Monitor Rack */
    PROP_tvscreen              = 77, /* Wall Monitor Screen */
    PROP_tv4screen            = 78, /* Wall Monitor Screens, 4-in-1 */
    PROP_wood_lg_crate1      = 79, /* Wooden Crate w/ #4 Label, Bahah */
    PROP_wood_lg_crate2      = 80, /* Wooden Crate, Darker Shading, Kapto|Enb */
    PROP_wood_md_crate3      = 81, /* Wooden Crates x8, Bahko */
    PROP_wood_sm_crate4      = 82, /* Wooden Crate w/ #2 Label, Bahko */
    PROP_wood_sm_crate5      = 83, /* Wooden Crate w/ #4 Label, Darker Shading, Bahah */
    PROP_wood_sm_crate6      = 84, /* Wooden Crate w/ UP Arrow, Kapto|Enb */
    PROP_wooden_table1        = 85, /* Wooden Table */
    PROP_swipe_card2            = 86, /* Keycard */
    PROP_borg_crate          = 87, /* Blue and Gold Printed Circuit Cube (Borg Crate) */
    PROP_boxes4x4              = 88, /* Metal Crate Stack, 4x4 */
    PROP_boxes3x4              = 89, /* Metal Crate Stack, 3x4 */
    PROP_boxes2x4              = 90, /* Metal Crate Stack, 2x4 */
    PROP_sec_panel            = 91, /* Security Card Panel */
    PROP_ICBM_nose            = 92, /* Silo Missile (ICBM), Nose Cone Only */
    PROP_ICBM                  = 93, /* Silo Missile (ICBM) */
    PROP_tuning_console1        = 94, /* Dual Consoles on Castors */
    PROP_desk_arecibo1        = 95, /* Computer Work Desk */
    PROP_locker3                = 96, /* Lockers, Single Venting */
    PROP_locker4                = 97, /* Lockers, Double Venting */
    PROP_roofgun                = 98, /* Ceiling Mounted Drone Gun */
    PROP_dest_engine            = 99, /* Frigate Engine */
    PROP_dest_exocet           = 100, /* Naval MK 29 Missile Launcher (Exocet) */
    PROP_dest_gun             = 101, /* Naval 100 mm Gun Turret (TR 100) */
    PROP_dest_harpoon         = 102, /* Naval MK 141 Launch Canisters (Harpoon) */
    PROP_dest_seawolf         = 103, /* Naval MK 26 Dual Missile Launcher (Seawolf) */
    PROP_window             = 104, /* Window Glass */
    PROP_window_lib_lg1     = 105, /* Window Glass, Lattice Frame, 4x10 (single-sided) */
    PROP_window_lib_sm1     = 106, /* Window Glass, Lattice Frame, 4x3 (double-sided) */
    PROP_window_cor11         = 107, /* Window Glass, Lattice Frame, 4x4 (single-sided) */
    PROP_jungle3_tree         = 108, /* Jungle Large Tree */
    PROP_palm                 = 109, /* Jungle Palm Tree */
    PROP_palmtree             = 110, /* Jungle Palm Tree, Resprouting After Loss of Fronds */
    PROP_plant2b               = 111, /* Jungle Plant, Low Shrub */
    PROP_labbench             = 112, /* Laboratory Table w/ Sink Drains */
    PROP_gasbarrel           = 113, /* White Bin */
    PROP_gasbarrels         = 114, /* White Bins x4 */
    PROP_bodyarmour         = 115, /* Body Armor */
    PROP_bodyarmourvest     = 116, /* Body Armor (Vest) */
    PROP_gastank               = 117, /* Bottling Tank */
    PROP_glassware1         = 118, /* Glass Cup */
    PROP_hatchbolt           = 119, /* Metallic Securing Strip (Hatch Bolt) */
    PROP_brakeunit           = 120, /* Train Brake Controller */
    PROP_ak47mag               = 121, /* Gun Magazine (KF7 Soviet) */
    PROP_m16mag             = 122, /* Gun Magazine (AR33 Assault Rifle) */
    PROP_mp5kmag               = 123, /* Gun Magazine (D5K Deutsche) */
    PROP_skorpionmag           = 124, /* Gun Magazine (Klobb) */
    PROP_spectremag         = 125, /* Gun Magazine (Phantom) */
    PROP_uzimag             = 126, /* Gun Magazine (ZMG (9mm)) */
    PROP_silencer             = 127, /* Silencer */
    PROP_chrextinguisher       = 128, /* Fire Extinguisher */
    PROP_boxcartridges       = 129, /* Box of Shells (Shotgun Cartridges) */
    PROP_fnp90mag             = 130, /* Gun Magazine (RC-P90) */
    PROP_goldenshells         = 131, /* Box of Shells (Golden Gun Bullets) */
    PROP_magnumshells         = 132, /* Box of Shells (Magnum Rounds) */
    PROP_wppkmag               = 133, /* Gun Magazine (PP7) */
    PROP_tt33mag               = 134, /* Gun Magazine (DD44 Dostovei) */
    PROP_sev_door             = 135, /* Grey Containment Door w/ Caution Stripes and Window */
    PROP_sev_door3           = 136, /* Grey Electronic Door w/ LEFT Arrow */
    PROP_sev_door3_wind     = 137, /* BETA Electronic Door w/ LEFT Arrow and Fake Window */
    PROP_sev_door4_wind     = 138, /* Grey Electronic Door w/ LEFT Arrow and Window */
    PROP_sev_trislide         = 139, /* Glass Door w/ Stone Frame */
    PROP_sev_door_v1           = 140, /* Grey Electronic Door w/ UP Arrow */
    PROP_steel_door1           = 141, /* Silver Corrugated Door w/ Caution Stripes */
    PROP_steel_door2           = 142, /* Rusty Door w/ Handle */
    PROP_steel_door3           = 143, /* Double Cross Brace Door */
    PROP_silo_lift_door     = 144, /* Elevator Door */
    PROP_steel_door2b         = 145, /* Rusty Door w/o Handle */
    PROP_door_roller1         = 146, /* Blue Bay Door w/ Caution Stripes */
    PROP_door_roller2         = 147, /* Blue Bay Door w/ Venting and Caution Stripes */
    PROP_door_roller3         = 148, /* Blue Bay Door w/ Venting and Caution Stripes */
    PROP_door_roller4         = 149, /* Cargo Bay Door w/ UP Arrow and Transportation Stripes */
    PROP_door_st_arec1       = 150, /* Blue Corrugated Door w/ Transportation Stripes */
    PROP_door_st_arec2       = 151, /* Blue Reversed Corrugated Door w/ Transportation Stripes */
    PROP_door_dest1         = 152, /* Grey Frigate Door w/ Indents and Caution Stripes */
    PROP_door_dest2         = 153, /* Grey Frigate Door w/ Indents, Caution Stripes and KEEP CLEAR Label */
    PROP_gas_plant_sw_do1     = 154, /* Grey Swinging Door w/ Blue Stripe */
    PROP_gas_plant_sw2_do1   = 155, /* Grey Swinging Door, Darker */
    PROP_gas_plant_sw3_do1   = 156, /* Grey Swinging Door, Lighter */
    PROP_gas_plant_sw4_do1   = 157, /* Light Wooden Door (Looks Like Sand) */
    PROP_gas_plant_met1_do1 = 158, /* Brown Electronic Door */
    PROP_gas_plant_wc_cub1   = 159, /* Bathroom Stall Door */
    PROP_gasplant_clear_door   = 160, /* Laboratory Glass Door */
    PROP_train_door         = 161, /* Dark Wooden Door */
    PROP_train_door2           = 162, /* Dark Wooden Door w/ Window */
    PROP_train_door3           = 163, /* Dark Wooden Door w/ Window + Shutter */
    PROP_door_eyelid           = 164, /* Eyelid Door */
    PROP_door_iris           = 165, /* Iris Door */
    PROP_sevdoorwood           = 166, /* Cabin Door */
    PROP_sevdoorwind           = 167, /* Weathered Swinging Door w/ Window */
    PROP_sevdoornowind       = 168, /* Weathered Swinging Door */
    PROP_sevdoormetslide       = 169, /* Brown Corrugated Electronic Door */
    PROP_cryptdoor1a           = 170, /* Stone Door w/ Prints (Set A) */
    PROP_cryptdoor1b           = 171, /* Sand Door w/ Damage (Set A) */
    PROP_cryptdoor2a           = 172, /* Stone Door w/ Prints, Darker (Set B) */
    PROP_cryptdoor2b           = 173, /* Sand Door w/ Damage, Darker (Set B) */
    PROP_cryptdoor3         = 174, /* Egyptian Moving Wall */
    PROP_cryptdoor4         = 175, /* Brown Sand Door (Temple) */
    PROP_vertdoor             = 176, /* Blast Door (Control) */
    PROP_hatchdoor           = 177, /* Train Floor Hatch */
    PROP_damgatedoor           = 178, /* Security Gate (Dam) */
    PROP_damtundoor         = 179, /* Tunnel Flood Door (Dam) */
    PROP_damchaindoor         = 180, /* Mesh Gate */
    PROP_silotopdoor           = 181, /* Launch Tube Ceiling Shutter (Silo) */
    PROP_doorprison1           = 182, /* Cell Door */
    PROP_doorstatgate         = 183, /* Park Gate */
    PROP_chrkalash           = 184, /* KF7 Soviet */
    PROP_chrgrenadelaunch     = 185, /* Grenade Launcher */
    PROP_chrknife             = 186, /* Hunting Knife */
    PROP_chrlaser             = 187, /* Moonraker Laser */
    PROP_chrm16             = 188, /* AR33 Assault Rifle */
    PROP_chrmp5k               = 189, /* D5K Deutsche */
    PROP_chrruger             = 190, /* Cougar Magnum */
    PROP_chrwppk               = 191, /* PP7 Special Issue */
    PROP_chrshotgun         = 192, /* Shotgun */
    PROP_chrskorpion           = 193, /* Klobb */
    PROP_chrspectre         = 194, /* Phantom */
    PROP_chruzi             = 195, /* ZMG (9mm) */
    PROP_chrgrenade         = 196, /* Hand Grenade */
    PROP_chrfnp90             = 197, /* RC-P90 */
    PROP_chrbriefcase         = 198, /* Briefcase */
    PROP_chrremotemine       = 199, /* Remote Mine */
    PROP_chrproximitymine     = 200, /* Proximity Mine */
    PROP_chrtimedmine         = 201, /* Timed Mine */
    PROP_chrrocket           = 202, /* Rocket */
    PROP_chrgrenaderound       = 203, /* Grenade Round */
    PROP_chrwppksil         = 204, /* PP7 (Silenced) */
    PROP_chrtt33               = 205, /* DD44 Dostovei */
    PROP_chrmp5ksil         = 206, /* D5K (Silenced) */
    PROP_chrautoshot           = 207, /* Automatic Shotgun */
    PROP_chrgolden           = 208, /* Golden Gun */
    PROP_chrthrowknife       = 209, /* Throwing Knife */
    PROP_chrsniperrifle     = 210, /* Sniper Rifle */
    PROP_chrrocketlaunch       = 211, /* Rocket Launcher */
    PROP_hatfurry             = 212, /* Fur Hat, Blue */
    PROP_hatfurrybrown       = 213, /* Fur Hat, Brown */
    PROP_hatfurryblack       = 214, /* Fur Hat, Black */
    PROP_hattbird             = 215, /* Side Cap, Light Green */
    PROP_hattbirdbrown       = 216, /* Side Cap, Dark Green */
    PROP_hathelmet           = 217, /* Combat Helmet, Green */
    PROP_hathelmetgrey       = 218, /* Combat Helmet, Grey */
    PROP_hatmoon               = 219, /* Elite Headgear */
    PROP_hatberet             = 220, /* Special Forces Beret, Black */
    PROP_hatberetblue         = 221, /* Special Forces Beret, Navy */
    PROP_hatberetred           = 222, /* Special Forces Beret, Burgundy */
    PROP_hatpeaked           = 223, /* Officer's Peaked Visor Cap */
    PROP_chrwristdart         = 224, /* Pchrwristdart (BETA) */
    PROP_chrexplosivepen       = 225, /* Pchrexplosivepen (BETA) */
    PROP_chrbombcase           = 226, /* Bomb Case (Briefcase Laying Down) */
    PROP_chrflarepistol     = 227, /* Pchrflarepistol (BETA Pickup) */
    PROP_chrpitongun           = 228, /* Pchrpitongun (BETA Pickup) */
    PROP_chrfingergun         = 229, /* Pchrfingergun (BETA Pickup) */
    PROP_chrsilverwppk       = 230, /* Pchrsilverwppk (BETA Pickup) */
    PROP_chrgoldwppk           = 231, /* Pchrgoldwppk (BETA Pickup) */
    PROP_chrdynamite           = 232, /* Pchrdynamite (BETA Pickup) */
    PROP_chrbungee           = 233, /* Pchrbungee (BETA Pickup) */
    PROP_chrdoordecoder     = 234, /* Door Decoder */
    PROP_chrbombdefuser     = 235, /* Bomb Defuser */
    PROP_chrbugdetector     = 236, /* Pchrbugdetector (BETA Pickup) */
    PROP_chrsafecrackercase = 237, /* Safe Cracker Case (Briefcase Laying Down) */
    PROP_chrcamera           = 238, /* Photo Camera (007) */
    PROP_chrlockexploder       = 239, /* Pchrlockexploder (BETA Pickup) */
    PROP_chrdoorexploder       = 240, /* Pchrdoorexploder (BETA Pickup) */
    PROP_chrkeyanalysercase = 241, /* Key Analyzer Case (Briefcase Laying Down) */
    PROP_chrweaponcase       = 242, /* Weapon Case (Briefcase Standing Up) */
    PROP_chrkeyyale         = 243, /* Yale Key */
    PROP_chrkeybolt         = 244, /* Bolt Key */
    PROP_chrbug             = 245, /* Covert Modem / Tracker Bug */
    PROP_chrmicrocamera     = 246, /* Micro Camera */
    PROP_floppy             = 247, /* Floppy Disc */
    PROP_chrgoldeneyekey       = 248, /* GoldenEye Key */
    PROP_chrpolarizedglasses   = 249, /* Polarized Glasses */
    PROP_chrcreditcard       = 250, /* Pchrcreditcard (BETA Pickup) */
    PROP_chrdarkglasses     = 251, /* Pchrdarkglasses (BETA Pickup) */
    PROP_chrgaskeyring       = 252, /* Gas Keyring */
    PROP_chrdatathief         = 253, /* Datathief */
    PROP_safe                 = 254, /* Safe Body */
    PROP_bomb                 = 255, /* Pbomb (BETA Pickup) */
    PROP_chrplans             = 256, /* Plans (Briefing Folder) */
    PROP_chrspyfile         = 257, /* Pchrspyfile (BETA Pickup) */
    PROP_chrblueprints       = 258, /* Pirate Blueprints */
    PROP_chrcircuitboard       = 259, /* Circuitboard */
    PROP_chrmap             = 260, /* Bunker Expansion Plans */
    PROP_chrspooltape         = 261, /* Pchrspooltape (BETA Pickup) */
    PROP_chraudiotape         = 262, /* Audiotape */
    PROP_chrmicrofilm         = 263, /* Pchrmicrofilm (BETA Pickup) */
    PROP_chrmicrocode         = 264, /* Pchrmicrocode (BETA Pickup) */
    PROP_chrlectre           = 265, /* Pchrlectre (BETA Pickup) */
    PROP_chrmoney             = 266, /* Pchrmoney (BETA Pickup) */
    PROP_chrgoldbar         = 267, /* Pchrgoldbar (BETA Pickup) */
    PROP_chrheroin           = 268, /* Pchrheroin (BETA Pickup) */
    PROP_chrclipboard         = 269, /* Clipboard */
    PROP_chrdossierred       = 270, /* Red Dossier */
    PROP_chrstafflist         = 271, /* Staff List */
    PROP_chrdattape         = 272, /* DAT */
    PROP_chrplastique         = 273, /* Plastique */
    PROP_chrblackbox           = 274, /* Black Box (Orange Flight Recorder) */
    PROP_chrvideotape         = 275, /* CCTV Tape (GoldenEye VHS) */
    PROP_nintendologo         = 276, /* Nintendo Logo */
    PROP_goldeneyelogo       = 277, /* GoldenEye Logo */
    PROP_walletbond         = 278, /* Classified Folder w/ Royal Crest (Folder Menus) */
    PROP_miltruck             = 279, /* Supply Truck */
    PROP_jeep                 = 280, /* Military Jeep */
    PROP_artic               = 281, /* Red Prime Mover */
    PROP_helicopter         = 282, /* Transport Helicopter w/ Natalya */
    PROP_tiger               = 283, /* Pirate Euro Chopper */
    PROP_milcopter           = 284, /* Hound Helicopter */
    PROP_hind                 = 285, /* Soviet Camouflage Chopper */
    PROP_artictrailer         = 286, /* Black Trailer */
    PROP_motorbike           = 287, /* Motorbike */
    PROP_tank                 = 288, /* Tank */
    PROP_apc                   = 289, /* Armored Personnel Carrier */
    PROP_speedboat           = 290, /* Speedboat */
    PROP_plane               = 291, /* Aeroplane */
    PROP_gun_runway1           = 292, /* Heavy Gun Emplacement */
    PROP_safedoor             = 293, /* Safe Door */
    PROP_key_holder         = 294, /* Key Rack */
    PROP_hatchsevx           = 295, /* Grating (Ventshaft Hatch) */
    PROP_sevdish               = 296, /* Satellite Dish (Severnaya) */
    PROP_archsecdoor1         = 297, /* Archives Moving Wall (Dark) */
    PROP_archsecdoor2         = 298, /* Archives Moving Wall (Light) */
    PROP_groundgun           = 299, /* Free Standing Drone Gun */
    PROP_trainextdoor         = 300, /* Train Exterior Door */
    PROP_carbmw             = 301, /* White Car #1 (BMW) */
    PROP_carescort           = 302, /* White Car #2 (Escort) */
    PROP_cargolf               = 303, /* White Car #3 (Golf) */
    PROP_carweird             = 304, /* Red Car (Cadillac) */
    PROP_carzil             = 305, /* Ourumov's Car (ZIL) */
    PROP_shuttle_door_l     = 306, /* Exhaust Bay Doors, Left Side */
    PROP_shuttle_door_r     = 307, /* Exhaust Bay Doors, Right Side */
    PROP_depot_gate_entry     = 308, /* Metallic Gate w/ Red Star */
    PROP_depot_door_steel     = 309, /* Rusty Door w/ Handle (Lo-Res) */
    PROP_glassware2         = 310, /* Beaker w/ Blue Topper */
    PROP_glassware3         = 311, /* Erlenmeyer Flask */
    PROP_glassware4         = 312, /* Set of Five Beakers */
    PROP_landmine             = 313, /* Land Mine */
    PROP_plant1             = 314, /* Jungle Plant, Withered and Dying */
    PROP_plant11               = 315, /* Jungle Plant, Turning Colour */
    PROP_plant2             = 316, /* Jungle Plant, Healthy and Thick */
    PROP_plant3             = 317, /* Jungle Plant, Tall Leaves */
    PROP_jungle5_tree         = 318, /* Jungle Tree, Moss Covered */
    PROP_legalpage           = 319, /* GoldenEye Certification Screen */
    PROP_st_pete_room_1i       = 320, /* Roads and Buildings #1 (stretch of road) */
    PROP_st_pete_room_2i       = 321, /* Roads and Buildings #2 (stretch of road) */
    PROP_st_pete_room_3t       = 322, /* Roads and Buildings #3 (intersection) */
    PROP_st_pete_room_5c       = 323, /* Roads and Buildings #4 (street corner) */
    PROP_st_pete_room_6c       = 324, /* Roads and Buildings #5 (street corner) */
    PROP_door_rollertrain     = 325, /* Roller Door */
    PROP_door_win             = 326, /* Glass Sliding Door (Aztec) */
    PROP_door_aztec         = 327, /* Stone Sliding Door (Aztec) */
    PROP_shuttle               = 328, /* Moonraker Shuttle */
    PROP_door_azt_desk       = 329, /* Boardroom Table (Aztec Exhaust Bay) */
    PROP_door_azt_desk_top   = 330, /* Boardroom Table Extension (Aztec Exhaust Bay) */
    PROP_door_azt_chair     = 331, /* Boardroom Chair (Aztec Exhaust Bay) */
    PROP_door_mf               = 332, /* Mainframe Door */
    PROP_flag                 = 333, /* Flag Tag Token */
    PROP_barricade           = 334, /* Road Barricade */
    PROP_modembox             = 335, /* Covert Modem Connection Screen */
    PROP_doorpanel           = 336, /* Sliding Door Activation Switch */
    PROP_doorconsole           = 337, /* Console w/ Activation Light */
    PROP_chrtesttube           = 338, /* Glass Test Tube */
    PROP_bollard               = 339,  /* Bollard */
    PROP_MAX
} PROP_IDS;

const char *PROP_DESCR[] =
{
    "PROP_alarm1",
    "PROP_alarm2",
    "PROP_explosionbit",
    "PROP_ammo_crate1",
    "PROP_ammo_crate2",
    "PROP_ammo_crate3",
    "PROP_ammo_crate4",
    "PROP_ammo_crate5",
    "PROP_bin1",
    "PROP_blotter1",
    "PROP_book1",
    "PROP_bookshelf1",
    "PROP_bridge_console1a",
    "PROP_bridge_console1b",
    "PROP_bridge_console2a",
    "PROP_bridge_console2b",
    "PROP_bridge_console3a",
    "PROP_bridge_console3b",
    "PROP_card_box1",
    "PROP_card_box2",
    "PROP_card_box3",
    "PROP_card_box4_lg",
    "PROP_card_box5_lg",
    "PROP_card_box6_lg",
    "PROP_cctv",
    "PROP_console1",
    "PROP_console2",
    "PROP_console3",
    "PROP_console_seva",
    "PROP_console_sevb",
    "PROP_console_sevc",
    "PROP_console_sevd",
    "PROP_console_sev2a",
    "PROP_console_sev2b",
    "PROP_console_sev2c",
    "PROP_console_sev2d",
    "PROP_console_sev_GEa",
    "PROP_console_sev_GEb",
    "PROP_desk1",
    "PROP_desk2",
    "PROP_desk_lamp2",
    "PROP_disc_reader",
    "PROP_disk_drive1",
    "PROP_filing_cabinet1",
    "PROP_jerry_can1",
    "PROP_keyboard1",
    "PROP_kit_units1",
    "PROP_letter_tray1",
    "PROP_mainframe1",
    "PROP_mainframe2",
    "PROP_metal_chair1",
    "PROP_metal_crate1",
    "PROP_metal_crate2",
    "PROP_metal_crate3",
    "PROP_metal_crate4",
    "PROP_missile_rack",
    "PROP_missile_rack2",
    "PROP_oil_drum1",
    "PROP_oil_drum2",
    "PROP_oil_drum3",
    "PROP_oil_drum5",
    "PROP_oil_drum6",
    "PROP_oil_drum7",
    "PROP_padlock",
    "PROP_phone1",
    "PROP_radio_unit1",
    "PROP_radio_unit2",
    "PROP_radio_unit3",
    "PROP_radio_unit4",
    "PROP_sat1_reflect",
    "PROP_satdish",
    "PROP_satbox",
    "PROP_stool1",
    "PROP_swivel_chair1",
    "PROP_torpedo_rack",
    "PROP_tv1",
    "PROP_tv_holder",
    "PROP_tvscreen",
    "PROP_tv4screen",
    "PROP_wood_lg_crate1",
    "PROP_wood_lg_crate2",
    "PROP_wood_md_crate3",
    "PROP_wood_sm_crate4",
    "PROP_wood_sm_crate5",
    "PROP_wood_sm_crate6",
    "PROP_wooden_table1",
    "PROP_swipe_card2",
    "PROP_borg_crate",
    "PROP_boxes4x4",
    "PROP_boxes3x4",
    "PROP_boxes2x4",
    "PROP_sec_panel",
    "PROP_ICBM_nose",
    "PROP_ICBM",
    "PROP_tuning_console1",
    "PROP_desk_arecibo1",
    "PROP_locker3",
    "PROP_locker4",
    "PROP_roofgun",
    "PROP_dest_engine",
    "PROP_dest_exocet",
    "PROP_dest_gun",
    "PROP_dest_harpoon",
    "PROP_dest_seawolf",
    "PROP_window",
    "PROP_window_lib_lg1",
    "PROP_window_lib_sm1",
    "PROP_window_cor11",
    "PROP_jungle3_tree",
    "PROP_palm",
    "PROP_palmtree",
    "PROP_plant2b",
    "PROP_labbench",
    "PROP_gasbarrel",
    "PROP_gasbarrels",
    "PROP_bodyarmour",
    "PROP_bodyarmourvest",
    "PROP_gastank",
    "PROP_glassware1",
    "PROP_hatchbolt",
    "PROP_brakeunit",
    "PROP_ak47mag",
    "PROP_m16mag",
    "PROP_mp5kmag",
    "PROP_skorpionmag",
    "PROP_spectremag",
    "PROP_uzimag",
    "PROP_silencer",
    "PROP_chrextinguisher",
    "PROP_boxcartridges",
    "PROP_fnp90mag",
    "PROP_goldenshells",
    "PROP_magnumshells",
    "PROP_wppkmag",
    "PROP_tt33mag",
    "PROP_sev_door",
    "PROP_sev_door3",
    "PROP_sev_door3_wind",
    "PROP_sev_door4_wind",
    "PROP_sev_trislide",
    "PROP_sev_door_v1",
    "PROP_steel_door1",
    "PROP_steel_door2",
    "PROP_steel_door3",
    "PROP_silo_lift_door",
    "PROP_steel_door2b",
    "PROP_door_roller1",
    "PROP_door_roller2",
    "PROP_door_roller3",
    "PROP_door_roller4",
    "PROP_door_st_arec1",
    "PROP_door_st_arec2",
    "PROP_door_dest1",
    "PROP_door_dest2",
    "PROP_gas_plant_sw_do1",
    "PROP_gas_plant_sw2_do1",
    "PROP_gas_plant_sw3_do1",
    "PROP_gas_plant_sw4_do1",
    "PROP_gas_plant_met1_do1",
    "PROP_gas_plant_wc_cub1",
    "PROP_gasplant_clear_door",
    "PROP_train_door",
    "PROP_train_door2",
    "PROP_train_door3",
    "PROP_door_eyelid",
    "PROP_door_iris",
    "PROP_sevdoorwood",
    "PROP_sevdoorwind",
    "PROP_sevdoornowind",
    "PROP_sevdoormetslide",
    "PROP_cryptdoor1a",
    "PROP_cryptdoor1b",
    "PROP_cryptdoor2a",
    "PROP_cryptdoor2b",
    "PROP_cryptdoor3",
    "PROP_cryptdoor4",
    "PROP_vertdoor",
    "PROP_hatchdoor",
    "PROP_damgatedoor",
    "PROP_damtundoor",
    "PROP_damchaindoor",
    "PROP_silotopdoor",
    "PROP_doorprison1",
    "PROP_doorstatgate",
    "PROP_chrkalash",
    "PROP_chrgrenadelaunch",
    "PROP_chrknife",
    "PROP_chrlaser",
    "PROP_chrm16",
    "PROP_chrmp5k",
    "PROP_chrruger",
    "PROP_chrwppk",
    "PROP_chrshotgun",
    "PROP_chrskorpion",
    "PROP_chrspectre",
    "PROP_chruzi",
    "PROP_chrgrenade",
    "PROP_chrfnp90",
    "PROP_chrbriefcase",
    "PROP_chrremotemine",
    "PROP_chrproximitymine",
    "PROP_chrtimedmine",
    "PROP_chrrocket",
    "PROP_chrgrenaderound",
    "PROP_chrwppksil",
    "PROP_chrtt33",
    "PROP_chrmp5ksil",
    "PROP_chrautoshot",
    "PROP_chrgolden",
    "PROP_chrthrowknife",
    "PROP_chrsniperrifle",
    "PROP_chrrocketlaunch",
    "PROP_hatfurry",
    "PROP_hatfurrybrown",
    "PROP_hatfurryblack",
    "PROP_hattbird",
    "PROP_hattbirdbrown",
    "PROP_hathelmet",
    "PROP_hathelmetgrey",
    "PROP_hatmoon",
    "PROP_hatberet",
    "PROP_hatberetblue",
    "PROP_hatberetred",
    "PROP_hatpeaked",
    "PROP_chrwristdart",
    "PROP_chrexplosivepen",
    "PROP_chrbombcase",
    "PROP_chrflarepistol",
    "PROP_chrpitongun",
    "PROP_chrfingergun",
    "PROP_chrsilverwppk",
    "PROP_chrgoldwppk",
    "PROP_chrdynamite",
    "PROP_chrbungee",
    "PROP_chrdoordecoder",
    "PROP_chrbombdefuser",
    "PROP_chrbugdetector",
    "PROP_chrsafecrackercase",
    "PROP_chrcamera",
    "PROP_chrlockexploder",
    "PROP_chrdoorexploder",
    "PROP_chrkeyanalysercase",
    "PROP_chrweaponcase",
    "PROP_chrkeyyale",
    "PROP_chrkeybolt",
    "PROP_chrbug",
    "PROP_chrmicrocamera",
    "PROP_floppy",
    "PROP_chrgoldeneyekey",
    "PROP_chrpolarizedglasses",
    "PROP_chrcreditcard",
    "PROP_chrdarkglasses",
    "PROP_chrgaskeyring",
    "PROP_chrdatathief",
    "PROP_safe",
    "PROP_bomb",
    "PROP_chrplans",
    "PROP_chrspyfile",
    "PROP_chrblueprints",
    "PROP_chrcircuitboard",
    "PROP_chrmap",
    "PROP_chrspooltape",
    "PROP_chraudiotape",
    "PROP_chrmicrofilm",
    "PROP_chrmicrocode",
    "PROP_chrlectre",
    "PROP_chrmoney",
    "PROP_chrgoldbar",
    "PROP_chrheroin",
    "PROP_chrclipboard",
    "PROP_chrdossierred",
    "PROP_chrstafflist",
    "PROP_chrdattape",
    "PROP_chrplastique",
    "PROP_chrblackbox",
    "PROP_chrvideotape",
    "PROP_nintendologo",
    "PROP_goldeneyelogo",
    "PROP_walletbond",
    "PROP_miltruck",
    "PROP_jeep",
    "PROP_artic",
    "PROP_helicopter",
    "PROP_tiger",
    "PROP_milcopter",
    "PROP_hind",
    "PROP_artictrailer",
    "PROP_motorbike",
    "PROP_tank",
    "PROP_apc",
    "PROP_speedboat",
    "PROP_plane",
    "PROP_gun_runway1",
    "PROP_safedoor",
    "PROP_key_holder",
    "PROP_hatchsevx",
    "PROP_sevdish",
    "PROP_archsecdoor1",
    "PROP_archsecdoor2",
    "PROP_groundgun",
    "PROP_trainextdoor",
    "PROP_carbmw",
    "PROP_carescort",
    "PROP_cargolf",
    "PROP_carweird",
    "PROP_carzil",
    "PROP_shuttle_door_l",
    "PROP_shuttle_door_r",
    "PROP_depot_gate_entry",
    "PROP_depot_door_steel",
    "PROP_glassware2",
    "PROP_glassware3",
    "PROP_glassware4",
    "PROP_landmine",
    "PROP_plant1",
    "PROP_plant11",
    "PROP_plant2",
    "PROP_plant3",
    "PROP_jungle5_tree",
    "PROP_legalpage",
    "PROP_st_pete_room_1i",
    "PROP_st_pete_room_2i",
    "PROP_st_pete_room_3t",
    "PROP_st_pete_room_5c",
    "PROP_st_pete_room_6c",
    "PROP_door_rollertrain",
    "PROP_door_win",
    "PROP_door_aztec",
    "PROP_shuttle",
    "PROP_door_azt_desk",
    "PROP_door_azt_desk_top",
    "PROP_door_azt_chair",
    "PROP_door_mf",
    "PROP_flag",
    "PROP_barricade",
    "PROP_modembox",
    "PROP_doorpanel",
    "PROP_doorconsole",
    "PROP_chrtesttube",
    "PROP_bollard",
    "PROP_MAX"
};