#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "IniSyntaxHighlighter.h" // Fixed case sensitivity
#include "emulatorutils.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QFont>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QPixmap>
#include <QIcon>
#include <QVariant>
#include <QProcess>
#include <QLineEdit>
#include <QtGlobal>

// Global color definitions.
QColor customRed(255, 0, 0);      // Red using RGB values
QColor customGreen(0, 255, 0);      // Green using RGB values
QColor customBlue(0, 0, 255);       // Blue using RGB values
QColor customYellow(255, 255, 0);   // Yellow using RGB values

// Define static regex patterns.
const QRegularExpression MainWindow::recoilRegex(
    QRegularExpression(R"(^P1_CtmRecoil[ \t]*=[ \t]*cmw[ \t]*1[ \t]*(.*?)[ \t]*(?=\r?\n|$))", QRegularExpression::MultilineOption)
);
const QRegularExpression MainWindow::damageRegex(
    QRegularExpression(R"(^P1_Damaged[ \t]*=[ \t]*cmw[ \t]*1[ \t]*(.*?)[ \t]*(?=\r?\n|$))", QRegularExpression::MultilineOption)
);
const QRegularExpression MainWindow::clipRegex(
    QRegularExpression(R"(^P1_Clip[ \t]*=[ \t]*cmw[ \t]*1[ \t]*(.*?)[ \t]*(?=\r?\n|$))", QRegularExpression::MultilineOption)
);
const QRegularExpression MainWindow::ammoRegex(
    QRegularExpression(R"(^P1_Ammo[ \t]*=[ \t]*(?:cmw[ \t]*1[ \t]*(.*?)[ \t]*(?=\r?\n|$)|[ \t]*(?=\r?\n|$)))", QRegularExpression::MultilineOption)
);
const QRegularExpression MainWindow::lifeRegex(
    QRegularExpression(R"(^P1_Life[ \t]*=[ \t]*(?:cmw[ \t]*1[ \t]*(.*?)[ \t]*(?=\r?\n|$)|[ \t]*(?=\r?\n|$)))", QRegularExpression::MultilineOption)
);
const QRegularExpression MainWindow::creditsRegex(
    QRegularExpression(R"(^Credits[ \t]*=[ \t]*(?:cmw[ \t]*1[ \t]*(.*?)[ \t]*(?=\r?\n|$)|[ \t]*(?=\r?\n|$)))", QRegularExpression::MultilineOption)
);

//
// MainWindow Constructor & Destructor
//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initializeUI();
    
    // Initialize the syntax highlighter for the plain text edit.
    new IniSyntaxHighlighter(ui->plainTextEdit_Generic->document());
    
    // Set platform-appropriate default paths
    #ifdef Q_OS_WIN
    ui->qmamehookerPathLineEdit->setText("C:/QMamehook");
    ui->demulShooterPathLineEdit->setText("C:/DemulShooter");
    #else
    ui->qmamehookerPathLineEdit->setText(QDir::homePath() + "/QMamehook");
    ui->demulShooterPathLineEdit->setText(QDir::homePath() + "/DemulShooter");
    #endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

///
/// Initialization helper – sets up UI elements, styles, and connects signals.
///
void MainWindow::initializeUI() {
    setupEmulatorComboBox();
    setupComboBoxStyles();
    setupOtherUIFields();
    setupColorComboBoxes();
    setupSignalConnections();
    setupDefaultIni();
    
    // Add context menu to the text editor
    ui->plainTextEdit_Generic->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plainTextEdit_Generic, &QPlainTextEdit::customContextMenuRequested,
            this, &MainWindow::showTextEditorContextMenu);
}

///
/// Sets up the emulator combo box with items and the max visible items.
///
void MainWindow::setupEmulatorComboBox() {
    EmulatorUtils::setupEmulatorComboBox(ui->emulatorComboBox);
}

///
/// Applies bold styling to divider items in the emulator combo box and optionally disables selection.
///
void MainWindow::setupComboBoxStyles() {
    QFont boldFont;
    boldFont.setBold(true);
    for (int i = 0; i < ui->emulatorComboBox->count(); ++i) {
        QString itemText = ui->emulatorComboBox->itemText(i);
        if (itemText.startsWith("----") && itemText.endsWith("----")) {
            ui->emulatorComboBox->setItemData(i, boldFont, Qt::FontRole);
            ui->emulatorComboBox->setItemData(i, QVariant(0), Qt::UserRole - 1);
        }
    }
}

///
/// Sets default text and items for various UI fields.
///
void MainWindow::setupOtherUIFields() {
    // Basic fields.
    ui->verboseComboBox->addItems({"Yes", "No"});
    ui->emulatorPathLineEdit->setText("Choose Path the Emulator executable");
    ui->romPathLineEdit->setText("C:/roms");
    ui->qmamehookerPathLineEdit->setText("C:/QMamehook");
    ui->demulShooterPathLineEdit->setText("C:/DemulShooter");

    // LED color combo boxes – add a placeholder.
    ui->P1Color->addItem("X");
    ui->P2Color->addItem("X");
    ui->P3Color->addItem("X");
    ui->P4Color->addItem("X");

    // Start command combo box.
    ui->StartCommands->addItems({
        "S0 - Start with solenoid enabled",
        "S1 - Start with rumble enabled",
        "S2 - Start with the RED LED enabled",
        "S3 - Start with the GREEN LED enabled",
        "S4 - Start with the BLUE LED enabled",
        "S6 - Start with everything enabled"
    });
    ui->StartCommands->setCurrentIndex(5);

    // Other combo boxes.
    ui->DeviceOutputMode->addItems({
        "------", "M0x0 - Mouse & Keyboard", "M0x1 - Gamepad, w/ Camera mapped to Right Stick",
        "M0x1L - Maps Camera to Left Stick instead (OpenFIRE exclusive)", "M0x2 - 'Hybrid'"
    });
    ui->OffscreenFiringMode->addItems({
        "------", "M1x0 - Disabled (not used in OpenFIRE)", "M1x1 - Fire in bottom-left corner (not used in OpenFIRE)",
        "M1x2 - Offscreen Button Mode enabled (i.e. offscreen trigger pulls generates a Right Click instead of a Left Click)",
        "M1x3 - Normal shot (always on when Offscreen Button Mode isn't set in OpenFIRE)"
    });
    ui->OffscreenFiringMode->setItemData(1, QVariant(0), Qt::UserRole - 1);
    ui->OffscreenFiringMode->setItemData(2, QVariant(0), Qt::UserRole - 1);

    ui->PedalMapping->addItems({
        "------", "M2x0 - Separate Button (as mapped)", "M2x1 - As Right Mouse", "M2x2 - As Middle Mouse (OpenFIRE exclusive)"
    });


    ui->AspectRatioCorrection->addItems({"------", "M3x0 - Fullscreen", "M3x1 - 4:3 Correction"});
    ui->RumbleOnlyMode->addItems({"------", "M6x0 - Disabled (Solenoid allowed)", "M6x1 - Enabled (Solenoid disabled, Rumble enabled)"});
    ui->AutoFireMode->addItems({"------", "M8x0 - Disabled (sustained fire is kept enabled in OpenFIRE)", "M8x1 - Auto fire on (enables Burst Fire in OpenFIRE)", "M8x2 - Auto fire always on rapid fire"});
    ui->DisplayMode->addItems({"------", "MDx1 - Life Only", "MDx2 - Ammo Only", "MDx3 - Life & Ammo Splitscreen", "MDx3B - Life Bar (Life Glyphs otherwise)"});
    ui->Recoil->addItems({"------", "Solenoid Single Pulse (recommended)", "Solenoid Switching", "Rumble Single Pulse (recommended for rumble)", "Rumble Switching", "Custom"});
    ui->Damaged->addItems({"------", "Rumble Single Pulse (recommended)", "Rumble Switching", "Custom"});
    ui->Clip->addItems({"------", "Red/Off", "Red/White", "White/Off", "Custom"});
    ui->Ammo->addItems({"------", "OLED On", "Custom"});
    ui->Life->addItems({"------", "OLED On", "Custom"});
    ui->Credits->addItems({"------", "OLED On (coming soon)", "Custom"});

    ui->P1Color->setCurrentIndex(1);
    ui->P2Color->setCurrentIndex(2);
    ui->P3Color->setCurrentIndex(3);
    ui->P4Color->setCurrentIndex(4);
}

///
/// Populates the LED color combo boxes with color icons and stores the QColor data.
///
void MainWindow::setupColorComboBoxes() {
    QList<QColor> colors = {
        QColor(255, 0, 0), QColor(0, 0, 255), QColor(0, 255, 0),
        QColor(255, 150, 0), QColor(255, 255, 0), QColor(255, 255, 255),
        QColor(0, 255, 255), QColor(255, 0, 255)
    };

    for (const QColor &color : colors) {
        QPixmap pixmap(20, 20);
        pixmap.fill(color);
        QIcon icon(pixmap);
        // Add icon with a blank text and store the color as userData.
        ui->P1Color->addItem(icon, " ", color);
        ui->P2Color->addItem(icon, " ", color);
        ui->P3Color->addItem(icon, " ", color);
        ui->P4Color->addItem(icon, " ", color);
    }
}

