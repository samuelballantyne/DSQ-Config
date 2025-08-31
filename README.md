# DSQ Config

DemulEasy is a straightforward, automated tool designed to simplify the configuration process for QMamehook and Demul Shooter on Windows. With an easy-to-use GUI, this tool lets you quickly set up emulator and game folder locations, adjust ini settings, and export the necessary bat and ini files—all in one go.

## Features

- **Automated Configuration:**  
  Easily specify your emulator folder, game folder, Demul Shooter, and QMamehook paths.

- **Customizable ini Settings:**  
  Fine-tune configuration parameters directly within the application using the provided text box.

- **Automated File Export:**  
  Export batch (bat) and ini files that work seamlessly with QMamehook and Demul Shooter.

- **User-Friendly GUI:**  
  Designed with simplicity in mind, DemulEasy offers an intuitive interface perfect for both beginners and advanced users.

- **Platform:**  
  Currently optimized for Windows, with potential plans for Linux compatibility in the future.

## Installation

1. **Download the Executable:**  
   Head over to the [Releases](#) page and download the latest version of DemulEasy (a ready-to-go .exe file).

2. **Run the Program:**  
   No need for additional libraries or dependencies—simply run the executable and you’re all set.

3. **Follow On-Screen Instructions:**  
   Configure your settings as prompted by the GUI, then export your files to the designated folders.

## Usage

1. **Launch DemulEasy.**

2. **Set Up Your Paths:**  
   - **Emulator Folder:** Specify where your emulator is located.  
   - **Game Folder:** Choose the folder for your game.  
   - **Demul Shooter Path:** Point to your Demul Shooter installation.  
   - **QMamehook Path:** Set the path for QMamehook.

3. **Adjust ini Settings:**  
   Use the provided text box to modify QMamehooks ini settings as needed. These will vary slightly from gun to gun but for OpenFIRE specifically here's a list of serial commands [MAMEHOOKER Documentation on Light Gun Serial Commands](https://github.com/TeamOpenFIRE/OpenFIRE-Firmware/wiki/MAMEHOOKER-Documentation#light-gun-serial-commands)
   
5. **Export Files:**  
   Click the export button to generate the bat and ini files, which will be saved to their respective folders.

## Future Plans

- **Linux Compatibility:**  
  While DemulEasy is currently Windows-only, there are plans to expand support to Linux.

- **Enhanced Features:**  
  Based on user feedback, future updates may include more advanced customization options and additional functionalities.

## Continuous Integration

A GitHub Actions workflow builds the project on Windows and packages the executable with its required Qt libraries. The workflow uploads a zipped artifact for download.

## Contributing

Contributions are welcome! If you’d like to improve DemulEasy, please feel free to fork the repository and submit a pull request.

## Contact

If you have any issues, suggestions, or questions, please open an issue on GitHub

---

Cheers, and happy gaming!
