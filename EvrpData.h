#include "EvrpUtils.h"
class EvrpData {
public:
	EvrpData(string instance);
    int pointsSize();
    int pointsStationsSize();
    vector<double> demands();
    double loadCapacity();
    vector<vector<double>> distMatrix();
    vector<vector<double>> distMatrixStations();
    vector<vector<double>> energyMatrix();
    vector<vector<double>> energyMatrixStations();
    int lastPoint();
    double requiredEnergyOneRoute(vector<int> route);
    bool requiredDemand(vector<int> route);
    double requiredEnergyJoined(vector<int> route1, vector<int> route2, int idxStation);
    double requiredEnergyRouteStations(vector<int> route);
    double requiredTotalEnergyRouteStations(vector<int> route);

    int _lastPoint;
	vector<pair<double, double>> _points, _pointsStations;
    vector<double> _demands;
    double _loadCapacity;
	vector<vector<double>> _distMatrix, _distMatrixStations, _energyMatrix, _energyMatrixStations;
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
        if (type == "d" or type == "c")
        {            
            pair <double, double> pointPair (stod(x), stod(y));
            _points.push_back(pointPair);
            
            string pointDemand;
            ss >> pointDemand; 
            _demands.push_back(stod(pointDemand));
        }
        else if (type == "f")
        {
            pair <double, double> pointPair (stod(x), stod(y));
            _pointsStations.push_back(pointPair);
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
    vector<vector<double>> auxMatrixStations(_pointsStations.size(), vector<double>(sizePoints, 0.0));

	_distMatrix = auxMatrix;
    _distMatrixStations = auxMatrixStations;
	_energyMatrix = auxMatrix;
    _energyMatrixStations = auxMatrixStations;

    // Preenchimento da matriz de distâncias e de energias.
    for (int i = 0; i < sizePoints; i++)
    {
        for (int j = 0; j < sizePoints; j++)
        {
            _distMatrix[i][j] = distancePoints(_points[i], _points[j]) * km_to_meter;
            _energyMatrix[i][j] = ENERGY_CONST * _distMatrix[i][j];
        }
    }

    // Preenchimento da matriz de energias das estações de recarga.
    for (int i = 0; i < _pointsStations.size(); i++)
    {
        for (int j = 0; j < sizePoints; j++)
        {
            _distMatrixStations[i][j] = distancePoints(_pointsStations[i], _points[j]) * km_to_meter;
            _energyMatrixStations[i][j] = ENERGY_CONST * _distMatrixStations[i][j];
        }
    }

    for (int i = 0; i < pointsStationsSize(); i++) {
        _demands.push_back(0);
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

vector<vector<double>> EvrpData::distMatrixStations(){
    return _distMatrixStations;
}

vector<vector<double>> EvrpData::energyMatrixStations(){
    return _energyMatrixStations;
}

double EvrpData::loadCapacity(){
    return _loadCapacity;
}

int EvrpData::pointsSize() {
    return _points.size();
}

int EvrpData::pointsStationsSize() {
    return _pointsStations.size();
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

bool EvrpData::requiredDemand(vector<int> route) {
    double demandSum = 0;
    for (int i = 0; i < route.size(); i++)
    {
        int elem = route[i];
        demandSum += _demands[elem];
    }
    if (demandSum <= _loadCapacity){
        return true;
    } else {
        return false;
    } 
}

double EvrpData::requiredEnergyJoined(vector<int> route1, vector<int> route2, int idxStation) {
    vector<vector<double>> energyMatrix = _energyMatrix;
    vector<vector<double>> distMatrix = _distMatrix;
    for (int i = 0; i < pointsStationsSize(); i++) {
        energyMatrix.push_back(_energyMatrixStations[i]);
        distMatrix.push_back(_distMatrixStations[i]);
    }

    double routeEnergy = 0;
    double demandSum = 0;
    for (int i = 0; i < route1.size(); i++)
    {
        int elem = route1[i];
        demandSum += _demands[elem];
    }

    for (int i = 0; i < route2.size(); i++)
    {
        int elem = route2[i];
        demandSum += _demands[elem];
    }

    int dest = route1[0];
    routeEnergy += _energyMatrix[0][dest];
    routeEnergy += _distMatrix[0][dest] * demandSum * ENERGY_CONST_LOAD;
    demandSum -= _demands[dest];
    for (int i = 0; i < route1.size() - 1; i++)
    {
        int orig = route1[i];
        int dest = route1[i+1];
        if (orig >= pointsSize())
        {
            routeEnergy = 0;
        }
        routeEnergy += energyMatrix[orig][dest];
        routeEnergy += distMatrix[orig][dest] * demandSum * ENERGY_CONST_LOAD;
        demandSum -= _demands[dest];
    }
    int idxCustomer = route1.size() - 1;
    int customer = route1[idxCustomer];
    routeEnergy += _energyMatrixStations[idxStation][customer];
    routeEnergy += _distMatrixStations[idxStation][customer] * demandSum * ENERGY_CONST_LOAD;
    return routeEnergy;
}

double EvrpData::requiredEnergyRouteStations(vector<int> route) {
    vector<vector<double>> energyMatrix = _energyMatrix;
    vector<vector<double>> distMatrix = _distMatrix;
    for (int i = 0; i < pointsStationsSize(); i++) {
        energyMatrix.push_back(_energyMatrixStations[i]);
        distMatrix.push_back(_distMatrixStations[i]);
    }

    double routeEnergy = 0;
    double demandSum = 0;
    for (int i = 0; i < route.size(); i++)
    {
        int elem = route[i];
        demandSum += _demands[elem];
    }

    int dest = route[0];
    routeEnergy += energyMatrix[0][dest];
    routeEnergy += distMatrix[0][dest] * demandSum * ENERGY_CONST_LOAD;
    demandSum -= _demands[dest];
    for (int i = 0; i < route.size() - 1; i++)
    {
        int orig = route[i];
        int dest = route[i+1];
        if (orig >= pointsSize())
        {
            routeEnergy = 0;
        }
        routeEnergy += energyMatrix[orig][dest];
        routeEnergy += distMatrix[orig][dest] * demandSum * ENERGY_CONST_LOAD;
        demandSum -= _demands[dest];
    }
    int idx = route.size() - 1;
    int orig = route[idx];
    routeEnergy += energyMatrix[orig][0];
    return routeEnergy;
}

double EvrpData::requiredTotalEnergyRouteStations(vector<int> route) {
    vector<vector<double>> energyMatrix = _energyMatrix;
    vector<vector<double>> distMatrix = _distMatrix;
    for (int i = 0; i < pointsStationsSize(); i++) {
        energyMatrix.push_back(_energyMatrixStations[i]);
        distMatrix.push_back(_distMatrixStations[i]);
    }

    double routeEnergy = 0;
    double demandSum = 0;
    for (int i = 0; i < route.size(); i++)
    {
        int elem = route[i];
        demandSum += _demands[elem];
    }

    int dest = route[0];
    routeEnergy += energyMatrix[0][dest];
    routeEnergy += distMatrix[0][dest] * demandSum * ENERGY_CONST_LOAD;
    demandSum -= _demands[dest];
    for (int i = 0; i < route.size() - 1; i++)
    {
        int orig = route[i];
        int dest = route[i+1];
        routeEnergy += energyMatrix[orig][dest];
        routeEnergy += distMatrix[orig][dest] * demandSum * ENERGY_CONST_LOAD;
        demandSum -= _demands[dest];
    }
    int idx = route.size() - 1;
    int orig = route[idx];
    routeEnergy += energyMatrix[orig][0];
    return routeEnergy;
}