///
/// Connects signals to slots for the UI elements.
///
void MainWindow::setupSignalConnections() {
    // Emulator combo box signals.
    connect(ui->emulatorComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateGamesList);
    connect(ui->emulatorComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateEmulatorPath);

    // Export and Launch button signals.
    connect(ui->exportButton, &QPushButton::clicked, this, [this]() { exportFiles(); });
    connect(ui->LaunchButton, &QPushButton::clicked, this, &MainWindow::launchGame);

    // Browse button signals.
    connect(ui->browseEmulatorButton, &QPushButton::clicked, this, &MainWindow::browseEmulatorPath);
    connect(ui->browseRomButton, &QPushButton::clicked, this, &MainWindow::browseRomPath);
    connect(ui->browseQmamehookerButton, &QPushButton::clicked, this, &MainWindow::browseQmamehookerPath);
    connect(ui->browseDemulButton, &QPushButton::clicked, this, &MainWindow::browseDemulPath);

    // DemulShooter extra arguments
    connect(ui->demulShooterArgsLineEdit, &QLineEdit::textChanged, this, &MainWindow::updateBatCommandLine);

    // INI update signals for various combo boxes.
    connect(ui->StartCommands, &QComboBox::currentTextChanged, this, &MainWindow::updateIniText);
    connect(ui->DeviceOutputMode, &QComboBox::currentTextChanged, this, &MainWindow::updateIniText);
    connect(ui->OffscreenFiringMode, &QComboBox::currentTextChanged, this, &MainWindow::updateIniText);
    connect(ui->PedalMapping, &QComboBox::currentTextChanged, this, &MainWindow::updateIniText);
    connect(ui->AspectRatioCorrection, &QComboBox::currentTextChanged, this, &MainWindow::updateIniText);
    connect(ui->RumbleOnlyMode, &QComboBox::currentTextChanged, this, &MainWindow::updateIniText);
    connect(ui->AutoFireMode, &QComboBox::currentTextChanged, this, &MainWindow::updateIniText);
    connect(ui->DisplayMode, &QComboBox::currentTextChanged, this, &MainWindow::updateIniText);
    connect(ui->P1Color, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateIniText);
    connect(ui->P2Color, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateIniText);
    connect(ui->P3Color, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateIniText);
    connect(ui->P4Color, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateIniText);

    // Parameter combo box and text field connections.
    connect(ui->Recoil, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == "------") {
            ui->Recoil_Text->setPlainText("");
            // Clear all Recoil lines
            QStringList lines = originalIniContent.split('\n');
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].startsWith("P1_CtmRecoil") || 
                    lines[i].startsWith("P2_CtmRecoil") || 
                    lines[i].startsWith("P3_CtmRecoil") || 
                    lines[i].startsWith("P4_CtmRecoil")) {
                    QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                    lines[i] = QString("%1_CtmRecoil =").arg(prefix);
                }
            }
            originalIniContent = lines.join('\n');
            ui->plainTextEdit_Generic->setPlainText(originalIniContent);
        } else if (text == "Solenoid Single Pulse (recommended)") ui->Recoil_Text->setPlainText("F0x2x1");
        else if (text == "Solenoid Switching") ui->Recoil_Text->setPlainText("F0x%s%");
        else if (text == "Rumble Single Pulse (recommended for rumble)") ui->Recoil_Text->setPlainText("F1x2x1");
        else if (text == "Rumble Switching") ui->Recoil_Text->setPlainText("F1x%s%");
    });

    connect(ui->Recoil_Text, &QTextEdit::textChanged, this, [this]() {
        QString value = ui->Recoil_Text->toPlainText().trimmed();
        QStringList lines = originalIniContent.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].startsWith("P1_CtmRecoil") || 
                lines[i].startsWith("P2_CtmRecoil") || 
                lines[i].startsWith("P3_CtmRecoil") || 
                lines[i].startsWith("P4_CtmRecoil")) {
                QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                int playerNum = prefix.mid(1).toInt(); // Get the number from P1, P2, etc.
                if (value.isEmpty()) {
                    lines[i] = QString("%1_CtmRecoil =").arg(prefix);
                } else {
                    lines[i] = QString("%1_CtmRecoil = cmw %2 %3").arg(prefix).arg(playerNum).arg(value);
                }
            }
        }
        originalIniContent = lines.join('\n');
        ui->plainTextEdit_Generic->setPlainText(originalIniContent);
    });

    connect(ui->Damaged, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == "------") {
            ui->Damaged_Text->setPlainText("");
            // Clear all Damaged lines
            QStringList lines = originalIniContent.split('\n');
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].startsWith("P1_Damaged") || 
                    lines[i].startsWith("P2_Damaged") || 
                    lines[i].startsWith("P3_Damaged") || 
                    lines[i].startsWith("P4_Damaged")) {
                    QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                    lines[i] = QString("%1_Damaged =").arg(prefix);
                }
            }
            originalIniContent = lines.join('\n');
            ui->plainTextEdit_Generic->setPlainText(originalIniContent);
        } else if (text == "Rumble Single Pulse (recommended)") ui->Damaged_Text->setPlainText("F1x2x1");
        else if (text == "Rumble Switching") ui->Damaged_Text->setPlainText("F1x%s%");
    });

    connect(ui->Damaged_Text, &QTextEdit::textChanged, this, [this]() {
        QString value = ui->Damaged_Text->toPlainText().trimmed();
        QStringList lines = originalIniContent.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].startsWith("P1_Damaged") || 
                lines[i].startsWith("P2_Damaged") || 
                lines[i].startsWith("P3_Damaged") || 
                lines[i].startsWith("P4_Damaged")) {
                QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                int playerNum = prefix.mid(1).toInt(); // Get the number from P1, P2, etc.
                if (value.isEmpty()) {
                    lines[i] = QString("%1_Damaged =").arg(prefix);
                } else {
                    lines[i] = QString("%1_Damaged = cmw %2 %3").arg(prefix).arg(playerNum).arg(value);
                }
            }
        }
        originalIniContent = lines.join('\n');
        ui->plainTextEdit_Generic->setPlainText(originalIniContent);
    });

    connect(ui->Clip, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == "------") {
            ui->Clip_Text->setPlainText("");
            // Clear all Clip lines
            QStringList lines = originalIniContent.split('\n');
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].startsWith("P1_Clip") || 
                    lines[i].startsWith("P2_Clip") || 
                    lines[i].startsWith("P3_Clip") || 
                    lines[i].startsWith("P4_Clip")) {
                    QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                    lines[i] = QString("%1_Clip =").arg(prefix);
                }
            }
            originalIniContent = lines.join('\n');
            ui->plainTextEdit_Generic->setPlainText(originalIniContent);
        } else if (text == "Red/Off") ui->Clip_Text->setPlainText("F2x1x255xF3x1x0xF4x1x0");
        else if (text == "Red/White") ui->Clip_Text->setPlainText("F2x1x255xF3x1x255xF4x1x255");
        else if (text == "White/Off") ui->Clip_Text->setPlainText("F2x1x255xF3x1x255xF4x1x255");
    });

    connect(ui->Clip_Text, &QTextEdit::textChanged, this, [this]() {
        QString value = ui->Clip_Text->toPlainText().trimmed();
        QStringList lines = originalIniContent.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].startsWith("P1_Clip") || 
                lines[i].startsWith("P2_Clip") || 
                lines[i].startsWith("P3_Clip") || 
                lines[i].startsWith("P4_Clip")) {
                QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                int playerNum = prefix.mid(1).toInt(); // Get the number from P1, P2, etc.
                if (value.isEmpty()) {
                    lines[i] = QString("%1_Clip =").arg(prefix);
                } else {
                    lines[i] = QString("%1_Clip = cmw %2 %3").arg(prefix).arg(playerNum).arg(value);
                }
            }
        }
        originalIniContent = lines.join('\n');
        ui->plainTextEdit_Generic->setPlainText(originalIniContent);
    });

    connect(ui->Ammo, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == "------") {
            ui->Ammo_Text->setPlainText("");
            // Clear all Ammo lines
            QStringList lines = originalIniContent.split('\n');
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].startsWith("P1_Ammo") || 
                    lines[i].startsWith("P2_Ammo") || 
                    lines[i].startsWith("P3_Ammo") || 
                    lines[i].startsWith("P4_Ammo")) {
                    QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                    lines[i] = QString("%1_Ammo =").arg(prefix);
                }
            }
            originalIniContent = lines.join('\n');
            ui->plainTextEdit_Generic->setPlainText(originalIniContent);
        } else if (text == "OLED On") ui->Ammo_Text->setPlainText("FDAx%s%");
    });

    connect(ui->Ammo_Text, &QTextEdit::textChanged, this, [this]() {
        QString value = ui->Ammo_Text->toPlainText().trimmed();
        QStringList lines = originalIniContent.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].startsWith("P1_Ammo") || 
                lines[i].startsWith("P2_Ammo") || 
                lines[i].startsWith("P3_Ammo") || 
                lines[i].startsWith("P4_Ammo")) {
                QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                int playerNum = prefix.mid(1).toInt(); // Get the number from P1, P2, etc.
                if (value.isEmpty()) {
                    lines[i] = QString("%1_Ammo =").arg(prefix);
                } else {
                    lines[i] = QString("%1_Ammo = cmw %2 %3").arg(prefix).arg(playerNum).arg(value);
                }
            }
        }
        originalIniContent = lines.join('\n');
        ui->plainTextEdit_Generic->setPlainText(originalIniContent);
    });

    connect(ui->Life, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == "------") {
            ui->Life_Text->setPlainText("");
            // Clear all Life lines
            QStringList lines = originalIniContent.split('\n');
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].startsWith("P1_Life") || 
                    lines[i].startsWith("P2_Life") || 
                    lines[i].startsWith("P3_Life") || 
                    lines[i].startsWith("P4_Life")) {
                    QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                    lines[i] = QString("%1_Life =").arg(prefix);
                }
            }
            originalIniContent = lines.join('\n');
            ui->plainTextEdit_Generic->setPlainText(originalIniContent);
        } else if (text == "OLED On") ui->Life_Text->setPlainText("FDLx%s%");
    });

    connect(ui->Life_Text, &QTextEdit::textChanged, this, [this]() {
        QString value = ui->Life_Text->toPlainText().trimmed();
        QStringList lines = originalIniContent.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].startsWith("P1_Life") || 
                lines[i].startsWith("P2_Life") || 
                lines[i].startsWith("P3_Life") || 
                lines[i].startsWith("P4_Life")) {
                QString prefix = lines[i].split("_")[0]; // Get P1, P2, etc.
                int playerNum = prefix.mid(1).toInt(); // Get the number from P1, P2, etc.
                if (value.isEmpty()) {
                    lines[i] = QString("%1_Life =").arg(prefix);
                } else {
                    lines[i] = QString("%1_Life = cmw %2 %3").arg(prefix).arg(playerNum).arg(value);
                }
            }
        }
        originalIniContent = lines.join('\n');
        ui->plainTextEdit_Generic->setPlainText(originalIniContent);
    });

    connect(ui->Credits, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (text == "------") {
            ui->Credits_Text->setPlainText("");
            // Clear Credits line
            QStringList lines = originalIniContent.split('\n');
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].startsWith("Credits")) {
                    lines[i] = "Credits =";
                    break;
                }
            }
            originalIniContent = lines.join('\n');
            ui->plainTextEdit_Generic->setPlainText(originalIniContent);
        } else if (text == "OLED On (coming soon)") ui->Credits_Text->setPlainText("cmw 1 F2x1x255xF3x1x255xF4x1x255");
    });

    connect(ui->Credits_Text, &QTextEdit::textChanged, this, [this]() {
        QString value = ui->Credits_Text->toPlainText().trimmed();
        QStringList lines = originalIniContent.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].startsWith("Credits")) {
                if (value.isEmpty()) {
                    lines[i] = "Credits =";
                } else {
                    lines[i] = QString("Credits = cmw 1 %1").arg(value);
                }
                break;
            }
        }
        originalIniContent = lines.join('\n');
        ui->plainTextEdit_Generic->setPlainText(originalIniContent);
    });

    connect(ui->Custom1, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        ui->Custom1_Text->setPlainText(text == "------" ? "" : "XX");
    });
    connect(ui->Custom2, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        ui->Custom2_Text->setPlainText(text == "------" ? "" : "XX");
    });
    connect(ui->Custom3, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        ui->Custom3_Text->setPlainText(text == "------" ? "" : "XX");
    });

    // TextEdit changes updating combo boxes and INI text.
    connect(ui->Recoil_Text, &QTextEdit::textChanged, this, [this]() {
        QString text = ui->Recoil_Text->toPlainText().trimmed();
        if (text.isEmpty()) ui->Recoil->setCurrentText("------");
        else if (text == "F0x2x1") ui->Recoil->setCurrentText("Solenoid Single Pulse (recommended)");
        else if (text == "F0x%s%") ui->Recoil->setCurrentText("Solenoid Switching");
        else if (text == "F1x2x1") ui->Recoil->setCurrentText("Rumble Single Pulse (recommended for rumble)");
        else if (text == "F1x%s%") ui->Recoil->setCurrentText("Rumble Switching");
        else ui->Recoil->setCurrentText("Custom");
        updateIniText();
    });
    connect(ui->Damaged_Text, &QTextEdit::textChanged, this, [this]() {
        QString text = ui->Damaged_Text->toPlainText().trimmed();
        if (text == "F1x2x1") ui->Damaged->setCurrentText("Rumble Single Pulse (recommended)");
        else if (text == "F1x%s%") ui->Damaged->setCurrentText("Rumble Switching");
        else if (text.isEmpty()) ui->Damaged->setCurrentText("------");
        else ui->Damaged->setCurrentText("Custom");
        updateIniText();
    });
    connect(ui->Clip_Text, &QTextEdit::textChanged, this, [this]() {
        QString text = ui->Clip_Text->toPlainText().trimmed();
        if (text == "XX" && ui->Clip->currentText() == "------") ui->Clip->setCurrentText("Red/Off");
        else if (text.isEmpty()) ui->Clip->setCurrentText("------");
        else ui->Clip->setCurrentText("Custom");
        updateIniText();
    });
    connect(ui->Ammo_Text, &QTextEdit::textChanged, this, [this]() {
        QString text = ui->Ammo_Text->toPlainText().trimmed();
        if (text == "FDAx%s%") ui->Ammo->setCurrentText("OLED On");
        else if (text.isEmpty()) ui->Ammo->setCurrentText("------");
        else ui->Ammo->setCurrentText("Custom");
        updateIniText();
    });
    connect(ui->Life_Text, &QTextEdit::textChanged, this, [this]() {
        QString text = ui->Life_Text->toPlainText().trimmed();
        if (text == "FDLx%s%") ui->Life->setCurrentText("OLED On");
        else if (text.isEmpty()) ui->Life->setCurrentText("------");
        else ui->Life->setCurrentText("Custom");
        updateIniText();
    });
    connect(ui->Credits_Text, &QTextEdit::textChanged, this, [this]() {
        QString text = ui->Credits_Text->toPlainText().trimmed();
        if (text == "XX") ui->Credits->setCurrentText("OLED On (coming soon)");
        else if (text.isEmpty()) ui->Credits->setCurrentText("------");
        else ui->Credits->setCurrentText("Custom");
        updateIniText();
    });

    // Connect the refresh INI button using findChild to avoid linter errors
    QPushButton* refreshButton = findChild<QPushButton*>("refreshIniButton");
    if (refreshButton) {
        connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshIni);
    } else {
        qWarning() << "Could not find refreshIniButton in the UI";
    }
    
    // Connect color dropdowns to update LmpStart values
    connect(ui->P1Color, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        updateLmpStartValue(1, ui->P1Color->itemData(index).value<QColor>());
    });
    connect(ui->P2Color, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        updateLmpStartValue(2, ui->P2Color->itemData(index).value<QColor>());
    });
    connect(ui->P3Color, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        updateLmpStartValue(3, ui->P3Color->itemData(index).value<QColor>());
    });
    connect(ui->P4Color, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        updateLmpStartValue(4, ui->P4Color->itemData(index).value<QColor>());
    });
}

