#include "saving.h"

class EvrpData {
public:
	EvrpData(string instance);
    int pointsSize();
    vector<double> demands();
    double loadCapacity();
    vector<vector<double>> distMatrix();
    vector<vector<double>> energyMatrix();
    

	vector<pair<double, double>> _points;
    vector<double> _demands;
    double _loadCapacity;
	vector<vector<double>> _distMatrix;
	vector<vector<double>> _energyMatrix;
}

EvrpData::EvrpData(string instance) {
    string myText;
	ifstream MyReadFile(instance);
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
            _points.push_back(pointPair);
            
            string pointDemand;
            ss >> pointDemand; 
            _demands.push_back(stod(pointDemand));
        }
        else if (word == "C")
        {
            string loadCapacityStr;
            ss >> loadCapacityStr;
            loadCapacityStr.erase(0, 1);
            loadCapacityStr.pop_back();
            _loadCapacity = stod(loadCapacityStr);
        }
    }
    MyReadFile.close();

	int sizePoints = _points.size();
    int lastPoint = sizePoints - 1;
	vector<vector<double>> auxMatrix(sizePoints, vector<double>(sizePoints, 0.0));

	_distMatrix = auxMatrix;
	_energyMatrix = auxMatrix;

    // Preenchimento da matriz de distâncias e de energias.
    for (int i = 0; i < sizePoints; i++)
    {
        for (int j = 0; j < sizePoints; j++)
        {
            _distMatrix[i][j] = distancePoints(_points[i], _points[j]);
            _energyMatrix[i][j] = ENERGY_CONST * _distMatrix[i][j];
        }
    }
}

vector<double> EvrpData::demands(){
    return _demands;
}

vector<vector<double>> EvrpData::energyMatrix(){
    return _energyMatrix;
}

vector<vector<double>> EvrpData::distMatrix(){
    return _distMatrix;
}

double EvrpData::loadCapacity(){
    return _loadCapacity;
}

int EvrpData::pointsSize() {
    return _points.size();
}

class EvrpSolution {
public:
    EvrpData _evrpData;
    vector<vector<int>> _routes;
    EvrpSolution(EvrpData evrpDataParameter);
    savingsAlg();
    
}

EvrpSolution::EvrpSolution(EvrpData evrpDataParameter) {
    _evrpData = evrpDataParameter;
    for (int i = 1; i <= lastPoint; i++)
    {
        vector<int> aux{i};
        _routes.push_back(aux);
    }
}

EvrpSolution::savingsAlg() {
    priority_queue<saving> savingQueue;
    // Cálculo das economias das arestas
    for (int i = 1; i < _evrpData.pointsSize() - 1; i++)
    {
        for (int j = i + 1; j <= _evrpData.pointsSize() - 1; j++)
        {
            double savingIJ = _evrpData.distMatrix()[0][i] + _evrpData.distMatrix()[0][j] - _evrpData.distMatrix()[i][j];
            struct saving savingStruct;
            savingStruct.pointA = i;
            savingStruct.pointB = j;
            savingStruct.saving = savingIJ;
            if (savingIJ > 0)
                savingQueue.push(savingStruct);
        }
    }

    while (!savingQueue.empty())
    {
        int pointA, idxA, pointB, idxB;
        pointA = savingQueue.top().pointA;
        pointB = savingQueue.top().pointB;
        savingQueue.pop();
        
        for (int i = 0; i < _routes.size(); i++)
        {
            for (int j = 0; j < _routes[i].size(); j++)
            {
                if (_routes[i][j] == pointA)
                {
                    idxA = i;
                }
                else if(_routes[i][j] == pointB)
                {
                    idxB = i;
                }
            }
        }
        if (!(_routes[idxA] == _routes[idxB]))
        {
            if (depositConect(_routes[idxA], pointA) && depositConect(_routes[idxB], pointB))
            {   
                double demandSum = 0;
                for (int i = 0; i < _routes[idxA].size(); i++)
                {
                    int elem = _routes[idxA][i];
                    demandSum += _evrpData.demands()[elem];
                }
                for (int i = 0; i < _routes[idxB].size(); i++)
                {
                    int elem = _routes[idxB][i];
                    demandSum += _evrpData.demands()[elem];
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
                        _routes.push_back(routesJoined);

                        if (idxA < idxB)
                        {
                            _routes.erase(routes.begin() + idxA);
                            _routes.erase(routes.begin() + idxB - 1);
                        }
                        else
                        {
                            _routes.erase(routes.begin() + idxA);
                            _routes.erase(routes.begin() + idxB);
                        }
                    }                    
                }
            }
        }     
    }

}
