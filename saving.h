#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
#include <algorithm>
#include<tuple>
using namespace std;
#define ENERGY_CONST_LOAD 0.129285172

// Calcula a energia total consumida em uma rota.
double requiredEnergyOneRoute(vector<vector<double>> distMatrix, vector<vector<double>> energyMatrix, 
                    vector<double> demands, vector<int> route)
{
    double routeEnergy = 0;
    double demandSum = 0;
    for (int i = 0; i < route.size(); i++)
    {
        int elem = route[i];
        demandSum += demands[elem];
    }

    int dest = route[0];
    routeEnergy += energyMatrix[0][dest];
    routeEnergy += distMatrix[0][dest] * demandSum * ENERGY_CONST_LOAD;
    demandSum -= demands[dest];
    for (int i = 0; i < route.size() - 1; i++)
    {
        int orig = route[i];
        int dest = route[i+1];
        routeEnergy += energyMatrix[orig][dest];
        routeEnergy += distMatrix[orig][dest] * demandSum * ENERGY_CONST_LOAD;
        demandSum -= demands[dest];
    }
    int idx = route.size() - 1;
    int orig = route[idx];
    routeEnergy += energyMatrix[orig][0];
    return routeEnergy;
}

// Estrutura de vizinhança 2-opt.
tuple<bool, vector<int>, double> twoOpt(vector<vector<double>> distMatrix, vector<vector<double>> energyMatrix, 
                    vector<double> demands, vector<int> route, double energyOrigRoute)
{
    vector<int> tempRoute = route;
    for (int i = 0; i < route.size() - 1; i++)
    {
        for (int j = i + 1; j < route.size(); j++)
        {
            vector<int> routeModified;
            vector<int> auxVector;
            for (int k = 0; k < i; k++)
            {
            routeModified.push_back(route[k]);
            }

            for (int k = i; k <= j; k++)
            {
            auxVector.push_back(route[k]);
            }
            reverse(auxVector.begin(), auxVector.end());

            for (int k = 0; k < auxVector.size(); k++)
            {
            routeModified.push_back(auxVector[k]);
            }

            for (int k = j+1; k < route.size(); k++)
            {
            routeModified.push_back(route[k]);
            }
            tempRoute = routeModified; 
        }
        double newEnergy = requiredEnergyOneRoute(distMatrix, energyMatrix, demands, tempRoute);
        if (newEnergy < energyOrigRoute)
        {
            tuple<bool, vector<int>, double> returnTuple;
            returnTuple = make_tuple(true, tempRoute, newEnergy);
            return returnTuple;
        }
    }
    tuple<bool, vector<int>, double> returnTuple;
    returnTuple = make_tuple(false, route, 0);
    return returnTuple;
}

// Calcula a energia total consumida na junção de duas rotas.
double requiredEnergy(vector<vector<double>> distMatrix, vector<vector<double>> energyMatrix, 
                    vector<double> demands, vector<int> routeA, vector<int> routeB, double demandSum, int pointA, int pointB)
{
    double routeEnergy = 0;
    vector<int> route_copied;
    route_copied = routeA;
    if (routeA[0] == pointA)
    {
        reverse(route_copied.begin(), route_copied.end());
    }
    int dest = route_copied[0];
    routeEnergy += energyMatrix[0][dest];
    routeEnergy += distMatrix[0][dest] * demandSum * ENERGY_CONST_LOAD;
    demandSum -= demands[dest];
    for (int i = 0; i < route_copied.size() - 1; i++)
    {
        int orig = route_copied[i];
        int dest = route_copied[i+1];
        routeEnergy += energyMatrix[orig][dest];
        routeEnergy += distMatrix[orig][dest] * demandSum * ENERGY_CONST_LOAD;
        demandSum -= demands[dest];
    }
    routeEnergy += energyMatrix[pointA][pointB];
    routeEnergy += distMatrix[pointA][pointB] * demandSum * ENERGY_CONST_LOAD;
    demandSum -= demands[pointB];

    vector<int> route_copied_B;
    route_copied_B = routeB;
    if (routeB[0] != pointB)
    {
        reverse(route_copied_B.begin(), route_copied_B.end());
    }
    for (int i = 0; i < route_copied_B.size() - 1; i++)
    {
        int orig = route_copied_B[i];
        int dest = route_copied_B[i+1];
        routeEnergy += energyMatrix[orig][dest];
        routeEnergy += distMatrix[orig][dest] * demandSum * ENERGY_CONST_LOAD;
        demandSum -= demands[dest];
    }
    routeEnergy += energyMatrix[dest][0];
    return routeEnergy;
}


