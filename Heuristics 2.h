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