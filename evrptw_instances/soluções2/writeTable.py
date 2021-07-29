import csv


with open("results.csv", 'w') as csvfile:
    csvwriter = csv.writer(csvfile) 
    csvwriter.writerow(['Instância', 'Energia total gasta', 'total de rotas', 'rotas factíveis'])


    for i in range(1, 92):
        fileString = "soluçãoinstance" + str(i) + ".txt"
        with open(fileString) as f:
            f.readline()
            contents = f.readlines() 
            energia_total = 0
            rotas_factiveis = 0
            for line in contents:
                split_line = line.split()
                energia = float(split_line[-1])
                energia_total += energia
                if energia <= 18000:
                    rotas_factiveis += 1
            field = [str(i), str(energia_total), str(len(contents)), str(rotas_factiveis)]
            csvwriter.writerow(field)

