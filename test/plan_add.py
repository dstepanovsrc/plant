#!/usr/bin/python
import os
import uuid
import subprocess
import sys

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the creation of new plan
# Test:
# plan add <plan_name>
# <check output>

# Define main variables
plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
check_string = (
        "Plan \"" + plan_name + "\" created successfully.\n"
        )

# Run all the process and check output
plant_check([plan_app, "plan", "add", plan_name], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
