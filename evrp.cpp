#include "EvrpData.h"
#include "EvrpSolution.h"
#include "Heuristics.h"
using namespace std;

int main(int argc, char** argv){

    EvrpData evrpDataobj = EvrpData(argv[1]);
    EvrpSolution evrpSolution = EvrpSolution(&evrpDataobj);
    evrpSolution.savingsAlg();
    evrpSolution.insertDepoAndEnergies();
    string strSolucao = "/Users/paulo/Desktop/evrp/evrptw_instances/soluções/solução";
    strSolucao = strSolucao + argv[1];
    hillClimbingEachRoute(&evrpSolution, 0);
    

    writeSolutionSavings(evrpSolution.routes(), evrpSolution.routesEnergy(), strSolucao);

	return 0;
}
