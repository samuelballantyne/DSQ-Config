#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QRegularExpression>
#include <QColor>
#include <QStringList>
#include <QPoint>
#include "emulatorutils.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    bool exportFiles(bool showMessage = true);
    void updateGamesList();
    void browseQmamehookerPath();
    void browseEmulatorPath();
    void browseRomPath();
    void browseDemulPath();
    void updateEmulatorPath();
    void updateIniText();
    void loadIniSettings(const QString &romName);
    void updateTextBox(const QString &text);
    void refreshIni();
    void showTextEditorContextMenu(const QPoint &pos);
    void launchGame();

private:
    Ui::MainWindow *ui;

    QString originalIniContent; // Stores the original structure of loaded INI files
    bool isLoadingIni = false; // Flag to indicate we're in the process of loading an INI file
    bool hasLoadedIni = false; // Flag to indicate if we're working with a loaded INI file
    static const QRegularExpression recoilRegex;
    static const QRegularExpression damageRegex;
    static const QRegularExpression clipRegex;
    static const QRegularExpression ammoRegex;
    static const QRegularExpression lifeRegex;
    static const QRegularExpression creditsRegex;

    // Helper methods to reduce duplicate code.
    void prepareDirectories(const QString &basePath, QDir &iniDir, QDir &batDir);
    void mapEmulator(QString &emulator, QString &demulShooterExe);
    QString mapRom(const QString &rom);
    void createFiles(const QString &rom,
                     const QString &emulatorInput,
                     QString demulShooterExeInput,
                     const QString &emulatorPath,
                     const QString &romPath,
                     const QString &qmamehookerPath,
                     const QString &demulShooterPath,
                     const QString &verbose,
                     const QString &iniContent);
    void updateLmpStartValue(int player, const QColor &color);
    bool safeReplaceLine(QStringList &lines, const QString &key, const QString &newLine);

    // New UI initialization helper methods.
    void initializeUI();
    void setupEmulatorComboBox();
    void setupComboBoxStyles();
    void setupOtherUIFields();
    void setupColorComboBoxes();
    void setupSignalConnections();
    void setupDefaultIni();
    
    // New helper function to ensure comboboxes are properly set
    void updateAllComboBoxes();
};

#endif // MAINWINDOW_H
