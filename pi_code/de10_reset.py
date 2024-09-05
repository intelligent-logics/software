# gameTitle = input("Sof name: ")

gameTitle = "fpganes"
game_path = f"E:\GHRepos\software\pi_code\autoquartus\{gameTitle}.sof@2"
cliCommand = fr"quartus_pgm -c 'DE-SoC [USB-1]' -m JTAG -o p;{game_path}"
print(cliCommand)