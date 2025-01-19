/*
struct coord3d
{
    char unk[0xc];
    float pos[3];
};

struct bonddata
{
    char unk[0x1AC];
    struct coord3d *coords;
    char unk2[0x34];
    float health;
    float armor;
    char unk3[0x14];
    int flash;
    char unk4[0x32C];
    float pos[3];
    char unk5[0x1CC8];
};

struct armorprop
{
    char unk1[0x03];
    char type;
    char unk2[0x54];
    float pos[3];
    char unk[0x20];
    float armorval;
};

static inline float pow2(float x) {
    return x * x;
}

void cal_dam(struct armorprop *prop)
{
    if(prop->type != 0x15) // armor type
        return;
    struct bonddata **pPlayer = (struct bonddata **)0x82F1FA98;
    const float *float1dot0 = (float *)0x82000AD8;
    const float *float50dot0 = (float *)0x82000B90;
    const float *float1000dot0 = (float *)0x8200371C;
    const float *float1000000dot0 = (float *)0x82003F0C;
    float nearest = (*float1000000dot0);
    char playergetarmor = 0, i;
    for(i = 0; i < 4; i++)
    {
        if (pPlayer[i] == NULL)
            continue;
        const float test = sqrt(pow2(pPlayer[i]->coords->pos[0] - prop->pos[0]) + pow2((pPlayer[i]->coords->pos[1]-(*float50dot0)) - prop->pos[1]) + pow2(pPlayer[i]->coords->pos[2] - prop->pos[2]));
        //const float test = sqrt(pow2(pPlayer[i]->pos[0] - prop->pos[0]) + pow2(pPlayer[i]->pos[1] - prop->pos[1]) + pow2(pPlayer[i]->pos[2] - prop->pos[2]));
        if(test < nearest)
        {
            nearest = test;
            playergetarmor = i;
        }
    }
    if(nearest > (*float1000dot0)) // nearest player is over 10 meters, abort
        return;
    pPlayer[playergetarmor]->armor = prop->armorval;
    // not part of original design
    //pPlayer[playergetarmor]->armor += prop->armorval;
    //if(pPlayer[playergetarmor]->armor > (*float1dot0))
    //    pPlayer[playergetarmor]->armor = *float1dot0;
    //pPlayer[playergetarmor]->flash = 0;
}
*/

