// GoldenEye XBLA sky editor
//   $ gcc -O2 -o skyedit skyedit.c
//   $ ./skyedit 0 "default.xex" "skies.txt"
//   $ ./skyedit 1 "default.xex" "editedskies.txt" "aztec.xex"
// License: The MIT License (MIT)

#include <stdio.h>
#include <stdlib.h>

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(disable:4996)
#endif

// Unused macros helpers
//#define MAX(x, y) (((x) > (y)) ? (x) : (y))
//#define MIN(x, y) (((x) < (y)) ? (x) : (y))
//#define ROUNDADDRUP(x) ((((x) + 0x3) | 0x3) ^ 0x3)
//#define ASCII2HEX(x) (((x) <= '9') ? (x) - '0' : ((x) & 0x7) + 9)
//#define ARRAYSIZE(array) ((size_t)(sizeof(array)/sizeof(*array)))
//#define CLAMP(value, min, max) (((value) < (min)) ? (min) : (((value) > (max)) ? (max) : ((value) < (min)) ? (min) : (value)))

#define LINE "__________________________________________________________________"

// Memory defines
#define DATARAMOFFSET               (0x82420000U) // data segment RAM offset (82420000 - 830CC340)
#define DATABINOFFSET               (0x00413000U) // data segment bin offset (00413000 - 010BF340)
#define XEXFILESIZEMIN              (15*0x100000L) // minimum XEX filesize supported (15MB)

// Address macros
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

#define data_read(addr)             (readarrayint(xexbin, DATA(addr))) // read 4 byte int from XEX's rodata segment
#define data_write(addr, val)       (writearrayint(xexbin, DATA(addr), val)) // write 4 byte int to XEX's rodata segment

#define FOG_BANK_TABLE_START        (0x82858860U) // fog bank start
#define FOG_BANK_TABLE_END          (0x82859510U) // fog bank end
#define FOG_SMALL_BANK_TABLE_START  (0x82859510U) // fog small bank start
#define FOG_SMALL_BANK_TABLE_END    (0x828595F0U) // fog small bank end

