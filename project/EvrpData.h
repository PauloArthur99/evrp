#include "EvrpUtils.h"
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