#pragma once

#include <string>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include "Core/HW/Memmap.h"

class DefaultGeckoCodes {
  public:
    void RunCodeInject(bool bNetplayEventCode, bool bIsRanked, bool bIsNight, u32 uGameMode, bool bDisableReplays);

  private:
    bool NetplayEventCode;
    bool IsRanked;
    bool IsNight;
    u32 GameMode;
    bool DisableReplays;


    // Default Rumble On [LittleCoaks]
    static const u32 aControllerRumble = 0x80366177;

    // Boot to Main Menu [LittleCoaks]
    static const u32 aBootToMainMenu = 0x8063F964;

    // Skip Memory Card Check on Main Menu [LittleCoaks]
    static const u32 aSkipMemCardCheck = 0x803C50E8;

    // Unlimited Extra Innings [LittleCoaks]
    static const u32 aUnlimitedExtraInnings = 0x80699A10;

    // Unlock Everything [LittleCoaks]
    static const u32 aUnlockEverything_1 = 0x800E870E;  // 0x0 loops of 0x2
    static const u32 aUnlockEverything_2 = 0x800E8710;  // 0x5 loops of 0x3
    static const u32 aUnlockEverything_3 = 0x800E8716;  // 0x5 loops of 0x1
    static const u32 aUnlockEverything_4 = 0x80361680;  // 0x29 loops of 0x1
    static const u32 aUnlockEverything_5 = 0x803616B0;  // 0x0 loops of 0x1 
    static const u32 aUnlockEverything_6 = 0x80361B20;  // 0x35 loops of 0x1
    static const u32 aUnlockEverything_7 = 0x80361C04;  // 0x3 loops of 0x1
    static const u32 aUnlockEverything_8 = 0x80361C14;  // 0x1 loops of 0x1

    // Pitch Clock [LittleCoaks]
    static const u32 aPitchClock_1 = 0x806b4490;
    static const u32 aPitchClock_2 = 0x806b42d0;
    static const u32 aPitchClock_3 = 0x806b46b8;

    // Bat Sound Effect on Start Game [LittleCoaks]
    static const u32 aBatSound = 0x80042cd0;  // write to 0x386001bb

    // Disable Music [LittleCoaks]
    static const u32 aDisableMusic_1 = 0x80062ab0;  // write to 0x38000000
    static const u32 aDisableMusic_2 = 0x806cccb0;  // write to 0x38000000

    // Never Cull [Roeming]
    static const u32 aNeverCull_1 = 0x8001dcf8;  // write to 0x38000007
    static const u32 aNeverCull_2 = 0x806aa4e4;  // write to 0x38000001
    static const u32 aNeverCull_3 = 0x806ab8b4;  // write to 0x38000001
    static const u32 aNeverCull_4 = 0x806f7b7c;  // write to 0x38000003 if equal to 0x881a0093

    // Ban Batter Pausing [LittleCoaks]
    static const u32 aBanBatterPausing = 0x806eed5c; // write to 0x38000000 if equal to 0xA0040006

    // Disable Replays [LittleCoaks]
    static const u32 aDisableReplays = 0x806bb214; // write  to 0x38000000 if equal to 0x38000001

    void InjectNetplayEventCode();
    void AddRankedCodes();


    struct DefaultGeckoCode
    {
      u32 addr;
      u32 conditionalVal;  // set to 0 if no onditional needed; condition is for when we don't want to write at an addr every frame
      std::vector<u32> codeLines;
    };
    // DefaultGeckoCode mGeckoCode;

    // Generate the Game ID at 0x802EBF8C when Start Game is pressed [LittleCoaks]
    const DefaultGeckoCode sGenerateGameID = {
        0x80042CCC, 0,
        {0x3C80802E, 0x6084BF8C, 0x7C6C42E6, 0x90640000, 0x3C80800F}
    };

    // Clear Game ID when exiting mid - game [LittleCoaks]
    const DefaultGeckoCode sClearGameID_1 =
        {0x806ed704, 0,
        {0x981F01D2, 0x3D00802E, 0x6108BF8C, 0x38000000, 0x90080000}
    };

    // Clear Game ID when exiting mid - game [LittleCoaks]
    const DefaultGeckoCode sClearGameID_2 =
        {0x806edf8c, 0,
        {0x981F01D2, 0x3D00802E, 0x6108BF8C, 0x38000000, 0x90080000}
    };

    // Clear Game ID when returning to main menu after game ends [LittleCoaks]
    const DefaultGeckoCode sClearGameID_3 = {
        0x8069AB2C, 0,
        {0x98050125, 0x3E40802E, 0x6252BF8C, 0x38600000, 0x90720000}
    };

