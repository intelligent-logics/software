Last Updated: 11/20/2024

Completed work:
	Stress Test ROM
		- Sound, Movement Test: Mario Jumping when 'A' is clicked
		- PPU Test: Click on Albert to change Color Palettes
		- Accumulator Test: Calculator, Register Status Flags 
		- Benchmark TesT: Calculates amount of cycles for common NES Instructions
		

Architecture:
	Right now, the bootloader menu runs on the MiSTer. The NES core can be flashed onto boards,
	but that is as far as we are in hardware. The microcontroller aspect has only recently begun
	development. So our total progress in the pre-alpha build is a working ROM on both FCEUX and
	the MiSTer. 


Run make to build.
Builds the NES ROM along with the .s file that generates from the C code.
EMS.C has been updated with better comments

Open ems.nes in FCEUX or on MiSTER core itself



1. Do all of this on WSL
2. Makesure you have CC65 installed.
3. Run make