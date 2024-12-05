
import csv
import random
import time

x_value = 0
total_1 = 1000
total_2 = 1000

fieldnames = ["instruction","x_value", "pin1", "pin2", "pin3", "pin4", "clock"]


with open('data.csv', 'w') as csv_file:
	csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
	csv_writer.writeheader()



pin1 = random.randint(0, 1)
pin2 = random.randint(0, 1)
pin3 = random.randint(0, 1)
pin4 = random.randint(0, 1)
clock = random.randint(0, 1)
while True:

	with open('data.csv', 'a') as csv_file:
		csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)

		info = {
		"instruction": f"{pin1}{pin2}{pin3}{pin4}",
		"x_value" : x_value,
		"pin1": pin1,
		"pin2": pin2,
		"pin3": pin3,
		"pin4": pin4,
		"clock": clock
		}

		csv_writer.writerow(info)

		x_value += 1
		pin1 = random.randint(0, 1)
		pin2 = random.randint(0, 1)
		pin3 = random.randint(0, 1)
		pin4 = random.randint(0, 1)
		clock = int(not clock)
	time.sleep(.1)
