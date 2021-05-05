#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;

// Retorna a distancia euclidiana de dois pontos estruturados como pares.
double distancePoints(pair<double, double> a, pair<double, double> b){
    double pot1, pot2;

    pot1 = pow((b.first - a.first), 2);
    pot2 = pow((b.second - a.second), 2);
    return sqrt(pot1 + pot2);
}

int main(){
    string myText;

    vector<pair<double, double>> points;
    vector<double> demands;

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
        if (type == "d" or type == "f" or type == "c")
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
        else
        {
            break;
        }
    }
    MyReadFile.close();

    int sizePoints = points.size();

    //matriz de distancias.
    vector<vector<double>> distMatrix(sizePoints, vector<double>(sizePoints, 0.0) );

    // Preenchimento da matriz de distâncias.
    for (int i = 0; i < points.size(); i++)
    {
        for (int j = 0; j < points.size(); j++)
        {
            distMatrix[i][j] = distancePoints(points[i], points[j]);
        }
    }
	return 0;
}