bool MainWindow::safeReplaceLine(QStringList &lines, const QString &key, const QString &newLine) {
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].startsWith(key)) {
            lines[i] = newLine;
            return true;
        }
    }
    return false;
}

void MainWindow::updateLmpStartValue(int player, const QColor &color) {
    if (!hasLoadedIni || originalIniContent.isEmpty()) return;
    
    QStringList lines = originalIniContent.split('\n');
    QString key = QString("P%1_LmpStart").arg(player);
    
    // Check if "X" is selected (index 0)
    if (player == 1 && ui->P1Color->currentIndex() == 0 ||
        player == 2 && ui->P2Color->currentIndex() == 0 ||
        player == 3 && ui->P3Color->currentIndex() == 0 ||
        player == 4 && ui->P4Color->currentIndex() == 0) {
        // Clear only this player's LmpStart value
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].startsWith(key)) {
                lines[i] = QString("%1 =").arg(key);
                break;
            }
        }
    } else {
        // Format: cmw <player> F2x1x<red>xF3x1x<green>xF4x1x<blue> | cmw <player> F2x1x0xF3x1x0xF4x1x0
        QString value = QString("cmw %1 F2x1x%2xF3x1x%3xF4x1x%4 | cmw %1 F2x1x0xF3x1x0xF4x1x0")
            .arg(player)
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue());
            
        bool found = false;
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].startsWith(key)) {
                lines[i] = QString("%1 = %2").arg(key).arg(value);
                found = true;
                break;
            }
        }
        
        if (!found) {
            lines.append(QString("%1 = %2").arg(key).arg(value));
        }
    }
    
    originalIniContent = lines.join('\n');
    ui->plainTextEdit_Generic->setPlainText(originalIniContent);
}

///
/// Loads the default INI template into the plain text edit.
///
void MainWindow::setupDefaultIni() {
    // Clear the text editor instead of setting a default INI
    ui->plainTextEdit_Generic->clear();
    ui->plainTextEdit_Bat->clear();
    
    // Reset the loaded flag
    originalIniContent = "";
    hasLoadedIni = false;
    
    // By default, only enable Player 1 controls
    // Other players will be enabled when we detect them in an INI file
    ui->P1Color->setEnabled(true);
    ui->P2Color->setEnabled(false);
    ui->P3Color->setEnabled(false);
    ui->P4Color->setEnabled(false);
    
    // Hide custom fields by default - they will be shown if detected in INI
    ui->lineEdit->setVisible(false);
    ui->Custom1->setVisible(false);
    ui->Custom1_Text->setVisible(false);
    
    ui->lineEdit_2->setVisible(false);
    ui->Custom2->setVisible(false);
    ui->Custom2_Text->setVisible(false);
    
    ui->lineEdit_3->setVisible(false);
    ui->Custom3->setVisible(false);
    ui->Custom3_Text->setVisible(false);
    
    // Reset custom field labels
    ui->lineEdit->setText("Custom 1.");
    ui->lineEdit_2->setText("Custom 2.");
    ui->lineEdit_3->setText("Custom 3.");
    
    // Make sure all standard controls are visible by default
    // They will be hidden if not found in the INI file
    ui->Recoil->setVisible(true);
    ui->Recoil_Text->setVisible(true);
    ui->Recoil_Label->setVisible(true);
    
    ui->Damaged->setVisible(true);
    ui->Damaged_Text->setVisible(true);
    ui->Damaged_Label->setVisible(true);
    
    ui->Clip->setVisible(true);
    ui->Clip_Text->setVisible(true);
    ui->Clip_Label->setVisible(true);
    
    ui->Ammo->setVisible(true);
    ui->Ammo_Text->setVisible(true);
    ui->Ammo_Label->setVisible(true);
    
    ui->Life->setVisible(true);
    ui->Life_Text->setVisible(true);
    ui->Life_Label->setVisible(true);
    
    ui->Credits->setVisible(true);
    ui->Credits_Text->setVisible(true);
    ui->Credits_Label->setVisible(true);
}

// ---------------------- Other Methods (unchanged logic) ---------------------- //

void MainWindow::prepareDirectories(const QString &basePath, QDir &iniDir, QDir &batDir)
{
    // Create an absolute path using QDir to ensure platform compatibility
    QDir baseDir(basePath);
    
    // Make sure base directory exists
    if (!baseDir.exists()) {
        if (!baseDir.mkpath(".")) {
            qWarning() << "Failed to create base directory:" << basePath;
            return;
        }
    }
    
    QString iniDirPath = basePath + "/ini";
    QString batDirPath = basePath + "/bat";
    
    iniDir = QDir(iniDirPath);
    batDir = QDir(batDirPath);

    if (!iniDir.exists()) {
        if (!iniDir.mkpath(".")) {
            qWarning() << "Failed to create 'ini' directory:" << iniDirPath;
        } else {
            qDebug() << "'ini' directory is ready:" << iniDir.absolutePath();
        }
    } else {
        qDebug() << "'ini' directory is ready:" << iniDir.absolutePath();
    }

    if (!batDir.exists()) {
        if (!batDir.mkpath(".")) {
            qWarning() << "Failed to create 'bat' directory:" << batDirPath;
        } else {
            qDebug() << "'bat' directory is ready:" << batDir.absolutePath();
        }
    } else {
        qDebug() << "'bat' directory is ready:" << batDir.absolutePath();
    }
}

void MainWindow::mapEmulator(QString &emulator, QString &demulShooterExe)
{
    // Use the utility class to handle emulator mapping
    EmulatorUtils::mapEmulator(emulator, demulShooterExe);
}

QString MainWindow::mapRom(const QString &rom)
{
    // Use the utility class to handle ROM mapping
    return EmulatorUtils::mapRom(rom);
}

void MainWindow::createFiles(const QString &rom,
                             const QString &emulatorInput,
                             QString demulShooterExeInput,
                             const QString &emulatorPath,
                             const QString &romPath,
                             const QString &qmamehookerPath,
                             const QString &demulShooterPath,
                             const QString &verbose,
                             const QString &iniContent,
                             const QString &batContent)
{
    // Mapping and file creation logic remains unchanged.
    Q_UNUSED(emulatorInput);
    Q_UNUSED(demulShooterExeInput);
    QString rom2 = EmulatorUtils::mapRom(rom);
    // Log the ROM mapping for debugging
    qDebug() << "ROM mapping:" << rom << "->" << rom2;

    QDir iniDir, batDir;
    Q_UNUSED(emulatorPath);
    Q_UNUSED(romPath);
    Q_UNUSED(demulShooterPath);
    Q_UNUSED(verbose);
    prepareDirectories(qmamehookerPath, iniDir, batDir);
    
    // Create the BAT file with provided content
    QString batFilePath = batDir.filePath(rom + ".bat");
    QFile batFile(batFilePath);
    if (batFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&batFile);
        out << batContent;
        batFile.close();
        qDebug() << "Batch file created at:" << batFilePath;
    } else {
        qWarning() << "Failed to create batch file at:" << batFilePath;
    }

    // Create the INI file with platform-independent path handling
    QString iniFilePath = iniDir.filePath(rom2 + ".ini");
    QFile iniFile(iniFilePath);
    
    if (iniFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&iniFile);
        out << iniContent;
        iniFile.close();
        qDebug() << "INI file created at:" << iniFilePath;
    } else {
        qWarning() << "Failed to create INI file at:" << iniFilePath;
    }
}

