#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setting default values for dropdowns and editable fields
    ui->emulatorComboBox->addItems({"----DemulShooter----", "Coastal", "Cxbx-Reloaded", "Demul 0.57 / 0.582 / 0.7a", "Demul 0.7a Only", "Demul 0.582 and Above", "Dolphin x64 v5.0", "Namco ES4 Games", "GameWax Games", "Global VR Games", "KONAMI Arcade", "TeknoParrot Loader (Lindbergh)", "Model2 Emulator v1.1a", "P&P Marketing Arcade", "TeknoParrot Loader (Raw Thrill)", "RingEdge 2 Games", "TeknoParrot Loader (RingWide)", "Taito Type X Games", "Windows Games", "----DemulShooter64----", "Adrenaline Amusements", "SEGA Amusement Linkage Live System", "Namco ES3 System", "Flycast v2.0", "RPCS3 System 357", "Sega Nu", "UNIS Technology", "Windows Games (64)"});
    ui->emulatorComboBox->setMaxVisibleItems(50);

    QFont boldFont;
    boldFont.setBold(true);

    for (int i = 0; i < ui->emulatorComboBox->count(); ++i) {
        QString itemText = ui->emulatorComboBox->itemText(i);
        if (itemText == "----DemulShooter----" || itemText == "----DemulShooter64----") {
            // Set the item to bold
            ui->emulatorComboBox->setItemData(i, boldFont, Qt::FontRole);
            // Optionally disable the item
            ui->emulatorComboBox->setItemData(i, QVariant(0), Qt::UserRole - 1);
        }
    }


    // Connecting emulator combo box change to update games list and path function
    connect(ui->emulatorComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateGamesList);
    connect(ui->emulatorComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateEmulatorPath);

    ui->verboseComboBox->addItems({"Yes", "No"});

    // Disable the ROM combo box until an emulator is selected
    //ui->romComboBox->setEnabled(false);

    // Connecting the export button to the export function
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::exportFiles);
    connect(ui->exportAllButton, &QPushButton::clicked, this, &MainWindow::exportAllFiles);

    // Setting default paths
    ui->emulatorPathLineEdit->setText("C:/Demul/");
    connect(ui->browseEmulatorButton, &QPushButton::clicked, this, &MainWindow::browseEmulatorPath);

    ui->romPathLineEdit->setText("C:/roms");
    connect(ui->browseRomButton, &QPushButton::clicked, this, &MainWindow::browseRomPath);

    ui->qmamehookerPathLineEdit->setText("C:/QMamehooker");
    connect(ui->browseQmamehookerButton, &QPushButton::clicked, this, &MainWindow::browseQmamehookerPath);

    ui->demulShooterPathLineEdit->setText("C:/DemulShooter/");
    connect(ui->browseDemulButton, &QPushButton::clicked, this, &MainWindow::browseDemulPath);

    ui->iniPathLineEdit->setText("C:/QMamehooker/ini");
    connect(ui->browseIniButton, &QPushButton::clicked, this, &MainWindow::browseIniPath);


    // Connecting emulator combo box change to update games list function
    connect(ui->emulatorComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateGamesList);

    // Load default INI settings into plain text edit box
    QString defaultIni = "[General]\n"
                         "MameStart=\"cmo 1 baud=9600_parity=N_data=8_stop=1\", \"cmo 2 baud=9600_parity=N_data=8_stop=1\", cmw 1 S6M1x1x, cmw 2 S6M1x1x\n"
                         "MameStop=cmw 1 E, cmw 2 E, cmc 1, cmc 2\n"
                         "StateChange=\n"
                         "OnRotate=\n"
                         "OnPause=\n"
                         "\n[KeyStates]\n"
                         "RefreshTime=\n"
                         "\n[Output]\n"
                         "P1_LmpStart=\n"
                         "P2_LmpStart=\n"
                         "P1_Ammo=\n"
                         "P2_Ammo=\n"
                         "P1_Clip=\n"
                         "P2_Clip=\n"
                         "P1_CtmRecoil=cmw 1 F0x%s%\n"
                         "P2_CtmRecoil=cmw 2 F0x%s%\n"
                         "P1_Life=\n"
                         "P2_Life=\n"
                         "P1_Damaged=cmw 1 F1x%s%\n"
                         "P2_Damaged=cmw 2 F1x%s%\n"
                         "Credits=\n";
    ui->plainTextEdit->setPlainText(defaultIni);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::exportFiles()
{
    QString emulator = ui->emulatorComboBox->currentText();
    QString emulatorexe;
    QString emulatorPath = ui->emulatorPathLineEdit->text();
    QString rom = ui->romComboBox->currentText();
    QString romPath = ui->romPathLineEdit->text();
    QString qmamehookerPath = ui->qmamehookerPathLineEdit->text();
    QString demulShooterPath = ui->demulShooterPathLineEdit->text();
    QString demulShooterExe = ui->demulShooterPathLineEdit->text();
    QString iniPath = ui->iniPathLineEdit->text();
    QString verbose = ui->verboseComboBox->currentText() == "Yes" ? "-v" : "";


    // Replace emulator name with required string for output
    if (emulator == "Coastal") {
        emulator = "coastal";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Cxbx-Reloaded") {
        emulator = "chihiro";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Demul 0.57 / 0.582 / 0.7a") {
        emulator = "demul057 / demul058 / demul07a";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Demul 0.7a Only") {
        emulator = "demul07a";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Demul 0.582 and Above") {
        emulator = "demul058";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Dolphin x64 v5.0") {
        emulator = "dolphin5";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Namco ES4 Games") {
        emulator = "es4";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "GameWax Games") {
        emulator = "gamewax";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Global VR Games") {
        emulator = "globalvr";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "KONAMI Arcade") {
        emulator = "konami";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "TeknoParrot Loader (Lindbergh)") {
        emulator = "lindbergh";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Model2 Emulator v1.1a") {
        emulator = "model2";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "P&P Marketing Arcade") {
        emulator = "ppmarket";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "TeknoParrot Loader (Raw Thrill)") {
        emulator = "rawthrill";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "RingEdge 2 Games") {
        emulator = "ringedge2";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "TeknoParrot Loader (RingWide)") {
        emulator = "ringwide";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Taito Type X Games") {
        emulator = "ttx";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Windows Games") {
        emulator = "windows";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Adrenaline Amusements") {
        emulator = "aagames";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "SEGA Amusement Linkage Live System") {
        emulator = "alls";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "Namco ES3 System") {
        emulator = "es3";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "Flycast v2.0" || emulator == "Flycast v2.0 (Continued)") {
        emulator = "flycast";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "RPCS3 System 357") {
        emulator = "rpcs3";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "Sega Nu") {
        emulator = "seganu";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "UNIS Technology") {
        emulator = "unis";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "Windows Games (64)") {
        emulator = "windows";
    }


        // Replace ROM name with required string for output
        if (rom == "Wild West Shootout") {
            rom = "wws";
        } else if (rom == "Virtua Cop 3") {
            rom = "vcop3";
        } else if (rom == "Confidential Mission") {
            rom = "confmiss";
        } else if (rom == "Death Crimson OX (USA)") {
            rom = "deathcox";
        } else if (rom == "Death Crimson OX (JAP)") {
            rom = "deathcoxo";
        } else if (rom == "House of The Dead II (US)") {
            rom = "hotd2";
        } else if (rom == "House of The Dead II") {
            rom = "hotd2o";
        } else if (rom == "House of The Dead II (Prototype)") {
            rom = "hotd2p";
        } else if (rom == "Lupin the Third (the shooting)") {
            rom = "lupinsho";
        } else if (rom == "The Maze of the Kings") {
            rom = "mok";
        } else if (rom == "Brave Fire Fighters") {
            rom = "braveff";
        } else if (rom == "Sega Clay Challenge") {
            rom = "claychal";
        } else if (rom == "Manic Panic Ghosts") {
            rom = "manicpnc";
        } else if (rom == "Pokasuka Ghosts") {
            rom = "pokasuka";
        } else if (rom == "Ranger Mission") {
            rom = "rangrmsn";
        } else if (rom == "Extreme Hunting") {
            rom = "xtrmhunt";
        } else if (rom == "Extreme Hunting 2") {
            rom = "xtrmhnt2";
        } else if (rom == "Ninja Assault (World)") {
            rom = "ninjaslt";
        } else if (rom == "Ninja Assault (Asia)") {
            rom = "ninjaslta";
        } else if (rom == "Ninja Assault (Japan)") {
            rom = "ninjasltj";
        } else if (rom == "Ninja Assault (US)") {
            rom = "ninjasltu";
        } else if (rom == "Point Blank X") {
            rom = "pblankx";
        } else if (rom == "Akuma Mortis Immortal") {
            rom = "akuma";
        } else if (rom == "Aliens Extermination") {
            rom = "aliens";
        } else if (rom == "Far Cry: Paradise Lost") {
            rom = "farcry";
        } else if (rom == "Fright Fear Land") {
            rom = "fearland";
        } else if (rom == "Castlevania: The Arcade") {
            rom = "hcv";
        } else if (rom == "Lethal Enforcers 3") {
            rom = "le3";
        } else if (rom == "Wartran Troopers") {
            rom = "wartran";
        } else if (rom == "Too Spicy") {
            rom = "2spicy";
        } else if (rom == "Ghost Squad Evolution") {
            rom = "gsquad";
        } else if (rom == "House of the Dead 4") {
            rom = "hotd4";
        } else if (rom == "House of the Dead 4: Special") {
            rom = "hotd4sp";
        } else if (rom == "House of the Dead: EX") {
            rom = "hotdex";
        } else if (rom == "Let's Go Jungle") {
            rom = "lgj";
        } else if (rom == "Let's Go Jungle Special") {
            rom = "lgjsp";
        } else if (rom == "Rambo") {
            rom = "rambo";
        } else if (rom == "Behind Enemy Lines") {
            rom = "bel";
        } else if (rom == "Gunblade NY") {
            rom = "gunblade";
        } else if (rom == "House of the Dead") {
            rom = "hotd";
        } else if (rom == "Rail Chase 2") {
            rom = "rchase2";
        } else if (rom == "Virtua Cop") {
            rom = "vcop";
        } else if (rom == "Virtua Cop 2") {
            rom = "vcop2";
        } else if (rom == "Police Trainer 2") {
            rom = "policetr2";
        } else if (rom == "Aliens Armageddon") {
            rom = "aa";
        } else if (rom == "Jurassic Park") {
            rom = "jp";
        } else if (rom == "Target: Terror - Gold") {
            rom = "ttg";
        } else if (rom == "Terminator Salvation") {
            rom = "ts";
        } else if (rom == "Walking Dead") {
            rom = "wd";
        } else if (rom == "Transformers: Shadow Rising") {
            rom = "tsr";
        } else if (rom == "Let's Go Island") {
            rom = "lgi";
        } else if (rom == "Let's Go Island 3D") {
            rom = "lgi3D";
        } else if (rom == "Operation G.H.O.S.T.") {
            rom = "og";
        } else if (rom == "Sega Dream Riders") {
            rom = "sdr";
        } else if (rom == "Sega Golden Gun") {
            rom = "sgg";
        } else if (rom == "Transformers: Human Alliance") {
            rom = "tha";
        } else if (rom == "Block King Ball Shooter") {
            rom = "bkbs";
        } else if (rom == "Elevator Action Death Parade") {
            rom = "eapd";
        } else if (rom == "Silent Hill: The Arcade") {
            rom = "sha";
        } else if (rom == "Gaia Attack 4") {
            rom = "gattack4";
        } else if (rom == "Gundam: Spirit of Zeon") {
            rom = "gsoz";
        } else if (rom == "Gundam: Spirit of Zeon (DualScreen)") {
            rom = "gsoz2p";
        } else if (rom == "Haunted Museum") {
            rom = "hmuseum";
        } else if (rom == "Haunted Museum 2") {
            rom = "hmuseum2";
        } else if (rom == "Alien Disco Safari") {
            rom = "ads";
        } else if (rom == "Art Is Dead") {
            rom = "artdead";
        } else if (rom == "Bug Busters") {
            rom = "bugbust";
        } else if (rom == "Friction") {
            rom = "friction";
        } else if (rom == "Heavy Fire Afghanistan") {
            rom = "hfa";
        } else if (rom == "Heavy Fire Afghanistan (Dual Player)") {
            rom = "hfa2p";
        } else if (rom == "Heavy Fire Shattered Spear") {
            rom = "hfss";
        } else if (rom == "Heavy Fire Shattered Spear (Dual Player)") {
            rom = "hfss2p";
        } else if (rom == "House of The Dead III") {
            rom = "hod3pc";
        } else if (rom == "House of The Dead: Overkill") {
            rom = "hodo";
        } else if (rom == "Reload") {
            rom = "reload";
        } else if (rom == "Drakon: Realm Keepers") {
            rom = "drk";
        } else if (rom == "Rabbids Hollywood Arcade") {
            rom = "rha";
        } else if (rom == "Tomb Raider Arcade") {
           rom = "tra";
        } else if (rom == "House of the Dead: Scarlet Dawn") {
            rom = "hodsd";
        } else if (rom == "Time Crisis 5") {
           rom = "tc5";
        } else if (rom == "Dark Escape 4D") {
             rom = "de4d";
        } else if (rom == "Deadstorm Pirates: Special Edition") {
            rom = "deadstorm";
        } else if (rom == "Sailor Zombies") {
            rom = "sailorz";
        } else if (rom == "Luigi Mansion Arcade") {
           rom = "lma";
        } else if (rom == "Elevator Action Invasion") {
           rom = "eai";
        } else if (rom == "Night Hunter Arcade") {
          rom = "nha";
        } else if (rom == "Raccoon Rampage") {
           rom = "racramp";
        } else if (rom == "DCOP") {
           rom = "dcop";
        } else if (rom == "Operation Wolf Returns") {
           rom = "opwolfr";
        } else if (rom == "House of the Dead: Remake (Arcade Plugin)") {
           rom = "hotdra";
        }



    // Create .bat file
    QFile batFile(rom + ".bat");
    if (batFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&batFile);
        out << "start \"Demul\" \"" << demulShooterPath << "DemulShooter.exe\" -target=" << emulator << " -rom=" << rom << "\n";
        out << "start /MIN \"Hooker\" \"" << qmamehookerPath << "QMamehook.exe\" -p \"" << iniPath << "\" " << verbose << "\n";
        out << "cd \"" << emulatorPath << "\"\n";
        out << "start \"" << emulator << "\" " << emulatorexe << " -" << rom << ".zip\n";
        batFile.close();
    }

    // Create .ini file
    QFile iniFile(rom + ".ini");
    if (iniFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&iniFile);
        QString iniContent = ui->plainTextEdit->toPlainText();
        out << iniContent;
        iniFile.close();
    }

    QMessageBox::information(this, "Export", "Batch and INI files have been successfully exported.");
}


