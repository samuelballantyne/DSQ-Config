#include "emulatorutils.h"
#include <QDir>
#include <QDebug>
#include <QFont>
#include <QComboBox>
#include <QRegularExpression>
#include <QHash>
#include <QPair>


EmulatorUtils::EmulatorUtils()
{
    // Constructor, empty for now
}

void EmulatorUtils::mapEmulator(QString &friendly, QString &demulShooterExe)
{
    static const QHash<QString, QPair<QString, QString>> map = {
        /* ------- 32-bit DemulShooter.exe targets ------- */
        { "Coastal",                          {"coastal",    "DemulShooter.exe"} },
        { "Cxbx-Reloaded",                    {"chihiro",    "DemulShooter.exe"} },
        { "Demul 0.57 / 0.582 / 0.7a",        {"demul057",   "DemulShooter.exe"} },
        { "Demul 0.7a Only",                  {"demul07a",   "DemulShooter.exe"} },
        { "Demul 0.582 and Above",            {"demul058",   "DemulShooter.exe"} },
        { "Dolphin x64 v5.0",                 {"dolphin5",   "DemulShooter.exe"} },
        { "Namco ES4 Games",                  {"es4",        "DemulShooter.exe"} },
        { "GameWax Games",                    {"gamewax",    "DemulShooter.exe"} },
        { "Global VR Games",                  {"globalvr",   "DemulShooter.exe"} },
        { "KONAMI Arcade",                    {"konami",     "DemulShooter.exe"} },
        { "TeknoParrot Loader (Lindbergh)",   {"lindbergh",  "DemulShooter.exe"} },
        { "Model2 Emulator v1.1a",            {"model2",     "DemulShooter.exe"} },
        { "P&P Marketing Arcade",             {"ppmarket",   "DemulShooter.exe"} },
        { "TeknoParrot Loader (Raw Thrill)",  {"rawthrill",  "DemulShooter.exe"} },
        { "RingEdge 2 Games",                 {"ringedge2",  "DemulShooter.exe"} },
        { "TeknoParrot Loader (RingWide)",    {"ringwide",   "DemulShooter.exe"} },
        { "SEGA Arcade (Plants vs Zombies)",  {"sega",       "DemulShooter.exe"} },   // NEW
        { "Taito Type X Games",               {"ttx",        "DemulShooter.exe"} },
        { "Windows Games",                    {"windows",    "DemulShooter.exe"} },

        /* ------- 64-bit DemulShooterX64.exe targets ---- */
        { "Adrenaline Amusements",            {"aagames",    "DemulShooterX64.exe"} },
        { "SEGA Amusement Linkage Live System",{"alls",      "DemulShooterX64.exe"} },
        { "Namco ES3 System",                 {"es3",        "DemulShooterX64.exe"} },
        { "Flycast v2.0",                     {"flycast",    "DemulShooterX64.exe"} },
        { "RPCS3 System 357",                 {"rpcs3",      "DemulShooterX64.exe"} },
        { "Raw Thrill Arcade (64-bit)",       {"rawthrill",  "DemulShooterX64.exe"} }, // NEW
        { "Sega Nu",                          {"seganu",     "DemulShooterX64.exe"} },
        { "UNIS Technology",                  {"unis",       "DemulShooterX64.exe"} },
        { "United Distribution Company",      {"udc",        "DemulShooterX64.exe"} }, // NEW
        { "Windows Games (64)",               {"windows",    "DemulShooterX64.exe"} }
    };

    if (map.contains(friendly)) {
        auto [tgt, exe] = map.value(friendly);
        friendly         = tgt;
        demulShooterExe  = exe;
    } else {
        demulShooterExe.clear();   // unknown – caller should handle
    }
}

