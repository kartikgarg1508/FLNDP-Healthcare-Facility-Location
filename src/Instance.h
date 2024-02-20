#pragma once

#include <iosfwd>
#include <vector>
#include <tuple>
using namespace std;

class Instance {
public:

   Instance(const char *fname);
   virtual ~Instance();

   int numNodes() const;
   int numTypesOfHF() const;
   int numTypesOfLinks() const;
   int numTimePeriods() const;
   long long LargeConstant() const;

   int existingFacilities(int node) const;
   bool existingLink(int fromNode, int toNode) const;

   int demandRate(int node, int timePeriod) const;
   long long fixedCostHF(int node, int typeHF, int timePeriod) const;
   long long fixedCostLink(int fromNode, int toNode, int typeLink, int timePeriod) const;
   long long operatingCostHF(int node, int typeHF, int timePeriod) const;
   long long operatingCostLink(int fromNode, int toNode, int typeLink, int timePeriod) const;

   double distance(int fromNode, int toNode) const;
   double averageSpeed(int fromNode, int toNode, int typeLink) const;

   long long travellingCost(int fromNode, int toNode, int typeLink, int timePeriod) const;
   long long servingCapacityExpansionCostHF(int node, int typeHF, int timePeriod) const;
   int linkCapacity(int fromNode, int toNode, int typeLink) const;
   long long constructionBudgetHF(int timePeriod) const;
   long long constructionBudgetLinks(int timePeriod) const;
   int population(int node, int timePeriod) const;
   int minimumPopulationRequired(int node, int typeHF) const;
   int minimumServingCapacity(int node, int typeHF) const;
   int maximumServingCapacityHF(int node, int typeHF) const;

   const std::string &fileName() const;
   friend std::ostream &operator<<(std::ostream &out, const Instance &inst);

protected:
   /**
    * Resize data structures to acommodate all instance parameters.
    */
   void resize();
   void resize(int numNodes, int numTypesOfHF, int numTypesOfLinks, int numTimePeriods);

private:
   std::string m_fname;
   int m_numNodes;
   int m_numTypesOfHF;
   int m_numTypesOfLink;
   int m_numTimePeriods;
   long long m_LargeConstant;

   vector<int> m_existingFacilities;
   vector<vector<bool>> m_existingLink;
   vector<vector<int>> m_demandRate;
   vector<vector<vector<long long>>> m_fixedCostHF;
   vector<vector<vector<vector<long long>>>> m_fixedCostLink;
   vector<vector<vector<long long>>> m_operatingCostHF;
   vector<vector<vector<vector<long long>>>> m_operatingCostLink;
   vector<vector<double>> m_distance;
   vector<vector<vector<double>>> m_averageSpeed;
   vector<vector<vector<vector<long long>>>> m_travellingCost;
   vector<vector<vector<long long>>> m_servingCapacityExpansionCost;
   vector<vector<vector<int>>> m_linkCapacity;
   vector<long long> m_FacilityConstBudget;
   vector<long long> m_LinkConstBudget;
   vector<int> m_population;
   vector<vector<int>> m_minimumPopulationForOpeningHF;
   vector<vector<int>> m_minimumServingCapacityHF;
   vector<vector<int>> m_maximumServingCapacityHF;

};