void MainWindow::exportAllFiles()
{
    QString emulator = ui->emulatorComboBox->currentText();
    QString emulatorPath = ui->emulatorPathLineEdit->text();
    QString romPath = ui->romPathLineEdit->text();
    QString qmamehookerPath = ui->qmamehookerPathLineEdit->text();
    QString demulShooterPath = ui->demulShooterPathLineEdit->text();
    QString iniPath = ui->iniPathLineEdit->text();
    QString verbose = ui->verboseComboBox->currentText() == "Yes" ? "-v" : "";
    QString emulatorExe = "Demul.exe";
    QString demulShooterExe = "DemulShooter.exe";

    // Replace emulator name with required string for output
    if (emulator == "Coastal") {
        emulator = "coastal";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Cxbx-Reloaded") {
        emulator = "chihiro";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Demul 0.57 / 0.582 / 0.7a") {
        emulator = "demul057 / demul058 / demul07a";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Demul 0.7a Only") {
        emulator = "demul07a";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Demul 0.582 and Above") {
        emulator = "demul058";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Dolphin x64 v5.0") {
        emulator = "dolphin5";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Namco ES4 Games") {
        emulator = "es4";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "GameWax Games") {
        emulator = "gamewax";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Global VR Games") {
        emulator = "globalvr";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "KONAMI Arcade") {
        emulator = "konami";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "TeknoParrot Loader (Lindbergh)") {
        emulator = "lindbergh";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Model2 Emulator v1.1a") {
        emulator = "model2";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "P&P Marketing Arcade") {
        emulator = "ppmarket";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "TeknoParrot Loader (Raw Thrill)") {
        emulator = "rawthrill";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "RingEdge 2 Games") {
        emulator = "ringedge2";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "TeknoParrot Loader (RingWide)") {
        emulator = "ringwide";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Taito Type X Games") {
        emulator = "ttx";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Windows Games") {
        emulator = "windows";
        demulShooterExe = "DemulShooter.exe";
    } else if (emulator == "Adrenaline Amusements") {
        emulator = "aagames";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "SEGA Amusement Linkage Live System") {
        emulator = "alls";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "Namco ES3 System") {
        emulator = "es3";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "Flycast v2.0" || emulator == "Flycast v2.0 (Continued)") {
        emulator = "flycast";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "RPCS3 System 357") {
        emulator = "rpcs3";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "Sega Nu") {
        emulator = "seganu";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "UNIS Technology") {
        emulator = "unis";
        demulShooterExe = "DemulShooter64.exe";
    } else if (emulator == "Windows Games (64)") {
        emulator = "windows";
    }

    // Iterate through all ROMs in the ROM combo box
    for (int i = 0; i < ui->romComboBox->count(); ++i) {
        QString rom = ui->romComboBox->itemText(i);

        // Replace ROM name with required string for output
        if (rom == "Wild West Shootout") {
            rom = "wws";
        } else if (rom == "Virtua Cop 3") {
            rom = "vcop3";
        } else if (rom == "Confidential Mission") {
            rom = "confmiss";
        } else if (rom == "Death Crimson OX (USA)") {
            rom = "deathcox";
        } else if (rom == "Death Crimson OX (JAP)") {
            rom = "deathcoxo";
        } else if (rom == "House of The Dead II (US)") {
            rom = "hotd2";
        } else if (rom == "House of The Dead II") {
            rom = "hotd2o";
        } else if (rom == "House of The Dead II (Prototype)") {
            rom = "hotd2p";
        } else if (rom == "Lupin the Third (the shooting)") {
            rom = "lupinsho";
        } else if (rom == "The Maze of the Kings") {
            rom = "mok";
        } else if (rom == "Brave Fire Fighters") {
            rom = "braveff";
        } else if (rom == "Sega Clay Challenge") {
            rom = "claychal";
        } else if (rom == "Manic Panic Ghosts") {
            rom = "manicpnc";
        } else if (rom == "Pokasuka Ghosts") {
            rom = "pokasuka";
        } else if (rom == "Ranger Mission") {
            rom = "rangrmsn";
        } else if (rom == "Extreme Hunting") {
            rom = "xtrmhunt";
        } else if (rom == "Extreme Hunting 2") {
            rom = "xtrmhnt2";
        } else if (rom == "Ninja Assault (World)") {
            rom = "ninjaslt";
        } else if (rom == "Ninja Assault (Asia)") {
            rom = "ninjaslta";
        } else if (rom == "Ninja Assault (Japan)") {
            rom = "ninjasltj";
        } else if (rom == "Ninja Assault (US)") {
            rom = "ninjasltu";
        } else if (rom == "Point Blank X") {
            rom = "pblankx";
        } else if (rom == "Akuma Mortis Immortal") {
            rom = "akuma";
        } else if (rom == "Aliens Extermination") {
            rom = "aliens";
        } else if (rom == "Far Cry: Paradise Lost") {
            rom = "farcry";
        } else if (rom == "Fright Fear Land") {
            rom = "fearland";
        } else if (rom == "Castlevania: The Arcade") {
            rom = "hcv";
        } else if (rom == "Lethal Enforcers 3") {
            rom = "le3";
        } else if (rom == "Wartran Troopers") {
            rom = "wartran";
        } else if (rom == "Too Spicy") {
            rom = "2spicy";
        } else if (rom == "Ghost Squad Evolution") {
            rom = "gsquad";
        } else if (rom == "House of the Dead 4") {
            rom = "hotd4";
        } else if (rom == "House of the Dead 4: Special") {
            rom = "hotd4sp";
        } else if (rom == "House of the Dead: EX") {
            rom = "hotdex";
        } else if (rom == "Let's Go Jungle") {
            rom = "lgj";
        } else if (rom == "Let's Go Jungle Special") {
            rom = "lgjsp";
        } else if (rom == "Rambo") {
            rom = "rambo";
        } else if (rom == "Behind Enemy Lines") {
            rom = "bel";
        } else if (rom == "Gunblade NY") {
            rom = "gunblade";
        } else if (rom == "House of the Dead") {
            rom = "hotd";
        } else if (rom == "Rail Chase 2") {
            rom = "rchase2";
        } else if (rom == "Virtua Cop") {
            rom = "vcop";
        } else if (rom == "Virtua Cop 2") {
            rom = "vcop2";
        } else if (rom == "Police Trainer 2") {
            rom = "policetr2";
        } else if (rom == "Aliens Armageddon") {
            rom = "aa";
        } else if (rom == "Jurassic Park") {
            rom = "jp";
        } else if (rom == "Target: Terror - Gold") {
            rom = "ttg";
        } else if (rom == "Terminator Salvation") {
            rom = "ts";
        } else if (rom == "Walking Dead") {
            rom = "wd";
        } else if (rom == "Transformers: Shadow Rising") {
            rom = "tsr";
        } else if (rom == "Let's Go Island") {
            rom = "lgi";
        } else if (rom == "Let's Go Island 3D") {
            rom = "lgi3D";
        } else if (rom == "Operation G.H.O.S.T.") {
            rom = "og";
        } else if (rom == "Sega Dream Riders") {
            rom = "sdr";
        } else if (rom == "Sega Golden Gun") {
            rom = "sgg";
        } else if (rom == "Transformers: Human Alliance") {
            rom = "tha";
        } else if (rom == "Block King Ball Shooter") {
            rom = "bkbs";
        } else if (rom == "Elevator Action Death Parade") {
            rom = "eapd";
        } else if (rom == "Silent Hill: The Arcade") {
            rom = "sha";
        } else if (rom == "Gaia Attack 4") {
            rom = "gattack4";
        } else if (rom == "Gundam: Spirit of Zeon") {
            rom = "gsoz";
        } else if (rom == "Gundam: Spirit of Zeon (DualScreen)") {
            rom = "gsoz2p";
        } else if (rom == "Haunted Museum") {
            rom = "hmuseum";
        } else if (rom == "Haunted Museum 2") {
            rom = "hmuseum2";
        } else if (rom == "Alien Disco Safari") {
            rom = "ads";
        } else if (rom == "Art Is Dead") {
            rom = "artdead";
        } else if (rom == "Bug Busters") {
            rom = "bugbust";
        } else if (rom == "Friction") {
            rom = "friction";
        } else if (rom == "Heavy Fire Afghanistan") {
            rom = "hfa";
        } else if (rom == "Heavy Fire Afghanistan (Dual Player)") {
            rom = "hfa2p";
        } else if (rom == "Heavy Fire Shattered Spear") {
            rom = "hfss";
        } else if (rom == "Heavy Fire Shattered Spear (Dual Player)") {
            rom = "hfss2p";
        } else if (rom == "House of The Dead III") {
            rom = "hod3pc";
        } else if (rom == "House of The Dead: Overkill") {
            rom = "hodo";
        } else if (rom == "Reload") {
            rom = "reload";
        } else if (rom == "Drakon: Realm Keepers") {
            rom = "drk";
        } else if (rom == "Rabbids Hollywood Arcade") {
            rom = "rha";
        } else if (rom == "Tomb Raider Arcade") {
            rom = "tra";
        } else if (rom == "House of the Dead: Scarlet Dawn") {
            rom = "hodsd";
        } else if (rom == "Time Crisis 5") {
            rom = "tc5";
        } else if (rom == "Dark Escape 4D") {
            rom = "de4d";
        } else if (rom == "Deadstorm Pirates: Special Edition") {
            rom = "deadstorm";
        } else if (rom == "Sailor Zombies") {
            rom = "sailorz";
        } else if (rom == "Luigi Mansion Arcade") {
            rom = "lma";
        } else if (rom == "Elevator Action Invasion") {
            rom = "eai";
        } else if (rom == "Night Hunter Arcade") {
            rom = "nha";
        } else if (rom == "Raccoon Rampage") {
            rom = "racramp";
        } else if (rom == "DCOP") {
            rom = "dcop";
        } else if (rom == "Operation Wolf Returns") {
            rom = "opwolfr";
        } else if (rom == "House of the Dead: Remake (Arcade Plugin)") {
            rom = "hotdra";
        }

        // Create .bat and .ini files for each ROM
        QFile batFile(rom + ".bat");
        if (batFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&batFile);
            out << "start \"" << emulator << "\" \"" << emulatorExe << "\" -rom \"" << rom << "\"\n";
            out << "start /MIN \"Hooker\" \"" << qmamehookerPath << "/QMamehook.exe\" -p \"" << iniPath << "\" " << verbose << "\n";
            batFile.close();
        }

        QFile iniFile(iniPath + "/" + rom + ".ini");
        if (iniFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&iniFile);
            out << ui->plainTextEdit->toPlainText();
            iniFile.close();
        }
    }
}



