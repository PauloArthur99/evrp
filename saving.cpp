#include "saving.h"
using namespace std;

int main(){
    string myText;

    vector<pair<double, double>> points;
    vector<double> demands;
    double loadCapacity;

    priority_queue<saving> savingQueue;

    ifstream MyReadFile("c101_21.txt");
    // Pulando a linha de cabeçalho.
    getline(MyReadFile, myText);
    
    // Laço de leitura dos dados do arquivo.
    while(getline(MyReadFile, myText)){
        // Usado para dividir as strings nos espaços vazios.
        istringstream ss(myText);
        string word, type, x, y;   
        
        ss >> word;
        ss >> type;
        ss >> x; 
        ss >> y;
        if (type == "d" or type == "c") // or type == "f")
        {            
            pair <double, double> pointPair (stod(x), stod(y));
            points.push_back(pointPair);
            
            string pointDemand;
            ss >> pointDemand; 
            demands.push_back(stod(pointDemand));
        }
        else if (word == "C")
        {
            string loadCapacityStr;
            ss >> loadCapacityStr;
            loadCapacityStr.erase(0, 1);
            loadCapacityStr.pop_back();
            loadCapacity = stod(loadCapacityStr);
        }
    }
    MyReadFile.close();

    int sizePoints = points.size();
    int lastPoint = sizePoints - 1;

    //matriz de distancias.
    vector<vector<double>> distMatrix(sizePoints, vector<double>(sizePoints, 0.0));
    vector<vector<double>> energyMatrix(sizePoints, vector<double>(sizePoints, 0.0));

    // Preenchimento da matriz de distâncias e de energias.
    for (int i = 0; i < sizePoints; i++)
    {
        for (int j = 0; j < sizePoints; j++)
        {
            distMatrix[i][j] = distancePoints(points[i], points[j]);
            energyMatrix[i][j] = ENERGY_CONST * distMatrix[i][j];
        }
    }
    
    // Cálculo das economias das arestas
    for (int i = 1; i < lastPoint; i++)
    {
        for (int j = i + 1; j <= lastPoint; j++)
        {
            double savingIJ = distMatrix[0][i] + distMatrix[0][j] - distMatrix[i][j];
            struct saving savingStruct;
            savingStruct.pointA = i;
            savingStruct.pointB = j;
            savingStruct.saving = savingIJ;
            if (savingIJ > 0)
                savingQueue.push(savingStruct);
        }
    }

    // Coleção de rotas
    vector<vector<int>> routes;
    for (int i = 1; i <= lastPoint; i++)
    {
        vector<int> aux{i};
        routes.push_back(aux);
    }

    while (!savingQueue.empty())
    {
        int pointA, idxA, pointB, idxB;
        pointA = savingQueue.top().pointA;
        pointB = savingQueue.top().pointB;
        savingQueue.pop();
        
        for (int i = 0; i < routes.size(); i++)
        {
            for (int j = 0; j < routes[i].size(); j++)
            {
                if (routes[i][j] == pointA)
                {
                    idxA = i;
                }
                else if(routes[i][j] == pointB)
                {
                    idxB = i;
                }
            }
        }
        if (!(routes[idxA] == routes[idxB]))
        {
            if (depositConect(routes[idxA], pointA) && depositConect(routes[idxB], pointB))
            {   
                double demandSum = 0;
                for (int i = 0; i < routes[idxA].size(); i++)
                {
                    int elem = routes[idxA][i];
                    demandSum += demands[elem];
                }
                for (int i = 0; i < routes[idxB].size(); i++)
                {
                    int elem = routes[idxB][i];
                    demandSum += demands[elem];
                } 

                if (demandSum <= loadCapacity)
                {
                    vector<int> routesJoined = joinTwoRoutes(routes[idxA], pointA, routes[idxB], pointB);
                    routes.push_back(routesJoined);

                    if (idxA < idxB)
                    {
                        routes.erase(routes.begin() + idxA);
                        routes.erase(routes.begin() + idxB - 1);
                    }
                    else
                    {
                        routes.erase(routes.begin() + idxA);
                        routes.erase(routes.begin() + idxB);
                    }
                }
            }
        }     
    }
    writeSolution(routes, "arquivoSoluções.txt");

	return 0;
}
