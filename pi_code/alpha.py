
import RPi.GPIO as GPIO
import time
GPIO.cleanup()
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(True)
PIN1=8
PIN2=10
GPIO.setup(PIN1, GPIO.IN)
GPIO.setup(PIN2, GPIO.IN)



import os
import subprocess

import time


#  /home/jonas/intelFPGA_lite/20.1/quartus/bin/quartus_pgm -l to find the  



QPATH = "/home/jonas/intelFPGA_lite/20.1/quartus/bin"
GAMEPATH = "/home/jonas/CODE/NES_Games/"

games_list = os.listdir(GAMEPATH)
cable_name = "DE-SoC [1-2]"
def flash_game(game):
	cmd = f'sudo box64 {QPATH}/quartus_pgm -c "{cable_name}" -m JTAG -o "P;{GAMEPATH}{game}@2"'
	os.system(cmd + " > /dev/null")

print(games_list)
try:
	while True:
		if GPIO.input(PIN1):
			selected_game = games_list[0]
			print(selected_game)
			flash_game(selected_game)
			time.sleep(0.1)
		elif GPIO.input(PIN2):
			selected_game = games_list[1]
			print(selected_game)
			flash_game(selected_game)
			time.sleep(0.1)
		time.sleep(0.05)
	
except KeyboardInterrupt:
	    print('Interrupted')
