import subprocess

path = "/Users/paulo/Desktop/evrp/project/evrptw_instances/evrp"

for i in range(1, 92):
    fileString = path + " instance" + str(i) + ".txt"
    subprocess.run(fileString, shell=True)
