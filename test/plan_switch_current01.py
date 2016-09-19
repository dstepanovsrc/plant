#!/usr/bin/python
import os
import uuid
import subprocess
import sys

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the current plan command after switch
# Test:
# plan add <plan_name1>
# plan add <plan_name2>
# plan switch <plan_name1>
# plan current
# <check output, plan_name1>

# Define main variables
plan_app = sys.argv[1] + "/../build/plant"
plan_name1 = str(uuid.uuid4())
plan_file_add(plan_name1)
plan_name2 = str(uuid.uuid4())
plan_file_add(plan_name2)
check_string = (
        "Plan: \"" + plan_name1 + "\"\n"
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name1])
plant_issue([plan_app, "plan", "add", plan_name2])
plant_issue([plan_app, "plan", "switch", plan_name1])
plant_check([plan_app, "plan", "current"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