void MainWindow::updateGamesList()
{
    QString emulator = ui->emulatorComboBox->currentText();
    ui->romComboBox->clear();
    ui->romComboBox->setEnabled(true);

    if (emulator == "Coastal") {
        ui->romComboBox->addItems({"Wild West Shootout"});
    } else if (emulator == "Cxbx-Reloaded") {
        ui->romComboBox->addItems({"Virtua Cop 3"});
    } else if (emulator == "Demul 0.57 / 0.582 / 0.7a") {
        ui->romComboBox->addItems({"Confidential Mission", "Death Crimson OX (USA)", "Death Crimson OX (JAP)", "House of The Dead II (US)", "House of The Dead II", "House of The Dead II (Prototype)", "Lupin the Third (the shooting)", "The Maze of the Kings"});
    } else if (emulator == "Demul 0.7a Only") {
        ui->romComboBox->addItems({"Brave Fire Fighters", "Sega Clay Challenge", "Manic Panic Ghosts", "Pokasuka Ghosts", "Ranger Mission", "Extreme Hunting", "Extreme Hunting 2"});
    } else if (emulator == "Demul 0.582 and Above") {
        ui->romComboBox->addItems({"Ninja Assault (World)", "Ninja Assault (Asia)", "Ninja Assault (Japan)", "Ninja Assault (US)"});
    } else if (emulator == "Dolphin x64 v5.0") {
        ui->romComboBox->addItems({"Parameter not used"});
    } else if (emulator == "Namco ES4 Games") {
        ui->romComboBox->addItems({"Point Blank X"});
    } else if (emulator == "GameWax Games") {
        ui->romComboBox->addItems({"Akuma Mortis Immortal"});
    } else if (emulator == "Global VR Games") {
        ui->romComboBox->addItems({"Aliens Extermination", "Far Cry: Paradise Lost", "Fright Fear Land"});
    } else if (emulator == "KONAMI Arcade") {
        ui->romComboBox->addItems({"Castlevania: The Arcade", "Lethal Enforcers 3", "Wartran Troopers"});
    } else if (emulator == "TeknoParrot Loader (Lindbergh)") {
        ui->romComboBox->addItems({"Too Spicy", "Ghost Squad Evolution", "House of the Dead 4", "House of the Dead 4: Special", "House of the Dead: EX", "Let's Go Jungle", "Let's Go Jungle Special", "Rambo"});
    } else if (emulator == "Model2 Emulator v1.1a") {
        ui->romComboBox->addItems({"Behind Enemy Lines", "Gunblade NY", "House of the Dead", "Rail Chase 2", "Virtua Cop", "Virtua Cop 2"});
    } else if (emulator == "P&P Marketing Arcade") {
        ui->romComboBox->addItems({"Police Trainer 2"});
    } else if (emulator == "TeknoParrot Loader (Raw Thrill)") {
        ui->romComboBox->addItems({"Aliens Armageddon", "Jurassic Park", "Target: Terror - Gold", "Terminator Salvation", "Walking Dead"});
    } else if (emulator == "RingEdge 2 Games") {
        ui->romComboBox->addItems({"Transformers: Shadow Rising"});
    } else if (emulator == "TeknoParrot Loader (RingWide)") {
        ui->romComboBox->addItems({"Let's Go Island", "Let's Go Island 3D", "Operation G.H.O.S.T.", "Sega Dream Riders", "Sega Golden Gun", "Transformers: Human Alliance"});
    } else if (emulator == "Taito Type X Games") {
        ui->romComboBox->addItems({"Block King Ball Shooter", "Elevator Action Death Parade", "Silent Hill: The Arcade", "Gaia Attack 4", "Gundam: Spirit of Zeon", "Gundam: Spirit of Zeon (DualScreen)", "Haunted Museum", "Haunted Museum 2"});
    } else if (emulator == "Windows Games") {
        ui->romComboBox->addItems({"Alien Disco Safari", "Art Is Dead", "Bug Busters", "Friction", "Heavy Fire Afghanistan", "Heavy Fire Afghanistan (Dual Player)", "Heavy Fire Shattered Spear", "Heavy Fire Shattered Spear (Dual Player)"});
    } else if (emulator == "Adrenaline Amusements") {
        ui->romComboBox->addItems({"Drakon: Realm Keepers", "Rabbids Hollywood Arcade", "Tomb Raider Arcade"});
    } else if (emulator == "SEGA Amusement Linkage Live System") {
        ui->romComboBox->addItems({"House of the Dead: Scarlet Dawn"});
    } else if (emulator == "Namco ES3 System") {
        ui->romComboBox->addItems({"Time Crisis 5"});
    } else if (emulator == "Flycast v2.0" || emulator == "Flycast v2.0 (Continued)") {
        ui->romComboBox->addItems({"Confidential Mission", "Death Crimson OX", "House of The Dead II (US)", "House of The Dead II", "House of The Dead II (Prototype)", "Lupin the Third (the shooting)", "The Maze of the Kings", "Ninja Assault (World)", "Ninja Assault (Asia)", "Ninja Assault (Japan)", "Ninja Assault (US)"});
    } else if (emulator == "RPCS3 System 357") {
        ui->romComboBox->addItems({"Dark Escape 4D", "Deadstorm Pirates: Special Edition", "Sailor Zombies"});
    } else if (emulator == "Sega Nu") {
        ui->romComboBox->addItems({"Luigi Mansion Arcade"});
    } else if (emulator == "UNIS Technology") {
        ui->romComboBox->addItems({"Elevator Action Invasion", "Night Hunter Arcade", "Raccoon Rampage"});
    } else if (emulator == "Windows Games (64)") {
        ui->romComboBox->addItems({"DCOP", "Operation Wolf Returns", "House of the Dead : Remake (Arcade Plugin)"});
    }
}

