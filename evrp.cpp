#include "EvrpData.h"
#include "EvrpSolution.h"
#include "Heuristics.h"
using namespace std;

int main(int argc, char** argv){

    string argvv = "instance29.txt";
    EvrpData evrpDataobj = EvrpData(argvv);
    EvrpSolution evrpSolution = EvrpSolution(&evrpDataobj);
    evrpSolution.savingsAlg();
    hillClimbingSolution(&evrpSolution, 0);
    joinRoutes(&evrpSolution);
    
    evrpSolution.insertDepoAndEnergies();
    string strSolucao = "/Users/paulo/Desktop/evrp/evrptw_instances/soluções/solução";
    strSolucao = strSolucao + argvv;

    writeSolutionSavings(evrpSolution.routes(), evrpSolution.routesEnergy(), strSolucao);

	return 0;
}
