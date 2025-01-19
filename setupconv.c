// GoldenEye XBLA setup converter
//   $ gcc -O2 -o setupconv setupconv.c
//   $ ./setupconv "usetupsevZ.bin" "converted.bin" 82DE19D8
// License: The MIT License (MIT)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "setupconv.h"
#include "cvector.h"

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(disable:4996)
#endif

// Unused macros helpers
//#define MAX(x, y) (((x) > (y)) ? (x) : (y))
//#define MIN(x, y) (((x) < (y)) ? (x) : (y))
//#define ROUNDADDRUP(x) ((((x) + 0x3) | 0x3) ^ 0x3)
//#define ARRAYSIZE(array) ((size_t)(sizeof(array)/sizeof(*array)))
//#define CLAMP(value, min, max) (((value) < (min)) ? (min) : (((value) > (max)) ? (max) : ((value) < (min)) ? (min) : (value)))

#define ASCII2HEX(x) (((x) <= '9') ? (x) - '0' : ((x) & 0x7) + 9)
#define LINE "__________________________________________________________________"

static size_t get_size_of_setup_object_type(const uint8_t object)
{
    switch(object)
    {
        case OBJTYPE_NULL:
            return 0x01;
        case OBJTYPE_DOOR:
            return 0x40;
        case OBJTYPE_DOOR_SCALE:
            return 0x02;
        case OBJTYPE_PROP:
            return 0x20;
        case OBJTYPE_KEY:
            return 0x21;
        case OBJTYPE_ALARM:
            return 0x20;
        case OBJTYPE_CCTV:
            return 0x3B;
        case OBJTYPE_AMMO_MAG:
            return 0x21;
        case OBJTYPE_WEAPON:
            return 0x22;
        case OBJTYPE_CHR:
            return 0x07;
        case OBJTYPE_MONITOR:
            return 0x40;
        case OBJTYPE_MULMONITOR:
            return 0x95;
        case OBJTYPE_RACK:
            return 0x20;
        case OBJTYPE_DRONE:
            return 0x36;
        case OBJTYPE_LINK_ITEMS:
            return 0x03;
        case OBJTYPE_15:
            return 0x01;
        case OBJTYPE_16:
            return 0x01;
        case OBJTYPE_HAT:
            return 0x20;
        case OBJTYPE_CHR_GRENADE_RNG:
            return 0x03;
        case OBJTYPE_LINK_PROPS:
            return 0x04;
        case OBJTYPE_AMMO_BOX:
            return 0x2D;
        case OBJTYPE_ARMOR:
            return 0x22;
        case OBJTYPE_TAG:
            return 0x04;
        case OBJTYPE_OBJECTIVE_START:
            return 0x04;
        case OBJTYPE_OBJECTIVE_END:
            return 0x01;
        case OBJTYPE_OBJECTIVE_DESTROY:
            return 0x02;
        case OBJTYPE_OBJECTIVE_BIT_TRUE:
            return 0x02;
        case OBJTYPE_OBJECTIVE_BIT_FALSE:
            return 0x02;
        case OBJTYPE_OBJECTIVE_COLLECT:
            return 0x02;
        case OBJTYPE_OBJECTIVE_THROW:
            return 0x02;
        case OBJTYPE_OBJECTIVE_PHOTO:
            return 0x04;
        case OBJTYPE_OBJECTIVE_UNKNOWN:
            return 0x01;
        case OBJTYPE_OBJECTIVE_ENTER_ROOM:
            return 0x04;
        case OBJTYPE_OBJECTIVE_THROW_IN_ROOM:
            return 0x05;
        case OBJTYPE_OBJECTIVE_KEY_ANALYZER:
            return 0x01;
        case OBJTYPE_BRIEFING:
            return 0x04;
        case OBJTYPE_GAS_PROP:
            return 0x20;
        case OBJTYPE_RENAME:
            return 0x0A;
        case OBJTYPE_LOCK:
            return 0x04;
        case OBJTYPE_VEHICLE:
            return 0x2C;
        case OBJTYPE_AIRCRAFT:
            return 0x2D;
        case OBJTYPE_41:
            return 0x01;
        case OBJTYPE_GLASS:
            return 0x20;
        case OBJTYPE_SAFE:
            return 0x20;
        case OBJTYPE_ITEM_IN_SAFE:
            return 0x05;
        case OBJTYPE_TANK:
            return 0x38;
        case OBJTYPE_CUTSCENE_CAM:
            return 0x07;
        case OBJTYPE_GLASS_TINTED:
            return 0x25;
        default:
            break;
    }
    printf("\n  Warning: Unknown object type %d, assuming size 1", object);
    return 0x01;
}

