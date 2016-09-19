#!/usr/bin/python
import os
import uuid
import subprocess
import sys
import common_func

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the add project command with two names
# Test:
# plan add <plan_name>
# project add <project_name>
# project add <project_name>
# plan show
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
project_name = str(uuid.uuid4())[:23]
check_string = (
        common_func.table_header() +
        common_func.create_project_str(1, 1, project_name, 0, 0) +
        common_func.create_project_str(2, 1, project_name, 0, 0)
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_issue([plan_app, "project", "add", project_name])
plant_issue([plan_app, "project", "add", project_name])
plant_check([plan_app, "plan", "show"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
