import subprocess

path = "/Users/paulo/Desktop/evrp/evrptw_instances/evrp"

for j in range(1, 11):
    for i in range(1, 92):
        fileString = path + " instance" + str(i) + ".txt " + str(j)
        subprocess.run(fileString, shell=True)
