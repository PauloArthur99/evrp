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

vector<int> joinTwoRoutes(vector<int> vectA, int elemA, vector<int> vectB, int elemB)
{
    bool firstA, firstB;
    if (vectA[0] = elemA)
    {
        firstA = true;
    }
    else
    {
        firstA = false;
    }
    if (vectB[0] = elemB)
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

int main(){
    string myText;

    vector<pair<double, double>> points;
    vector<double> demands;

    priority_queue<saving> savingQueue;

    ifstream MyReadFile("c101_21.txt");
    // Pulando a linha de cabeçalho.
    getline(MyReadFile, myText);
    
    // Laço de leitura dos dados do arquivo.
    while(getline(MyReadFile, myText)){
        // Used to split string around spaces.
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
            if (type == "c")
            {
                string pointDemand;
                ss >> pointDemand; 
                demands.push_back(stod(pointDemand));
            }
        }
        else if (type == "f")
        {
        }
        else
        {
            break;
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
            for (int j = 0; i < routes[i].size(); i++)
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
        if (routes[idxA] != routes[idxB])
        {
            if (depositConect(routes[idxA], pointA), depositConect(routes[idxB], pointB))
            {
                // if (capacidade)
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

	return 0;
}
