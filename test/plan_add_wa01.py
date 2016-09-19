#!/usr/bin/python
import os
import uuid
import subprocess
import sys

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the creation of new plan if there is no plan name
# Test:
# plan add
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
check_string = (
        "Not enough arguments, for the current command.\n"
        )

# Run all the process and check output
plant_check([plan_app, "plan", "add"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
