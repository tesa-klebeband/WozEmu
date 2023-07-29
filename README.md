# WozEmu
An Apple-I emulator written in c++
## Building
### Requirements
* g++
* make

To build WozEmu navigate to the root of this project and run `make`. All output files will be stored under the `build/` directory.
## Using WozEmu
`build/wozemu`

WozEmu uses the console in order to emulate a virtual Display and Keyboard. Upon executing WozEmu, the typical '\' from the Apple-I computer will appear on your console indicating that wozmon is ready to take commands.

To open the WozEmu menu, press **^C**.
| Key | Command                                    |
| --- | ------------------------------------------ |
| q   | Exit WozEmu                                |
| x   | Close the menu                             |
| r   | Reset the emulated CPU                     |
| p   | Power-cycle the emulated Apple-I           |
| l   | Load a raw binary file at a memory address |
| c   | Clear the screen                           |

## License
All files within this repo are released under the GNU GPL V3 License as per the LICENSE file stored in the root of this repo.
