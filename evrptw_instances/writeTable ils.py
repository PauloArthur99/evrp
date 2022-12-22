import csv
import numpy as np


with open("results ils.csv", 'w') as csvfile:
    csvwriter = csv.writer(csvfile) 
    csvwriter.writerow(['Instância', 'Média energia total gasta', 'Média tempo de duração', 'Média número de Veículos', 'Desvio Padrão', 'Mediana'])

    for i in range(1, 92):
        lista_energias = []
        lista_time = []
        lista_veic =[]
        for iteration in range(1, 11):
            fileString = "soluções" + str(iteration) + "/soluçãoinstance" + str(i) + ".txt"
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
                duration_time = float(split_line[-1])

                ultima_linha = contents[-1]
                split_line_num_veic = ultima_linha.split()
                num_veic = int(split_line_num_veic[-1])

                lista_veic.append(num_veic)
                lista_time.append(duration_time)
                lista_energias.append(energia_total)
        media_veic = sum(lista_veic)/len(lista_veic)
        media_tempo = sum(lista_time)/len(lista_time)
        media_energia = sum(lista_energias)/len(lista_energias)
        desv_padr = np.std(lista_energias, ddof=1)
        median = np.median(lista_energias)
        field = [str(i), str(round(media_energia, 2)), str(round(media_tempo, 2)), str(media_veic), str(round(desv_padr, 2)), str(round(median, 2))]
        csvwriter.writerow(field)