static void write_fog_settings(unsigned char *xexbin, FILE *txtoutput)
{
    fprintf(txtoutput, "START OF SKY BANKS\n\n");
    for(size_t index = FOG_BANK_TABLE_START; index < FOG_BANK_TABLE_END - 0x38; index += 0x38) // load each fog bank
    {
        fprintf(txtoutput, "level: %04X\n", (data_read(index + 0x00) & 0xFFFF0000) >> 16);
        fprintf(txtoutput, "near_clip: %04X\n", data_read(index + 0x00) & 0xFFFF);
        fprintf(txtoutput, "far_fog: %04X\n", (data_read(index + 0x04) & 0xFFFF0000) >> 16);
        fprintf(txtoutput, "near_fog: %04X\n", data_read(index + 0x04) & 0xFFFF);
        fprintf(txtoutput, "max_obj_vis: %04X\n", (data_read(index + 0x08) & 0xFFFF0000) >> 16);
        fprintf(txtoutput, "far_obj_obfusc_dist: %04X\n", data_read(index + 0x08) & 0xFFFF);
        fprintf(txtoutput, "far_intensity: %04X\n", (data_read(index + 0x0C) & 0xFFFF0000) >> 16);
        fprintf(txtoutput, "near_intensity: %04X\n", data_read(index + 0x0C) & 0xFFFF);
        fprintf(txtoutput, "fog_rgb: %d %d %d\n", (data_read(index + 0x10) & 0xFF000000) >> 24, (data_read(index + 0x10) & 0x00FF0000) >> 16, (data_read(index + 0x10) & 0x0000FF00) >> 8);
        fprintf(txtoutput, "sky_flag: %02X\n", data_read(index + 0x10) & 0xFF);
        fprintf(txtoutput, "sky_height: %04X\n", (data_read(index + 0x14) & 0xFFFF0000) >> 16);
        fprintf(txtoutput, "unknown16: %04X\n", data_read(index + 0x14) & 0xFFFF);
        fprintf(txtoutput, "unknown18: %04X\n", (data_read(index + 0x18) & 0xFFFF0000) >> 16);
        fprintf(txtoutput, "unknown1A: %04X\n", data_read(index + 0x18) & 0xFFFF);
        fprintf(txtoutput, "sea_height: %04X\n", (data_read(index + 0x1C) & 0xFFFF0000) >> 16);
        fprintf(txtoutput, "unknown1E: %04X\n", data_read(index + 0x1C) & 0xFFFF);
        fprintf(txtoutput, "unknown20: %04X\n", (data_read(index + 0x20) & 0xFFFF0000) >> 16);
        fprintf(txtoutput, "unknown22: %04X\n", data_read(index + 0x20) & 0xFFFF);
        fprintf(txtoutput, "new_fog_dist: %08X\n", data_read(index + 0x24));
        fprintf(txtoutput, "new_fog_rgb: %d %d %d\n", (data_read(index + 0x28) & 0xFF000000) >> 24, (data_read(index + 0x28) & 0x00FF0000) >> 16, (data_read(index + 0x28) & 0x0000FF00) >> 8);
        fprintf(txtoutput, "unknown2B: %02X\n", data_read(index + 0x28) & 0xFF);
        fprintf(txtoutput, "far_clip_plane: %08X\n", data_read(index + 0x2C));
        fprintf(txtoutput, "near_clip_plane: %08X\n", data_read(index + 0x30));
        fprintf(txtoutput, "point_of_infinity_(unused): %08X\n\n", data_read(index + 0x34));
    }
    fprintf(txtoutput, "END OF SKY BANKS\n\nSTART OF SMALL SKY BANKS\n\n");
    for(size_t index = FOG_SMALL_BANK_TABLE_START; index < FOG_SMALL_BANK_TABLE_END - 0x38; index += 0x38) // load each small fog bank
    {
        fprintf(txtoutput, "level: %08X\n", data_read(index + 0x00));
        fprintf(txtoutput, "fog_rgb: %d %d %d\n", (data_read(index + 0x04) & 0xFF000000) >> 24, (data_read(index + 0x04) & 0x00FF0000) >> 16, (data_read(index + 0x04) & 0x0000FF00) >> 8);
        fprintf(txtoutput, "sky_flag: %02X\n", data_read(index + 0x04) & 0xFF);
        fprintf(txtoutput, "sky_height: %08X\n", data_read(index + 0x08));
        fprintf(txtoutput, "unknown0C: %08X\n", data_read(index + 0x0C));
        fprintf(txtoutput, "cloud_r: %08X\n", data_read(index + 0x10));
        fprintf(txtoutput, "cloud_g: %08X\n", data_read(index + 0x14));
        fprintf(txtoutput, "cloud_b: %08X\n", data_read(index + 0x18));
        fprintf(txtoutput, "sea_flag: %08X\n", data_read(index + 0x1C));
        fprintf(txtoutput, "sea_height: %08X\n", data_read(index + 0x20));
        fprintf(txtoutput, "sea_image: %08X\n", data_read(index + 0x24));
        fprintf(txtoutput, "sea_r: %08X\n", data_read(index + 0x28));
        fprintf(txtoutput, "sea_g: %08X\n", data_read(index + 0x2C));
        fprintf(txtoutput, "sea_b: %08X\n", data_read(index + 0x30));
        fprintf(txtoutput, "unknown34: %08X\n\n", data_read(index + 0x34));
    }
    fprintf(txtoutput, "END OF SKY BANKS");
}

