#!/usr/bin/python
import os
import uuid
import subprocess
import sys

DEVNULL = open(os.devnull, 'w')
plant_config_dir = os.path.expanduser("~") + "/.plantrc/"
plan_file = []

# Append element to the plan file list
def plan_file_add(name):
    plan_file.append(plant_config_dir + name)

# Remove the created plans
def remove_created_plans():
    for f in plan_file:
        retcode = subprocess.call(["rm", f], stdout=DEVNULL, stderr=subprocess.STDOUT)

# Common function to run any "plant" command
def plant_issue(run_arg):
    if subprocess.call(run_arg, stdout=DEVNULL, stderr=subprocess.STDOUT) != 0:
        print "Can't exec test commands: " + str(run_arg)
        remove_created_plans()
        sys.exit(1)

# Common function to run any "plant" command and check its output
def plant_check(run_arg, check):
    plan_exec = subprocess.Popen(run_arg, stdout=subprocess.PIPE)
    plan_out = plan_exec.stdout.read()
    # Check output
    c_list = check.splitlines(False)
    p_list = plan_out.splitlines(False)
    if len(c_list) != len(p_list):
        print "Not expected \"plant\" application output:"
        print plan_out
        # Don't forget to remove plan file
        remove_created_plans()
        sys.exit(1)
    for c_line, p_line in zip(c_list, p_list):
        if c_line == "DONT CHECK":
            continue
        if c_line != p_line:
            print "Not expected \"plant\" application output:"
            print plan_out
            print "Got     : \"" + p_line + "\""
            print "Expected: \"" + c_line + "\""
            # Don't forget to remove plan file
            remove_created_plans()
            sys.exit(1)

def table_header():
    th = (
            "  #  | Name                                     | S | Prog |   Spent    |  Estm  |  Assign\n"
            "---------------------------------------------------------------------------------------------\n"
            )
    return th 

def create_project_str(id, ident, name, completed, overall):
    str_all = " " + '{:03d}'.format(id) + " |"
    str_name = ""
    for i in range(0, ident):
        str_name += " "
    str_name += name
    str_name += " (" + str(completed) + "/" + str(overall) + ")"
    str_name_len = len(str_name)
    for i in range(str_name_len, 42):
        str_name += " "
    str_all += str_name
    if completed == overall:
        str_all += "| C |"
    else:
        str_all += "| P |"
    str_all += "      |            |        |"
    str_all += "\n"
    return str_all;

def create_task_str(id, ident, name, status, progress, time, estimation, assign):
    str_all = " " + '{:03d}'.format(id) + " |"
    str_name = ""
    for i in range(0, ident):
        str_name += " "
    str_name += name
    str_name_len = len(str_name)
    for i in range(str_name_len, 42):
        str_name += " "
    str_all += str_name
    str_all += "| " + status + " |"
    str_all += " " + '{:03d}'.format(progress) + "% |"
    str_all += " " + time + " |"
    str_all += " " + '{:5d}'.format(estimation) + "h |"
    str_all += assign
    str_all += "\n"
    return str_all;