QString MainWindow::generateBatContent(const QString &rom,
                                       const QString &emulatorInput,
                                       QString demulShooterExeInput,
                                       const QString &emulatorPath,
                                       const QString &romPath,
                                       const QString &qmamehookerPath,
                                       const QString &demulShooterPath,
                                       const QString &verbose,
                                       const QString &demulShooterArgs)
{
    QString emulator = emulatorInput;
    QString demulShooterExe = demulShooterExeInput;
    EmulatorUtils::mapEmulator(emulator, demulShooterExe);
    QString rom2 = EmulatorUtils::mapRom(rom);

    QDir iniDir, batDir;
    prepareDirectories(qmamehookerPath, iniDir, batDir);

    QString iniDirPath = iniDir.absolutePath();
    QFileInfo emulatorFileInfo(emulatorPath);
    QString emulatorDirectory = emulatorFileInfo.absolutePath();
    QString emulatorExecutable = emulatorFileInfo.fileName();

    QString content;
    QTextStream out(&content);
    out << "start \"Demul\" \"" << QDir::toNativeSeparators(demulShooterPath + "/" + demulShooterExe)
        << "\" -target=" << emulator
        << " -rom=" << rom2;
    if (!demulShooterArgs.trimmed().isEmpty())
        out << ' ' << demulShooterArgs.trimmed();
    out << "\n";
    out << "start /MIN \"Hooker\" \"" << QDir::toNativeSeparators(qmamehookerPath + "/QMamehook.exe")
        << "\" -p \"" << QDir::toNativeSeparators(iniDirPath) << "\" " << verbose << " -c \n";
    out << "cd \"" << QDir::toNativeSeparators(emulatorDirectory) << "\"\n";

    if (emulator == "demul07a") {
        QString runTarget = EmulatorUtils::demulRunParameter(rom2);
        out << "start \"demul07a\" \"" << emulatorExecutable << "\" -run=" << runTarget
            << " -rom=" << rom2;
    } else if (emulator == "flycast") {
        out << "start \"" << emulator << "\" " << emulatorExecutable
            << " -config window:fullscreen=yes \"" << QDir::toNativeSeparators(romPath + "/" + rom2 + ".zip") << "\"";
    } else if (emulator == "lindbergh" || emulator == "ringwide" || emulator == "rawthrill") {
        out << "start \"" << emulator << "\" " << emulatorExecutable << " --profile=" << rom2 + ".xml";
    } else {
        out << "start \"" << emulator << "\" " << emulatorExecutable << " " << rom2;
    }

    return content;
}

bool MainWindow::exportFiles(bool showMessage)
{
    QString emulator = ui->emulatorComboBox->currentText();
    QString emulatorPath = ui->emulatorPathLineEdit->text();
    QString rom = ui->romComboBox->currentText();
    QString romPath = ui->romPathLineEdit->text();
    QString qmamehookerPath = ui->qmamehookerPathLineEdit->text();
    QString demulShooterPath = ui->demulShooterPathLineEdit->text();
    QString verbose = (ui->verboseComboBox->currentText() == "Yes") ? "-v" : "";
    QString iniContent = ui->plainTextEdit_Generic->toPlainText();
    QString batContent = ui->plainTextEdit_Bat->toPlainText();

    // Check for Windows paths on non-Windows platforms
    #ifndef Q_OS_WIN
    if (qmamehookerPath.startsWith("C:/") || qmamehookerPath.startsWith("C:\\")) {
        QMessageBox::warning(this, "Path Warning",
                             "You're using Windows-style paths (C:/) on a non-Windows system.\n"
                             "Please use paths appropriate for your operating system.");
        qWarning() << "Export failed due to invalid path format.";
        return false;
    }
    #endif

    createFiles(rom, emulator, QString(), emulatorPath, romPath, qmamehookerPath, demulShooterPath, verbose, iniContent, batContent);

    if (showMessage)
        QMessageBox::information(this, "Export", "Batch and INI files have been successfully exported.");

    return true;
}

void MainWindow::updateGamesList()
{
    QString emulator = ui->emulatorComboBox->currentText();
    ui->romComboBox->clear();
    ui->romComboBox->setEnabled(true);

    // Reset loaded INI state when changing emulators
    originalIniContent = "";
    hasLoadedIni = false;
    ui->plainTextEdit_Generic->clear();
    ui->plainTextEdit_Bat->clear();
    ui->demulShooterArgsLineEdit->clear();
    
    // Set player controls (only Player 1 enabled by default)
    ui->P1Color->setEnabled(true);
    ui->P2Color->setEnabled(false);
    ui->P3Color->setEnabled(false);
    ui->P4Color->setEnabled(false);

    disconnect(ui->romComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
               this, &MainWindow::loadIniSettings);

    // Use the utility class to populate the ROM combo box
    EmulatorUtils::updateGamesList(emulator, ui->romComboBox);

    connect(ui->romComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &MainWindow::loadIniSettings);
}


void MainWindow::browseQmamehookerPath()
{
    qDebug() << "browseQmamehookerPath triggered";
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select QMamehooker Directory"), ui->qmamehookerPathLineEdit->text(), QFileDialog::DontUseNativeDialog);
    if (!dir.isEmpty()) {
        ui->qmamehookerPathLineEdit->setText(dir);
    }
}

void MainWindow::browseEmulatorPath()
{
    qDebug() << "browseEmulatorPath triggered";
    QString file = QFileDialog::getOpenFileName(this, tr("Select Emulator Executable"), ui->emulatorPathLineEdit->text(), tr("Executables (*.exe);;All Files (*)"), nullptr, QFileDialog::DontUseNativeDialog);
    if (!file.isEmpty()) {
        ui->emulatorPathLineEdit->setText(file);
    }
}

void MainWindow::browseRomPath()
{
    qDebug() << "browseRomPath triggered";
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select ROM Directory"), ui->romPathLineEdit->text(), QFileDialog::DontUseNativeDialog);
    if (!dir.isEmpty()) {
        ui->romPathLineEdit->setText(dir);
    }
}

void MainWindow::browseDemulPath()
{
    qDebug() << "browseDemulPath triggered";
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select DemulShooter Directory"), ui->demulShooterPathLineEdit->text(), QFileDialog::DontUseNativeDialog);
    if (!dir.isEmpty()) {
        ui->demulShooterPathLineEdit->setText(dir);
    }
}

