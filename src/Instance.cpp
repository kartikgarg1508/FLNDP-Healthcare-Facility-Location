#include "Instance.h"
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric> // std::accumulate

using namespace std;

Instance::Instance(const char* fname) {
   m_fname = fname;
   std::ifstream fid(fname);
   if (!fid) {
      std::cout << "Instance file " << fname << " could not be read." << std::endl;
      std::exit(EXIT_FAILURE);
   }

   std::vector <int> dscheck;
   std::string buf, lastenv;
   while (std::getline(fid, buf)) {
      if (buf.length() == 0 || buf.find_first_not_of(" ") == std::string::npos) {
         // Skip empty lines
      } else if (buf == "NumberOfNodes") {
         lastenv = buf;
         fid >> m_numNodes;
      } else if (buf == "NumberOfTypesOfHf") {
         lastenv = buf;
         fid >> m_numTypesOfHF;
      } else if (buf == "NumberOfTypesOfLinks") {
         lastenv = buf;
         fid >> m_numTypesOfLink;
      } else if (buf == "NumberOfTimePeriods") {
         lastenv = buf;
         fid >> m_numTimePeriods;
      } else if (buf == "LargeConstant") {
         lastenv = buf;
         fid >> m_LargeConstant;
         resize();
      } else if (buf == "ExistingNodes") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; ++i)
            fid >> m_existingFacilities[i];

      } else if (buf == "ExistingLinks") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numNodes; j++)
               fid >> m_existingLink[i][j];
         }
      } else if (buf == "DemandRate") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numTimePeriods; j++)
               fid >> m_demandRate[i][j];
         }
      } else if (buf == "Distance") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numNodes; j++)
               fid >> m_distance[i][j];
         }
      } else if (buf == "Population") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numTimePeriods; j++)
               fid >> m_population[i][j];
         }
      } else if (buf == "MinPopulationForOpening") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numTypesOfHF; j++)
               fid >> m_minimumPopulationForOpeningHF[i][j];
         }
      } else if (buf == "MinimumServingCapacity") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numTypesOfHF; j++)
               fid >> m_minimumServingCapacityHF[i][j];
         }
      } else if (buf == "MaximumServingCapacity") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numTypesOfHF; j++)
               fid >> m_maximumServingCapacityHF[i][j];
         }
      } else if (buf == "FacilityBudget") {
         lastenv = buf;
         for (int i = 0; i < m_numTimePeriods; i++)
            fid >> m_FacilityConstBudget[i];

      } else if (buf == "LinkBudget") {
         lastenv = buf;
         for (int i = 0; i < m_numTimePeriods; i++)
            fid >> m_LinkConstBudget[i];

      } else if (buf == "FixedCostHF") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numTypesOfHF; j++) {
               for (int k = 0; k < m_numTimePeriods; k++)
                  fid >> m_fixedCostHF[i][j][k];
            }
         }
      } else if (buf == "FixedCostLink") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numNodes; j++) {
               for (int k = 0; k < m_numTypesOfLink; k++) {
                  for (int l = 0; l < m_numTimePeriods; l++)
                     fid >> m_fixedCostLink[i][j][k][l];
               }
            }
         }
      } else if (buf == "OperatingCostHF") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numTypesOfHF; j++) {
               for (int k = 0; k < m_numTimePeriods; k++)
                  fid >> m_operatingCostHF[i][j][k];
            }
         }
      } else if (buf == "OperatingCostLink") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numNodes; j++) {
               for (int k = 0; k < m_numTypesOfLink; k++) {
                  for (int l = 0; l < m_numTimePeriods; l++)
                     fid >> m_operatingCostLink[i][j][k][l];
               }
            }
         }
      } else if (buf == "AverageSpeed") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numNodes; j++) {
               for (int k = 0; k < m_numTypesOfLink; k++)
                  fid >> m_averageSpeed[i][j][k];
            }
         }
      } else if (buf == "TravellingCost") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numNodes; j++) {
               for (int k = 0; k < m_numTypesOfLink; k++) {
                  for (int l = 0; l < m_numTimePeriods; l++)
                     fid >> m_travellingCost[i][j][k][l];
               }
            }
         }
      } else if (buf == "ServingCapacityExpansionCost") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numTypesOfHF; j++) {
               for (int k = 0; k < m_numTimePeriods; k++)
                  fid >> m_servingCapacityExpansionCost[i][j][k];
            }
         }
      } else if (buf == "LinkCapacity") {
         lastenv = buf;
         for (int i = 0; i < m_numNodes; i++) {
            for (int j = 0; j < m_numNodes; j++) {
               for (int k = 0; k < m_numTypesOfLink; k++)
                  fid >> m_linkCapacity[i][j][k];
            }
         }
      } else {
         std::cout << "Unknow line content: " << buf << std::endl;
         std::cout << "Line length: " << buf.length() << std::endl;
         std::cout << "Last section read: " << lastenv << std::endl;
         std::exit(EXIT_FAILURE);
      }
   }

   fid.close();
}