    // Clear port info after game ends [LittleCoaks]
    const DefaultGeckoCode sClearPortInfo = {
        0x8063F14C, 0x38600000,
        {0x38600000, 0x3CA0802E, 0x60A5BF90, 0x98650001, 0xB0650002, 0xB0650004}
    };

    // Clear hit result after each pitch [PeacockSlayer]
    const DefaultGeckoCode sClearHitResult = {
        0x806bbf88, 0,
        {0x99090037, 0x3ea08089, 0x62b53baa, 0x99150000, 0x3aa00000}
    };

    // Store Port Info->0x802EBF94 (fielding port) and 0x802EBF95 (batting port) [LittleCoaks]
    const DefaultGeckoCode sStorePortInfo_1 = {
        0x80042CD8, 0,
        {0x9421FFB0, 0xBDC10008, 0x3DE0802E, 0x61EFBF91, 0x3E008089, 0x62102ACA,
        0x8A100000, 0x2C100001, 0x4182000C, 0x3A000001, 0x48000008, 0x3A000005,
        0x9A0F0000, 0x3E00800E, 0x6210874D, 0x8A100000, 0x3A100001, 0x9A0F0001,
        0xB9C10008, 0x38210050, 0x38A0007F}
    };

    // Store Port Info [LittleCoaks]
    const DefaultGeckoCode sStorePortInfo_2 = {
        0x806706B8, 0x3c608089,
        {0x3FE08089, 0x63FF3928, 0x7C04F800, 0x3FE0802E, 0x63FFBF91, 0x41820018, 0x887F0000,
        0x987F0004, 0x887F0001, 0x987F0003, 0x48000014, 0x887F0001, 0x987F0004, 0x887F0000,
        0x987F0003, 0x3C608089}
    };

    // Remember Who Quit; stores the port who quit a game at 0x802EBF93 [LittleCoaks]
    const DefaultGeckoCode sRememberWhoQuit_1 = {
        0x806ed700, 0,
        {0xb08300fe, 0x3e80802e, 0x6294bf93, 0x8ab40001, 0x9ab40000}
    };

    // Remember Who Quit [LittleCoaks]
    const DefaultGeckoCode sRememberWhoQuit_2 = {
        0x806edf88, 0,
        {0xb08300fe, 0x3e80802e, 0x6294bf93, 0x8ab40002, 0x9ab40000}
    };

    // Anti Quick Pitch [PeacockSlayer, LittleCoaks]
    const DefaultGeckoCode sAntiQuickPitch = {
        0x806B406C, 0xa01e0006,
        {0x3DC08089, 0x61CE80DE, 0x89CE0000, 0x2C0E0000, 0x4082001C, 0x38000000,
        0x3DC08089, 0x61CE099D, 0x89CE0000, 0x2C0E0001, 0x41820008, 0xA01E0006}
    };

    // Default Competitive Rules [LittleCoaks]
    const DefaultGeckoCode sDefaultCompetitiveRules = {
        0x80049D08, 0,
        {0x3DC08035, 0x61CE323B, 0x3A000000, 0x7DEE80AE, 0x2C0F0001, 0x4182001C,
        0x3A100001, 0x2C100012, 0x4082FFEC, 0x39C00004, 0x39E00001, 0x4800000C,
        0x39C00002, 0x39E00000, 0x99C3003E, 0x99E30048, 0x99E3004C, 0x3A000001,
        0x9A03003F, 0x3C60803C}
    };