void MainWindow::updateIniText()
{
    // Bail if we're mid-load
    if (isLoadingIni)
        return;

    // Only proceed if we already have file content we want to preserve
    if (!(hasLoadedIni && !originalIniContent.isEmpty()))
        return;

    /*****  small helpers  *****/
    auto safeReplaceLine = [](QStringList &lines,
                              const QString  &keyPrefix,
                              const QString  &newLine) -> bool
    {
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].trimmed().startsWith(keyPrefix, Qt::CaseInsensitive)) {
                lines[i] = newLine;
                return true;
            }
        }
        return false;
    };

    auto removeLinesMatching = [](QStringList &lines,
                                  const QRegularExpression &rx)
    {
        for (int i = 0; i < lines.size(); ) {
            if (rx.match(lines[i]).hasMatch())
                lines.removeAt(i);        // ← don't ++i here
            else
                ++i;
        }
    };

    /*****  working copy of the whole file  *****/
    QString updatedContent = originalIniContent;

    /* -------------------------------------------------------------------- */
    /* 1.  Detect how many players are in play                              */
    /* -------------------------------------------------------------------- */
    int playerCount = 1;
    if (ui->P4Color->isEnabled())      playerCount = 4;
    else if (ui->P3Color->isEnabled()) playerCount = 3;
    else if (ui->P2Color->isEnabled()) playerCount = 2;

    /* -------------------------------------------------------------------- */
    /* 2.  Update the MameStart line (start flags etc.)                     */
    /* -------------------------------------------------------------------- */
    const QMap<QString, QString> startMap = {
                                             {"S0 - Start with solenoid enabled",          "S0"},
                                             {"S1 - Start with rumble enabled",            "S1"},
                                             {"S2 - Start with the RED LED enabled",       "S2"},
                                             {"S3 - Start with the GREEN LED enabled",     "S3"},
                                             {"S4 - Start with the BLUE LED enabled",      "S4"},
                                             {"S6 - Start with everything enabled",        "S6"},
                                             };

    const QString start = startMap.value(ui->StartCommands->currentText());

    if (!start.isEmpty()) {
        QRegularExpression mameStartRx(R"(MameStart\s*=\s*(.*))");
        auto match = mameStartRx.match(updatedContent);
        if (match.hasMatch()) {
            QString mameStartLine  = match.captured(0);
            QString mameStartValue = match.captured(1);

            // Build mode flags string from UI selections
            QStringList modeFlags;

            // Device Output Mode
            if (ui->DeviceOutputMode->currentText().contains("M0x")) {
                QString mode = ui->DeviceOutputMode->currentText().mid(0, 4);
                modeFlags << mode;
            }

            // Offscreen Firing Mode
            if (ui->OffscreenFiringMode->currentText().contains("M1x")) {
                QString mode = ui->OffscreenFiringMode->currentText().mid(0, 4);
                modeFlags << mode;
            }

            // Pedal Mapping
            if (ui->PedalMapping->currentText().contains("M2x")) {
                QString mode = ui->PedalMapping->currentText().mid(0, 4);
                modeFlags << mode;
            }

            // Aspect Ratio Correction
            if (ui->AspectRatioCorrection->currentText().contains("M3x")) {
                QString mode = ui->AspectRatioCorrection->currentText().mid(0, 4);
                modeFlags << mode;
            }

            // Rumble Only Mode
            if (ui->RumbleOnlyMode->currentText().contains("M6x")) {
                QString mode = ui->RumbleOnlyMode->currentText().mid(0, 4);
                modeFlags << mode;
            }

            // Auto Fire Mode
            if (ui->AutoFireMode->currentText().contains("M8x")) {
                QString mode = ui->AutoFireMode->currentText().mid(0, 4);
                modeFlags << mode;
            }

            // Display Mode
            if (ui->DisplayMode->currentText().contains("MDx")) {
                QString mode = ui->DisplayMode->currentText();
                if (mode.contains("MDx3B")) {
                    modeFlags << "MDx3B";
                } else {
                    modeFlags << mode.mid(0, 4);
                }
            }

            QString modeFlagsStr = modeFlags.join('x');
            if (!modeFlagsStr.isEmpty()) {
                modeFlagsStr = 'x' + modeFlagsStr;
            }

            // Split the MameStart value into individual commands
            QStringList commands = mameStartValue.split(',');
            QStringList newCommands;

            for (const QString &cmd : commands) {
                QString trimmedCmd = cmd.trimmed();
                
                // Handle cmw commands (player-specific commands)
                if (trimmedCmd.startsWith("cmw")) {
                    QRegularExpression playerCmdRx(R"(cmw\s+(\d+)\s+(?:S[0-6])?(?:x[^,\s]*)?)");
                    auto playerMatch = playerCmdRx.match(trimmedCmd);
                    
                    if (playerMatch.hasMatch()) {
                        int player = playerMatch.captured(1).toInt();
                        // Add both start flag and mode flags
                        newCommands.append(QString("cmw %1 %2%3").arg(player).arg(start).arg(modeFlagsStr));
                    } else {
                        newCommands.append(trimmedCmd);
                    }
                } else if (trimmedCmd.startsWith("\"cmo")) {
                    // Keep cmo commands with quotes
                    newCommands.append(trimmedCmd);
                } else if (trimmedCmd.startsWith("cmo")) {
                    // Add quotes around cmo commands
                    newCommands.append(QString("\"%1\"").arg(trimmedCmd));
                } else {
                    // Keep other commands unchanged
                    newCommands.append(trimmedCmd);
                }
            }

            // Reconstruct the MameStart line
            QString newStartValue = newCommands.join(", ");
            updatedContent.replace(mameStartLine, "MameStart = " + newStartValue);
        }
    }

    /* -------------------------------------------------------------------- */
    /* 3.  Re-write the [General] section, preserving Flags line            */
    /* -------------------------------------------------------------------- */
    const int generalPos = updatedContent.indexOf("[General]");
    if (generalPos >= 0) {
        int nextPos = updatedContent.indexOf("[", generalPos + 1);
        if (nextPos < 0) nextPos = updatedContent.length();

        QStringList lines = updatedContent.mid(generalPos,
                                               nextPos - generalPos).split('\n');

        // Keep only non-mode-flag lines and preserve Flags line
        QStringList newLines;
        for (const QString &line : lines) {
            QString trimmed = line.trimmed();
            if (!trimmed.startsWith("M0x") && !trimmed.startsWith("M1x") && 
                !trimmed.startsWith("M2x") && !trimmed.startsWith("M3x") &&
                !trimmed.startsWith("M6x") && !trimmed.startsWith("M8x") &&
                !trimmed.startsWith("MDx")) {
                newLines.append(line);
            }
        }

        // Stitch the cleaned list back together
        QString newGeneral = newLines.join('\n').trimmed() + '\n';
        updatedContent.replace(generalPos, nextPos - generalPos, newGeneral);
    }

    /* -------------------------------------------------------------------- */
    /* 4.  Helper to write Output-section keys (Recoil, Damage, etc.)        */
    /* -------------------------------------------------------------------- */
    auto writePlayerKey = [&](const QString &templateKey,
                              const QString &value)
    {
        if (value.isEmpty()) return;

        QStringList lines = updatedContent.split('\n');
        bool   modified   = false;

        for (int player = 1; player <= playerCount; ++player) {
            // skip disabled colours
            bool enabled = (player == 1 && ui->P1Color->isEnabled()) ||
                           (player == 2 && ui->P2Color->isEnabled()) ||
                           (player == 3 && ui->P3Color->isEnabled()) ||
                           (player == 4 && ui->P4Color->isEnabled());
            if (!enabled) continue;

            const QString key = QString(templateKey).arg(player);
            const QString newLine =
                QString("%1 = cmw %2 %3").arg(key).arg(player).arg(value);

            if (!safeReplaceLine(lines, key, newLine)) {
                // key didn't exist – append it just inside [Output]
                int outPos = lines.indexOf(QRegularExpression(R"(\[Output\])"));
                if (outPos >= 0)
                    lines.insert(outPos + 1, newLine);
            }
            modified = true;
        }

        if (modified)
            updatedContent = lines.join('\n');
    };

    /* 4a –- Recoil ------------------------------------------------------- */
    const QMap<QString, QString> recoilMap = {
                                              {"------", ""},
                                              {"Solenoid Single Pulse (recommended)",       "F0x2x1"},
                                              {"Solenoid Switching",                        "F0x%s%"},
                                              {"Rumble Single Pulse (recommended for rumble)", "F1x2x1"},
                                              {"Rumble Switching",                          "F1x%s%"},
                                              };
    writePlayerKey("P%1_CtmRecoil",
                   ui->Recoil->isVisible()
                       ? (!ui->Recoil_Text->toPlainText().isEmpty()
                              ? ui->Recoil_Text->toPlainText()
                              : recoilMap.value(ui->Recoil->currentText()))
                       : "");

    /* 4b –- Damage ------------------------------------------------------- */
    const QMap<QString, QString> damageMap = {
                                              {"------", ""},
                                              {"Rumble Single Pulse (recommended)", "F1x2x1"},
                                              {"Rumble Switching",                  "F1x%s%"},
                                              };
    writePlayerKey("P%1_Damaged",
                   ui->Damaged->isVisible()
                       ? (!ui->Damaged_Text->toPlainText().isEmpty()
                              ? ui->Damaged_Text->toPlainText()
                              : damageMap.value(ui->Damaged->currentText()))
                       : "");

    /* 4c –- Clip --------------------------------------------------------- */
    writePlayerKey("P%1_Clip",
                   ui->Clip->isVisible()
                       ? (!ui->Clip_Text->toPlainText().isEmpty()
                              ? ui->Clip_Text->toPlainText()
                              : (ui->Clip->currentText() == "Red/Off" ? "XX" : ""))
                       : "");

    /* 4d –- Ammo --------------------------------------------------------- */
    writePlayerKey("P%1_Ammo",
                   ui->Ammo->isVisible()
                       ? (!ui->Ammo_Text->toPlainText().isEmpty()
                              ? ui->Ammo_Text->toPlainText()
                              : (ui->Ammo->currentText() == "OLED On" ? "DFAx%s%" : ""))
                       : "");

    /* 4e –- Life --------------------------------------------------------- */
    writePlayerKey("P%1_Life",
                   ui->Life->isVisible()
                       ? (!ui->Life_Text->toPlainText().isEmpty()
                              ? ui->Life_Text->toPlainText()
                              : (ui->Life->currentText() == "OLED On" ? "DFLx%s%" : ""))
                       : "");

    /* 4f –- Credits (single key) ---------------------------------------- */
    if (ui->Credits->isVisible()) {
        const QString credits =
            !ui->Credits_Text->toPlainText().isEmpty()
                ? ui->Credits_Text->toPlainText()
                : (ui->Credits->currentText() == "OLED On (coming soon)" ? "XX" : "");

        if (!credits.isEmpty()) {
            QStringList lines = updatedContent.split('\n');
            if (!safeReplaceLine(lines, "Credits", "Credits = cmw 1 " + credits)) {
                int outPos = lines.indexOf(QRegularExpression(R"(\[Output\])"));
                if (outPos >= 0)
                    lines.insert(outPos + 1, "Credits = cmw 1 " + credits);
            }
            updatedContent = lines.join('\n');
        }
    }

    /* -------------------------------------------------------------------- */
    /* 5.  Custom 1-3 (same helper)                                         */
    /* -------------------------------------------------------------------- */
    auto handleCustom = [&](QComboBox *cb,
                            QTextEdit  *txt,
                            QLabel     *label,        // ← was QLineEdit*
                            const QString &defaultName)
    {
        if (!cb->isVisible() || label->text() == defaultName)
            return;

        const QString value =
            !txt->toPlainText().isEmpty() ? txt->toPlainText()
                                          : (cb->currentText() == "------"
                                                 ? ""
                                                 : cb->currentText());

        if (value.isEmpty()) return;

        const QString settingName = label->text();
        writePlayerKey("P%1_" + settingName, value);
    };

    handleCustom(ui->Custom1, ui->Custom1_Text, ui->lineEdit,   "Custom 1.");
    handleCustom(ui->Custom2, ui->Custom2_Text, ui->lineEdit_2, "Custom 2.");
    handleCustom(ui->Custom3, ui->Custom3_Text, ui->lineEdit_3, "Custom 3.");

    /* -------------------------------------------------------------------- */
    /* 6.  Push the updated text back to the UI                             */
    /* -------------------------------------------------------------------- */
    ui->plainTextEdit_Generic->setPlainText(updatedContent);
}


void MainWindow::updateEmulatorPath()
{
    QString emulator = ui->emulatorComboBox->currentText();
    QString emulatorPath, romPath;
    
    EmulatorUtils::updateEmulatorPath(emulator, emulatorPath, romPath);
    
    ui->emulatorPathLineEdit->setText(emulatorPath);
    ui->romPathLineEdit->setText(romPath);
    
    // Set platform-appropriate default paths for the QMamehooker and DemulShooter paths
    if (ui->qmamehookerPathLineEdit->text().startsWith("C:/")) {
        #ifdef Q_OS_WIN
        ui->qmamehookerPathLineEdit->setText("C:/QMamehook");
        ui->demulShooterPathLineEdit->setText("C:/DemulShooter");
        #else
        ui->qmamehookerPathLineEdit->setText(QDir::homePath() + "/QMamehook");
        ui->demulShooterPathLineEdit->setText(QDir::homePath() + "/DemulShooter");
        #endif
    }
}

