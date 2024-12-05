# Intelligent-Logics

FolderWise breakdown v
## EMS, emstest
- code to compile NES Benchmark ROM and test it
- Import the ROM into an emulator like FCEUX, or follow your emulator 
  specific instruction to launch

## pi_code
- Code to record CPU instructions from Mister FPGA Board using GPIO
- Must use a terminal multiplexer, as read_instruct.py must be run while viz.py is running
- create a terminal session, run read_instruct.py, split off a new terminal window, run viz.py
- make_data.py provided to test that viz.py is working, generates fake data to graph

## All other folders
- contain a mix of deprecated work and unused features