QString EmulatorUtils::mapRom(const QString &rom)
{
    static const QHash<QString, QString> romMap = {
        /* Coastal */
        { "Wild West Shootout",                          "wws" },

        /* Chihiro */
        { "Virtua Cop 3",                                "vcop3" },

        /* Model 2 */
        { "Behind Enemy Lines",                          "bel" },
        { "Gunblade NY",                                 "gunblade" },
        { "House of the Dead",                           "hotd" },
        { "Rail Chase 2",                                "rchase2" },
        { "Virtua Cop",                                  "vcop" },
        { "Virtua Cop 2",                                "vcop2" },

        /* Demul (057/058/07a) */
        { "Confidential Mission",                        "confmiss" },
        { "Death Crimson OX (USA)",                      "deathcox" },
        { "Death Crimson OX (JAP)",                      "deathcoxo" },
        { "House of The Dead II (US)",                   "hotd2" },
        { "House of The Dead II",                        "hotd2o" },
        { "House of The Dead II (Prototype)",            "hotd2p" },
        { "Brave Fire Fighters",                         "braveff" },
        { "Sega Clay Challenge",                         "claychal" },
        { "Manic Panic Ghosts",                          "manicpnc" },
        { "Pokasuka Ghosts",                             "pokasuka" },
        { "Ranger Mission",                              "rangrmsn" },
        { "Sports Shooting USA",                         "sprtshot" },       // NEW
        { "Extreme Hunting",                             "xtrmhunt" },
        { "Extreme Hunting 2",                           "xtrmhnt2" },
        { "The Maze of the Kings",                       "mok" },            // CHANGED (was mazan)
        { "Lupin the Third (the shooting)",              "lupinsho" },
        { "Ninja Assault (World)",                       "ninjaslt" },
        { "Ninja Assault (Asia)",                        "ninjaslta" },
        { "Ninja Assault (Japan)",                       "ninjasltj" },
        { "Ninja Assault (US)",                          "ninjasltu" },

        /* Es4 */
        { "Point Blank X",                               "pblankx" },

        /* GameWax */
        { "Akuma Mortis Immortal",                       "akuma" },

        /* Global VR */
        { "Aliens Extermination",                        "aliens" },
        { "Far Cry: Paradise Lost",                      "farcry" },
        { "Fright Fear Land",                            "fearland" },

        /* KONAMI */
        { "Castlevania: The Arcade",                     "hcv" },
        { "Lethal Enforcers 3",                          "le3" },
        { "Wartran Troopers",                            "wartran" },

        /* Lindbergh (TeknoParrot) */
        { "Too Spicy",                                   "2spicy" },
        { "Ghost Squad Evolution",                       "gsquad" },
        { "House of the Dead 4",                         "hotd4" },
        { "House of the Dead 4: Special",                "hotd4sp" },
        { "House of the Dead: EX",                       "hotdex" },
        { "Let's Go Jungle",                             "lgj" },
        { "Let's Go Jungle Special",                     "lgjsp" },
        { "Rambo",                                       "rambo" },

        /* RingWide */
        { "Let's Go Island",                             "lgi" },
        { "Let's Go Island 3D",                          "lgi3D" },
        { "Medaru no Gunman",                            "mng" },            // NEW
        { "Operation G.H.O.S.T.",                        "og" },
        { "Sega Dream Riders",                           "sdr" },
        { "Sega Golden Gun",                             "sgg" },
        { "Transformers: Human Alliance",                "tha" },

        /* RingEdge 2 */
        { "Transformers: Shadow Rising",                 "tsr" },

        /* Raw Thrill (32-bit) */
        { "Aliens Armageddon",                           "aa" },
        { "Jurassic Park",                               "jp" },
        { "Target: Terror - Gold",                       "ttg" },
        { "Terminator Salvation",                        "ts" },
        { "Walking Dead",                                "wd" },

        /* Raw Thrill Arcade (64-bit) */
        { "Nerf Arcade",                                 "nerfa" },           // NEW

        /* P&P Marketing */
        { "Police Trainer 2",                            "policetr2" },

        /* Sega Arcade (PVZ) */
        { "Plants Vs Zombies: Last Stand",               "pvz" },             // NEW

        /* Taito Type-X */
        { "Block King Ball Shooter",                     "bkbs" },
        { "Elevator Action Death Parade",                "eapd" },
        { "Silent Hill: The Arcade",                     "sha" },
        { "Gaia Attack 4",                               "gattack4" },
        { "Gundam: Spirit of Zeon",                      "gsoz" },
        { "Gundam: Spirit of Zeon (DualScreen)",         "gsoz2p" },
        { "Haunted Museum",                              "hmuseum" },
        { "Haunted Museum 2",                            "hmuseum2" },
        { "Music Gun Gun! 2",                            "mgungun2" },        // NEW

        /* Windows (32-bit) */
        { "Alien Disco Safari",                          "ads" },
        { "Art Is Dead",                                 "artdead" },
        { "Bug Busters",                                 "bugbust" },
        { "Colt's Wild West Shootout",                   "coltwws" },         // NEW
        { "Friction",                                    "friction" },
        { "Heavy Fire Afghanistan",                      "hfa" },
        { "Heavy Fire Afghanistan (Dual Player)",        "hfa2p" },
        { "Heavy Fire Shattered Spear",                  "hfss" },
        { "Heavy Fire Shattered Spear (Dual Player)",    "hfss2p" },
        { "House of The Dead II (PC)",                   "hod2pc" },          // NEW
        { "House of The Dead III (PC)",                  "hod3pc" },          // NEW
        { "House of The Dead: Overkill",                 "hodo" },            // NEW
        { "Mad Bullets",                                 "madbul" },          // NEW
        { "Reload",                                      "reload" },          // NEW

        /* Windows (64-bit) */
        { "Big Buck Hunter: Ultimate Trophy",            "bbhut" },           // NEW
        { "DCOP",                                        "dcop" },
        { "Operation Wolf Returns",                      "opwolfr" },
        { "House of the Dead: Remake (Arcade Plugin)",   "hotdra" },

        /* Sega Nu */
        { "Luigi Mansion Arcade",                        "lma" },

        /* UNIS */
        { "Elevator Action Invasion",                    "eai" },
        { "Night Hunter Arcade",                         "nha" },
        { "Raccoon Rampage",                             "racramp" },

        /* UDC */
        { "Mars Sortie",                                 "marss" }
    };

    if (romMap.contains(rom))
        return romMap.value(rom);

    // fallback : lowercase alnum only
    QString simplified = rom.toLower();
simplified.remove(QRegularExpression("[^a-z0-9]"));
    return simplified.isEmpty() ? "unknown" : simplified;
}