// Retorna a distancia euclidiana de dois pontos estruturados como pairs.
double distancePoints(pair<double, double> a, pair<double, double> b)
{
    double pot1, pot2;
    pot1 = pow((b.first - a.first), 2);
    pot2 = pow((b.second - a.second), 2);
    return sqrt(pot1 + pot2);
}

// escreve as coordenadas dos pontos num arquivo.
void writePoints(vector<pair<double,double>> points , string fileName)
{
    ofstream MyFile(fileName); 
    MyFile << "PointID    Coordenadas\n";

    for (int i = 0; i < points.size(); i ++)
    {
        if (i < 10)
        {
            MyFile << i << "          " << points[i].first << " " << points[i].second; 
        }
        else if (i >= 100)
        {
            MyFile << i << "        " << points[i].first << " " << points[i].second;
        }
        else
        {
            MyFile << i << "         " << points[i].first << " " << points[i].second;
        }
        MyFile << "\n";
    }
}

// escreve as rotas encontradas em um arquivo
void writeSolution(vector<vector<int>> routes, vector<double> routesEnergy, string fileName, int counter, vector<vector<double>> solutionEvol)
{
    ofstream MyFile(fileName); 
    MyFile << "RouteID    RoutePoints           Energy\n";

    for (int i = 0; i < routes.size(); i++)
    {
        if (i + 1 < 10)
        {
            MyFile << "R" << i + 1 << "         ";
        }
        else
        {
            MyFile << "R" << i + 1 << "        ";
        }
        for (int j = 0; j < routes[i].size(); j++)
        {
            MyFile << routes[i][j] << " ";
        }
        MyFile << "    " << routesEnergy[i] << "\n";
    }
    MyFile << "--------------------------------------------------------------------------" << "\n";
    MyFile << "Número de movimentos:" << " " << counter << "\n";
    MyFile << "Evolução da Solução:" << "\n";
    for (int i = 0; i < solutionEvol.size(); i++)
    {
        if (i + 1 < 10)
        {
            MyFile << "R" << i + 1 << "         ";
        }
        else
        {
            MyFile << "R" << i + 1 << "        ";
        }
        for (int j = 0; j < solutionEvol[i].size(); j++)
        {
            MyFile << solutionEvol[i][j] << " ";
        }
        MyFile << "\n";
    }
}

vector<int> joinTwoRoutes(vector<int> vectA, int elemA, vector<int> vectB, int elemB)
{
    bool firstA, firstB;
    if (vectA[0] == elemA)
    {
        firstA = true;
    }
    else
    {
        firstA = false;
    }
    if (vectB[0] == elemB)
    {
        firstB = true;
    }
    else
    {
        firstB = false;
    }

    if (firstA == firstB && vectA.size() > 1 && vectB.size() > 1)
    {
        if (vectA.size() < vectB.size())
        {
            reverse(vectA.begin(), vectA.end());
        }
        else
        {
            reverse(vectB.begin(), vectB.end());
        }
    }

    vector<int> v;
    if(vectA.back() == elemA && vectB[0] == elemB)
    {
        v.insert(v.end(), vectA.begin(), vectA.end());
        v.insert(v.end(), vectB.begin(), vectB.end());
    }
    else
    {
        v.insert(v.end(), vectB.begin(), vectB.end());
        v.insert(v.end(), vectA.begin(), vectA.end());
    }
    return v;
}

bool depositConect(vector<int> route, int point)
{
    if (route[0] == point)
    {
        return true;
    }
    else if (route.back() == point)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

// Struct utilizada para descrever as economias
struct saving
{
    int pointA, pointB;
    double saving;
};

bool operator< (const saving sav1, const saving sav2)
{
	return sav1.saving < sav2.saving;	
}