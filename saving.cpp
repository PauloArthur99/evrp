#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
using namespace std;
#define ENERGY_CONST 275795.1416

// Retorna a distancia euclidiana de dois pontos estruturados como pairs.
double distancePoints(pair<double, double> a, pair<double, double> b){
    double pot1, pot2;

    pot1 = pow((b.first - a.first), 2);
    pot2 = pow((b.second - a.second), 2);
    return sqrt(pot1 + pot2);
}

// Struct utilizada para descrever as economias
struct saving
{
    int pointA, pointB;
    double saving;
};

bool operator< (const saving sav1, const saving sav2)
{
	return sav1.saving > sav2.saving;	
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
            savingQueue.push(savingStruct);
        }
    }
    cout << savingQueue.top().saving;
    savingQueue.pop();
    cout << savingQueue.top().saving;
    cout << savingQueue.top().saving;
    savingQueue.pop();
    cout << savingQueue.top().saving;
    cout << savingQueue.top().saving;
    savingQueue.pop();
    cout << savingQueue.top().saving;
    cout << savingQueue.top().saving;
    savingQueue.pop();
    cout << savingQueue.top().saving;

	return 0;
}