void EmulatorUtils::updateGamesList(const QString &emuFriendly, QComboBox *romBox)
{
    if (!romBox) return;
    romBox->clear();
    romBox->setEnabled(true);

    static const QHash<QString, QStringList> gameList = {
        /* Only show games the wiki lists for that *friendly* emulator name */
        { "Coastal",                     { "Wild West Shootout" } },
        { "Cxbx-Reloaded",               { "Virtua Cop 3" } },

        { "Demul 0.57 / 0.582 / 0.7a",
            { "Confidential Mission",
              "Death Crimson OX (USA)", "Death Crimson OX (JAP)",
              "House of The Dead II (US)", "House of The Dead II",
              "House of The Dead II (Prototype)", "Lupin the Third (the shooting)",
              "The Maze of the Kings", "Sports Shooting USA",
              "Ninja Assault (World)", "Ninja Assault (Asia)",
              "Ninja Assault (Japan)", "Ninja Assault (US)",
              "Brave Fire Fighters", "Sega Clay Challenge", "Manic Panic Ghosts",
              "Pokasuka Ghosts", "Ranger Mission",
              "Extreme Hunting", "Extreme Hunting 2" } },

        { "Demul 0.7a Only",
            { "Brave Fire Fighters", "Sega Clay Challenge", "Manic Panic Ghosts",
              "Pokasuka Ghosts", "Ranger Mission", "Extreme Hunting", "Extreme Hunting 2" } },

        { "Demul 0.582 and Above",
            { "Ninja Assault (World)", "Ninja Assault (Asia)",
              "Ninja Assault (Japan)", "Ninja Assault (US)" } },

        { "Dolphin x64 v5.0",           { "Parameter not used" } },

        { "Namco ES4 Games",            { "Point Blank X" } },
        { "GameWax Games",              { "Akuma Mortis Immortal" } },
        { "Global VR Games",            { "Aliens Extermination", "Far Cry: Paradise Lost", "Fright Fear Land" } },
        { "KONAMI Arcade",              { "Castlevania: The Arcade", "Lethal Enforcers 3", "Wartran Troopers" } },

        { "TeknoParrot Loader (Lindbergh)",
            { "Too Spicy", "Ghost Squad Evolution", "House of the Dead 4",
              "House of the Dead 4: Special", "House of the Dead: EX",
              "Let's Go Jungle", "Let's Go Jungle Special", "Rambo" } },

        { "Model2 Emulator v1.1a",
            { "Behind Enemy Lines", "Gunblade NY", "House of the Dead",
              "Rail Chase 2", "Virtua Cop", "Virtua Cop 2" } },

        { "P&P Marketing Arcade",       { "Police Trainer 2" } },

        { "TeknoParrot Loader (Raw Thrill)",
            { "Aliens Armageddon", "Jurassic Park", "Target: Terror - Gold",
              "Terminator Salvation", "Walking Dead" } },

        { "RingEdge 2 Games",           { "Transformers: Shadow Rising" } },

        { "TeknoParrot Loader (RingWide)",
            { "Let's Go Island", "Let's Go Island 3D", "Medaru no Gunman",
              "Operation G.H.O.S.T.", "Sega Dream Riders",
              "Sega Golden Gun", "Transformers: Human Alliance" } },

        { "SEGA Arcade (Plants vs Zombies)", { "Plants Vs Zombies: Last Stand" } },

        { "Taito Type X Games",
            { "Block King Ball Shooter", "Elevator Action Death Parade",
              "Silent Hill: The Arcade", "Gaia Attack 4",
              "Gundam: Spirit of Zeon", "Gundam: Spirit of Zeon (DualScreen)",
              "Haunted Museum", "Haunted Museum 2", "Music Gun Gun! 2" } },

        { "Windows Games",
            { "Alien Disco Safari", "Art Is Dead", "Bug Busters", "Colt's Wild West Shootout",
              "Friction", "Heavy Fire Afghanistan", "Heavy Fire Afghanistan (Dual Player)",
              "Heavy Fire Shattered Spear", "Heavy Fire Shattered Spear (Dual Player)",
              "House of The Dead II (PC)", "House of The Dead III (PC)",
              "House of The Dead: Overkill", "Mad Bullets", "Reload" } },

        /* ---------- 64-bit group ---------- */
        { "Adrenaline Amusements",      { "Drakon: Realm Keepers", "Rabbids Hollywood Arcade", "Tomb Raider Arcade" } },
        { "SEGA Amusement Linkage Live System", { "House of the Dead: Scarlet Dawn" } },
        { "Namco ES3 System",          { "Time Crisis 5" } },
        { "Flycast v2.0",
            { "Confidential Mission", "Death Crimson OX", "House of The Dead II (US)",
              "House of The Dead II", "House of The Dead II (Prototype)",
              "Lupin the Third (the shooting)", "The Maze of the Kings",
              "Ninja Assault (World)", "Ninja Assault (Asia)",
              "Ninja Assault (Japan)", "Ninja Assault (US)" } },
        { "RPCS3 System 357",          { "Dark Escape 4D", "Deadstorm Pirates: Special Edition", "Sailor Zombies" } },
        { "Raw Thrill Arcade (64-bit)",{ "Nerf Arcade" } },
        { "Sega Nu",                   { "Luigi Mansion Arcade" } },
        { "United Distribution Company",{ "Mars Sortie" } },
        { "UNIS Technology",           { "Elevator Action Invasion", "Night Hunter Arcade", "Raccoon Rampage" } },
        { "Windows Games (64)",
            { "Big Buck Hunter: Ultimate Trophy", "DCOP", "Operation Wolf Returns",
              "House of the Dead: Remake (Arcade Plugin)" } }
    };

    if (gameList.contains(emuFriendly))
        romBox->addItems(gameList.value(emuFriendly));
    else
        romBox->setEnabled(false);   // unknown emulator
}

