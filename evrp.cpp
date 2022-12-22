#include "EvrpData.h"
#include "EvrpSolution.h"
#include "Heuristics.h"
using namespace std::chrono;
using namespace std;

int main(int argc, char** argv){
    auto start = high_resolution_clock::now();
    EvrpData evrpDataobj = EvrpData(argv[1]);
    EvrpSolution evrpSolution = EvrpSolution(&evrpDataobj);

    evrpSolution.savingsAlg();

    hillClimbingSolution(&evrpSolution, 0);

    EvrpSolution evrpSolutionCopy = EvrpSolution(&evrpDataobj);
    pertubation(&evrpSolution, &evrpSolutionCopy);

    joinRoutes(&evrpSolution);
    hillClimbingEachRoute(&evrpSolution, 2);
    evrpSolution.insertDepoAndEnergies();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    double duration_time = duration.count();

    string strSolucao = "/Users/paulo/Desktop/evrp/evrptw_instances/soluções";
    strSolucao = strSolucao + argv[2] + "/solução";
    strSolucao = strSolucao + argv[1];
    writeSolutionSavings(evrpSolution.routes(), evrpSolution.routesEnergy(), strSolucao, duration_time, evrpSolution.routes_size());

	return 0;
}