    // Manual Fielder Select 4.0 [PeacockSlayer, LittleCoaks]
    const DefaultGeckoCode sManualSelect = {
        0x80678F8C, 0x88061BD1,
        {0x7C6E1B78, 0x7C0802A6, 0x90010004, 0x9421FF00, 0xBC610008, 0x3D20802E, 0x6129BF97,
         0x3C808089, 0x6084269E, 0xA0840000, 0x2C040004, 0x418201B8, 0x3C808089, 0x60842973,
         0x88840000, 0x2C040003, 0x40800024, 0x3C808089, 0x60842701, 0x88840000, 0x2C040000,
         0x41820010, 0x2C040003, 0x41820008, 0x48000184, 0x3D008089, 0x61082898, 0xA1080000,
         0x88890000, 0x71050010, 0x2C050010, 0x4082000C, 0x38800000, 0x98890000, 0x71050800,
         0x2C050800, 0x4082000C, 0x38800001, 0x98890000, 0x71050400, 0x2C050400, 0x4082000C,
         0x38800002, 0x98890000, 0x71050020, 0x2C050020, 0x4082000C, 0x38800003, 0x98890000,
         0x71050040, 0x2C050040, 0x4082000C, 0x38800004, 0x98890000, 0x2C040005, 0x40800108,
         0x2C040000, 0x41820100, 0x2C040003, 0x41820038, 0x2C040004, 0x4182000C, 0x3884FFFF,
         0x48000098, 0x3CE08089, 0x60E726B2, 0x88E70000, 0x2C070001, 0x41820014, 0x3CE08089,
         0x60E70E80, 0x39000004, 0x48000010, 0x3CE08089, 0x60E70B38, 0x39000008, 0x38A00000,
         0x38800000, 0x3CC08088, 0x60C6F368, 0xC3870000, 0xC3A60000, 0xFFDCE828, 0xFFDEF02A,
         0xFFC0F210, 0x7F883C2E, 0xC3A60008, 0xFF9CE828, 0xFF9CE02A, 0xFF80E210, 0xFFDEE02A,
         0x2C050000, 0x4182000C, 0xFC0AF040, 0x4180000C, 0xFD40F090, 0x7CA42B78, 0x38A50001,
         0x38C60268, 0x2C050009, 0x4082FFB0, 0x3CC08088, 0x60C6F53B, 0x39000000, 0x1D280268,
         0x7CA930AE, 0x2C05000F, 0x4082000C, 0x38A00002, 0x7CA931AE, 0x39080001, 0x2C080009,
         0x4180FFE0, 0x1CA40268, 0x38E0000F, 0x7CE531AE, 0x3CC08089, 0x60C62800, 0x98860001,
         0x98860007, 0x48000028, 0x38800000, 0x98890000, 0xB8610008, 0x80010104, 0x38210100,
         0x7C0803A6, 0x7DC37378, 0x88061BD1, 0x4800001C, 0xB8610008, 0x80010104, 0x38210100,
         0x7C0803A6, 0x7DC37378, 0x38000001}
    };

    // Night time Mario Stadium [LittleCoaks]
    const DefaultGeckoCode sNightStadium = {
        0x80650678, 0x98030058,
        {0x98030058, 0x89240009, 0x2C090000, 0x4082000C, 0x3A400001, 0x9A44000A}};

    // Pitch Clock (10 seconds) [LittleCoaks]
    const DefaultGeckoCode sPitchClock = {
        0x806B4070, 0x540005ef,
        {0x3DC08089, 0x61CE0AE0, 0xA1CE0000, 0x2C0E0258, // last 4 bytes of this instruction determine pitch clock length
        0x40820008, 0x38000100, 0x540005EF}
    };

    // Remove Dingus Bunting [LittleCoaks]
    const DefaultGeckoCode sRemoveDingus = {
        0x8069811C, 0x38000001,
        {0x3DC08089, 0x61CE2899,
        0x880E0000, 0x70000010,
        0x2C000000, 0x41820008, 0x38000001}};

    // Store Random Batting Ints [Roeming]
    const DefaultGeckoCode sStoreRandBattingInts = {
        0x80651E68, 0x98040091,
        {0x98040091, 0x3CA08089,
        0x38A52684, 0x3CC0802F,
        0x38C6C010, 0x80850000,
        0xB0860000, 0x80850004,
        0xB0860002, 0xA0850018, 0xB0860004}}; // Store Rand1 at 802ec010, Rand2 at 802ec012 and Rand3, at 802ec014

