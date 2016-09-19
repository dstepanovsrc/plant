#!/usr/bin/python
import os
import uuid
import subprocess
import sys
import common_func

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the start and stop commands
# Test:
# plan add <plan_name>
# task add <task_name>
# task start 1
# task start 1
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
task_name = str(uuid.uuid4())[:23]
check_string = (
        "Can't start the task, because it is already running.\n"
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_issue([plan_app, "task", "add", task_name])
plant_issue([plan_app, "task", "start", "1"])
plant_check([plan_app, "task", "start", "1"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