void MainWindow::loadIniSettings(const QString &romName)
{
    // Reset INI state when changing ROMs
    originalIniContent = "";
    hasLoadedIni = false;
    ui->plainTextEdit_Generic->clear();
    ui->plainTextEdit_Bat->clear();

    // Always hide custom fields - they will be edited in the text editor
    ui->lineEdit->setVisible(false);
    ui->Custom1->setVisible(false);
    ui->Custom1_Text->setVisible(false);
    ui->lineEdit_2->setVisible(false);
    ui->Custom2->setVisible(false);
    ui->Custom2_Text->setVisible(false);
    ui->lineEdit_3->setVisible(false);
    ui->Custom3->setVisible(false);
    ui->Custom3_Text->setVisible(false);

    QString rom2 = EmulatorUtils::mapRom(romName);
    if (rom2.isEmpty()) return;

    QString qmamehookerPath = ui->qmamehookerPathLineEdit->text();
    QString iniPath = qmamehookerPath + "/ini/" + rom2 + ".ini";
    QFile iniFile(iniPath);

    QString defaultHeader = "[General]\n"
                          "MameStart=\"cmo 1 baud=9600_parity=N_data=8_stop=1\", \"cmo 2 baud=9600_parity=N_data=8_stop=1\", cmw 1 S6, cmw 2 S6\n"
                          "MameStop=cmw 1 E, cmw 2 E, cmc 1, cmc 2\n"
                          "StateChange=\n"
                          "OnRotate=\n"
                          "OnPause=\n\n"
                          "[KeyStates]\n"
                          "RefreshTime=\n\n";

    QString iniContent;
    if (iniFile.exists()) {
        if (iniFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            iniContent = iniFile.readAll();
            iniFile.close();

            // Check if [General] section exists
            if (!iniContent.contains("[General]")) {
                // Add default header at the start
                iniContent = defaultHeader + iniContent;
            }
        }
    } else {
        // If file doesn't exist, start with default header
        iniContent = defaultHeader;
    }

    // Store the original INI content for structure preservation
    originalIniContent = iniContent;
    hasLoadedIni = true;

    // Load the INI file content
    ui->plainTextEdit_Generic->setPlainText(iniContent);

    // Load or generate BAT file content
    QString batPath = qmamehookerPath + "/bat/" + rom2 + ".bat";
    QString batContent;
    QFile batFile(batPath);
    if (batFile.exists() && batFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        batContent = batFile.readAll();
        batFile.close();
        QStringList batLines = batContent.split('\n');
        if (!batLines.isEmpty()) {
            QRegularExpression argsRegex(R"(start\s+"Demul"\s+".*"\s+-target=[^\s]+\s+-rom=[^\s]+\s*(.*))", QRegularExpression::CaseInsensitiveOption);
            auto match = argsRegex.match(batLines[0]);
            if (match.hasMatch()) ui->demulShooterArgsLineEdit->setText(match.captured(1).trimmed());
        }
    } else {
        QString verbose = (ui->verboseComboBox->currentText() == "Yes") ? "-v" : "";
        batContent = generateBatContent(romName,
                                        ui->emulatorComboBox->currentText(),
                                        QString(),
                                        ui->emulatorPathLineEdit->text(),
                                        ui->romPathLineEdit->text(),
                                        qmamehookerPath,
                                        ui->demulShooterPathLineEdit->text(),
                                        verbose,
                                        ui->demulShooterArgsLineEdit->text());
    }
    ui->plainTextEdit_Bat->setPlainText(batContent);

    // Load General settings first
    int generalSectionPos = iniContent.indexOf("[General]");
    if (generalSectionPos >= 0) {
        // Find the end of the General section
        int nextSectionPos = iniContent.indexOf("[", generalSectionPos + 1);
        if (nextSectionPos < 0) nextSectionPos = iniContent.length();

        // Extract the General section
        QString generalSection = iniContent.mid(generalSectionPos, nextSectionPos - generalSectionPos);

        qDebug() << "Loaded General section:" << generalSection;

        // Look for each of the General section settings with more flexible regex patterns

        // First, extract any mode flags from the MameStart command
        QRegularExpression mameStartRegex(R"(MameStart\s*=.*?cmw\s+\d+\s+S\d+(?:x(?:[^,\s]+))*)");
        auto mameStartMatch = mameStartRegex.match(generalSection);

        QString modeFlags;
        if (mameStartMatch.hasMatch()) {
            QString startCommand = mameStartMatch.captured(0);
            // Extract the part after S6x (or any S command)
            QRegularExpression flagsRegex(R"(S\d+(x.+)(?=\s|,|$))");
            auto flagsMatch = flagsRegex.match(startCommand);
            if (flagsMatch.hasMatch()) {
                modeFlags = flagsMatch.captured(1);
                // Remove the leading 'x' if present
                if (modeFlags.startsWith('x')) {
                    modeFlags.remove(0, 1);
                }
                qDebug() << "Extracted mode flags from MameStart:" << modeFlags;
            }
        }

        // Device Output Mode (M0)
        // Match M0x0, M0x1, M0x1L, M0x2 etc. allowing for optional spaces and other characters around it
        QRegularExpression deviceModeRegex(R"((?:^|x)M0x(\d+L?)(?:$|x))");
        auto deviceMatch = deviceModeRegex.match(modeFlags.isEmpty() ? generalSection : modeFlags);
        if (deviceMatch.hasMatch()) {
            QString value = deviceMatch.captured(1);
            QString fullMatch = "M0x" + value;
            qDebug() << "Found Device Mode:" << fullMatch;


            if (value == "0") {
                ui->DeviceOutputMode->setCurrentText("M0x0 - Mouse & Keyboard");
            } else if (value == "1") {
                ui->DeviceOutputMode->setCurrentText("M0x1 - Gamepad, w/ Camera mapped to Right Stick");
            } else if (value == "1L") {
                ui->DeviceOutputMode->setCurrentText("M0x1L - Maps Camera to Left Stick instead (OpenFIRE exclusive)");
            } else if (value == "2") {
                ui->DeviceOutputMode->setCurrentText("M0x2 - 'Hybrid'");
            } else {
                ui->DeviceOutputMode->setCurrentIndex(0); // Default to first item
            }
        } else {
            qDebug() << "Device Mode not found";
            ui->DeviceOutputMode->setCurrentIndex(0); // Default to first item
        }

        // Offscreen Firing Mode (M1)
        QRegularExpression offscreenRegex(R"((?:^|x)M1x(\d+)(?:$|x))");
        auto offscreenMatch = offscreenRegex.match(modeFlags.isEmpty() ? generalSection : modeFlags);
        if (offscreenMatch.hasMatch()) {
            QString value = offscreenMatch.captured(1);
            QString fullMatch = "M1x" + value;
            qDebug() << "Found Offscreen Mode:" << fullMatch;

            if (value == "0") {
                ui->OffscreenFiringMode->setCurrentText("M1x0 - Disabled (not used in OpenFIRE)");
            } else if (value == "1") {
                ui->OffscreenFiringMode->setCurrentText("M1x1 - Fire in bottom-left corner (not used in OpenFIRE)");
            } else if (value == "2") {
                ui->OffscreenFiringMode->setCurrentText("M1x2 - Offscreen Button Mode enabled (i.e. offscreen trigger pulls generates a Right Click instead of a Left Click)");
            } else if (value == "3") {
                ui->OffscreenFiringMode->setCurrentText("M1x3 - Normal shot (always on when Offscreen Button Mode isn't set in OpenFIRE)");
            } else {
                ui->OffscreenFiringMode->setCurrentIndex(0); // Default to first item
            }
        } else {
            qDebug() << "Offscreen Mode not found";
            ui->OffscreenFiringMode->setCurrentIndex(0); // Default to first item
        }

        // Pedal Mapping (M2)
        QRegularExpression pedalRegex(R"((?:^|x)M2x(\d+)(?:$|x))");
        auto pedalMatch = pedalRegex.match(modeFlags.isEmpty() ? generalSection : modeFlags);
        if (pedalMatch.hasMatch()) {
            QString value = pedalMatch.captured(1);
            QString fullMatch = "M2x" + value;
            qDebug() << "Found Pedal Mode:" << fullMatch;

            if (value == "0") {
                ui->PedalMapping->setCurrentText("M2x0 - Separate Button (as mapped)");
            } else if (value == "1") {
                ui->PedalMapping->setCurrentText("M2x1 - As Right Mouse");
            } else if (value == "2") {
                ui->PedalMapping->setCurrentText("M2x2 - As Middle Mouse (OpenFIRE exclusive)");
            } else {
                ui->PedalMapping->setCurrentIndex(0); // Default to first item
            }
        } else {
            qDebug() << "Pedal Mode not found";
            ui->PedalMapping->setCurrentIndex(0); // Default to first item
        }

        // Aspect Ratio Correction (M3)
        QRegularExpression aspectRegex(R"((?:^|x)M3x(\d+)(?:$|x))");
        auto aspectMatch = aspectRegex.match(modeFlags.isEmpty() ? generalSection : modeFlags);
        if (aspectMatch.hasMatch()) {
            QString value = aspectMatch.captured(1);
            QString fullMatch = "M3x" + value;
            qDebug() << "Found Aspect Ratio Mode:" << fullMatch;

            if (value == "0") {
                ui->AspectRatioCorrection->setCurrentText("M3x0 - Fullscreen");
            } else if (value == "1") {
                ui->AspectRatioCorrection->setCurrentText("M3x1 - 4:3 Correction");
            } else {
                ui->AspectRatioCorrection->setCurrentIndex(0); // Default to first item
            }
        } else {
            qDebug() << "Aspect Ratio Mode not found";
            ui->AspectRatioCorrection->setCurrentIndex(0); // Default to first item
        }

        // Rumble Only Mode (M6)
        QRegularExpression rumbleRegex(R"((?:^|x)M6x(\d+)(?:$|x))");
        auto rumbleMatch = rumbleRegex.match(modeFlags.isEmpty() ? generalSection : modeFlags);
        if (rumbleMatch.hasMatch()) {
            QString value = rumbleMatch.captured(1);
            QString fullMatch = "M6x" + value;
            qDebug() << "Found Rumble Only Mode:" << fullMatch;

            if (value == "0") {
                ui->RumbleOnlyMode->setCurrentText("M6x0 - Disabled (Solenoid allowed)");
            } else if (value == "1") {
                ui->RumbleOnlyMode->setCurrentText("M6x1 - Enabled (Solenoid disabled, Rumble enabled)");
            } else {
                ui->RumbleOnlyMode->setCurrentIndex(0); // Default to first item
            }
        } else {
            qDebug() << "Rumble Only Mode not found";
            ui->RumbleOnlyMode->setCurrentIndex(0); // Default to first item
        }

        // Auto Fire Mode (M8)
        QRegularExpression autoFireRegex(R"((?:^|x)M8x(\d+)(?:$|x))");
        auto autoFireMatch = autoFireRegex.match(modeFlags.isEmpty() ? generalSection : modeFlags);
        if (autoFireMatch.hasMatch()) {
            QString value = autoFireMatch.captured(1);
            QString fullMatch = "M8x" + value;
            qDebug() << "Found Auto Fire Mode:" << fullMatch;

            if (value == "0") {
                ui->AutoFireMode->setCurrentText("M8x0 - Disabled (sustained fire is kept enabled in OpenFIRE)");
            } else if (value == "1") {
                ui->AutoFireMode->setCurrentText("M8x1 - Auto fire on (enables Burst Fire in OpenFIRE)");
            } else if (value == "2") {
                ui->AutoFireMode->setCurrentText("M8x2 - Auto fire always on rapid fire");
            } else {
                ui->AutoFireMode->setCurrentIndex(0); // Default to first item
            }
        } else {
            ui->AutoFireMode->setCurrentIndex(0); // Default to first item
        }

        // Display Mode (MD)
        QRegularExpression displayRegex(R"((?:^|x)MDx(\d+)(?:B)?(?:$|x))");
        auto displayMatch = displayRegex.match(modeFlags.isEmpty() ? generalSection : modeFlags);
        if (displayMatch.hasMatch()) {
            QString value = displayMatch.captured(1);
            QString fullCapture = displayMatch.captured(0);
            QString fullMatch = fullCapture.contains("B") ? "MDx" + value + "B" : "MDx" + value;
            qDebug() << "Found Display Mode:" << fullMatch;

            if (value == "1") {
                ui->DisplayMode->setCurrentText("MDx1 - Life Only");
            } else if (value == "2") {
                ui->DisplayMode->setCurrentText("MDx2 - Ammo Only");
            } else if (value == "3") {
                if (fullCapture.contains("MDx3B")) {
                    ui->DisplayMode->setCurrentText("MDx3B - Life Bar (Life Glyphs otherwise)");
                } else {
                    ui->DisplayMode->setCurrentText("MDx3 - Life & Ammo Splitscreen");
                }
            } else {
                ui->DisplayMode->setCurrentIndex(0); // Default to first item
            }
        } else {
            ui->DisplayMode->setCurrentIndex(0); // Default to first item
        }

        // Start Command (S)
        QRegularExpression startRegex(R"(MameStart\s*=.*?cmw\s+\d+\s+(S[0-6])(?:x|[\s,]))");
        auto startMatch = startRegex.match(iniContent);
        if (startMatch.hasMatch()) {
            QString value = startMatch.captured(1);
            qDebug() << "Updating Start Commands dropdown to:" << value;
            if (value == "S0") {
                ui->StartCommands->setCurrentText("S0 - Start with solenoid enabled");
            } else if (value == "S1") {
                ui->StartCommands->setCurrentText("S1 - Start with rumble enabled");
            } else if (value == "S2") {
                ui->StartCommands->setCurrentText("S2 - Start with the RED LED enabled");
            } else if (value == "S3") {
                ui->StartCommands->setCurrentText("S3 - Start with the GREEN LED enabled");
            } else if (value == "S4") {
                ui->StartCommands->setCurrentText("S4 - Start with the BLUE LED enabled");
            } else if (value == "S6") {
                ui->StartCommands->setCurrentText("S6 - Start with everything enabled");
            } else {
                ui->StartCommands->setCurrentIndex(5); // Default to S6
            }
            qDebug() << "Updating Start Commands dropdown to:" << ui->StartCommands->currentText();
        } else {
            ui->StartCommands->setCurrentIndex(5); // Default to S6
        }
    }

    // Detect the number of players in the INI file
    int playerCount = 0; // Default to 0 player, we'll set it to at least 1 later

    // First, check if there's an [Output] section
    int outputSectionPos = iniContent.indexOf("[Output]");
    if (outputSectionPos >= 0) {
        // Find the end of the Output section
        int nextSectionPos = iniContent.indexOf("[", outputSectionPos + 1);
        if (nextSectionPos < 0) nextSectionPos = iniContent.length();

        // Extract the Output section
        QString outputSection = iniContent.mid(outputSectionPos, nextSectionPos - outputSectionPos);

        // Check for player entries using various patterns
    QSet<int> foundPlayers;

        // Pattern for P1_Something, Player1_Something, etc.
        QRegularExpression playerPattern(R"(P(\d+)(?:_|\s*=|\s+))");
        auto playerMatches = playerPattern.globalMatch(outputSection);
        while (playerMatches.hasNext()) {
            auto match = playerMatches.next();
        bool ok;
        int player = match.captured(1).toInt(&ok);
        if (ok && player > 0) {
            foundPlayers.insert(player);
        }
    }

        // Pattern for Player1, Player 1, etc.
        QRegularExpression player2Pattern(R"(Player\s*(\d+)(?:_|\s*=|\s+))");
        auto player2Matches = player2Pattern.globalMatch(outputSection);
        while (player2Matches.hasNext()) {
            auto match = player2Matches.next();
            bool ok;
            int player = match.captured(1).toInt(&ok);
            if (ok && player > 0) {
                foundPlayers.insert(player);
            }
        }

        // Also check MameStart line for player indicators
        QRegularExpression mameStartRegex(R"(MameStart\s*=\s*(?:.*cmw\s+(\d+).*))");
        auto mameStartMatches = mameStartRegex.globalMatch(iniContent);
        while (mameStartMatches.hasNext()) {
            auto match = mameStartMatches.next();
        bool ok;
        int player = match.captured(1).toInt(&ok);
        if (ok && player > 0) {
            foundPlayers.insert(player);
        }
    }

        // If we found players, determine the max player count
    if (!foundPlayers.isEmpty()) {
        playerCount = *std::max_element(foundPlayers.begin(), foundPlayers.end());
        }
    }

    // Ensure at least 1 player
    playerCount = qMax(1, playerCount);

    qDebug() << "Detected" << playerCount << "players in the INI file";

    // Enable or disable player controls based on detected player count
    ui->P1Color->setEnabled(playerCount >= 1);
    ui->P2Color->setEnabled(playerCount >= 2);
    ui->P3Color->setEnabled(playerCount >= 3);
    ui->P4Color->setEnabled(playerCount >= 4);

    // Detect existing settings in the INI file
    QMap<QString, QSet<int>> settingsMap; // Maps setting name to player numbers

    // Scan for all settings in the [Output] section
    if (outputSectionPos >= 0) {
        int nextSectionPos = iniContent.indexOf("[", outputSectionPos + 1);
        if (nextSectionPos < 0) nextSectionPos = iniContent.length();

        QString outputSection = iniContent.mid(outputSectionPos, nextSectionPos - outputSectionPos);

        // Find all player-based settings (P1_Something, P2_Something, etc.)
        QRegularExpression settingPattern(R"(P(\d+)_(\w+)\s*=)");
        auto settingMatches = settingPattern.globalMatch(outputSection);

        while (settingMatches.hasNext()) {
            auto match = settingMatches.next();
            bool ok;
            int player = match.captured(1).toInt(&ok);
            if (ok && player > 0) {
                QString settingName = match.captured(2);
                if (!settingsMap.contains(settingName)) {
                    settingsMap[settingName] = QSet<int>();
                }
                settingsMap[settingName].insert(player);
            }
        }
    }

    // Check if we need to hide any existing settings or add new ones
    QStringList knownSettings = {"CtmRecoil", "Damaged", "Clip", "Ammo", "Life", "Credits"};

    // Hide/disable settings that don't exist in the INI file
    for (const QString& setting : knownSettings) {
        bool hasAnySetting = settingsMap.contains(setting);

        // If the setting exists for at least one player
        if (hasAnySetting) {
            // Check which specific UI component to update
            if (setting == "CtmRecoil") {
                ui->Recoil->setVisible(true);
                ui->Recoil_Text->setVisible(true);
                ui->Recoil_Label->setVisible(true);
            } else if (setting == "Damaged") {
                ui->Damaged->setVisible(true);
                ui->Damaged_Text->setVisible(true);
                ui->Damaged_Label->setVisible(true);
            } else if (setting == "Clip") {
                ui->Clip->setVisible(true);
                ui->Clip_Text->setVisible(true);
                ui->Clip_Label->setVisible(true);
            } else if (setting == "Ammo") {
                ui->Ammo->setVisible(true);
                ui->Ammo_Text->setVisible(true);
                ui->Ammo_Label->setVisible(true);
            } else if (setting == "Life") {
                ui->Life->setVisible(true);
                ui->Life_Text->setVisible(true);
                ui->Life_Label->setVisible(true);
            } else if (setting == "Credits") {
                ui->Credits->setVisible(true);
                ui->Credits_Text->setVisible(true);
                ui->Credits_Label->setVisible(true);
            }
        } else {
            // If the setting doesn't exist, hide the corresponding UI elements
            if (setting == "CtmRecoil") {
                ui->Recoil->setVisible(false);
                ui->Recoil_Text->setVisible(false);
                ui->Recoil_Label->setVisible(false);
            } else if (setting == "Damaged") {
                ui->Damaged->setVisible(false);
                ui->Damaged_Text->setVisible(false);
                ui->Damaged_Label->setVisible(false);
            } else if (setting == "Clip") {
                ui->Clip->setVisible(false);
                ui->Clip_Text->setVisible(false);
                ui->Clip_Label->setVisible(false);
            } else if (setting == "Ammo") {
                ui->Ammo->setVisible(false);
                ui->Ammo_Text->setVisible(false);
                ui->Ammo_Label->setVisible(false);
            } else if (setting == "Life") {
                ui->Life->setVisible(false);
                ui->Life_Text->setVisible(false);
                ui->Life_Label->setVisible(false);
            } else if (setting == "Credits") {
                ui->Credits->setVisible(false);
                ui->Credits_Text->setVisible(false);
                ui->Credits_Label->setVisible(false);
            }
        }
    }

    // Remove the custom field handling section
    // Check for custom settings that are not in our predefined list
    for (auto it = settingsMap.begin(); it != settingsMap.end(); ++it) {
        QString settingName = it.key();
        if (!knownSettings.contains(settingName) && settingName != "LmpStart") {
            // Only log that we found a custom setting
            qDebug() << "Found custom setting:" << settingName;
        }
    }

    // Extract specific parameters for the UI controls
    // Update Recoil settings if they're visible
    if (ui->Recoil->isVisible()) {
    auto recoilMatch = recoilRegex.match(iniContent);
    if (recoilMatch.hasMatch()) {
        QString value = recoilMatch.captured(1).trimmed();
        ui->Recoil_Text->setPlainText(value);
        if (value == "F0x2x1") ui->Recoil->setCurrentText("Solenoid Single Pulse (recommended)");
        else if (value == "F0x%s%") ui->Recoil->setCurrentText("Solenoid Switching");
        else if (value == "F1x2x1") ui->Recoil->setCurrentText("Rumble Single Pulse (recommended for rumble)");
        else if (value == "F1x%s%") ui->Recoil->setCurrentText("Rumble Switching");
        else { ui->Recoil->setCurrentText("Custom"); ui->Recoil_Text->setPlainText(value); }
    } else {
        ui->Recoil_Text->clear();
        ui->Recoil->setCurrentText("------");
        }
    }

    // Update Damaged settings if they're visible
    if (ui->Damaged->isVisible()) {
    auto damageMatch = damageRegex.match(iniContent);
    if (damageMatch.hasMatch()) {
        QString value = damageMatch.captured(1).trimmed();
        ui->Damaged_Text->setPlainText(value);
        if (value == "F1x2x1") ui->Damaged->setCurrentText("Rumble Single Pulse (recommended)");
        else if (value == "F1x%s%") ui->Damaged->setCurrentText("Rumble Switching");
        else { ui->Damaged->setCurrentText("Custom"); ui->Damaged_Text->setPlainText(value); }
    } else {
        ui->Damaged_Text->clear();
        ui->Damaged->setCurrentText("------");
        }
    }

    // Update Clip settings if they're visible
    if (ui->Clip->isVisible()) {
    auto clipMatch = clipRegex.match(iniContent);
    if (clipMatch.hasMatch()) {
        QString value = clipMatch.captured(1).trimmed();
        ui->Clip_Text->setPlainText(value);
        if (value == "XX") ui->Clip->setCurrentText("Red/Off");
        else { ui->Clip->setCurrentText("Custom"); ui->Clip_Text->setPlainText(value); }
    } else {
        ui->Clip_Text->clear();
        ui->Clip->setCurrentText("------");
        }
    }

    // Update Ammo settings if they're visible
    if (ui->Ammo->isVisible()) {
    auto ammoMatch = ammoRegex.match(iniContent);
    if (ammoMatch.hasMatch()) {
        QString value = ammoMatch.captured(1).trimmed();
        ui->Ammo_Text->setPlainText(value);
        if (value == "DFAx%s%") ui->Ammo->setCurrentText("OLED On");
        else { ui->Ammo->setCurrentText("Custom"); ui->Ammo_Text->setPlainText(value); }
    } else {
        ui->Ammo_Text->clear();
        ui->Ammo->setCurrentText("------");
        }
    }

    // Update Life settings if they're visible
    if (ui->Life->isVisible()) {
    auto lifeMatch = lifeRegex.match(iniContent);
    if (lifeMatch.hasMatch()) {
        QString value = lifeMatch.captured(1).trimmed();
        ui->Life_Text->setPlainText(value);
        if (value == "XX") ui->Life->setCurrentText("OLED On");
        else { ui->Life->setCurrentText("Custom"); ui->Life_Text->setPlainText(value); }
    } else {
        ui->Life_Text->clear();
        ui->Life->setCurrentText("------");
        }
    }

    // Update Credits settings
    auto creditsMatch = creditsRegex.match(iniContent);
    if (creditsMatch.hasMatch()) {
        QString value = creditsMatch.captured(1).trimmed();
        ui->Credits_Text->setPlainText(value);
        if (value == "XX") ui->Credits->setCurrentText("OLED On (coming soon)");
        else { ui->Credits->setCurrentText("Custom"); ui->Credits_Text->setPlainText(value); }
    } else {
        ui->Credits_Text->clear();
        ui->Credits->setCurrentText("------");
    }

    updateAllComboBoxes();
}