static size_t get_size_of_setup_intro_type(const uint8_t object)
{
    switch(object)
    {
        case INTROTYPE_SPAWN:
            return 0x03;
        case INTROTYPE_ITEM:
            return 0x04;
        case INTROTYPE_AMMO:
            return 0x04;
        case INTROTYPE_SWIRL:
            return 0x08;
        case INTROTYPE_ANIM:
            return 0x02;
        case INTROTYPE_CUFF:
            return 0x02;
        case INTROTYPE_CAMERA:
            return 0x0A;
        case INTROTYPE_WATCH:
            return 0x03;
        case INTROTYPE_CREDITS:
            return 0x02;
        case INTROTYPE_END:
            return 0x01;
        default:
            break;
    }
    printf("\n  Warning: Unknown object type %d, assuming size 1", object);
    return 0x01;
}

static void byteswap_setup(uint8_t *setupbin, const long int size)
{
    for(long int index = 0; index < size; index += 4) // byteswap setup
    {
        uint32_t *intarray = (uint32_t *)&setupbin[index];
        *intarray = (uint32_t)((setupbin[index + 0] << 24) | (setupbin[index + 1] << 16) | (setupbin[index + 2] << 8) | (setupbin[index + 3])); // reassign to swapped value
    }
}

cvector_vector_type(uint64_t) memtask = NULL;

static void mem_track(void *setupbin, void *task)
{
    uint64_t byte_offset = (uint64_t)((int64_t)task - (int64_t)setupbin);
    for(size_t index = 0; index < cvector_size(memtask); index++)
    {
        if(byte_offset == memtask[index]) // task already exists, abort
            return;
    }
    cvector_push_back(memtask, byte_offset);
}

static void mem_shift_setup(uint8_t *setupbin, const size_t size, const uint32_t offset_address, const int32_t offset_amount)
{
    uint32_t *setup = (uint32_t *)setupbin;
    printf("\n  %08X: %d", offset_address, offset_amount);
    memmove(&setupbin[offset_address + offset_amount], &setupbin[offset_address], size);
    for(size_t index = 0; index < cvector_size(memtask); index++)
    {
        if(memtask[index] >= offset_address)
        {
            memtask[index] += offset_amount;
        }
        if(setup[memtask[index] / 4] >= offset_address)
        {
            setup[memtask[index] / 4] += offset_amount;
        }
    }
}

static void mem_update_offsets(uint32_t *setup, const uint32_t memoffset)
{
    for(size_t index = 0; index < cvector_size(memtask); index++)
    {
        setup[memtask[index] / 4] += memoffset;
    }
    cvector_free(memtask);
    memtask = NULL;
}

