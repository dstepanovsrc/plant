#!/usr/bin/python
import os
import uuid
import subprocess
import sys

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the add task command with wrong id
# Test:
# plan add <plan_name>
# project add <project_name>
# task add <task_name> <wrong_id>
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
project_name = str(uuid.uuid4())[:23]
task_name = str(uuid.uuid4())[:23]
check_string = (
        "There is no item with the requested id.\n"
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_issue([plan_app, "project", "add", project_name])
plant_check([plan_app, "task", "add", task_name, "10"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
