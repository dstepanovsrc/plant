#!/usr/bin/python
import os
import uuid
import subprocess
import sys

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the show task command without arguments
# Test:
# plan add <plan_name>
# task add <task_name1>
# project add <project_name2>
# project add <project_name3>
# task add <task_name4>
# project add <project_name5>
# project add <project_name31>
# task add <task_name32>
# project add <project_name33>
# project add <project_name6>
# project add <project_name34>
# task add <task_name35>
# task add <task_name21>
# project add <project_name22>
# project add <project_name23>
# project add <project_name24>
# project add <project_name331>
# project add <project_name332>
# task add <task_name333>
# project add <project_name51>
# task add <task_name61>
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
# task show
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
task_name1 = str(uuid.uuid4())[:23]
project_name2 = str(uuid.uuid4())[:23]
task_name21 = str(uuid.uuid4())[:23]
project_name22 = str(uuid.uuid4())[:23]
project_name23 = str(uuid.uuid4())[:23]
project_name24 = str(uuid.uuid4())[:23]
project_name3 = str(uuid.uuid4())[:23]
project_name31 = str(uuid.uuid4())[:23]
task_name32 = str(uuid.uuid4())[:23]
project_name33 = str(uuid.uuid4())[:23]
project_name331 = str(uuid.uuid4())[:23]
project_name332 = str(uuid.uuid4())[:23]
task_name333 = str(uuid.uuid4())[:23]
project_name34 = str(uuid.uuid4())[:23]
task_name35 = str(uuid.uuid4())[:23]
task_name4 = str(uuid.uuid4())[:23]
project_name5 = str(uuid.uuid4())[:23]
project_name51 = str(uuid.uuid4())[:23]
project_name6 = str(uuid.uuid4())[:23]
task_name61 = str(uuid.uuid4())[:23]
check_string = (
        "Not enough arguments, for the current command.\n"
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_issue([plan_app, "task", "add", task_name1])
plant_issue([plan_app, "project", "add", project_name2])
plant_issue([plan_app, "project", "add", project_name3])
plant_issue([plan_app, "task", "add", task_name4])
plant_issue([plan_app, "project", "add", project_name5])
plant_issue([plan_app, "project", "add", project_name31, "3"])
plant_issue([plan_app, "task", "add", task_name32, "3"])
plant_issue([plan_app, "project", "add", project_name33, "3"])
plant_issue([plan_app, "project", "add", project_name6])
plant_issue([plan_app, "project", "add", project_name34, "3"])
plant_issue([plan_app, "task", "add", task_name35, "3"])
plant_issue([plan_app, "task", "add", task_name21, "2"])
plant_issue([plan_app, "project", "add", project_name22, "2"])
plant_issue([plan_app, "project", "add", project_name23, "2"])
plant_issue([plan_app, "project", "add", project_name24, "2"])
plant_issue([plan_app, "project", "add", project_name331, "10"])
plant_issue([plan_app, "project", "add", project_name332, "10"])
plant_issue([plan_app, "task", "add", task_name333, "10"])
plant_issue([plan_app, "project", "add", project_name51, "17"])
plant_issue([plan_app, "task", "add", task_name61, "19"])
plant_check([plan_app, "task", "show"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