static int convert_setup(uint8_t *setupbin, long int *size, const uint32_t memoffset)
{
    byteswap_setup(setupbin, *size);
    uint32_t *setup = (uint32_t *)setupbin;
    uint32_t *waypoints = &setup[0];
    uint32_t *waygroups = &setup[1];
    uint32_t *intro = &setup[2];
    uint32_t *objlist = &setup[3];
    uint32_t *patrols = &setup[4];
    uint32_t *ailists = &setup[5];
    uint32_t *padlist = &setup[6];
    uint32_t *padadvlist = &setup[7];
    uint32_t *padnames = &setup[8];
    uint32_t *padadvnames = &setup[9];
    if((*waypoints % 4 != 0) || (*waygroups % 4 != 0) || (*intro % 4 != 0) || (*objlist % 4 != 0) || (*patrols % 4 != 0) || (*ailists % 4 != 0) || (*padlist % 4 != 0) || (*padadvlist % 4 != 0) || (*padnames % 4 != 0) || (*padadvnames % 4 != 0)) // header pointers were not 4 byte aligned
    {
        printf("\n  Error: Setup header contained a non-byte aligned pointer");
        return 0;
    }
    size_t waypoints_size = 0;
    size_t waygroups_size = 0;
    size_t intro_size = 0;
    size_t objlist_size = 0;
    size_t patrols_size = 0;
    size_t ailists_size = 0;
    size_t padlist_size = 0;
    size_t padadvlist_size = 0;

    if(*waypoints != 0)
    {
        struct waypoint *entry = (struct waypoint *)&setupbin[*waypoints];
        for(size_t index = 0; entry[index].id > -1; index++)
        {
            mem_track(setupbin, &entry[index].pad);
            waypoints_size += sizeof(struct waypoint);
        }
        mem_track(setupbin, waypoints);
        waypoints_size += sizeof(struct waypoint);
    }
    if(*waygroups != 0)
    {
        struct waygroup *entry = (struct waygroup *)&setupbin[*waygroups];
        for(size_t index = 0; entry[index].pad1 != 0; index++)
        {
            mem_track(setupbin, &entry[index].pad1);
            mem_track(setupbin, &entry[index].pad2);
            waygroups_size += sizeof(struct waygroup);
        }
        mem_track(setupbin, waygroups);
        waygroups_size += sizeof(struct waygroup);
    }
    if(*ailists != 0)
    {
        struct ailist *entry = (struct ailist *)&setupbin[*ailists];
        for(size_t index = 0; entry[index].list != 0; index++)
        {
            mem_track(setupbin, &entry[index].list);
            ailists_size += sizeof(struct ailist);
        }
        mem_track(setupbin, ailists);
        ailists_size += sizeof(struct ailist);
    }
    if(*patrols != 0)
    {
        struct patrol *entry = (struct patrol *)&setupbin[*patrols];
        for(size_t index = 0; entry[index].list != 0; index++)
        {
            mem_track(setupbin, &entry[index].list);
            patrols_size += sizeof(struct patrol);
        }
        mem_track(setupbin, patrols);
        patrols_size += sizeof(struct patrol);
    }
    if(*padlist != 0)
    {
        struct pad *entry = (struct pad *)&setupbin[*padlist];
        for(size_t index = 0; entry[index].plink != 0; index++)
        {
            mem_track(setupbin, &entry[index].plink);
            padlist_size += sizeof(struct pad);
        }
        mem_track(setupbin, padlist);
        padlist_size += sizeof(struct pad);
    }
    if(*padadvlist != 0)
    {
        struct padadv *entry = (struct padadv *)&setupbin[*padadvlist];
        for(size_t index = 0; entry[index].plink != 0; index++)
        {
            mem_track(setupbin, &entry[index].plink);
            padadvlist_size += sizeof(struct padadv);
        }
        mem_track(setupbin, padadvlist);
        padadvlist_size += sizeof(struct padadv);
    }
    if(*padnames != 0)
    {
        uint32_t *entry = (uint32_t *)&setupbin[*padnames];
        for(size_t index = 0; entry[index] != 0; index++)
        {
            mem_track(setupbin, &entry[index]);
        }
        mem_track(setupbin, padnames);
    }
    if(*padadvnames != 0)
    {
        uint32_t *entry = (uint32_t *)&setupbin[*padadvnames];
        for(size_t index = 0; entry[index] != 0; index++)
        {
            mem_track(setupbin, &entry[index]);
        }
        mem_track(setupbin, padadvnames);
    }
    if(*intro != 0)
    {
        size_t index = *intro;
        while(index < (size_t)*size)
        {
            const uint8_t introtype = setupbin[index];
            //printf("\n  %s", introtype <= INTROTYPE_END ? INTRO_DESCR[introtype] : "INTROTYPE_ERROR");
            if(introtype >= INTROTYPE_END)
                break;
            const size_t typesize = get_size_of_setup_intro_type(introtype) * 4;
            index += typesize;
            intro_size += typesize;
        }
        mem_track(setupbin, intro);
        intro_size += 4;
    }

    printf("\n\n  Tweaking Objlist lengths...");
    if(*objlist != 0)
    {
        mem_track(setupbin, objlist);
        size_t index = *objlist;
        while(index < (size_t)*size)
        {
            const uint8_t objtype = setupbin[index];
            //printf("\n  %s", objtype <= OBJTYPE_END ? OBJ_DESCR[objtype] : "OBJTYPE_ERROR");
            if(objtype >= OBJTYPE_END)
                break;
            const size_t typesize = get_size_of_setup_object_type(objtype) * 4;
            index += typesize;
            objlist_size += typesize;
            switch(objtype)
            {
                case OBJTYPE_DOOR: // door has 0x4 taken off the end, reducing from 0x100 to 0xFC
                    mem_shift_setup(setupbin, (size_t)*size - index, (uint32_t)index, -0x04);
                    index += -0x04;
                    *size += -0x04;
                    objlist_size += -0x04;
                    printf(" (%s)", OBJ_DESCR[objtype]);
                    break;
                case OBJTYPE_WEAPON: // weapon has 0x4 added to end, increasing from 0x88 to 0x8C
                    mem_shift_setup(setupbin, (size_t)*size - index, (uint32_t)index, 0x04);
                    memset(&setupbin[index], 0, 0x04);
                    index += 0x04;
                    *size += 0x04;
                    objlist_size += 0x04;
                    printf(" (%s)", OBJ_DESCR[objtype]);
                    break;
                case OBJTYPE_VEHICLE: // vehicle has an additional 0x3C tacked onto them
                    mem_shift_setup(setupbin, (size_t)*size - index, (uint32_t)index, 0x3C);
                    memset(&setupbin[index], 0, 0x3C);
                    index += 0x3C;
                    *size += 0x3C;
                    objlist_size += 0x3C;
                    printf(" (%s)", OBJ_DESCR[objtype]);
                    break;
                default:
                    break;
            }
        }
        objlist_size += 4;
    }

    mem_shift_setup(setupbin, (size_t)*size - 0x28, 0x28, 0x20); // update header for new XBLA header with lengths
    printf(" (New XBLA Header)");
    *size += 0x20;
    setup[0x28 / 4] = waypoints_size;
    setup[0x2C / 4] = waygroups_size;
    setup[0x30 / 4] = intro_size;
    setup[0x34 / 4] = objlist_size;
    setup[0x38 / 4] = patrols_size;
    setup[0x3C / 4] = ailists_size;
    setup[0x40 / 4] = padlist_size;
    setup[0x44 / 4] = padadvlist_size;

    mem_update_offsets(setup, memoffset);
    byteswap_setup(setupbin, *size);
    return 1;
}

