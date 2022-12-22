void hillClimbingEachRoute(EvrpSolution* evrpSolution, int neighborhood){
    for (int i = 0; i < evrpSolution->routes_size(); i++)
    {
        bool ok = true;
        vector<int> routeModified;
        tuple<bool, vector<int>, double> returnTuple;
        while (ok)
        {
            returnTuple = evrpSolution->get_neighbor(i, neighborhood);
            ok = get<0>(returnTuple);
            routeModified = get<1>(returnTuple);
            //requiredEnergy = get<2>(returnTuple);
            if (ok)
            {
                evrpSolution->set_route(i, routeModified);
            }
        }
    }
}

void hillClimbingSolution(EvrpSolution* evrpSolution, int neighborhood){
    #define VIZINHO_2OPT_STAR_FIRST 0
    #define VIZINHO_2OPT_STAR_BEST 1
    bool ok = true;
    while (ok)
    {
        ok = evrpSolution->twoOptStarFirst();
    }
    
}

void joinRoutes(EvrpSolution* evrpSolution){
    bool ok = true;
    while (ok)
    {
        ok = evrpSolution->joinRoutesSolution();
    }
}

void pertubation(EvrpSolution* evrpSolution, EvrpSolution* evrpSolutionCopy){
    evrpSolutionCopy->set_routes(evrpSolution->routes());
    int iter_counter = 0;
    while (true)
    {
        evrpSolutionCopy->shuffle();
        hillClimbingSolution(evrpSolutionCopy, 0);
        

        if (evrpSolutionCopy->requiredEnergySolution() < evrpSolution->requiredEnergySolution() && evrpSolution->feasibleRoutes()) {
            evrpSolution->set_routes(evrpSolutionCopy->routes());
            iter_counter = 0;
        } else {
            evrpSolutionCopy->set_routes(evrpSolution->routes());
            iter_counter++;
        }
        if (iter_counter == 100){
            break;
        }
    }
    
}