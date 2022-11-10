#include "EvrpData.h"
#include "EvrpSolution.h"
#include "Heuristics.h"
#include <chrono>
using namespace std::chrono;
using namespace std;

int main(int argc, char** argv){

    EvrpData evrpDataobj = EvrpData(argv[1]);
    EvrpSolution evrpSolution = EvrpSolution(&evrpDataobj);

    evrpSolution.savingsAlg();

    hillClimbingSolution(&evrpSolution, 0);

    pertubation(&evrpSolution);

    joinRoutes(&evrpSolution);
    hillClimbingEachRoute(&evrpSolution, 2);

    evrpSolution.insertDepoAndEnergies();
    string strSolucao = "/Users/paulo/Desktop/evrp/evrptw_instances/soluções/solução";
    strSolucao = strSolucao + argv[1];
    writeSolutionSavings(evrpSolution.routes(), evrpSolution.routesEnergy(), strSolucao, 0, 0);

	return 0;
}