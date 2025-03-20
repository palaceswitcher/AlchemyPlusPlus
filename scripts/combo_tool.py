#!/usr/bin/env python3
#Alchemy++ Combo Editor by PalaceSwitcher
import json
import typing
import sys
import os

# Set to True if your terminal does not support ANSI formatting codes
DISABLE_ANSI_CODES = False

# Text formatting
if DISABLE_ANSI_CODES:
	TXT_FORMAT_END = ""
	TXT_BOLD = ""
	TXT_GREEN = ""
	TXT_BLUE = ""
else:
	TXT_FORMAT_END = "\033[0m"
	TXT_BOLD = "\033[1m"
	TXT_GREEN = "\033[0;32m"
	TXT_BLUE = "\033[0;34m"

# OS checking
clear_cmd = "clear" #Default UNIX clear command
if os.name == "nt":
	clear_cmd = "cls" #Windows clear command

HELP_MENU = f"""A combination can be added to the selected element by entering two comma-separated lists for the recipe elements and the results. Any recipe elements or result elements that do not exist will be added automatically. Additionally, recipes with only one other element will also add the opposite combination to the recipe element.

The example below adds a combination with fire and water that results in the elements "vent" and "geyser".
Example:
  fire,water vent,geyser
The resulting combination here is "<cur_elem> + fire + water = geyser, vent".

{TXT_BOLD}Commands:{TXT_FORMAT_END}
 {TXT_GREEN}-e, --element {TXT_BLUE}id{TXT_FORMAT_END}
 Selects element, asks for element id if no argument given
    Example: -e water

 {TXT_GREEN}-r, --remove {TXT_BLUE}c1,c2..{TXT_FORMAT_END}
 Remove combination from currently selected element
    Example: -r fire,fire

 {TXT_GREEN}-d, --delete {TXT_BLUE}e1,e2..{TXT_FORMAT_END}
 Delete element(s)
    Example: -d lead,algae

 {TXT_GREEN}-s {TXT_BLUE}path{TXT_FORMAT_END}
 Save changes to file, optionally to a new file if a parameter is provided

 {TXT_GREEN}-x, --exit{TXT_FORMAT_END}
 Exit script
Press enter to exit this menu. """

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
		elems.update({elem:{"combos":[]}}) #Add new element if doesn't exist

# Add combo to element data.
# elem Element data
def add_combo(element: dict, combo_elems: list[str], combo_results: list[str]) -> dict:
	element["combos"].append({"elems":combo_elems, "results":combo_results}) #Add combo to element
	element["combos"] = sorted(element["combos"], key=lambda e: e["elems"]) #Sort recipes
	return element