Instance::~Instance() {
   // Empty by design
}

int Instance::numNodes() const {
   return m_numNodes;
}

int Instance::numTypesOfHF() const {
   return m_numTypesOfHF;
}

int Instance::numTypesOfLink() const {
   return m_numTypesOfLink;
}

int Instance::numTimePeriods() const {
   return m_numTimePeriods;
}

long long Instance::LargeConstant() const {
   return m_LargeConstant;
}

int Instance::existingFacilities(int node) const {
   return m_existingFacilities[node];
}

int Instance::existingLink(int fromNode, int toNode) const {
   return m_existingLink[fromNode][toNode];
}

int Instance::demandRate(int node, int timePeriod) const {
   return m_demandRate[node][timePeriod];
}

long long Instance::fixedCostHF(int node, int typeHF, int timePeriod) const {
   return m_fixedCostHF[node][typeHF][timePeriod];
}

long long Instance::fixedCostLink(int fromNode, int toNode, int typeLink, int timePeriod) const {
   return m_fixedCostLink[fromNode][toNode][typeLink][timePeriod];
}

long long Instance::operatingCostHF(int node, int typeHF, int timePeriod) const {
   return m_operatingCostHF[node][typeHF][timePeriod];
}

long long Instance::operatingCostLink(int fromNode, int toNode, int typeLink, int timePeriod) const {
   return m_operatingCostLink[fromNode][toNode][typeLink][timePeriod];
}

double Instance::distance(int fromNode, int toNode) const {
   return m_distance[fromNode][toNode];
}

double Instance::averageSpeed(int fromNode, int toNode, int typeLink) const {
   return m_averageSpeed[fromNode][toNode][typeLink];
}

long long Instance::travellingCost(int fromNode, int toNode, int typeLink, int timePeriod) const {
   return m_travellingCost[fromNode][toNode][typeLink][timePeriod];
}

long long Instance::servingCapacityExpansionCostHF(int node, int typeHF, int timePeriod) const {
   return m_servingCapacityExpansionCost[node][typeHF][timePeriod];
}

int Instance::linkCapacity(int fromNode, int toNode, int typeLink) const {
   return m_linkCapacity[fromNode][toNode][typeLink];
}

long long Instance::constructionBudgetHF(int timePeriod) const {
   return m_FacilityConstBudget[timePeriod];
}

long long Instance::constructionBudgetLinks(int timePeriod) const {
   return m_LinkConstBudget[timePeriod];
}

int Instance::population(int node, int timePeriod) const {
   return m_population[node][timePeriod];
}

int Instance::minimumPopulationRequired(int node, int typeHF) const {
   return m_minimumPopulationForOpeningHF[node][typeHF];
}

int Instance::minimumServingCapacity(int node, int typeHF) const {
   return m_minimumServingCapacityHF[node][typeHF];
}

int Instance::maximumServingCapacityHF(int node, int typeHF) const {
   return m_maximumServingCapacityHF[node][typeHF];
}


const std::string & Instance::fileName() const {
   return m_fname;
}

