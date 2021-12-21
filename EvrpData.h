#include "EvrpUtils.h"
class EvrpData {
public:
	EvrpData(string instance);
    int pointsSize();
    vector<double> demands();
    double loadCapacity();
    vector<vector<double>> distMatrix();
    vector<vector<double>> energyMatrix();
    int lastPoint();
    double requiredEnergyOneRoute(vector<int> route);

    int _lastPoint;
	vector<pair<double, double>> _points;
    vector<double> _demands;
    double _loadCapacity;
	vector<vector<double>> _distMatrix;
	vector<vector<double>> _energyMatrix;
};

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
    _lastPoint = sizePoints - 1;
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

int EvrpData::lastPoint(){
    return _lastPoint;
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

double EvrpData::requiredEnergyOneRoute(vector<int> route) {
    double routeEnergy = 0;
    double demandSum = 0;
    for (int i = 0; i < route.size(); i++)
    {
        int elem = route[i];
        demandSum += _demands[elem];
    }

    int dest = route[0];
    routeEnergy += _energyMatrix[0][dest];
    routeEnergy += _distMatrix[0][dest] * demandSum * ENERGY_CONST_LOAD;
    demandSum -= _demands[dest];
    for (int i = 0; i < route.size() - 1; i++)
    {
        int orig = route[i];
        int dest = route[i+1];
        routeEnergy += _energyMatrix[orig][dest];
        routeEnergy += _distMatrix[orig][dest] * demandSum * ENERGY_CONST_LOAD;
        demandSum -= _demands[dest];
    }
    int idx = route.size() - 1;
    int orig = route[idx];
    routeEnergy += _energyMatrix[orig][0];
    return routeEnergy;
}