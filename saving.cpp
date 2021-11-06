#include "saving.h"
#define ENERGY_CONST 86.20810869
using namespace std;

int main(int argc, char** argv){
    string myText;

    vector<pair<double, double>> points;
    vector<double> demands;
    double loadCapacity;

    priority_queue<saving> savingQueue;

    ifstream MyReadFile(argv[1]);
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

    //writePoints(points, "PointCoord.txt");

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
                    double spentEnergy = requiredEnergy(distMatrix, energyMatrix, 
                    demands, routes[idxA], routes[idxB], demandSum, pointA, pointB);

                    double spentEnergyReverse = requiredEnergy(distMatrix, energyMatrix, 
                    demands, routes[idxB], routes[idxA], demandSum, pointB, pointA);

                    if (spentEnergy <= 18000 && spentEnergyReverse <= 18000)
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
    }
    //writeSolution(routes, "arquivoSoluções.txt");

    // Evolução do valor das rotas da instância.
    vector<vector<double>> solutionEvol;
    int counter = 0;
    for (int i = 0; i < routes.size(); i++)
    {
        vector<double> auxVet{};
        solutionEvol.push_back(auxVet);
        double requiredEnergy = requiredEnergyOneRoute(distMatrix, energyMatrix, demands, routes[i]);
        solutionEvol[i].push_back(requiredEnergy);

        bool ok = true;
        vector<int> routeModified;
        tuple<bool, vector<int>, double> returnTuple;
        while (ok)
        {
            counter++;
            returnTuple = twoOpt(distMatrix, energyMatrix, demands, routes[i], requiredEnergy);
            ok = get<0>(returnTuple);
            routeModified = get<1>(returnTuple);
            requiredEnergy = get<2>(returnTuple);
            if (ok)
            {
                routes[i] = routeModified;
                solutionEvol[i].push_back(get<2>(returnTuple));
            }
        }
    } 

    vector<double> routesEnergy;
    for (int i = 0; i < routes.size(); i++)
    {
        double customerDemand = 0;
        double energySum = 0;
        for (int j = 0; j < routes[i].size(); j++)
        {
            int customer = routes[i][j];
            customerDemand += demands[customer];
        }

        routes[i].insert(routes[i].begin(), 0);
        routes[i].push_back(0);

        for (int j = 0; j < routes[i].size()-1; j++)
        {
            int orig = routes[i][j];
            int dest = routes[i][j+1];
            energySum += energyMatrix[orig][dest];
            energySum += distMatrix[orig][dest] * customerDemand * ENERGY_CONST_LOAD;
            customerDemand -= demands[dest];
        }
        routesEnergy.push_back(energySum);
    }
/*
    for (int i = 0; i < routesEnergy.size(); i++)
    {
        cout << routesEnergy[i] << "\n";
    }
*/
    string strSolucao = "/Users/paulo/Desktop/evrp/evrptw_instances/soluções/solução";
    strSolucao = strSolucao + argv[1];
    
    writeSolution(routes, routesEnergy, strSolucao, counter, solutionEvol);

	return 0;
}
