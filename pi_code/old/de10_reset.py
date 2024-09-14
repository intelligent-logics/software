# quartus command (from Patrick's PC):
# quartus_pgm -c "DE-SoC [USB-1]" -m JTAG -o p;E:\GHRepos\software\pi_code\autoquartus\fpganes.sof@2
# 
# quartus command (from Jonas's PC):
# quartus_pgm -c "DE-SoC [USB-1]" -m JTAG -o p;C:\Users\jonas\Documents\GitHub-Repos\software\pi_code\autoquartus\fpganes.sof@2
# 
# quartus command (from Raspberry Pi):
# quartus_pgm -c "DE-SoC [USB-1]" -m JTAG -o p;/home/intelligentlogics/CODE/sofFiles/fpganes.sof@2

# gameTitle = input("Sof name: ")

import os

gameTitle = "fpganes"
# game_path = f"/home/intelligentlogics/CODE/sofFiles/{gameTitle}.sof@2"
game_path = f"C:\\Users\\jonas\\Documents\\GitHub-Repos\\software\\pi_code\\autoquartus\\{gameTitle}.sof@2"
cliCommand = fr"quartus_pgm -c 'DE-SoC [USB-1]' -m JTAG -o p;{game_path}"
# print(cliCommand)
os.system(cliCommand)