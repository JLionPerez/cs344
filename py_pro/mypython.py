# Title: Program Py - Python Exploration
# Description: A program that generates random numbers and letters into three different files
# Author: Joelle Perez
# Date: 11 November 2019

import random
import string

# Function name: create_files()
# Purpose: Creates three files with random lowercase strings of length 10 with a newline at the end
# Arguments: none
# Returns: none
# cite: https://www.guru99.com/reading-and-writing-files-in-python.html
# cite: https://pynative.com/python-generate-random-string/
def create_files():
	for x in range(3):
		f= open("egg_%d" %x, "w+")
		rand_str = ''.join([random.choice(string.ascii_lowercase) for n in range(10)])
		f.write("%s\n" %rand_str)
		f.close()

# Function name: read_files()
# Purpose: Reads and prints out string for each file
# Arguments: none
# Returns: none
# cite: https://www.afternerd.com/blog/how-to-print-without-a-newline-in-python/#python3
def read_files():
	for x in range(3):
		f= open("egg_%d" %x, "r")
		if f.mode == "r":
			file_str = f.readlines()
			for x in file_str:
				print(x, end = '')

# Function name: rand_num()
# Purpose: Randomly generates two numbers to be outputted and multiplied
# Arguments: none
# Returns: none
# cite: https://www.geeksforgeeks.org/python-randint-function/
def rand_num():
	num1 = random.randint(1, 42)
	num2 = random.randint(1, 42)
	product = num1 * num2
	print("%d\n%d\n%d" %(num1,num2,product))

# cite: https://www.guru99.com/learn-python-main-function-with-examples-understand-main.html
if __name__ == "__main__":
	create_files()
	read_files()
	rand_num()