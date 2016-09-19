#!/usr/bin/python
import os
import uuid
import subprocess
import sys
import common_func

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the remove project command, empty all project and tasks lists
# Test:
# plan add <plan_name>
# project add <project_name1>
# project add <project_name2>
# project add <project_name3>
# project add <project_name21>
# project add <project_name22>
# project add <project_name23>
# project add <project_name221>
# project add <project_name222>
# project add <project_name223>
# project add <project_name2211>
# project add <project_name2212>
# project remove 0
# plan show
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
project_name1 = str(uuid.uuid4())[:23]
project_name2 = str(uuid.uuid4())[:23]
project_name3 = str(uuid.uuid4())[:23]
project_name21 = str(uuid.uuid4())[:23]
project_name22 = str(uuid.uuid4())[:23]
project_name23 = str(uuid.uuid4())[:23]
project_name221 = str(uuid.uuid4())[:23]
project_name222 = str(uuid.uuid4())[:23]
project_name223 = str(uuid.uuid4())[:23]
project_name2211 = str(uuid.uuid4())[:23]
project_name2212 = str(uuid.uuid4())[:23]
check_string = (
        common_func.table_header()
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_issue([plan_app, "project", "add", project_name1])
plant_issue([plan_app, "project", "add", project_name2])
plant_issue([plan_app, "project", "add", project_name3])
plant_issue([plan_app, "project", "add", project_name21, "2"])
plant_issue([plan_app, "project", "add", project_name22, "2"])
plant_issue([plan_app, "project", "add", project_name23, "2"])
plant_issue([plan_app, "project", "add", project_name221, "4"])
plant_issue([plan_app, "project", "add", project_name222, "4"])
plant_issue([plan_app, "project", "add", project_name223, "4"])
plant_issue([plan_app, "project", "add", project_name2211, "5"])
plant_issue([plan_app, "project", "add", project_name2212, "5"])
plant_issue([plan_app, "project", "remove", "0"])
plant_check([plan_app, "plan", "show"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
