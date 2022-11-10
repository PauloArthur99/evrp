import csv


with open("results.csv", 'w') as csvfile:
    csvwriter = csv.writer(csvfile) 
    csvwriter.writerow(['Instância', 'Energia total gasta', 'tempo alg de economias', 'tempo 2-opt'])


    for i in range(1, 92):
        fileString = "soluçãoinstance" + str(i) + ".txt"
        with open(fileString) as f:
            f.readline()
            contents = f.readlines() 
            energia_total = 0
            for j in range(len(contents) - 2):
                line = contents[j]
                split_line = line.split()
                energia = float(split_line[-1])
                energia_total += energia
            penultima_linha = contents[-2]
            split_line = penultima_linha.split()
            savings_time = float(split_line[-1])
            ultima_linha = contents[-1]
            split_line = ultima_linha.split()
            two_opt_time = float(split_line[-1])

            field = [str(i), str(energia_total), str(savings_time), str(two_opt_time)]
            csvwriter.writerow(field)

