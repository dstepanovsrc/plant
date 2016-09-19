#!/usr/bin/python
import os
import uuid
import subprocess
import sys
import common_func

from common_func import plan_file_add, plant_issue, plant_check, remove_created_plans

# Check the add project command for the multi projects
# Test:
# plan add <plan_name>
# project add <project_name1>
# project add <project_name2>
# project add <project_name3>
# project add <project_name21>
# project add <project_name22>
# project add <project_name4>
# project add <project_name23>
# project add <project_name11>
# project add <project_name12>
# project add <project_name13>
# project add <project_name221>
# project add <project_name222>
# project add <project_name31>
# project add <project_name41>
# plan show
# <check output>

plan_app = sys.argv[1] + "/../build/plant"
plan_name = str(uuid.uuid4())
plan_file_add(plan_name)
project_name1 = str(uuid.uuid4())[:23]
project_name11 = str(uuid.uuid4())[:23]
project_name12 = str(uuid.uuid4())[:23]
project_name13 = str(uuid.uuid4())[:23]
project_name2 = str(uuid.uuid4())[:23]
project_name21 = str(uuid.uuid4())[:23]
project_name22 = str(uuid.uuid4())[:23]
project_name221 = str(uuid.uuid4())[:23]
project_name222 = str(uuid.uuid4())[:23]
project_name23 = str(uuid.uuid4())[:23]
project_name3 = str(uuid.uuid4())[:23]
project_name31 = str(uuid.uuid4())[:23]
project_name4 = str(uuid.uuid4())[:23]
project_name41 = str(uuid.uuid4())[:23]
check_string = (
        common_func.table_header() +
        common_func.create_project_str(1, 1, project_name1, 3, 3) +
        common_func.create_project_str(2, 2, project_name11, 0, 0) +
        common_func.create_project_str(3, 2, project_name12, 0, 0) +
        common_func.create_project_str(4, 2, project_name13, 0, 0) +
        common_func.create_project_str(5, 1, project_name2, 3, 3) +
        common_func.create_project_str(6, 2, project_name21, 0, 0) +
        common_func.create_project_str(7, 2, project_name22, 2, 2) +
        common_func.create_project_str(8, 3, project_name221, 0, 0) +
        common_func.create_project_str(9, 3, project_name222, 0, 0) +
        common_func.create_project_str(10, 2, project_name23, 0, 0) +
        common_func.create_project_str(11, 1, project_name3, 1, 1) +
        common_func.create_project_str(12, 2, project_name31, 0, 0) +
        common_func.create_project_str(13, 1, project_name4, 1, 1) +
        common_func.create_project_str(14, 2, project_name41, 0, 0)
        )

# Run all the process and check output
plant_issue([plan_app, "plan", "add", plan_name])
plant_issue([plan_app, "project", "add", project_name1])
plant_issue([plan_app, "project", "add", project_name2])
plant_issue([plan_app, "project", "add", project_name3])
plant_issue([plan_app, "project", "add", project_name21, "2"])
plant_issue([plan_app, "project", "add", project_name22, "2"])
plant_issue([plan_app, "project", "add", project_name4])
plant_issue([plan_app, "project", "add", project_name23, "2"])
plant_issue([plan_app, "project", "add", project_name11, "1"])
plant_issue([plan_app, "project", "add", project_name12, "1"])
plant_issue([plan_app, "project", "add", project_name13, "1"])
plant_issue([plan_app, "project", "add", project_name221, "7"])
plant_issue([plan_app, "project", "add", project_name222, "7"])
plant_issue([plan_app, "project", "add", project_name31, "11"])
plant_issue([plan_app, "project", "add", project_name41, "13"])
plant_check([plan_app, "plan", "show"], check_string)

# Remove plan files
remove_created_plans()

sys.exit(0)
