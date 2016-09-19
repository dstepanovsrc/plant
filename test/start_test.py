#!/usr/bin/python
import os
import sys
import subprocess

skip_files = ["start_test.py",
        "common_func.py"]
test_dir = sys.argv[1]

files = os.listdir(test_dir)
test_all = 0
test_pass = 0
test_fail = 0
for f in files:
    if f.endswith('.py') == False:
        continue
    cont_flag = False
    for skip in skip_files:
        if f == skip:
            cont_flag = True
            break
    if cont_flag == True:
        continue

    test_all += 1
    print "Run " + f
    test_script = test_dir + "/" + f
    retcode = subprocess.call(["python", test_script, test_dir])
    if retcode == 0:
        print "Test " + f + ": PASSED"
        test_pass += 1
    else:
        print "Test " + f + ": FAILED"
        test_fail += 1

print ""
print "=============================="
print "Test run: " + str(test_all)
print "    Test passed: " + str(test_pass)
print "    Test failed: " + str(test_fail)