void Instance::resize() {

   m_existingFacilities.resize(m_numNodes);

   m_existingLink.resize(m_numNodes, std::vector<int>(m_numNodes));

   m_demandRate.resize(m_numNodes, std::vector<int>(m_numTimePeriods));

   m_fixedCostHF.resize(m_numNodes, std::vector<std::vector<long long>>(m_numTypesOfHF, std::vector<long long>(m_numTimePeriods)));

   m_fixedCostLink.resize(m_numNodes, std::vector<std::vector<std::vector<long long>>>(m_numNodes, std::vector<std::vector<long long>>(m_numTypesOfLink, std::vector<long long>(m_numTimePeriods))));

   m_operatingCostHF.resize(m_numNodes, std::vector<std::vector<long long>>(m_numTypesOfHF, std::vector<long long>(m_numTimePeriods)));

   m_operatingCostLink.resize(m_numNodes, std::vector<std::vector<std::vector<long long>>>(m_numNodes, std::vector<std::vector<long long>>(m_numTypesOfLink, std::vector<long long>(m_numTimePeriods))));

   m_distance.resize(m_numNodes, std::vector<double>(m_numNodes));

   m_averageSpeed.resize(m_numNodes, std::vector<std::vector<double>>(m_numNodes, std::vector<double>(m_numTypesOfLink)));

   m_travellingCost.resize(m_numNodes, std::vector<std::vector<std::vector<long long>>>(m_numNodes, std::vector<std::vector<long long>>(m_numTypesOfLink, std::vector<long long>(m_numTimePeriods))));

   m_servingCapacityExpansionCost.resize(m_numNodes, std::vector<std::vector<long long>>(m_numTypesOfHF, std::vector<long long>(m_numTimePeriods)));

   m_linkCapacity.resize(m_numNodes, std::vector<std::vector<int>>(m_numNodes, std::vector<int>(m_numTypesOfLink)));

   m_FacilityConstBudget.resize(m_numTimePeriods);

   m_LinkConstBudget.resize(m_numTimePeriods);

   m_population.resize(m_numNodes, std::vector<int>(m_numTimePeriods));

   m_minimumPopulationForOpeningHF.resize(m_numNodes, std::vector<int>(m_numTypesOfHF));

   m_minimumServingCapacityHF.resize(m_numNodes, std::vector<int>(m_numTypesOfHF));

   m_maximumServingCapacityHF.resize(m_numNodes, std::vector<int>(m_numTypesOfHF));
}

void Instance::resize(int numNodes, int numTypesOfHF, int numTypesOfLink, int numTimePeriods, long long LargeConstant) {
   m_numNodes = numNodes;
   m_numTypesOfHF = numTypesOfHF;
   m_numTypesOfLink = numTypesOfLink;
   m_numTimePeriods = numTimePeriods;
   m_LargeConstant = LargeConstant;

   resize();
}

std::ostream &operator<<(std::ostream &out, const Instance &inst) {
   out << "Number Of Nodes\n" << inst.numNodes() << "\n";
   out << "Number Of Types Of Healthcare Facilities\n" << inst.numTypesOfHF() << "\n";
   out << "Number Of Types Of Links\n" << inst.numTypesOfLink() << "\n";
   out << "Number Of Time Periods\n" << inst.numTimePeriods() << "\n";
   out << "LargeConstant\n" << inst.LargeConstant() << "\n";

   out << "Existing Nodes\n";
   for (int i = 0; i < inst.numNodes(); i++) {
      out << inst.existingFacilities(i);
      if (i < inst.numNodes() - 1)
         out << " ";
   }

   out << "\nExisting Links\n";
   for (int i = 0; i < inst.numNodes(); i++) {
      for (int j = 0; j < inst.numNodes(); j++) {
         out << inst.existingLink(i, j);
         if (j < inst.numNodes() - 1)
            out << " ";
      }
      out << "\n";
   }

   out << "Demand rate of patient zone i at period t\n";
   for (int i = 0; i < inst.numNodes(); i++) {
      for (int j = 0; j < inst.numTimePeriods(); j++) {
         out << inst.demandRate(i, j);
         if (j < inst.numTimePeriods() - 1)
            out << " ";
      }
      out << "\n";
   }

   out << "Fixed Cost of constructing an HF h at node i at period t\n";
   for (int i = 0; i < inst.numNodes(); i++) {
      for (int j = 0; j < inst.numTypesOfHF(); j++) {
         for (int k = 0; k < inst.numTimePeriods(); k++) {
            out << inst.fixedCostHF(i, j, k);
            if (k < inst.numTimePeriods() - 1)
               out << " ";
         }
         out << "\n";
      }
      out << "\n";
   }

   return out;
}