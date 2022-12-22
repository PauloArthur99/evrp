import csv


with open("results AE.csv", 'w') as csvfile:
    csvwriter = csv.writer(csvfile) 
    csvwriter.writerow(['Instância', 'Energia total gasta', 'Número de Veículos'])


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

            ultima_linha = contents[-1]
            split_line_num_veic = ultima_linha.split()
            num_veic = int(split_line_num_veic[-1])

            field = [str(i), str(round(energia_total, 2)), str(num_veic)]
            csvwriter.writerow(field)

