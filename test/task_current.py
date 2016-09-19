#!/usr/bin/python
import os
import uuid
import subprocess
import sys
import common_func

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the current task command for the multi projects
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
# task active 3
# task start 13
# task start 16
# task active 20
# task current
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
        common_func.create_task_str(3, 2, task_name21, "A", 0, "   0:00:00", 0, "") +
        common_func.create_task_str(13, 3, task_name333, "A", 0, "   0:00:00", 0, "") +
        "DONT CHECK\n" +
        "DONT CHECK\n" +
        common_func.create_task_str(16, 1, task_name4, "A", 0, "   0:00:00", 0, "") +
        "DONT CHECK\n" +
        "DONT CHECK\n" +
        common_func.create_task_str(20, 2, task_name61, "A", 0, "   0:00:00", 0, "")
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
plant_issue([plan_app, "task", "active", "3"])
plant_issue([plan_app, "task", "start", "13"])
plant_issue([plan_app, "task", "start", "16"])
plant_issue([plan_app, "task", "active", "20"])
plant_check([plan_app, "task", "current"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
