import subprocess

path = "/Users/paulo/Desktop/evrp/evrptw_instances/evrp"

for i in range(1, 92):
    fileString = path + " instance" + str(i) + ".txt Alg_Economias"
    subprocess.run(fileString, shell=True)
