import subprocess

path = "/Users/paulo/Desktop/evrp/evrptw_instances/saving"

for i in range(1, 92):
    fileString = path + " instance" + str(i) + ".txt"
    subprocess.run(fileString, shell=True)
