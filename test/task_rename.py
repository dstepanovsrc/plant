#!/usr/bin/python
import os
import uuid
import subprocess
import sys
import common_func

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the rename task command
# Test:
# plan add <plan_name>
# task add <task_name1>
# task rename 1 <task_name2>
# plan show
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
task_name1 = str(uuid.uuid4())[:23]
task_name2 = str(uuid.uuid4())[:23]
check_string1 = (
        common_func.table_header() +
        common_func.create_task_str(1, 1, task_name1, "P", 0, "   0:00:00", 0, "")
        )
check_string2 = (
        common_func.table_header() +
        common_func.create_task_str(1, 1, task_name2, "P", 0, "   0:00:00", 0, "")
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_issue([plan_app, "task", "add", task_name1])
plant_check([plan_app, "plan", "show"], check_string1)
plant_issue([plan_app, "task", "rename", "1", task_name2])
plant_check([plan_app, "plan", "show"], check_string2)

# Remove plan files
remove_created_plans()

sys.exit(0)
