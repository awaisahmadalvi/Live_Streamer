#!/usr/bin/env python2
import sys

print sys.argv
mylist = ["spam", "ham", "eggs"]
print ", ".join(mylist)

def modify_string(original):
	original += " that has been modified."
	return original

int_condition = raw_input("How manyintegers? ")

try:
	int_condition = int(int_condition)
except ValueError:
	sys.exit("You must enter an integer")

ints = list()
count = 0

while count < int_condition:
	new_int = raw_input("Please enter integer {0}: ".format(count + 1))
	isint = False
	try:
		new_int = int(new_int)
		isint = True
	except:
		print("You must enter an integer")
	if isint == True:
		ints.append(new_int)
		count += 1
str1 = "Original "
print(str1)
print(modify_string(str1))

print("Using a for loop")
for value in ints:
	print(str(value))

print("Using a while loop")
total = len(ints)
count = 0
while count < total:
	print(str(ints[count]))
	count += 1

if int_condition < 6:
	sys.exit("int_condition must be >= 6")
else:
	print("int_condition was >= 6 - continuing")


hello_str = "Hello World"
hello_int = 21
hello_bool = True
hello_tuple = (21, 32)
hello_list = ["Hello,", "this", "is", "a", "list"]

hello_list = list()
hello_list.append("Hello,")
hello_list.append("this")
hello_list.append("is")
hello_list.append("a")
hello_list.append("list")
hello_list[4] = hello_list[4]+"ASD"