const unsigned int armor_fix_code[] = {
    0x90610014, // stw r3,14h(r1) 
    0x81610014, // lwz r11,14h(r1) 
    0x896B0003, // lbz r11,3(r11) 
    0x2B0B0015, // cmplwi cr6,r11,21        ; 15h 
    0x419A0008, // beq cr6,cal_dam + 0018h (820a0438h) 
    0x480001BC, // b cal_dam + 01d0h (820a05f0h) 
    0x3D6082F1, // lis r11,-32015       ; 82F1h 
    0x616BFA98, // ori r11,r11,64152        ; 0FA98h 
    0x9161FFE4, // stw r11,-1Ch(r1) 
    0x3D608200, // lis r11,-32256       ; 8200h 
    0x616B0AD8, // ori r11,r11,2776     ; 0AD8h 
    0x9161FFDC, // stw r11,-24h(r1) 
    0x3D608200, // lis r11,-32256       ; 8200h 
    0x616B0B90, // ori r11,r11,2960     ; 0B90h 
    0x9161FFD8, // stw r11,-28h(r1) 
    0x3D608200, // lis r11,-32256       ; 8200h 
    0x616B371C, // ori r11,r11,14108        ; 371Ch 
    0x9161FFD4, // stw r11,-2Ch(r1) 
    0x3D608200, // lis r11,-32256       ; 8200h 
    0x616B3F0C, // ori r11,r11,16140        ; 3F0Ch 
    0x9161FFE8, // stw r11,-18h(r1) 
    0x8161FFE8, // lwz r11,-18h(r1) 
    0xC00B0000, // lfs fr0,0(r11) 
    0xD001FFEC, // stfs fr0,-14h(r1) 
    0x39600000, // li r11,0 
    0x9961FFE0, // stb r11,-20h(r1) 
    0x39600000, // li r11,0 
    0x9961FFD0, // stb r11,-30h(r1) 
    0x48000010, // b cal_dam + 0080h (820a04a0h) 
    0x8961FFD0, // lbz r11,-30h(r1) 
    0x396B0001, // addi r11,r11,1 
    0x9961FFD0, // stb r11,-30h(r1) 
    0x8961FFD0, // lbz r11,-30h(r1) 
    0x7D6B0774, // extsb r11,r11 
    0x2F0B0004, // cmpwi cr6,r11,4 
    0x4098010C, // bge cr6,cal_dam + 0198h (820a05b8h) 
    0x8961FFD0, // lbz r11,-30h(r1) 
    0x7D6B0774, // extsb r11,r11 
    0x556B103A, // slwi r11,r11,2 
    0x8141FFE4, // lwz r10,-1Ch(r1) 
    0x7D6A582E, // lwzx r11,r10,r11 
    0x2B0B0000, // cmplwi cr6,r11,0 
    0x409A0008, // bne cr6,cal_dam + 00b0h (820a04d0h) 
    0x4BFFFFC8, // b cal_dam + 0074h (820a0494h) 
    0x81610014, // lwz r11,14h(r1) 
    0x8941FFD0, // lbz r10,-30h(r1) 
    0x7D4A0774, // extsb r10,r10 
    0x554A103A, // slwi r10,r10,2 
    0x8121FFE4, // lwz r9,-1Ch(r1) 
    0x7D49502E, // lwzx r10,r9,r10 
    0x814A01AC, // lwz r10,1ACh(r10) 
    0xC00A000C, // lfs fr0,0Ch(r10) 
    0xC1AB0058, // lfs fr13,58h(r11) 
    0xEC006828, // fsubs fr0,fr0,fr13 
    0xD001FFF4, // stfs fr0,-0Ch(r1) 
    0x81610014, // lwz r11,14h(r1) 
    0x8941FFD0, // lbz r10,-30h(r1) 
    0x7D4A0774, // extsb r10,r10 
    0x554A103A, // slwi r10,r10,2 
    0x8121FFE4, // lwz r9,-1Ch(r1) 
    0x7D49502E, // lwzx r10,r9,r10 
    0x814A01AC, // lwz r10,1ACh(r10) 
    0xC00A0010, // lfs fr0,10h(r10) 
    0x8141FFD8, // lwz r10,-28h(r1) 
    0xC1AA0000, // lfs fr13,0(r10) 
    0xEC006828, // fsubs fr0,fr0,fr13 
    0xC1AB005C, // lfs fr13,5Ch(r11) 
    0xEC006828, // fsubs fr0,fr0,fr13 
    0xD001FFF8, // stfs fr0,-8(r1) 
    0x81610014, // lwz r11,14h(r1) 
    0x8941FFD0, // lbz r10,-30h(r1) 
    0x7D4A0774, // extsb r10,r10 
    0x554A103A, // slwi r10,r10,2 
    0x8121FFE4, // lwz r9,-1Ch(r1) 
    0x7D49502E, // lwzx r10,r9,r10 
    0x814A01AC, // lwz r10,1ACh(r10) 
    0xC00A0014, // lfs fr0,14h(r10) 
    0xC1AB0060, // lfs fr13,60h(r11) 
    0xEC006828, // fsubs fr0,fr0,fr13 
    0xD001FFFC, // stfs fr0,-4(r1) 
    0xC001FFFC, // lfs fr0,-4(r1) 
    0xC1A1FFFC, // lfs fr13,-4(r1) 
    0xEC000372, // fmuls fr0,fr0,fr13 
    0xC1A1FFF8, // lfs fr13,-8(r1) 
    0xC181FFF8, // lfs fr12,-8(r1) 
    0xEDAD0332, // fmuls fr13,fr13,fr12 
    0xC181FFF4, // lfs fr12,-0Ch(r1) 
    0xC161FFF4, // lfs fr11,-0Ch(r1) 
    0xED8C02F2, // fmuls fr12,fr12,fr11 
    0xEDAC682A, // fadds fr13,fr12,fr13 
    0xEC0D002A, // fadds fr0,fr13,fr0 
    0xEC00002C, // fsqrts fr0,fr0 
    0xD001FFF0, // stfs fr0,-10h(r1) 
    0xC001FFF0, // lfs fr0,-10h(r1) 
    0xC1A1FFEC, // lfs fr13,-14h(r1) 
    0xFF006800, // fcmpu cr6,fr0,fr13 
    0x40980014, // bge cr6,cal_dam + 0194h (820a05b4h) 
    0xC001FFF0, // lfs fr0,-10h(r1) 
    0xD001FFEC, // stfs fr0,-14h(r1) 
    0x8961FFD0, // lbz r11,-30h(r1) 
    0x9961FFE0, // stb r11,-20h(r1) 
    0x4BFFFEE0, // b cal_dam + 0074h (820a0494h) 
    0xC001FFEC, // lfs fr0,-14h(r1) 
    0x8161FFD4, // lwz r11,-2Ch(r1) 
    0xC1AB0000, // lfs fr13,0(r11) 
    0xFF006800, // fcmpu cr6,fr0,fr13 
    0x40990008, // ble cr6,cal_dam + 01b0h (820a05d0h) 
    0x48000024, // b cal_dam + 01d0h (820a05f0h) 
    0x81610014, // lwz r11,14h(r1) 
    0x8941FFE0, // lbz r10,-20h(r1) 
    0x7D4A0774, // extsb r10,r10 
    0x554A103A, // slwi r10,r10,2 
    0x8121FFE4, // lwz r9,-1Ch(r1) 
    0x7D49502E, // lwzx r10,r9,r10 
    0xC00B0084, // lfs fr0,84h(r11) 
    0xD00A01E8, // stfs fr0,1E8h(r10) 
    //0x4E800020, // blr 
};