using namespace std;
class EvrpSolution {
public:
    void set_route(int i, vector<int> new_route);
    void set_routes(vector<vector<int>> new_routes);
    vector<vector<int>> routes();
    int routes_size();
    vector<double> routesEnergy();
    EvrpSolution(EvrpData* evrpDataParameter);
    void savingsAlg();
    void insertDepoAndEnergies();   
    tuple<bool, vector<int>, double> twoOptFirst(int idxRoute);
    tuple<bool, vector<int>, double> twoOptFirstWithStations(int idxRoute);
    tuple<bool, vector<int>, double> twoOptBest(int idxRoute);
    tuple<bool, vector<int>, double> get_neighbor(int idxRoute, int neighborhood);
    double requiredEnergySolution();
    bool twoOptStarFirst();
    bool twoOptStarBest();
    bool joinRoutesSolution();
    void shuffle();


    EvrpData* _evrpData;
    vector<vector<int>> _routes;
    vector<double> _routesEnergy;
};

int EvrpSolution::routes_size(){
    return _routes.size();
}

void EvrpSolution::shuffle() {
    // obtain a time-based seed:
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);

    srand(time(0));
    int rand_idx = rand() % routes_size();
    std::shuffle(_routes[rand_idx].begin(), _routes[rand_idx].end(), e);
}

vector<vector<int>> EvrpSolution::routes() {
    return _routes;
}

void EvrpSolution::set_route(int i, vector<int> new_route) {
    _routes[i] =  new_route;
}

