import subprocess

path = "/Users/paulo/Desktop/evrp/evrptw_instances/saving2"

for i in range(1, 92):
    fileString = path + " instance" + str(i) + ".txt"
    subprocess.run(fileString, shell=True)