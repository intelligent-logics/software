# To check which USB port the DE10-nano is plugged into, run:
# /home/jonas/intelFPGA_lite/20.1/quartus/bin/quartus_pgm --auto
#
# Should be something like:
# 
# 1) DE-SoC [3-1]

import os
import subprocess
import curses

QUARTUSPATH = "/home/jonas/intelFPGA_lite/20.1/quartus/bin"
GAMEPATH = "/home/jonas/CODE/NES_Games/"
DE10NANOUSBPORT = "DE-SoC [3-1]"

games_list = os.listdir(GAMEPATH)

print('Available NES games to play:')
print()

for x in games_list:
    print(x)


def string_selector(strings):
    def main(stdscr):
        curses.curs_set(0)
        current_row = 0


        while True:
            stdscr.clear()
                         
            height,width = stdscr.getmaxyx()
            x = width//2 - 1
            y = height//2 
            stdscr.addstr(y,x,"{Available Games}")

            for idx, string in enumerate(strings):
                string = string[:-4]
                x = width//2 - len(strings)//2
                y = height//2 - len(strings)//2 + idx + 2

                if idx == current_row:
                    stdscr.attron(curses.A_REVERSE)
                    stdscr.addstr(y,x,string)
                    stdscr.attroff(curses.A_REVERSE)
                else:
                    stdscr.addstr(y,x,string)

            key = stdscr.getch()
            if key == ord('q') or key == 27:
                return None
            elif key == curses.KEY_UP and current_row > 0:
                current_row -=1
            elif key == curses.KEY_DOWN and current_row < len(strings) - 1:
                current_row +=1
            elif key in [curses.KEY_ENTER, ord('\n')]:
                selected_game = strings[current_row]
                cmd = f'sudo box64 {QUARTUSPATH}/quartus_pgm -c "{DE10NANOUSBPORT}" -m JTAG -o "P;{GAMEPATH}{selected_game}@2"'
                # subprocess.run(cmd, stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                os.system(cmd + "/dev/null 2>&1")

                return strings[current_row]
            

    return curses.wrapper(main)
 
while True:
    string_selector(games_list)
    # curses.curs_set(1)