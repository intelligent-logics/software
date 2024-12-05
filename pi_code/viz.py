import plotext as plt
import pandas as pd
import time
from collections import Counter
import os
import numpy as np

def visualize_instructions():
    while True:
        
        data = pd.read_csv('data.csv', dtype={'instruction': str})

        instruction_counts = Counter(data['instruction'])

        
        instructions = [str(x) for x in list(instruction_counts.keys())]
        counts = [np.log(int(x)) for x in list(instruction_counts.values())]

        # Clear the screen
        #os.system('cls' if os.name == 'nt' else 'clear')
#        plt.clt()
        plt.cld()
        plt.clt()
      
        plt.title("Instruction Types")
        plt.bar(instructions, counts, orientation='horizontal')
        
        plt.show()

     
        time.sleep(0.5)

visualize_instructions()