void MainWindow::updateTextBox(const QString &text) {
    Q_UNUSED(text); // Placeholder for text box updates.
}

void MainWindow::refreshIni() {
    QString romName = ui->romComboBox->currentText();
    if (!romName.isEmpty()) {
        // Reset UI state before reloading
        setupDefaultIni();

        // Load the INI for the selected ROM
        loadIniSettings(romName);
        updateAllComboBoxes();

        qDebug() << "INI and BAT files refreshed for ROM:" << romName;
    } else {
        qDebug() << "No ROM selected to refresh INI.";
    }
}

// Add this function after loadIniSettings

void MainWindow::updateAllComboBoxes() {
    // Make sure all dropdowns are properly set based on their current values
    // This helps ensure the UI state matches the INI file settings
    
    // Update the player color dropdowns from LmpStart commands in the INI
    if (hasLoadedIni && !originalIniContent.isEmpty()) {
        // Get the max player count
        int playerCount = 1;
        if (ui->P4Color->isEnabled()) playerCount = 4;
        else if (ui->P3Color->isEnabled()) playerCount = 3;
        else if (ui->P2Color->isEnabled()) playerCount = 2;
        
        for (int player = 1; player <= playerCount; player++) {
            QComboBox* playerColor = nullptr;
            if (player == 1) playerColor = ui->P1Color;
            else if (player == 2) playerColor = ui->P2Color;
            else if (player == 3) playerColor = ui->P3Color;
            else if (player == 4) playerColor = ui->P4Color;
            
            if (playerColor && playerColor->isEnabled()) {
                // Define regex pattern to extract RGB values from LmpStart command
                QRegularExpression lmpPattern(QString(R"(P%1_LmpStart\s*=\s*cmw\s+%1\s+F2x1x(\d+)xF3x1x(\d+)xF4x1x(\d+))").arg(player));
                auto lmpMatch = lmpPattern.match(originalIniContent);
                
                if (lmpMatch.hasMatch()) {
                    // Extract RGB values from the LmpStart command
                    int red = lmpMatch.captured(1).toInt();
                    int green = lmpMatch.captured(2).toInt();
                    int blue = lmpMatch.captured(3).toInt();
                    QColor color(red, green, blue);
                    
                    qDebug() << QString("Found Player %1 color: RGB(%2,%3,%4)").arg(player).arg(red).arg(green).arg(blue);
                    
                    // Find the matching color in the dropdown
                    bool foundColor = false;
                    for (int i = 0; i < playerColor->count(); i++) {
                        QColor itemColor = playerColor->itemData(i).value<QColor>();
                        if (itemColor.red() == red && itemColor.green() == green && itemColor.blue() == blue) {
                            playerColor->setCurrentIndex(i);
                            foundColor = true;
                            break;
                        }
                    }
                    
                    // If exact color not found, set to closest color or add a custom color option
                    if (!foundColor && playerColor->count() > 0) {
                        // Add custom color option if it doesn't exactly match
                        // Use the first entry for now (default "------" option)
                        playerColor->setCurrentIndex(0);
                    }
                } else {
                    // If no LmpStart command found, set to default (first item in dropdown)
                    playerColor->setCurrentIndex(0);
                }
            }
        }
    }
    
    // Manually trigger update signals for all relevant combo boxes
    // Device Output Mode
    QString deviceText = ui->DeviceOutputMode->currentText();
    if (deviceText.contains("M0x")) {
        qDebug() << "Updating Device Mode dropdown to:" << deviceText;
        
        // Select an item if it's not already selected
        for (int i = 0; i < ui->DeviceOutputMode->count(); i++) {
            if (ui->DeviceOutputMode->itemText(i).contains(deviceText.left(4))) {
                ui->DeviceOutputMode->setCurrentIndex(i);
                break;
            }
        }
    }
    
    // Offscreen Firing Mode
    QString offscreenText = ui->OffscreenFiringMode->currentText();
    if (offscreenText.contains("M1x")) {
        qDebug() << "Updating Offscreen Mode dropdown to:" << offscreenText;
        
        // Select an item if it's not already selected
        for (int i = 0; i < ui->OffscreenFiringMode->count(); i++) {
            if (ui->OffscreenFiringMode->itemText(i).contains(offscreenText.left(4))) {
                ui->OffscreenFiringMode->setCurrentIndex(i);
                break;
            }
        }
    }
    
    // Pedal Mapping
    QString pedalText = ui->PedalMapping->currentText();
    if (pedalText.contains("M2x")) {
        qDebug() << "Updating Pedal Mode dropdown to:" << pedalText;
        
        // Select an item if it's not already selected
        for (int i = 0; i < ui->PedalMapping->count(); i++) {
            if (ui->PedalMapping->itemText(i).contains(pedalText.left(4))) {
                ui->PedalMapping->setCurrentIndex(i);
                break;
            }
        }
    }
    
    // Aspect Ratio Correction
    QString aspectText = ui->AspectRatioCorrection->currentText();
    if (aspectText.contains("M3x")) {
        qDebug() << "Updating Aspect Ratio dropdown to:" << aspectText;
        
        // Select an item if it's not already selected
        for (int i = 0; i < ui->AspectRatioCorrection->count(); i++) {
            if (ui->AspectRatioCorrection->itemText(i).contains(aspectText.left(4))) {
                ui->AspectRatioCorrection->setCurrentIndex(i);
                break;
            }
        }
    }
    
    // Rumble Only Mode
    QString rumbleText = ui->RumbleOnlyMode->currentText();
    if (rumbleText.contains("M6x")) {
        qDebug() << "Updating Rumble Mode dropdown to:" << rumbleText;
        
        // Select an item if it's not already selected
        for (int i = 0; i < ui->RumbleOnlyMode->count(); i++) {
            if (ui->RumbleOnlyMode->itemText(i).contains(rumbleText.left(4))) {
                ui->RumbleOnlyMode->setCurrentIndex(i);
                break;
            }
        }
    }
    
    // Auto Fire Mode
    QString autoFireText = ui->AutoFireMode->currentText();
    if (autoFireText.contains("M8x")) {
        qDebug() << "Updating Auto Fire Mode dropdown to:" << autoFireText;
        
        // Select an item if it's not already selected
        for (int i = 0; i < ui->AutoFireMode->count(); i++) {
            if (ui->AutoFireMode->itemText(i).contains(autoFireText.left(4))) {
                ui->AutoFireMode->setCurrentIndex(i);
                break;
            }
        }
    }
    
    // Display Mode
    QString displayText = ui->DisplayMode->currentText();
    if (displayText.contains("MDx")) {
        qDebug() << "Updating Display Mode dropdown to:" << displayText;
        
        // Select an item if it's not already selected
        for (int i = 0; i < ui->DisplayMode->count(); i++) {
            if (ui->DisplayMode->itemText(i).contains(displayText.left(5))) {
                ui->DisplayMode->setCurrentIndex(i);
                break;
            }
        }
    }
    
    // Start Commands
    QString startText = ui->StartCommands->currentText();
    if (startText.contains("S")) {
        qDebug() << "Updating Start Commands dropdown to:" << startText;
        
        // Get just the S command part (S0, S1, etc.)
        QString sCommand = "";
        for (int i = 0; i < startText.length(); i++) {
            if (startText[i] == 'S' && i+1 < startText.length() && startText[i+1].isDigit()) {
                sCommand = startText.mid(i, 2);
                break;
            }
        }
        
        // Select an item if it's not already selected
        for (int i = 0; i < ui->StartCommands->count(); i++) {
            if (ui->StartCommands->itemText(i).contains(sCommand)) {
                ui->StartCommands->setCurrentIndex(i);
                break;
            }
        }
    }
}