void EmulatorUtils::updateEmulatorPath(const QString &emulator, QString &emulatorPath, QString &romPath)
{
    #ifdef Q_OS_WIN
    // Windows-specific paths
    if (emulator == "Coastal") {
        emulatorPath = "C:/Coastal/Coastal.exe";
        romPath = "C:/Coastal/roms";
    } else if (emulator == "Cxbx-Reloaded") {
        emulatorPath = "C:/Cxbx-Reloaded/chihiro.exe";
        romPath = "C:/Cxbx-Reloaded/roms";
    } else if (emulator == "Demul 0.57 / 0.582 / 0.7a" || emulator == "Demul 0.7a Only" || emulator == "Demul 0.582 and Above") {
        emulatorPath = "C:/Demul/demul.exe";
        romPath = "C:/Demul/roms";
    } else if (emulator == "Dolphin x64 v5.0") {
        emulatorPath = "C:/Dolphin/Dolphin.exe";
        romPath = "C:/Dolphin/roms";
    } else if (emulator == "Namco ES4 Games") {
        emulatorPath = "C:/NamcoES4/es4.exe";
        romPath = "C:/NamcoES4/roms";
    } else if (emulator == "GameWax Games") {
        emulatorPath = "C:/GameWax/GameWax.exe";
        romPath = "C:/GameWax/roms";
    } else if (emulator == "Global VR Games") {
        emulatorPath = "C:/GlobalVR/GlobalVR.exe";
        romPath = "C:/GlobalVR/roms";
    } else if (emulator == "KONAMI Arcade") {
        emulatorPath = "C:/KONAMI/Arcade.exe";
        romPath = "C:/KONAMI/roms";
    } else if (emulator == "TeknoParrot Loader (Lindbergh)" ||
               emulator == "TeknoParrot Loader (Raw Thrill)" ||
               emulator == "TeknoParrot Loader (RingWide)") {
        emulatorPath = "C:/TeknoParrot/TeknoParrotUi.exe";
        romPath = "C:/TeknoParrot/roms";
    } else if (emulator == "Model2 Emulator v1.1a") {
        emulatorPath = "C:/Sega Model 2/emulator_multicpu.exe";
        romPath = "C:/Sega Model 2/roms";
    } else if (emulator == "P&P Marketing Arcade") {
        emulatorPath = "C:/PandP/Arcade.exe";
        romPath = "C:/PandP/roms";
    } else if (emulator == "RingEdge 2 Games") {
        emulatorPath = "C:/RingEdge2/ringedge2.exe";
        romPath = "C:/RingEdge2/roms";
    } else if (emulator == "Taito Type X Games") {
        emulatorPath = "C:/TaitoTypeX/typex_loader.exe";
        romPath = "C:/TaitoTypeX/roms";
    } else if (emulator == "Windows Games") {
        emulatorPath = "Choose path to executable";
        romPath = "Choose path to ROMs";
    } else if (emulator == "Adrenaline Amusements") {
        emulatorPath = "C:/Adrenaline/Adrenaline.exe";
        romPath = "C:/Adrenaline/roms";
    } else if (emulator == "SEGA Amusement Linkage Live System") {
        emulatorPath = "C:/SEGA/AmusementLinkage.exe";
        romPath = "C:/SEGA/roms/";
    } else if (emulator == "Namco ES3 System") {
        emulatorPath = "C:/NamcoES3/es3.exe";
        romPath = "C:/NamcoES3/roms";
    } else if (emulator == "Flycast v2.0") {
        emulatorPath = "C:/Flycast/flycast.exe";
        romPath = "C:/Flycast/roms";
    } else if (emulator == "RPCS3 System 357") {
        emulatorPath = "C:/RPCS3/rpcs3.exe";
        romPath = "C:/RPCS3/roms";
    } else if (emulator == "Sega Nu") {
        emulatorPath = "C:/SegaNu/seganu.exe";
        romPath = "C:/SegaNu/roms";
    } else if (emulator == "UNIS Technology") {
        emulatorPath = "C:/UNIS/UNIS.exe";
        romPath = "C:/UNIS/roms";
    } else if (emulator == "Windows Games (64)") {
        emulatorPath = "Choose path to executable";
        romPath = "Choose path to ROMs";
    }
    #else
    // macOS/Linux paths
    QString homeDir = QDir::homePath();
    QString appsDir = homeDir + "/Applications";
    QString gamesDir = homeDir + "/Games";
    
    if (emulator == "Coastal") {
        emulatorPath = appsDir + "/Coastal/Coastal";
        romPath = gamesDir + "/Coastal/roms";
    } else if (emulator == "Cxbx-Reloaded") {
        emulatorPath = appsDir + "/Cxbx-Reloaded/chihiro";
        romPath = gamesDir + "/Cxbx-Reloaded/roms";
    } else if (emulator == "Demul 0.57 / 0.582 / 0.7a" || emulator == "Demul 0.7a Only" || emulator == "Demul 0.582 and Above") {
        emulatorPath = appsDir + "/Demul/demul";
        romPath = gamesDir + "/Demul/roms";
    } else if (emulator == "Dolphin x64 v5.0") {
        emulatorPath = appsDir + "/Dolphin/Dolphin";
        romPath = gamesDir + "/Dolphin/roms";
    } else if (emulator == "Namco ES4 Games") {
        emulatorPath = appsDir + "/NamcoES4/es4";
        romPath = gamesDir + "/NamcoES4/roms";
    } else if (emulator == "GameWax Games") {
        emulatorPath = appsDir + "/GameWax/GameWax";
        romPath = gamesDir + "/GameWax/roms";
    } else if (emulator == "Global VR Games") {
        emulatorPath = appsDir + "/GlobalVR/GlobalVR";
        romPath = gamesDir + "/GlobalVR/roms";
    } else if (emulator == "KONAMI Arcade") {
        emulatorPath = appsDir + "/KONAMI/Arcade";
        romPath = gamesDir + "/KONAMI/roms";
    } else if (emulator == "TeknoParrot Loader (Lindbergh)" ||
               emulator == "TeknoParrot Loader (Raw Thrill)" ||
               emulator == "TeknoParrot Loader (RingWide)") {
        emulatorPath = appsDir + "/TeknoParrot/TeknoParrotUi";
        romPath = gamesDir + "/TeknoParrot/roms";
    } else if (emulator == "Model2 Emulator v1.1a") {
        emulatorPath = appsDir + "/Model2/emulator_multicpu";
        romPath = gamesDir + "/Model2/roms";
    } else if (emulator == "P&P Marketing Arcade") {
        emulatorPath = appsDir + "/PandP/Arcade";
        romPath = gamesDir + "/PandP/roms";
    } else if (emulator == "RingEdge 2 Games") {
        emulatorPath = appsDir + "/RingEdge2/ringedge2";
        romPath = gamesDir + "/RingEdge2/roms";
    } else if (emulator == "Taito Type X Games") {
        emulatorPath = appsDir + "/TaitoTypeX/typex_loader";
        romPath = gamesDir + "/TaitoTypeX/roms";
    } else if (emulator == "Windows Games") {
        emulatorPath = "Choose path to executable";
        romPath = "Choose path to ROMs";
    } else if (emulator == "Adrenaline Amusements") {
        emulatorPath = appsDir + "/Adrenaline/Adrenaline";
        romPath = gamesDir + "/Adrenaline/roms";
    } else if (emulator == "SEGA Amusement Linkage Live System") {
        emulatorPath = appsDir + "/SEGA/AmusementLinkage";
        romPath = gamesDir + "/SEGA/roms";
    } else if (emulator == "Namco ES3 System") {
        emulatorPath = appsDir + "/NamcoES3/es3";
        romPath = gamesDir + "/NamcoES3/roms";
    } else if (emulator == "Flycast v2.0") {
        emulatorPath = appsDir + "/Flycast/flycast";
        romPath = gamesDir + "/Flycast/roms";
    } else if (emulator == "RPCS3 System 357") {
        emulatorPath = appsDir + "/RPCS3/rpcs3";
        romPath = gamesDir + "/RPCS3/roms";
    } else if (emulator == "Sega Nu") {
        emulatorPath = appsDir + "/SegaNu/seganu";
        romPath = gamesDir + "/SegaNu/roms";
    } else if (emulator == "UNIS Technology") {
        emulatorPath = appsDir + "/UNIS/UNIS";
        romPath = gamesDir + "/UNIS/roms";
    } else if (emulator == "Windows Games (64)") {
        emulatorPath = "Choose path to executable";
        romPath = "Choose path to ROMs";
    }
    #endif
}

