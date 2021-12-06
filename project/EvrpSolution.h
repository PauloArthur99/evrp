class EvrpSolution {
public:
    EvrpData* _evrpData;
    vector<vector<int>> _routes;
    vector<double> _routesEnergy;
    vector<vector<int>> routes();
    vector<double> routesEnergy();
    EvrpSolution(EvrpData* evrpDataParameter);
    void savingsAlg();
    void insertDepoAndEnergies();   
};

vector<vector<int>> EvrpSolution::routes() {
    return _routes;
}

vector<double> EvrpSolution::routesEnergy() {
    return _routesEnergy;
}

EvrpSolution::EvrpSolution(EvrpData* evrpDataParameter) {
    _evrpData = evrpDataParameter;
    for (int i = 1; i <= _evrpData->lastPoint(); i++)
    {
        vector<int> aux{i};
        _routes.push_back(aux);
    }
}

void EvrpSolution::savingsAlg() {
    priority_queue<saving> savingQueue;
    vector<vector<double>> distMatrix = _evrpData->distMatrix();
    vector<vector<double>> energyMatrix = _evrpData->energyMatrix();
    vector<double> demands = _evrpData->demands();

    // Cálculo das economias das arestas
    for (int i = 1; i < _evrpData->pointsSize() - 1; i++)
    {
        for (int j = i + 1; j <= _evrpData->pointsSize() - 1; j++)
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
                    demandSum += demands[elem];
                }
                for (int i = 0; i < _routes[idxB].size(); i++)
                {
                    int elem = _routes[idxB][i];
                    demandSum += demands[elem];
                } 

                if (demandSum <= _evrpData->loadCapacity())
                {   
                    double spentEnergy = requiredEnergy(distMatrix, energyMatrix, 
                    demands, _routes[idxA], _routes[idxB], demandSum, pointA, pointB);

                    double spentEnergyReverse = requiredEnergy(distMatrix, energyMatrix, 
                    demands, _routes[idxB], _routes[idxA], demandSum, pointB, pointA);

                    if (spentEnergy <= 18000 && spentEnergyReverse <= 18000)
                    {
                        vector<int> routesJoined = joinTwoRoutes(_routes[idxA], pointA, _routes[idxB], pointB);
                        _routes.push_back(routesJoined);

                        if (idxA < idxB)
                        {
                            _routes.erase(_routes.begin() + idxA);
                            _routes.erase(_routes.begin() + idxB - 1);
                        }
                        else
                        {
                            _routes.erase(_routes.begin() + idxA);
                            _routes.erase(_routes.begin() + idxB);
                        }
                    }                    
                }
            }
        }     
    }

}

void EvrpSolution::insertDepoAndEnergies() {
    vector<double> demands = _evrpData->demands();
    vector<vector<double>> distMatrix = _evrpData->distMatrix();
    vector<vector<double>> energyMatrix = _evrpData->energyMatrix();

    for (int i = 0; i < _routes.size(); i++)
    {
        double customerDemand = 0;
        double energySum = 0;
        for (int j = 0; j < _routes[i].size(); j++)
        {
            int customer = _routes[i][j];
            customerDemand += demands[customer];
        }

        _routes[i].insert(_routes[i].begin(), 0);
        _routes[i].push_back(0);

        for (int j = 0; j < _routes[i].size()-1; j++)
        {
            int orig = _routes[i][j];
            int dest = _routes[i][j+1];
            energySum += energyMatrix[orig][dest];
            energySum += distMatrix[orig][dest] * customerDemand * ENERGY_CONST_LOAD;
            customerDemand -= demands[dest];
        }
        _routesEnergy.push_back(energySum);
    }
}