    // Checksum Calculator [LitleCoaks]
    const DefaultGeckoCode sChecksum = {
        0x8069A160, 0x3c80800f,
        {0x7C0802A6, 0x90010004, 0x9421FF00, 0xBC610008, 0x38800000, 0x3CA08089,
         0x60A52AAA, 0x88A50000, 0x7C852214, 0x3CA08089, 0x60A52AAB, 0x88A50000,
         0x7C852214, 0x3CA08089, 0x60A509A1, 0x88A50000, 0x7C852214, 0x3CA08089,
         0x60A52857, 0x88A50000, 0x7C852214, 0x3CA08036, 0x60A5F3A9, 0x88A50000,
         0x7C852214, 0x3CA08089, 0x60A509AA, 0x88A50000, 0x7C852214, 0x3CA08087,
         0x60A52540, 0x88A50000, 0x7C852214, 0x3CA08089, 0x60A50971, 0x88A50000,
         0x7C852214, 0x3CA08089, 0x60A528A3, 0x88A50000, 0x7C852214, 0x3CA08089,
         0x60A5294D, 0x88A50000, 0x7C852214, 0x3CA08089, 0x60A5296F, 0x88A50000,
         0x7C852214, 0x3CA08089, 0x60A5296B, 0x88A50000, 0x7C852214, 0x3CA08089,
         0x60A52973, 0x88A50000, 0x7C852214, 0x3CA08089, 0x60A52AD6, 0x88A50000,
         0x7C852214, 0x3CA08089, 0x60A52AD7, 0x88A50000, 0x7C852214, 0x3CA08089,
         0x60A52AD8, 0x88A50000, 0x7C852214, 0x3CA08089, 0x60A509BA, 0x88A50000,
         0x7C852214, 0x3CA08088, 0x60A5F09D, 0x88A50000, 0x7C852214, 0x3CA08088,
         0x60A5F1F1, 0x88A50000, 0x7C852214, 0x3CA08088, 0x60A5F345, 0x88A50000,
         0x7C852214, 0x3CA08089, 0x60A538AD, 0x88A50000, 0x7C852214, 0x3CA08089,
         0x60A509A3, 0x88A50000, 0x7C852214, 0x3CA08089, 0x60A53BAA, 0x88A50000,
         0x7C852214, 0x3CA08089, 0x60A528A4, 0x88A50000, 0x7C852214, 0x3CA08089,
         0x60A528CA, 0x88A50000, 0x7C852214, 0x3CA08089, 0x60A50971, 0x88A50000,
         0x7C852214, 0x3CA08089, 0x60A50AD9, 0x88A50000, 0x7C852214, 0x3CA08089,
         0x60A50B38, 0x80A50000, 0x7C852214, 0x3CA08089, 0x60A50B3C, 0x80A50000,
         0x7C852214, 0x3CA08089, 0x60A50B40, 0x80A50000, 0x7C852214, 0x3CA0802E,
         0x60A5BFB8, 0x90850000, 0xB8610008, 0x80010104, 0x38210100, 0x7C0803A6,
         0x3C80800F}};  // 0x802EBFB8 == checksum addr


    // Hold Z for Easy Batting [Roeming]
    const DefaultGeckoCode sEasyBattingZ = {
        0x806A82B0, 0x28000000,
        {0x3CA08089, 0x38852990, 0x80040000, 0x1C000004, 0x38852A78, 0x7C840214,
        0x80040000, 0x1C000010,0x3885392C, 0x7C840214, 0xA0040000, 0x70000010, 0x28000000}
    };


    // Hazardless Stadiums [LittleCoaks, PeacockSlayer]
    const DefaultGeckoCode sHazardless = {
        0x80699508, 0x88a40009,
        {0x88A40009, 0x9421FFB0, 0xBDC10008, 0x3E608035, 0x6273323B, 0x3AA00000, 0x7E93A8AE,
         0x2C140001, 0x418200D8, 0x3AB50001, 0x2C150012, 0x4082FFEC, 0x2C050000, 0x418200C4,
         0x2C050006, 0x408000BC, 0x2C050001, 0x41820028, 0x2C050002, 0x4182003C, 0x2C050003,
         0x41820050, 0x2C050004, 0x4182005C, 0x2C050005, 0x4182007C, 0x48000090, 0x3E608070,
         0x627356C8, 0x3E803860, 0x92930000, 0x3E803800, 0x92931638, 0x48000074, 0x3E608070,
         0x6273FC30, 0x3E806000, 0x92930000, 0x3E803800, 0x9293376C, 0x48000058, 0x3E608069,
         0x62739E54, 0x3E803800, 0x92930000, 0x48000044, 0x3E60807C, 0x6273D098, 0x3A800000,
         0x3AA00000, 0x9A930011, 0x3AB50001, 0x3A730014, 0x2C150010, 0x4180FFF0, 0x4800001C,
         0x3E608072, 0x6273F950, 0x3E806000, 0x92930000, 0x92934A54, 0x92934A60, 0xB9C10008,
         0x38210050}
    };

    void WriteAsm(DefaultGeckoCode CodeBlock);
    u32 aWriteAddr;  // address where the first code gets written to

    std::vector<DefaultGeckoCode> sRequiredCodes =
    {sGenerateGameID, sClearGameID_1, sClearGameID_2, sClearGameID_3,
    sClearPortInfo, sClearHitResult, sStorePortInfo_1, sStorePortInfo_2,
        sRememberWhoQuit_1, sRememberWhoQuit_2, sStoreRandBattingInts, sChecksum};

    std::vector<DefaultGeckoCode> sNetplayCodes =
    {sAntiQuickPitch, sDefaultCompetitiveRules, sManualSelect, sRemoveDingus};
};