void MainWindow::updateBatCommandLine() {
    QString emulatorFriendly = ui->emulatorComboBox->currentText();
    QString emulator = emulatorFriendly;
    QString demulShooterExe;
    EmulatorUtils::mapEmulator(emulator, demulShooterExe);

    QString rom = ui->romComboBox->currentText();
    QString rom2 = EmulatorUtils::mapRom(rom);
    QString demulShooterPath = ui->demulShooterPathLineEdit->text();
    QString args = ui->demulShooterArgsLineEdit->text().trimmed();

    QString line = QString("start \"Demul\" \"%1/%2\" -target=%3 -rom=%4")
                       .arg(QDir::toNativeSeparators(demulShooterPath), demulShooterExe, emulator, rom2);
    if (!args.isEmpty()) line += " " + args;

    QStringList lines = ui->plainTextEdit_Bat->toPlainText().split('\n');
    if (!lines.isEmpty()) {
        lines[0] = line;
        ui->plainTextEdit_Bat->setPlainText(lines.join('\n'));
    }
}

// Add this new function after initializeUI
void MainWindow::showTextEditorContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Insert Value"), this);
    
    // Add common INI values
    QAction *actionF0x2x1 = contextMenu.addAction("F0x2x1 (Solenoid Single Pulse)");
    QAction *actionF0xsx = contextMenu.addAction("F0x%s% (Solenoid Switching)");
    QAction *actionF1x2x1 = contextMenu.addAction("F1x2x1 (Rumble Single Pulse)");
    QAction *actionF1xsx = contextMenu.addAction("F1x%s% (Rumble Switching)");
    QAction *actionXX = contextMenu.addAction("XX (LED On/Off)");
    QAction *actionDFAxsx = contextMenu.addAction("DFAx%s% (OLED Display)");
    
    // Add separator
    contextMenu.addSeparator();
    
    // Add player-specific values
    QAction *actionP1 = contextMenu.addAction("P1_ (Player 1)");
    QAction *actionP2 = contextMenu.addAction("P2_ (Player 2)");
    QAction *actionP3 = contextMenu.addAction("P3_ (Player 3)");
    QAction *actionP4 = contextMenu.addAction("P4_ (Player 4)");
    
    // Add separator
    contextMenu.addSeparator();
    
    // Add common commands
    QAction *actionCmw = contextMenu.addAction("cmw (Command)");
    QAction *actionEquals = contextMenu.addAction("= (Equals)");
    
    // Execute the context menu
    QAction *selectedAction = contextMenu.exec(ui->plainTextEdit_Generic->mapToGlobal(pos));
    
    if (selectedAction) {
        QString textToInsert;
        if (selectedAction == actionF0x2x1) textToInsert = "F0x2x1";
        else if (selectedAction == actionF0xsx) textToInsert = "F0x%s%";
        else if (selectedAction == actionF1x2x1) textToInsert = "F1x2x1";
        else if (selectedAction == actionF1xsx) textToInsert = "F1x%s%";
        else if (selectedAction == actionXX) textToInsert = "XX";
        else if (selectedAction == actionDFAxsx) textToInsert = "DFAx%s%";
        else if (selectedAction == actionP1) textToInsert = "P1_";
        else if (selectedAction == actionP2) textToInsert = "P2_";
        else if (selectedAction == actionP3) textToInsert = "P3_";
        else if (selectedAction == actionP4) textToInsert = "P4_";
        else if (selectedAction == actionCmw) textToInsert = "cmw";
        else if (selectedAction == actionEquals) textToInsert = "=";
        
        // Insert the text at the cursor position
        QTextCursor cursor = ui->plainTextEdit_Generic->textCursor();
        cursor.insertText(textToInsert);
    }
}

// Add the launchGame implementation at the end of the file
void MainWindow::launchGame()
{
    // First export the files
    if (!exportFiles(false)) {
        QMessageBox::warning(this, "Export Error",
                             "Failed to export files; game launch aborted.");
        qWarning() << "Export failed; game launch aborted.";
        return;
    }

    // Get the current ROM name and create the BAT file path
    QString rom = ui->romComboBox->currentText();
    QString rom2 = EmulatorUtils::mapRom(rom);
    QString qmamehookerPath = ui->qmamehookerPathLineEdit->text();
    QString batFilePath = qmamehookerPath + "/bat/" + rom + ".bat";

    // Launch the BAT file
    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(QFileInfo(batFilePath).absolutePath());
    
    #ifdef Q_OS_WIN
    // On Windows, use cmd.exe to run the batch file
    process->start("cmd.exe", QStringList() << "/c" << QDir::toNativeSeparators(batFilePath));
    #else
    // On other platforms, make the file executable and run it directly
    QFile::setPermissions(batFilePath, QFile::permissions(batFilePath) | QFile::ExeUser);
    process->start("/bin/sh", QStringList() << QDir::toNativeSeparators(batFilePath));
    #endif

    // Connect the finished signal to clean up the process
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            process, &QProcess::deleteLater);

    // Handle any errors
    connect(process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        QMessageBox::warning(this, "Launch Error",
                           QString("Failed to launch the game: %1").arg(error));
    });
}
