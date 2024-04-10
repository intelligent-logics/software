Completed work:
	Fully-functional fake menu.
	NES Core breakdown (compilation TBD)
	Understanding of MiSTer and how it relates to our own project.

Work to be done (semester 1):
	NES Core compiles
	NES Core runs a program and outputs video and audio
	Microcrontoller basic functionality (read and move SD card contents)
	Microcontroller intermediate functionality (indexing and hex dumping/flashing)

Architecture:
	Right now, the bootloader menu runs on the MiSTer. The NES core can be flashed onto boards,
	but that is as far as we are in hardware. The microcontroller aspect has only recently begun
	development. So our total progress in the pre-alpha build is a working ROM on both FCEUX and
	the MiSTer. 

Known Bugs:
	Compilation errors of newest NES core used (Xilinx). Declaration problems with includes.
	Bootloader has no known bugs as of the latest build. Any flaws right now are design choices and subjective.
	


Run make to build.
Builds the NES ROM along with the .s file that generates from the C code.
EMS.C has been updated with better comments


1. Do all of this on WSL
2. Makesure you have CC65 installed.
3. Run make