void EvrpSolution::set_routes(vector<vector<int>> new_routes) {
    _routes =  new_routes;
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

                    if (spentEnergy <= ENERGY_BATTERY && spentEnergyReverse <= ENERGY_BATTERY)
                    {
                        vector<int> routesJoined = joinTwoRoutesSavings(_routes[idxA], pointA, _routes[idxB], pointB);
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
    vector<vector<double>> distMatrixStations = _evrpData->distMatrixStations();
    vector<vector<double>> energyMatrix = _evrpData->energyMatrix();
    vector<vector<double>> energyMatrixStations = _evrpData->energyMatrixStations();
    int stationsSize = _evrpData->pointsStationsSize();

    for (int i = 0; i < stationsSize; i++) {
        energyMatrix.push_back(energyMatrixStations[i]);
        distMatrix.push_back(distMatrixStations[i]);
    }

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
            if (newEnergy < bestEnergy)
            {
            bestRoute = tempRoute;
            bestEnergy = newEnergy;
            bestFound = true;
            }
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
    #define VIZINHO_2OPT_STATIONS 2

    if (neighborhood == VIZINHO_2OPT_FIRST) {
        return twoOptFirst(idxRoute);
    } else if (neighborhood == VIZINHO_2OPT_BEST) {
        return twoOptBest(idxRoute);
    } else {
        return twoOptFirstWithStations(idxRoute);
    }
}

// Estrutura de vizinhança 2-opt* FirstFit.
bool EvrpSolution::twoOptStarFirst()
{
    vector<int> route1, route2;
    vector<int> temp1, temp2;

    for (int i = 0; i < _routes.size(); i++) {
        tuple<bool, vector<int>, double> returnTuple;
        returnTuple = twoOptFirst(i);
        if (get<0>(returnTuple)) {
            _routes[i] = get<1>(returnTuple);
            return true;
        }
    }

    for (int idxRoute1 = 0; idxRoute1 < _routes.size() - 1; idxRoute1++){
        route1 = _routes[idxRoute1];
        for (int idxRoute2 = idxRoute1 + 1; idxRoute2 < _routes.size(); idxRoute2++){
            route2 = _routes[idxRoute2];
            
            double oldEnergy = _evrpData->requiredEnergyOneRoute(_routes[idxRoute1]);
            oldEnergy += _evrpData->requiredEnergyOneRoute(_routes[idxRoute2]);

            for (int i = 1; i < route1.size(); i++)
            {
                for (int j = 1; j < route2.size(); j++)
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

                    double newEnergy = _evrpData->requiredEnergyOneRoute(temp1);
                    newEnergy += _evrpData->requiredEnergyOneRoute(temp2);

                    
                    if (newEnergy < oldEnergy)
                    {
                        bool demandOK = _evrpData->requiredDemand(temp1) && _evrpData->requiredDemand(temp2);
                        if (demandOK)
                        {
                            _routes[idxRoute1] = temp1;
                            _routes[idxRoute2] = temp2;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

double EvrpSolution::requiredEnergySolution(){
    vector<int> tempRoute;
    double totalEnergy = 0;
    for (int i = 0; i < _routes.size(); i++){
        tempRoute = _routes[i];
        totalEnergy += _evrpData->requiredEnergyOneRoute(tempRoute);
    }
    return totalEnergy;
}

// Estrutura de vizinhança 2-opt* BestFit.
bool EvrpSolution::twoOptStarBest()
{
    vector<int> route1, route2;
    vector<int> temp1, temp2, best1, best2;
    bool bestFound = false;
    double bestEconomy = 0;
    int bestIdx1, bestIdx2;


    for (int i = 0; i < _routes.size(); i++) {
        tuple<bool, vector<int>, double> returnTuple;
        returnTuple = twoOptBest(i);
        if (get<0>(returnTuple)) {
            _routes[i] = get<1>(returnTuple);
            return true;
        }
    }

    for (int idxRoute1 = 0; idxRoute1 < _routes.size() - 1; idxRoute1++){
        route1 = _routes[idxRoute1];
        for (int idxRoute2 = idxRoute1 + 1; idxRoute2 < _routes.size(); idxRoute2++){
            route2 = _routes[idxRoute2];
            
            double oldEnergy = _evrpData->requiredEnergyOneRoute(_routes[idxRoute1]);
            oldEnergy += _evrpData->requiredEnergyOneRoute(_routes[idxRoute2]);

            for (int i = 1; i < route1.size(); i++)
            {
                for (int j = 1; j < route2.size(); j++)
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

                    double newEnergy = _evrpData->requiredEnergyOneRoute(temp1);
                    newEnergy += _evrpData->requiredEnergyOneRoute(temp2);

                    if (newEnergy < oldEnergy)
                    {
                        bool demandOK = _evrpData->requiredDemand(temp1) && _evrpData->requiredDemand(temp2);
                        if (demandOK)
                        {
                            if ((oldEnergy - newEnergy) > bestEconomy) {
                                best1 = temp1;
                                best2 = temp2;
                                bestIdx1 = idxRoute1;
                                bestIdx2 = idxRoute2;
                                bestFound = true;
                            }
                        }
                    }
                }
            }
        }
    }

    if (bestFound)
    {
    _routes[bestIdx1] = best1;
    _routes[bestIdx2] = best2;
    return true;        
    }
    return false;
}

bool EvrpSolution::joinRoutesSolution()
{
    vector<int> route1, route2;
    vector<vector<double>> energyMatrix = _evrpData->energyMatrix();
    vector<vector<double>> energyMatrixStations = _evrpData->energyMatrixStations();
    /*
    for (int i = 0; i < _evrpData->pointsStationsSize(); i++) {
        energyMatrix.push_back(energyMatrixStations[i]);
    }
    */
    for (int idxRoute1 = 0; idxRoute1 < _routes.size() - 1; idxRoute1++) {
        route1 = _routes[idxRoute1];

        for (int idxRoute2 = idxRoute1 + 1; idxRoute2 < _routes.size(); idxRoute2++) {
            route2 = _routes[idxRoute2];
            vector<int> tempRoute = joinTwoRoutes(route1, route2);
            bool demandOK = _evrpData->requiredDemand(tempRoute);

            if (demandOK) {
                for (int idxStation = 0; idxStation < _evrpData->pointsStationsSize(); idxStation++) {
                    double newEnergyRoute1 = _evrpData->requiredEnergyJoined(route1, route2, idxStation);

                    if (newEnergyRoute1 <= ENERGY_BATTERY) {
                        int first2 = route2[0];
                        double distDepoFirst = energyMatrix[0][first2];
                        double stationDistance2 = energyMatrixStations[idxStation][first2];

                        if (stationDistance2 <= distDepoFirst){
                            route1.push_back(idxStation + _evrpData->pointsSize());
                            vector<int> newRoute = joinTwoRoutes(route1, route2);
                            _routes.push_back(newRoute);
                            _routes.erase(_routes.begin() + idxRoute1);
                            _routes.erase(_routes.begin() + idxRoute2 - 1);
                            return true;
                        } else {
                            double newEnergyRoute2 = _evrpData->requiredEnergyRouteStations(route2) 
                                                    - distDepoFirst + stationDistance2;
                            if (newEnergyRoute2 <= ENERGY_BATTERY) {
                                route1.push_back(idxStation + _evrpData->pointsSize());
                                vector<int> newRoute = joinTwoRoutes(route1, route2);
                                _routes.push_back(newRoute);
                                _routes.erase(_routes.begin() + idxRoute1);
                                _routes.erase(_routes.begin() + idxRoute2 - 1);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Estrutura de vizinhança 2-opt.
tuple<bool, vector<int>, double> EvrpSolution::twoOptFirstWithStations(int idxRoute)
{
    vector<int> route = _routes[idxRoute];
    vector<int> tempRoute;
    double energyOrigRoute = _evrpData->requiredTotalEnergyRouteStations(route);
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
        double tempEnergy = _evrpData->requiredEnergyRouteStations(tempRoute);
        if (tempEnergy <= ENERGY_BATTERY) {
            double newEnergy = _evrpData->requiredTotalEnergyRouteStations(tempRoute);
            if (newEnergy < energyOrigRoute)
            {
                tuple<bool, vector<int>, double> returnTuple;
                returnTuple = make_tuple(true, tempRoute, newEnergy);
                return returnTuple;
            }
        }
    }
    tuple<bool, vector<int>, double> returnTuple;
    returnTuple = make_tuple(false, route, 0);
    return returnTuple;
}