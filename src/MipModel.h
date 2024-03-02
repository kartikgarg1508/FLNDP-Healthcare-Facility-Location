#pragma once

#include "Instance.h"

#define IL_STD
#include <ilcplex/ilocplex.h>

class MipModel
{
public:
   using Var1D = IloArray<IloNumVar>;
   using Var2D = IloArray<Var1D>;
   using Var3D = IloArray<Var2D>;
   using Var4D = IloArray<Var3D>;
   using Var5D = IloArray <Var4D>;

   MipModel(const Instance &inst);
   virtual ~MipModel();

   const Instance &instance() const;

   void setQuiet(bool toggle);
   void writeLp(const char *fname);
   void writeSolution(const char *fname);

   void maxThreads(int value);
   void timeLimit(int maxSeconds);

   double solve(const MipModel &mip);
   double objValue() const;
   double relativeGap() const;
   double objLb() const;

   friend std::ostream &operator<<(std::ostream &out, const MipModel &mip);

protected:
   const Instance &m_inst;

   IloEnv m_env;
   IloModel m_model;
   IloCplex m_cplex;
   IloObjective m_obj, m_obj2, m_obj3;

   Var3D m_z;
   Var4D m_x;
   Var2D m_eta;
   Var4D m_y;
   Var3D m_delta;
   Var3D m_q;
   Var3D m_v;
   Var2D m_a;
   Var3D m_e;
   Var5D m_w;

};