void EmulatorUtils::setupEmulatorComboBox(QComboBox *box)
{
    box->addItems({
        "----DemulShooter----",
        "Coastal", "Cxbx-Reloaded", "Demul 0.57 / 0.582 / 0.7a",
        "Demul 0.582 and Above", "Demul 0.7a Only", "Dolphin x64 v5.0",
        "GameWax Games", "Global VR Games", "KONAMI Arcade",
        "Model2 Emulator v1.1a", "Namco ES4 Games", "P&P Marketing Arcade",
        "RingEdge 2 Games", "SEGA Arcade (Plants vs Zombies)",
        "Taito Type X Games", "TeknoParrot Loader (Lindbergh)",
        "TeknoParrot Loader (Raw Thrill)", "TeknoParrot Loader (RingWide)",
        "Windows Games",
        "----DemulShooter64----",
        "Adrenaline Amusements", "Flycast v2.0", "Namco ES3 System",
        "Raw Thrill Arcade (64-bit)", "RPCS3 System 357",
        "SEGA Amusement Linkage Live System", "Sega Nu",
        "UNIS Technology", "United Distribution Company",
        "Windows Games (64)"
    });

    box->setMaxVisibleItems(60);

    QFont bold; bold.setBold(true);
    for (int i = 0; i < box->count(); ++i) {
        const QString txt = box->itemText(i);
        if (txt.startsWith("----")) {
            box->setItemData(i, bold, Qt::FontRole);
            box->setItemData(i, QVariant(0), Qt::UserRole - 1);   // disable selection
        }
    }
}