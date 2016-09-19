#!/usr/bin/python
import os
import uuid
import subprocess
import sys
import common_func

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the remove project command for the empty plan
# Test:
# plan add <plan_name>
# project remove 0
# plan show
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
project_name1 = str(uuid.uuid4())[:23]
check_string1 = (
        "Successfull removal of all projects and tasks in the current plan.\n"
        )
check_string2 = (
        common_func.table_header()
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_check([plan_app, "project", "remove", "0"], check_string1)
plant_check([plan_app, "plan", "show"], check_string2)

# Remove plan files
remove_created_plans()

sys.exit(0)
