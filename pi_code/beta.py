
import os
import subprocess
import curses
import gpiod
import time



QPATH = "/home/jonas/intelFPGA_lite/20.1/quartus/bin"
GAMEPATH = "/home/jonas/CODE/NES_Games/"

games_list = os.listdir(GAMEPATH)

#  /home/jonas/intelFPGA_lite/20.1/quartus/bin/quartus_pgm -l to find the  


	selected_game = games_list[0]
	cmd = f'sudo box64 {QPATH}/quartus_pgm -c "DE-SoC [3-2]" -m JTAG -o "P;{GAMEPATH}{selected_game}@2"'
	# 	subprocess.run(cmd, stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
	os.system(cmd + "/dev/null 2>&1")


