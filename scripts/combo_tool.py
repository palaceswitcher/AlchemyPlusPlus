#!/usr/bin/env python3
#Alchemy++ Combo Editor by PalaceSwitcher
import json
import typing
import sys
import os

# OS checking
clear_cmd = "clear" #Default UNIX clear command
if os.name == "nt":
	clear_cmd = "cls" #Windows clear command

# Drag and drop functionality
try:
	filename = sys.argv[1]
except IndexError:
	filename = input("Filename: ")

# File error handling
while True:
	try:
		f = open(filename, "r") #Get file
		break
	except FileNotFoundError as not_found:
		print("File \""+not_found.filename+"\" not found!")
		filename = input("Filename: ")

json_data = f.read() #Load combo file

# JSON Validation
combo_dict = {}
try:
	combo_dict = json.loads(json_data) #Combo dictionary data
except ValueError as e:
	print("JSON is malformed.")

# Add element to the dictionary of element
def add_elem(elems: dict, elem: str):
	if elem not in elems:
		elems.update({elem:{"combos":{}}}) #Add new element if doesn't exist

# Add combo to an element's list of combos
def add_combo(combos: dict, elem: str, results: list[str]) -> dict:
	combos.update({elem:results}) #Add combo to element
	keys = list(combos.keys())
	keys.sort()
	sorted_combos = {i: combos[i] for i in keys} #Sort combos
	return sorted_combos

cur_elem_sel = "air" #Currently selected element string ID
while True:
	os.system(clear_cmd)
	print("\033[1m"+cur_elem_sel.title().replace("_"," ")+" Combinations:\033[0m")

	elem = combo_dict[cur_elem_sel] #Get element
	elem_combos = elem["combos"] #Get element combos
	for combo in elem_combos:
		print(f"{cur_elem_sel} + {combo} = {", ".join(combo_dict[cur_elem_sel]["combos"][combo])}")

	new_elem = input("""
Type an element and its results to add to the currently selected element.
Commands:
\033[0;31m-c\033[0m   Change Element Selection
\033[0;31m-b\033[0m   Cancel Combo Creation
\033[0;31m-r\033[0m   Remove Combo
\033[0;31m-d\033[0m   Delete Element
\033[0;31m-s\033[0m   Save Changes
\033[0;31m-x\033[0m   Exit
Element/Selection: """).strip().replace(" ","_")
	# Change selection mode
	if new_elem == "-x":	break
	elif new_elem == "-b":	continue
	elif new_elem == "-s":
		open(filename, "w").write(json.dumps(combo_dict, sort_keys=True, indent=2))
	elif new_elem == "-c":
		new_elem_sel = input("Element: ").replace(" ","_")
		try:
			combo_dict[new_elem_sel]
			cur_elem_sel = new_elem_sel
		except Exception as e:
			if input("Not a valid element! Create it (y/n)? ").strip()[0] == "y":
				add_elem(combo_dict, new_elem_sel)
				cur_elem_sel = new_elem_sel
	else:
		result_elems = input("Results (comma separated)/Selection: ").replace(" ","_")
		if result_elems == "-b" or result_elems == "":	continue #Stop if user cancels selection
		# Remove combo
		elif new_elem == "-r":
			elem_combos.pop(result_elems, None)
			combo_dict[result_elems]["combos"].pop(cur_elem_sel, None)
			continue
		# Delete entire element
		elif new_elem == "-d":
			if result_elems == cur_elem_sel:
				cur_elem_sel = "air" #TODO GO TO PREVIOUS SELECTED ELEMENT INSTEAD
			combo_dict.pop(result_elems, None)
			continue
		add_elem(combo_dict, new_elem) #Add new element if doesn't exist and a command wasn't entered
		result_elems = result_elems.strip().split(",") #Get comma-separated list of new elements
		# Add entries for result elements
		for res_elem in result_elems:
			add_elem(combo_dict, res_elem)
		elem_combos.update({new_elem:result_elems}) #Update combos for element
		elem["combos"] = add_combo(elem_combos, new_elem, result_elems)
		combo_dict[new_elem]["combos"] = add_combo(combo_dict[new_elem]["combos"], cur_elem_sel, result_elems) #Update combos for other element
