#include "EvrpData.h"
#include "EvrpSolution.h"
using namespace std;

int main(int argc, char** argv){

    EvrpData evrpDataobj = EvrpData(argv[1]);
    EvrpSolution evrpSolution = EvrpSolution(&evrpDataobj);
    evrpSolution.savingsAlg();
    evrpSolution.insertDepoAndEnergies();
    string strSolucao = "/Users/paulo/Desktop/evrp/project/evrptw_instances/soluções/solução";
    strSolucao = strSolucao + argv[1];
    
    

    writeSolutionSavings(evrpSolution.routes(), evrpSolution.routesEnergy(), strSolucao);

	return 0;
}