void MainWindow::browseQmamehookerPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select QMamehooker Directory"), ui->qmamehookerPathLineEdit->text());
    if (!dir.isEmpty()) {
        ui->qmamehookerPathLineEdit->setText(dir);
    }
}

void MainWindow::browseEmulatorPath()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select Emulator Executable"), ui->emulatorPathLineEdit->text(), tr("Executables (*.exe);;All Files (*)"));
    if (!file.isEmpty()) {
        ui->emulatorPathLineEdit->setText(file);
    }
}

void MainWindow::browseRomPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select ROM Directory"), ui->romPathLineEdit->text());
    if (!dir.isEmpty()) {
        ui->romPathLineEdit->setText(dir);
    }
}

void MainWindow::browseDemulPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select XX Directory"), ui->demulShooterPathLineEdit->text());
    if (!dir.isEmpty()) {
        ui->demulShooterPathLineEdit->setText(dir);
    }
}

void MainWindow::browseIniPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select XX Directory"), ui->iniPathLineEdit->text());
    if (!dir.isEmpty()) {
        ui->iniPathLineEdit->setText(dir);
    }
}

void MainWindow::updateEmulatorPath()
{
    QString emulator = ui->emulatorComboBox->currentText();

    if (emulator == "Coastal") {
        ui->emulatorPathLineEdit->setText("C:/Coastal/Coastal.exe");
        ui->romPathLineEdit->setText("C:/Coastal/roms/");
    } else if (emulator == "Cxbx-Reloaded") {
        ui->emulatorPathLineEdit->setText("C:/Cxbx-Reloaded/chihiro.exe");
    } else if (emulator == "Demul 0.57 / 0.582 / 0.7a" || emulator == "Demul 0.7a Only" || emulator == "Demul 0.582 and Above") {
        ui->emulatorPathLineEdit->setText("C:/Demul/demul.exe");
    } else if (emulator == "Dolphin x64 v5.0") {
        ui->emulatorPathLineEdit->setText("C:/Dolphin/Dolphin.exe");
    } else if (emulator == "Namco ES4 Games") {
        ui->emulatorPathLineEdit->setText("C:/NamcoES4/es4.exe");
    } else if (emulator == "GameWax Games") {
        ui->emulatorPathLineEdit->setText("C:/GameWax/GameWax.exe");
    } else if (emulator == "Global VR Games") {
        ui->emulatorPathLineEdit->setText("C:/GlobalVR/GlobalVR.exe");
    } else if (emulator == "KONAMI Arcade") {
        ui->emulatorPathLineEdit->setText("C:/KONAMI/Arcade.exe");
    } else if (emulator == "TeknoParrot Loader (Lindbergh)" || emulator == "TeknoParrot Loader (Raw Thrill)" || emulator == "TeknoParrot Loader (RingWide)") {
        ui->emulatorPathLineEdit->setText("C:/TeknoParrot/TeknoParrotUi.exe");
    } else if (emulator == "Model2 Emulator v1.1a") {
        ui->emulatorPathLineEdit->setText("C:/Model2Emulator/emulator.exe");
    } else if (emulator == "P&P Marketing Arcade") {
        ui->emulatorPathLineEdit->setText("C:/PandP/Arcade.exe");
    } else if (emulator == "RingEdge 2 Games") {
        ui->emulatorPathLineEdit->setText("C:/RingEdge2/ringedge2.exe");
    } else if (emulator == "Taito Type X Games") {
        ui->emulatorPathLineEdit->setText("C:/TaitoTypeX/typex_loader.exe");
    } else if (emulator == "Windows Games") {
        ui->emulatorPathLineEdit->setText("Choose path to executable");
    } else if (emulator == "Adrenaline Amusements") {
        ui->emulatorPathLineEdit->setText("C:/Adrenaline/Adrenaline.exe");
    } else if (emulator == "SEGA Amusement Linkage Live System") {
        ui->emulatorPathLineEdit->setText("C:/SEGA/AmusementLinkage.exe");
    } else if (emulator == "Namco ES3 System") {
        ui->emulatorPathLineEdit->setText("C:/NamcoES3/es3.exe");
    } else if (emulator == "Flycast v2.0") {
        ui->emulatorPathLineEdit->setText("C:/Flycast/flycast.exe");
    } else if (emulator == "RPCS3 System 357") {
        ui->emulatorPathLineEdit->setText("C:/RPCS3/rpcs3.exe");
    } else if (emulator == "Sega Nu") {
        ui->emulatorPathLineEdit->setText("C:/SegaNu/seganu.exe");
    } else if (emulator == "UNIS Technology") {
        ui->emulatorPathLineEdit->setText("C:/UNIS/UNIS.exe");
    } else if (emulator == "Windows Games (64)") {
        ui->emulatorPathLineEdit->setText("Choose path to executable");
    }
}

