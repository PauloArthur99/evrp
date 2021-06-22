#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
#include <algorithm>
using namespace std;
#define ENERGY_CONST 275795.1416

// Retorna a distancia euclidiana de dois pontos estruturados como pairs.
double distancePoints(pair<double, double> a, pair<double, double> b)
{
    double pot1, pot2;
    pot1 = pow((b.first - a.first), 2);
    pot2 = pow((b.second - a.second), 2);
    return sqrt(pot1 + pot2);
}

// escreve as rotas encontradas em um arquivo
void writeSolution(vector<vector<int>> routes, string fileName)
{
    ofstream MyFile(fileName); 
    MyFile << "RouteID    RoutePoints\n";

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