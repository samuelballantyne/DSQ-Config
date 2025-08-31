#ifndef EMULATORUTILS_H
#define EMULATORUTILS_H

#include <QString>
#include <QStringList>
#include <QComboBox>

class EmulatorUtils
{
public:
    EmulatorUtils();
    
    // Utility functions moved from MainWindow
    static void updateEmulatorPath(const QString &emulator, QString &emulatorPath, QString &romPath);
    static void updateGamesList(const QString &emulator, QComboBox *romComboBox);
    static QString mapRom(const QString &rom);
    static void mapEmulator(QString &emulator, QString &demulShooterExe);
    static void setupEmulatorComboBox(QComboBox *emulatorComboBox);

private:
    // Any private constants/methods if needed
};

#endif // EMULATORUTILS_H 