#!/usr/bin/python
import os
import uuid
import subprocess
import sys

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the remove project command with not enough arguments
# Test:
# plan add <plan_name>
# project remove
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
project_name1 = str(uuid.uuid4())[:23]
check_string = (
        "Not enough arguments, for the current command.\n"
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_check([plan_app, "project", "remove"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
