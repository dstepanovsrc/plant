#!/usr/bin/python
import os
import uuid
import subprocess
import sys
import common_func

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the remove task command, for all possible tasks
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
# task remove 1
# task remove 8
# task remove 13
# task remove 11
# task remove 2
# task remove 11
# task remove 14
# plan show
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
        common_func.table_header() +
        common_func.create_project_str(1, 1, project_name2, 3, 3) +
        common_func.create_project_str(2, 2, project_name22, 0, 0) +
        common_func.create_project_str(3, 2, project_name23, 0, 0) +
        common_func.create_project_str(4, 2, project_name24, 0, 0) +
        common_func.create_project_str(5, 1, project_name3, 3, 3) +
        common_func.create_project_str(6, 2, project_name31, 0, 0) +
        common_func.create_project_str(7, 2, project_name33, 2, 2) +
        common_func.create_project_str(8, 3, project_name331, 0, 0) +
        common_func.create_project_str(9, 3, project_name332, 0, 0) +
        common_func.create_project_str(10, 2, project_name34, 0, 0) +
        common_func.create_project_str(11, 1, project_name5, 1, 1) +
        common_func.create_project_str(12, 2, project_name51, 0, 0) +
        common_func.create_project_str(13, 1, project_name6, 0, 0)
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
plant_issue([plan_app, "task", "remove", "1"])
plant_issue([plan_app, "task", "remove", "8"])
plant_issue([plan_app, "task", "remove", "13"])
plant_issue([plan_app, "task", "remove", "11"])
plant_issue([plan_app, "task", "remove", "2"])
plant_issue([plan_app, "task", "remove", "11"])
plant_issue([plan_app, "task", "remove", "14"])
plant_issue([plan_app, "task", "remove", "1"])
plant_check([plan_app, "plan", "show"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