err_msg = "" #Error message to display, if applicable
in_help_menu = False
cur_elem_sel = "air" #Currently selected element string ID
while True:
	os.system(clear_cmd) # Clear screen

	if in_help_menu:
		input(HELP_MENU)
		in_help_menu = False
		continue
	if cur_elem_sel not in combo_dict: add_elem(combo_dict, cur_elem_sel)
	print(f"{TXT_BOLD}{cur_elem_sel.title().replace("_"," ")} Combinations:{TXT_FORMAT_END}")

	for i, combo in enumerate(combo_dict[cur_elem_sel]["combos"]):
		print(f"{cur_elem_sel} + {' + '.join(combo['elems'])} = {', '.join(combo['results'])}")

	print("\nType a list of comma-seperated combination elements and results, or enter -h or --help for a list of commands.\n")
	print(err_msg, end="") #Print error message
	err_msg = ""
	args: list[str] = input("Element/Selection (comma separated): ").strip().split()

	if len(args[0]) < 1:	continue
	match args[0]:
		case "":	continue
	# Change element
		case "-e" | "--element":
			if len(args) < 2:
				new_elem_sel = input("Element?: ").strip().replace(' ','_')
				if new_elem_sel == "":	continue
			else:
				new_elem_sel = args[1].replace(' ', '_') #Optional parameter

			try:
				combo_dict[new_elem_sel]
				cur_elem_sel = new_elem_sel
			except KeyError:
				if input("Element \""+new_elem_sel+"\" is not a valid element. Create it (y/n)? ").strip()[0] == 'y':
					add_elem(combo_dict, new_elem_sel)
					cur_elem_sel = new_elem_sel
	# Delete elements
		case "-d" | "--delete":
			if len(args) >= 2:
				deleted_elems = args[1].split(',')
			else:
				deleted_elems = input("Elements?: ").split(',')
			for e in deleted_elems:
				if e == cur_elem_sel:
					cur_elem_sel = "air" #TODO GO TO PREVIOUS SELECTED ELEMENT INSTEAD
				combo_dict.pop(e, None)
			continue
	# Display help menu
		case "-h" | "--help":
			in_help_menu = True
			continue
	# Remove combo
		case "-r" | "--remove":
			if len(args) >= 2:
				combo_recipes: list[str] = args[1:] #TODO FIX THIS
			else:
				combo_recipes: list[str] = input("Combos?: ").split()
			split_recipes: list[list[str]] = [x.split(',') for x in combo_recipes]

			# Remove each specified combo
			for recipe in split_recipes:
				recipe.sort() #Sort recipe data to match how it's stored
				print(type(recipe))
				for i, elem_combos in enumerate(combo_dict[cur_elem_sel]["combos"]):
					if recipe == elem_combos["elems"]:
						combo_dict[cur_elem_sel]["combos"].pop(i)
			# Redundantly remove if the combo only has one element
				if len(recipe) == 1:
					for i, elem_combos in enumerate(combo_dict[recipe[0]]["combos"]):
						if len(elem_combos["elems"]) == 1 and elem_combos["elems"][0] == cur_elem_sel:
							combo_dict[recipe[0]]["combos"].pop(i)
			continue
	# Save changes
		case "-s" | "--save":
			path = filename #Save current file by default
			if len(args) >= 2:
				path = args[1] #Optional argument to save to another file
				filename = path #Change current file
			open(path, 'w').write(json.dumps(combo_dict, sort_keys=True, indent='\t', separators=(',', ':')))
	# Exit
		case "-x" | "--exit":	break
	# Non-command
		case _:
			if args[0][:1] == '-' or args[0][:2] == "--": #False command failsafe
				err_msg = "\""+args[0]+"\" is not a valid command.\n"
				continue
			combo_elems: list[str] = [x.strip() for x in args[0].split(',')]
			if len(args) < 2:
				combo_results = input("Result(s)?: ").strip()
				if (combo_results == ''):	continue
			else:
				combo_results = args[1]
			combo_results = [x.strip() for x in combo_results.split(',')]
			combo_elems.sort()
			combo_results.sort()

			# Add elements that don't exist
			for combo_elem in combo_elems:
				if combo_elem not in combo_dict:
					add_elem(combo_dict, combo_elem)
			for combo_res in combo_results:
				if combo_res not in combo_dict:
					add_elem(combo_dict, combo_res)

			# Add combos
			combo_exists = False
			for i, combo in enumerate(combo_dict[cur_elem_sel]["combos"]):
				if combo["elems"] == combo_elems:
					combo_exists = True
					combo_dict[cur_elem_sel]["combos"][i] = {"elems": combo["elems"], "results": combo_results}
					break
			if not combo_exists:
				combo_dict[cur_elem_sel] = add_combo(combo_dict[cur_elem_sel], combo_elems, combo_results)
			# Add to other combo if combo is with only one other element
			if len(combo_elems) == 1:
				combo_exists = False
				for i, combo in enumerate(combo_dict[combo_elems[0]]["combos"]):
					if combo["elems"] == [cur_elem_sel]:
						combo_exists = True
						combo_dict[combo_elems[0]]["combos"][i] = {"elems": combo["elems"], "results": combo_results}
						break
				if not combo_exists:
					combo_dict[combo_elems[0]] = add_combo(combo_dict[combo_elems[0]], [cur_elem_sel], combo_results)