static int read_fog_settings(unsigned char *xexbin, FILE *txtinput)
{
    unsigned int dataint[13] = {0};
    unsigned int datashort[16] = {0};
    unsigned int databyte[8] = {0};
    fseek(txtinput, 0x14, SEEK_SET);
    for(size_t index = FOG_BANK_TABLE_START; index < FOG_BANK_TABLE_END - 0x38; index += 0x38) // load each fog bank
    {
        if(data_read(index) == 0) // reached end of list
            break;
        if(fscanf(txtinput, "level: %04X\nnear_clip: %04X\nfar_fog: %04X\nnear_fog: %04X\nmax_obj_vis: %04X\nfar_obj_obfusc_dist: %04X\nfar_intensity: %04X\nnear_intensity: %04X\nfog_rgb: %u %u %u\nsky_flag: %02X\nsky_height: %04X\nunknown16: %04X\nunknown18: %04X\nunknown1A: %04X\nsea_height: %04X\nunknown1E: %04X\nunknown20: %04X\nunknown22: %04X\nnew_fog_dist: %08X\nnew_fog_rgb: %u %u %u\nunknown2B: %02X\nfar_clip_plane: %08X\nnear_clip_plane: %08X\npoint_of_infinity_(unused): %08X\n\n", &datashort[0], &datashort[1], &datashort[2], &datashort[3], &datashort[4], &datashort[5], &datashort[6], &datashort[7], &databyte[0], &databyte[1], &databyte[2], &databyte[3], &datashort[8], &datashort[9], &datashort[10], &datashort[11], &datashort[12], &datashort[13], &datashort[14], &datashort[15], &dataint[0], &databyte[4], &databyte[5], &databyte[6], &databyte[7], &dataint[1], &dataint[2], &dataint[3]) != 28)
        {
            printf("\n  Error: Could not parse sky settings (layout changed, aborted)");
            return 0;
        }
        data_write(index + 0x00, ((datashort[0] & 0xFFFF) << 16) | (datashort[1] & 0xFFFF));
        data_write(index + 0x04, ((datashort[2] & 0xFFFF) << 16) | (datashort[3] & 0xFFFF));
        data_write(index + 0x08, ((datashort[4] & 0xFFFF) << 16) | (datashort[5] & 0xFFFF));
        data_write(index + 0x0C, ((datashort[6] & 0xFFFF) << 16) | (datashort[7] & 0xFFFF));
        data_write(index + 0x10, ((databyte[0] & 0xFF) << 24) | ((databyte[1] & 0xFF) << 16) | ((databyte[2] & 0xFF) << 8) | (databyte[3] & 0xFF));
        data_write(index + 0x14, ((datashort[8] & 0xFFFF) << 16) | (datashort[9] & 0xFFFF));
        data_write(index + 0x18, ((datashort[10] & 0xFFFF) << 16) | (datashort[11] & 0xFFFF));
        data_write(index + 0x1C, ((datashort[12] & 0xFFFF) << 16) | (datashort[13] & 0xFFFF));
        data_write(index + 0x20, ((datashort[14] & 0xFFFF) << 16) | (datashort[15] & 0xFFFF));
        data_write(index + 0x24, dataint[0]);
        data_write(index + 0x28, ((databyte[4] & 0xFF) << 24) | ((databyte[5] & 0xFF) << 16) | ((databyte[6] & 0xFF) << 8) | (databyte[7] & 0xFF));
        data_write(index + 0x2C, dataint[1]);
        data_write(index + 0x30, dataint[2]);
        data_write(index + 0x34, dataint[3]);
    }
    fscanf(txtinput, "END OF SKY BANKS\n\nSTART OF SMALL SKY BANKS\n\n"); // skip ahead
    for(size_t index = FOG_SMALL_BANK_TABLE_START; index < FOG_SMALL_BANK_TABLE_END - 0x38; index += 0x38) // load each small fog bank
    {
        if(fscanf(txtinput, "level: %08X\nfog_rgb: %u %u %u\nsky_flag: %02X\nsky_height: %08X\nunknown0C: %08X\ncloud_r: %08X\ncloud_g: %08X\ncloud_b: %08X\nsea_flag: %08X\nsea_height: %08X\nsea_image: %08X\nsea_r: %08X\nsea_g: %08X\nsea_b: %08X\nunknown34: %08X\n\n", &dataint[0], &databyte[0], &databyte[1], &databyte[2], &databyte[3], &dataint[1], &dataint[2], &dataint[3], &dataint[4], &dataint[5], &dataint[6], &dataint[7], &dataint[8], &dataint[9], &dataint[10], &dataint[11], &dataint[12]) != 17)
        {
            printf("\n  Error: Could not parse small sky settings (layout changed, aborted)");
            return 0;
        }
        data_write(index + 0x00, dataint[0]);
        data_write(index + 0x04, ((databyte[0] & 0xFF) << 24) | ((databyte[1] & 0xFF) << 16) | ((databyte[2] & 0xFF) << 8) | (databyte[3] & 0xFF));
        data_write(index + 0x08, dataint[1]);
        data_write(index + 0x0C, dataint[2]);
        data_write(index + 0x10, dataint[3]);
        data_write(index + 0x14, dataint[4]);
        data_write(index + 0x18, dataint[5]);
        data_write(index + 0x1C, dataint[6]);
        data_write(index + 0x20, dataint[7]);
        data_write(index + 0x24, dataint[8]);
        data_write(index + 0x28, dataint[9]);
        data_write(index + 0x2C, dataint[10]);
        data_write(index + 0x30, dataint[11]);
        data_write(index + 0x34, dataint[12]);
    }
    return 1;
}

