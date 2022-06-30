# Bootstrapping
import importlib.util

# Try to give a nicer error message if there are modules missing
def bootstrap():
    modules_to_verify = [
        "psutil",
        "tkinter"
    ]
    for module in modules_to_verify:
        loader = importlib.util.find_spec(module)
        if loader is None:
            print("Couldn't find module - '{0}'".format(module))
            print()
            print("Try installing module")
            print("  python -m pip install {0}".format(module))
            print()
            input("Press Enter to continue...")
            exit()

bootstrap()


import datetime
import glob
import json
#import kubernetes          # If missing, run "python -m pip install kubernetes"
import os
import psutil
import subprocess
import sys
import time
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
from tkinter.filedialog import asksaveasfile
from types import SimpleNamespace
from configparser import ConfigParser

window_title = "Neurons Launcher" 
version_number = 0.01

source_folder = "Src"
solution_name = "neurons.sln"

config_file_name = "Launcher.ini"

user_input_build_config = ""
ui_build_configurations = [
    "debug",
    "profile",
    "release"
]

def timestamp():
    now = datetime.datetime.now()
    return now.strftime("%Y%m%d_%H:%M:%S")

def print_out(str):
    global output_textbox
    output_textbox.insert(tk.END, str + "\n")
    output_textbox.see(tk.END)
    return

# command - array of parameters to pass to command processor or a string with space-separated parameters; First parameter is  the exe to start
# working_folder - path to run the command from; note: cwd will be restored before this function returns
# Returns - string of stdout
def run_command(command, working_folder=".", wait_for_exit=True):
    output = ""
    old_working_directory = os.getcwd()
    try:
        os.chdir(working_folder)

        parameters = command
        if (type(command) == str):
            parameters = command.split()

        print_out("{0} - {1}".format(timestamp(), parameters))
        if parameters[0] == "start":
            os.system(command)
        else:
            if (wait_for_exit):
                result = subprocess.run(parameters, capture_output=True)
                output = result.stdout.decode(sys.stdout.encoding)
                error = result.stderr.decode(sys.stdout.encoding)
                if (len(output) > 0):
                    print_out(output)
                if (len(error) > 0):
                    print_out("stderr:")
                    print_out(error)
            else:
                process = subprocess.Popen(parameters)
    finally:
        # Restore the previous working directory
        os.chdir(old_working_directory)
    return output



def open_solution():
    path_to_solution = os.path.join(source_folder, solution_name)
    run_command("start {0}".format(path_to_solution))

def open_folder():
    run_command("start .")

def command_prompt():
    run_command("start cmd .")

def git_gui():
    run_command("git gui", wait_for_exit=False)

def placeholder():
    return

def build_local_ui(root):
    global user_input_build_config

    frame = tk.Frame(root, borderwidth=3, relief=tk.RIDGE, padx=3, pady=3)
    frame.pack()
    tk.Label(frame, text="Solution").pack(side=tk.TOP)
    tk.Button(frame, text="Open Solution", command=open_solution).pack(padx=5, side=tk.LEFT)
    tk.Button(frame, text="Open Folder", command=open_folder).pack(padx=5, side=tk.LEFT)
    tk.Button(frame, text="Command Prompt", command=command_prompt).pack(padx=5, side=tk.LEFT)

    frame = tk.Frame(root, borderwidth=3, relief=tk.RIDGE, padx=3, pady=3)
    frame.pack()
    tk.Label(frame, text="Git").pack(side=tk.TOP)
    tk.Button(frame, text="Open Git Gui", command=git_gui).pack(padx=5, side=tk.LEFT)


def build_tools_ui(root):
    frame = tk.Frame(root, borderwidth=3, relief=tk.RIDGE, padx=3, pady=3)
    frame.pack()
    tk.Button(frame, text="Placeholder", command=placeholder).pack(padx=5, side=tk.LEFT)
    tk.Label(frame, text="???").pack(side=tk.LEFT)


# notebook - Instance of ttk.Notebook to build a new tab in
# tab_name - Title of new tab
# build_ui_function - Reference to function that will populate the tab
#                   - Takes a root ui panel as it's only parameter
def add_tab(notebook, tab_name, build_ui_function):
    new_tab = tk.Frame(notebook)
    notebook.add(new_tab, text=tab_name)
    build_ui_function(new_tab)

def LoadConfig():
    config = ConfigParser()
    config.read(config_file_name)
    
    config.add_section('main')
    config.set('main', 'key1', 'value1')
    config.set('main', 'key2', 'value2')
    config.set('main', 'key3', 'value3')

    with open(config_file_name, 'w') as f:
        config.write(f)

def Main():
    # WIP - Saving/loading settings from a .ini file
    # LoadConfig()

    window = tk.Tk()
    global window_title
    global version_number
    if version_number > 0:
        window_title = "{0} v{1}".format(window_title, version_number)
    window.title(window_title)

    window_panes = ttk.PanedWindow(window, orient=tk.VERTICAL)
    window_panes.pack(fill=tk.BOTH, expand=1)

    notebook = ttk.Notebook(window_panes)
    notebook.pack(expand=1, fill="both")
    window_panes.add(notebook)

    bottom_pane = tk.Frame(window_panes)#, borderwidth=3, relief=tk.RIDGE, padx=3, pady=3)
    bottom_pane.pack(expand=1, fill=tk.BOTH)
    window_panes.add(bottom_pane)

    global output_textbox
    output_textbox = tk.Text(bottom_pane, width=10, height=5, wrap=tk.NONE)
    output_textbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=tk.YES)
    scroll = tk.Scrollbar(bottom_pane, orient=tk.VERTICAL, command=output_textbox.yview)
    scroll.pack(side=tk.RIGHT, fill=tk.Y)

    add_tab(notebook, 'Local', build_local_ui)
    add_tab(notebook, 'Tools', build_tools_ui)

    window.mainloop()

if __name__ == '__main__':
    Main()
