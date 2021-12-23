using namespace std;
class EvrpSolution {
public:
    void set_route(int i, vector<int> new_route);
    vector<vector<int>> routes();
    int routes_size();
    vector<double> routesEnergy();
    EvrpSolution(EvrpData* evrpDataParameter);
    void savingsAlg();
    void insertDepoAndEnergies();   
    tuple<bool, vector<int>, double> twoOptFirst(int idxRoute);
    tuple<bool, vector<int>, double> twoOptBest(int idxRoute);
    tuple<bool, vector<int>, double> get_neighbor(int idxRoute, int neighborhood);
    double requiredEnergySolution();

    EvrpData* _evrpData;
    vector<vector<int>> _routes;
    vector<double> _routesEnergy;
};

vector<vector<int>> EvrpSolution::routes() {
    return _routes;
}

void EvrpSolution::set_route(int i, vector<int> new_route) {
    _routes[i] =  new_route;
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

// Estrutura de vizinhança 2-opt.
tuple<bool, vector<int>, double> EvrpSolution::twoOptFirst(int idxRoute)
{
    vector<int> route = _routes[idxRoute];
    vector<int> tempRoute;
    double energyOrigRoute = _evrpData->requiredEnergyOneRoute(route);
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
        double newEnergy = _evrpData->requiredEnergyOneRoute(tempRoute);
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

// Estrutura de vizinhança 2-opt Best.
tuple<bool, vector<int>, double> EvrpSolution::twoOptBest(int idxRoute)
{
    vector<int> route = _routes[idxRoute];
    double energyOrigRoute = _evrpData->requiredEnergyOneRoute(route);
    vector<int> tempRoute = route;
    vector<int> bestRoute;
    bool bestFound = false;
    double bestEnergy = std::numeric_limits<double>::infinity();
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
        double newEnergy = _evrpData->requiredEnergyOneRoute(tempRoute);
        if (newEnergy < energyOrigRoute)
        {
            bestRoute = tempRoute;
            bestEnergy = newEnergy;
            bestFound = true;
        }
    }
    tuple<bool, vector<int>, double> returnTuple;
    if (bestFound)
    {
        returnTuple = make_tuple(true, bestRoute, bestEnergy);
        return returnTuple;
    }else{
        returnTuple = make_tuple(false, route, 0);
        return returnTuple;
    }
}

tuple<bool, vector<int>, double> EvrpSolution::get_neighbor(int idxRoute, int neighborhood) {
    #define VIZINHO_2OPT_FIRST 0
    #define VIZINHO_2OPT_BEST 1

    if (neighborhood == VIZINHO_2OPT_FIRST) {
        return twoOptFirst(idxRoute);
    } else {
        return twoOptBest(idxRoute);
    }
}

int EvrpSolution::routes_size(){
    return _routes.size();
}

// Estrutura de vizinhança 2-opt*.
bool twoOptStarFirst()
{
    double energyOrig = requiredEnergySolution();
    vector<int> route1, route2;
    vector<int> temp1, temp2;

    for (int idxRoute1 = 0; idxRoute1 < _routes.size(); idxRoute1++){
        route1 = _routes[idxRoute1];
        for (int idxRoute2 = 0; idxRoute2 < _routes.size(); idxRoute2++){
            route2 _routes[idxRoute2];
            for (int i = 1; i < route.size(); i++)
            {
                for (int j = 1; j < route.size(); j++)
                {
                    vector<int> routeModified1, routeModified2;
                    for (int k = 0; k < i; k++)
                    {
                    routeModified1.push_back(route1[k]);
                    }
                    for (int k = j; k < route2.size(); k++)
                    {
                    routeModified1.push_back(route2[k]);
                    }
                    
                    for (int k = 0; k < j; k++)
                    {
                    routeModified2.push_back(route2[k]);
                    }
                    for (int k = i; k < route1.size(); k++)
                    {
                    routeModified2.push_back(route1[k]);
                    }
                    temp1 = routeModified1;
                    temp2 = routeModified2;
                }

                double newEnergy = requiredEnergyOneRoute(temp1);
                newEnergy += requiredEnergyOneRoute(temp2);
                vector<int> auxRoute;
                for (int i = 0; i < _routes.size(); i++){
                    if (i == idxRoute1 || i == idxRoute2){
                        continue;
                    }
                    auxRoute = _routes[i];
                    newEnergy += _evrpData->requiredEnergyOneRoute(auxRoute)
                }
                if (newEnergy < energyOrig)
                {
                    _routes[idxRoute1] = temp1;
                    _routes[idxRoute2] = temp2;
                    return true;
                }
            }
        }
    }
    tuple<bool, vector<int>, double> returnTuple;
    returnTuple = make_tuple(false, route, 0);
    return returnTuple;
}

double EvrpSolution::requiredEnergySolution(){
    vector<int> tempRoute;
    double totalEnergy = 0;
    for (int i = 0; i < _routes.size(); i++){
        tempRoute = _routes[i];
        totalEnergy += _evrpData->requiredEnergyOneRoute(tempRoute)
    }
    return totalEnergy;
}