int main(int argc, char **argv)
{
    printf("\n Sky bank exporter/importer for GoldenEye XBLA Bean (%s)\n%s\n", __DATE__, LINE);
    int opmode = 0;
    if(argc > 2)
    {
        opmode = argv[1][0] == '1';
    }
    if((opmode == 0 && argc != 4) || (opmode == 1 && argc != 5)) // no file provided or too many arguments
    {
        printf("\n  About: Export/import new sky settings into GoldenEye XBLA\n\n  Syntax: %s operation_mode (0 = export/1 = import) \"input XEX\" \"input txt\" \"output XEX\" \n  Example: %s 0 \"default.xex\" \"skies.txt\"\n  Example: %s 1 \"default.xex\" \"editedskies.txt\" \"aztec.xex\"\n", argv[0], argv[0], argv[0]);
        goto exit;
    }
    FILE *xexinput = fopen(argv[2], "rb"); // load XEX from argument
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
    if(!opmode)
    {
        printf("\n  Operation Mode: Export Settings\n  Input XEX: %s\n  Sky Settings: %s", argv[2], argv[3]);
    }
    else
    {
        printf("\n  Operation Mode: Import Settings\n  Input XEX: %s\n  Sky Settings: %s\n  Output XEX: %s\n", argv[2], argv[3], argv[4]);
    }

    unsigned char *xexbin = malloc(filesize);
    if(xexbin == NULL)
    {
        printf("\n  Error: Aborted, not enough memory to load XEX");
        goto error_xex;
    }
    fread(xexbin, filesize, 1, xexinput); // read XEX to file buffer

    if(!opmode)
    {
        FILE *txtoutput = fopen(argv[3], "wb"); // save txt
        if(txtoutput == NULL)
        {
            printf("\n  Error: Aborted, could not write txt output");
            goto error_output;
        }
        write_fog_settings(xexbin, txtoutput);
        fclose(txtoutput);
    }
    else
    {
        FILE *txtinput = fopen(argv[3], "rb"); // load txt
        if(txtinput == NULL)
        {
            printf("\n  Error: Aborted, could not read txt input");
            goto error_output;
        }
        const int successful_read = read_fog_settings(xexbin, txtinput);
        fclose(txtinput);
        if(!successful_read)
            goto error_output;
        FILE *xexoutput = fopen(argv[4], "wb"); // save XEX
        if(xexoutput == NULL)
        {
            printf("\n  Error: Aborted, could not save XEX output");
            goto error_output;
        }
        fwrite(xexbin, 1, filesize, xexoutput);
        fclose(xexoutput);
    }
    printf("\n  Finished Successfully!");

error_output:
    free(xexbin);
error_xex:
    fclose(xexinput);
exit:
    printf("\n%s\n", LINE);
    return 0;
}