int main(int argc, char **argv)
{
    printf("\n GoldenEye 007 Setup Converter For XBLA (%s)\n%s\n", __DATE__, LINE);
    if(argc != 4) // no file provided or too many arguments
    {
        printf("\n  About: Convert N64 setup for GoldenEye XBLA\n\n  Syntax: %s \"input setup\" \"output setup\" memory_offset\n  Example: %s \"usetupsevZ.bin\" \"converted.bin\" 82DE19D8\n", argv[0], argv[0]);
        goto exit;
    }
    FILE *setupinput = fopen(argv[1], "rb"); // load setup from argument
    if(setupinput == NULL)
    {
        printf("\n  Error: Aborted, setup cannot be opened");
        goto exit;
    }
    if(fseek(setupinput, 0, SEEK_END) != 0)
    {
        printf("\n  Error: Aborted, could not find end to setup file");
        goto error_setup;
    }
    long int filesize = ftell(setupinput); // get setup filesize
    if(filesize == -1)
    {
        printf("\n  Error: Aborted, could not find size to setup file");
        goto error_setup;
    }
    if(filesize % 4 != 0)
    {
        printf("\n  Error: Aborted, setup file size not aligned to 4 bytes");
        goto error_setup;
    }
    if(fseek(setupinput, 0, SEEK_SET) != 0)
    {
        printf("\n  Error: Aborted, could not find beginning of setup file");
        goto error_setup;
    }
    if(!((argv[3][7] != '\0') && (argv[3][8] == '\0')))
    {
        printf("\n  Error: Aborted, memory offset must be 4 bytes long (eg: 82DE19D8)");
        goto error_setup;
    }
    const uint32_t memoffset = (ASCII2HEX(argv[3][0]) << 28) | (ASCII2HEX(argv[3][1]) << 24) | (ASCII2HEX(argv[3][2]) << 20) | (ASCII2HEX(argv[3][3]) << 16) | (ASCII2HEX(argv[3][4]) << 12) | (ASCII2HEX(argv[3][5]) << 8) | (ASCII2HEX(argv[3][6]) << 4) | ASCII2HEX(argv[3][7]);
    if(memoffset % 4 != 0)
    {
        printf("\n  Error: Aborted, memory offset must be aligned to 4 bytes");
        goto error_setup;
    }
    printf("\n  Input setup: %s\n  RAM Address: %08X", argv[1], memoffset);

    uint8_t *setupbin = calloc((size_t)filesize * 2, 1);
    if(setupbin == NULL)
    {
        printf("\n  Error: Aborted, not enough memory to load setup");
        goto error_setup;
    }
    fread(setupbin, filesize, 1, setupinput); // read setup to file buffer

    printf("\n  Converting Setup...");
    if(!convert_setup(setupbin, &filesize, memoffset))
    {
        printf("\n  Error: Aborted, setup was unable to be converted");
        goto error_output;
    }

    FILE *setupoutput = fopen(argv[2], "wb"); // save setup
    if(setupoutput == NULL)
    {
        printf("\n  Error: Aborted, could not save setup output");
        goto error_output;
    }
    fwrite(setupbin, 1, filesize, setupoutput);
    fclose(setupoutput);

    printf("\n\n  Finished Successfully!\n  Output setup:\t%s", argv[2]);

error_output:
    free(setupbin);
error_setup:
    fclose(setupinput);
exit:
    printf("\n%s\n", LINE);
    return 0;
}