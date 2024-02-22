#include "MipModel.h"

#include <iostream>

using namespace std;

MipModel::MipModel(const Instance &inst) : m_inst(inst)
{
   // Basic CPLEX variables.
   m_model = IloModel(m_env);
   m_cplex = IloCplex(m_model);
   m_model.setName("HLFNDP");

   // Helper objects.
   IloExpr expr(m_env);
   char buf[128] = "";

   double beta = 0.9;
   double bigM = 1e8;

   // Create decision variable z;
   m_z = Var3D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_z[i] = Var2D(m_env, m_inst.numTypesOfHF());

      for (int j = 0; j < m_inst.numTypesOfHF(); j++)
      {
         m_z[i][j] = Var1D(m_env, m_inst.numTimePeriods() + 1);

         for (int k = 0; k < m_inst.numTimePeriods() + 1; k++)
         {
            m_z[i][j][k] = IloNumVar(m_env, 0.0, 1.0, IloNumVar::Bool, buf);
            if (k == 0)
            {
               if (m_inst.existingFacilities(i, j) == 1)
                  m_z[i][j][k].setBounds(1.0, 1.0);
               else
                  m_z[i][j][k].setBounds(0.0, 0.0);
            }
         }
      }
   }

   // Create decision variable x;
   m_x = Var4D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_x[i] = Var3D(m_env, m_inst.numNodes());

      for (int j = 0; j < m_inst.numNodes(); j++)
      {
         if (j <= i)
            continue;

         m_x[i][j] = Var2D(m_env, m_inst.numTypesOfLink());

         for (int k = 0; k < m_inst.numTypesOfLink(); k++)
         {
            m_x[i][j][k] = Var1D(m_env, m_inst.numTimePeriods() + 1);

            for (int l = 0; l < m_inst.numTimePeriods() + 1; l++)
            {
               m_x[i][j][k][l] = IloNumVar(m_env, 0.0, 1.0, IloNumVar::Bool, buf);
               if (l == 0)
               {
                  if (m_inst.existingLink(i, j, k) == 1)
                     m_x[i][j][k][l].setBounds(1.0, 1.0);
                  else
                     m_x[i][j][k][l].setBounds(0.0, 0.0);
               }
               else
                  expr += (int)m_inst.operatingCostLink(i, j, k, l - 1) * m_x[i][j][k][l];
            }
         }
      }
   }

   // Create decision variable eta
   m_eta = Var2D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_eta[i] = Var1D(m_env, m_inst.numTimePeriods());

      for (int j = 0; j < m_inst.numTimePeriods(); j++)
         m_eta[i][j] = IloNumVar(m_env, 0.0, 1.0, IloNumVar::Bool, buf);
   }

   // Create decision variable y;
   m_y = Var4D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_y[i] = Var3D(m_env, m_inst.numNodes());

      for (int j = 0; j < m_inst.numNodes(); j++)
      {

         m_y[i][j] = Var2D(m_env, m_inst.numTypesOfLink());

         for (int k = 0; k < m_inst.numTypesOfLink(); k++)
         {
            m_y[i][j][k] = Var1D(m_env, m_inst.numTimePeriods());

            for (int l = 0; l < m_inst.numTimePeriods(); l++)
            {
               m_y[i][j][k][l] = IloNumVar(m_env, 0.0, IloInfinity, IloNumVar::Int, buf);
               expr += (int)m_inst.travellingCost(i, j, k, l) * m_y[i][j][k][l];
            }
         }
      }
   }

   // Create decision variable delta;
   m_delta = Var3D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_delta[i] = Var2D(m_env, m_inst.numTypesOfHF());

      for (int j = 0; j < m_inst.numTypesOfHF(); j++)
      {
         m_delta[i][j] = Var1D(m_env, m_inst.numTimePeriods());

         for (int k = 0; k < m_inst.numTimePeriods(); k++)
            m_delta[i][j][k] = IloNumVar(m_env, 0.0, 1.0, IloNumVar::Bool, buf);
      }
   }

   // Create decision variable q;
   m_q = Var3D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_q[i] = Var2D(m_env, m_inst.numTypesOfHF());

      for (int j = 0; j < m_inst.numTypesOfHF(); j++)
      {
         m_q[i][j] = Var1D(m_env, m_inst.numTimePeriods() + 1);

         for (int k = 0; k < m_inst.numTimePeriods() + 1; k++)
         {
            m_q[i][j][k] = IloNumVar(m_env, 0., IloInfinity, IloNumVar::Int, buf);
            if (k == 0)
               m_q[i][j][k].setBounds(m_inst.minimumServingCapacity(i, j), m_inst.minimumServingCapacity(i, j));
            else
               expr += (int)m_inst.operatingCostHF(i, j, k - 1) * m_q[i][j][k];
         }
      }
   }

   // Create decision variable v;
   m_v = Var3D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_v[i] = Var2D(m_env, m_inst.numTypesOfHF());

      for (int j = 0; j < m_inst.numTypesOfHF(); j++)
      {
         m_v[i][j] = Var1D(m_env, m_inst.numTimePeriods());

         for (int k = 0; k < m_inst.numTimePeriods(); k++)
         {
            m_v[i][j][k] = IloNumVar(m_env, 0., IloInfinity, IloNumVar::Int, buf);
            expr += (int)m_inst.servingCapacityExpansionCostHF(i, j, k) * m_v[i][j][k];
         }
      }
   }

   // Create decision varable w to linearise the problem;
   m_w = Var5D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_w[i] = Var4D(m_env, m_inst.numNodes());

      for (int j = 0; j < m_inst.numNodes(); j++)
      {
         m_w[i][j] = Var3D(m_env, m_inst.numTypesOfHF());

         for (int h = 0; h < m_inst.numTypesOfHF(); h++)
         {
            m_w[i][j][h] = Var2D(m_env, m_inst.numTypesOfLink());

            for (int r = 0; r < m_inst.numTypesOfLink(); r++)
            {
               m_w[i][j][h][r] = Var1D(m_env, m_inst.numTimePeriods() + 1);

               for (int t = 0; t < m_inst.numTimePeriods() + 1; t++)
                  m_w[i][j][h][r][t] = IloNumVar(m_env, 0., IloInfinity, IloNumVar::Int, buf);
            }
         }
      }
   }

   m_obj = IloObjective(m_env, expr, IloObjective::Minimize, "ConstructionCost");
   m_model.add(m_obj);
   expr.clear();

   // Create decision variable e (envy);
   m_e = Var3D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_e[i] = Var2D(m_env, m_inst.numNodes());

      for (int j = 0; j < m_inst.numNodes(); j++)
      {
         m_e[i][j] = Var1D(m_env, m_inst.numTimePeriods());

         for (int k = 0; k < m_inst.numTimePeriods(); k++)
         {
            m_e[i][j][k] = IloNumVar(m_env, 0., IloInfinity, IloNumVar::Float, buf);
            expr += m_e[i][j][k];
         }
      }
   }

   m_obj2 = IloObjective(m_env, expr, IloObjective::Minimize, "Envy");
   m_model.add(m_obj2);
   expr.clear();

   // Create decision variable a (accessibilty)
   m_a = Var2D(m_env, m_inst.numNodes());
   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      m_a[i] = Var1D(m_env, m_inst.numTimePeriods());

      for (int j = 0; j < m_inst.numTimePeriods(); j++)
      {
         m_a[i][j] = IloNumVar(m_env, 0., IloInfinity, IloNumVar::Float, buf);
         expr += m_a[i][j];
      }
   }

   m_obj3 = IloObjective(m_env, expr, IloObjective::Maximize, "Accessibility");
   m_model.add(m_obj3);
   expr.clear();

   // Accessibility constraints (7)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
      {

         for (int j = 0; j < m_inst.numNodes(); j++)
         {
            for (int h = 0; h < m_inst.numTypesOfHF(); h++)
            {
               for (int r = 0; r < m_inst.numTypesOfLink(); r++)
               {

                  if (i != j and m_q[j][h][t].getImpl() and m_x[i][j][r][t].getImpl())
                     expr += (m_w[i][j][h][r][t] * pow((m_inst.distance(i, j) / m_inst.averageSpeed(i, j, r)), -beta)) / m_inst.demandRate(j, t - 1);
               }
            }
         }

         for (int h = 0; h < m_inst.numTypesOfHF(); h++)
         {
            if (m_q[i][h][t].getImpl())
               expr += (m_q[i][h][t] / m_inst.demandRate(i, t - 1));
         }

         IloConstraint c = m_a[i][t - 1] - expr == 0;
         c.setName(buf);
         m_model.add(c);
         expr.clear();
      }
   }

   // Envy Constraints (8) and (39) and (40)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int j = 0; j < m_inst.numNodes(); j++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            if (m_e[i][j][t - 1].getImpl() == nullptr)
               continue;

            IloConstraint c = m_e[i][j][t - 1] >= 0;
            c.setName(buf);
            m_model.add(c);
            expr.clear();

            if (!m_a[j][t - 1].getImpl())
               continue;
            if (!m_a[i][t - 1].getImpl())
               continue;

            c = m_e[i][j][t - 1] - m_a[j][t - 1] + m_a[i][t - 1] >= 0;
            c.setName(buf);
            m_model.add(c);
            expr.clear();
         }
      }
   }

   // Flow Conservation in Supply node (9)

   for (int j = 0; j < m_inst.numNodes(); j++)
   {
      for (int h = 0; h < m_inst.numTypesOfHF(); h++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            if (m_q[j][h][t].getImpl() == nullptr)
               continue;
            if (m_z[j][h][t].getImpl() == nullptr)
               continue;

            for (int i = 0; i < m_inst.numNodes(); i++)
            {
               for (int r = 0; r < m_inst.numTypesOfLink(); r++)
               {

                  if (m_y[i][j][r][t - 1].getImpl())
                     expr += m_y[i][j][r][t - 1];
                  if (m_y[j][i][r][t - 1].getImpl())
                     expr -= m_y[j][i][r][t - 1];
               }
            }

            expr += m_inst.demandRate(j, t - 1);

            IloConstraint c = expr <= m_q[j][h][t] + bigM * (1 - m_z[j][h][t]);
            c.setName(buf);
            m_model.add(c);
            expr.clear();
         }
      }
   }

   // Flow Conservation in Demand Node (10) and (11)

   for (int j = 0; j < m_inst.numNodes(); j++)
   {
      for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
      {

         if (m_eta[j][t - 1].getImpl() == nullptr)
            continue;

         for (int i = 0; i < m_inst.numNodes(); i++)
         {
            for (int r = 0; r < m_inst.numTypesOfLink(); r++)
            {

               if (m_y[i][j][r][t - 1].getImpl())
                  expr -= m_y[i][j][r][t - 1];
               if (m_y[j][i][r][t - 1].getImpl())
                  expr += m_y[j][i][r][t - 1];
            }
         }

         IloConstraint c = expr <= m_inst.demandRate(j, t - 1) + bigM * (1 - m_eta[j][t - 1]);
         c.setName(buf);
         m_model.add(c);

         c = expr >= m_inst.demandRate(j, t - 1) - bigM * (1 - m_eta[j][t - 1]);
         c.setName(buf);
         m_model.add(c);
         expr.clear();
      }
   }

   // (12)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
      {
         if (!m_eta[i][t - 1].getImpl())
            continue;

         for (int h = 0; h < m_inst.numTypesOfHF(); h++)
         {
            if (m_z[i][h][t].getImpl())
               expr += m_z[i][h][t];
         }

         IloConstraint c = m_eta[i][t - 1] + expr == 1;
         c.setName(buf);
         m_model.add(c);
         expr.clear();
      }
   }

   // Facility Construction Budget (13)

   for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
   {

      for (int i = 0; i < m_inst.numNodes(); i++)
      {
         for (int h = 0; h < m_inst.numTypesOfHF(); h++)
         {
            if (!m_z[i][h][t].getImpl())
               continue;

            expr += m_inst.fixedCostHF(i, h, t - 1) * (m_z[i][h][t] - m_z[i][h][t - 1]);
         }
      }

      expr -= m_inst.constructionBudgetHF(t - 1);
      IloConstraint c = expr <= 0;
      c.setName(buf);
      m_model.add(c);
      expr.clear();
   }

   // Link Construction Budget (14)

   for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
   {

      for (int i = 0; i < m_inst.numNodes(); i++)
      {
         for (int j = i + 1; j < m_inst.numNodes(); j++)
         {
            for (int r = 0; r < m_inst.numTypesOfLink(); r++)
            {
               if (!m_x[i][j][r][t].getImpl())
                  continue;

               expr += m_inst.fixedCostLink(i, j, r, t - 1) * (m_x[i][j][r][t] - m_x[i][j][r][t - 1]);
            }
         }
      }

      expr -= m_inst.constructionBudgetLinks(t - 1);
      IloConstraint c = expr <= 0;
      c.setName(buf);
      m_model.add(c);
      expr.clear();
   }

   // At max 1 type of link to be constructed (15)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int j = i + 1; j < m_inst.numNodes(); j++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            for (int r = 0; r < m_inst.numTypesOfLink(); r++)
            {
               if (m_x[i][j][r][t].getImpl())
                  expr += m_x[i][j][r][t];
            }

            IloConstraint c = expr <= 1;
            c.setName(buf);
            m_model.add(c);
            expr.clear();
         }
      }
   }

   // transfer <= capacity of link (16) (17)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int j = i + 1; j < m_inst.numNodes(); j++)
      {
         for (int r = 0; r < m_inst.numTypesOfLink(); r++)
         {
            for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
            {

               if (m_y[i][j][r][t - 1].getImpl() == nullptr)
                  continue;

               if (m_x[i][j][r][t].getImpl() == nullptr)
                  continue;

               IloConstraint c = m_y[i][j][r][t - 1] <= (int)m_inst.fixedCostLink(i, j, r, t - 1) * m_x[i][j][r][t];
               c.setName(buf);
               m_model.add(c);
               expr.clear();
            }
         }
      }
   }

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int j = i + 1; j < m_inst.numNodes(); j++)
      {
         for (int r = 0; r < m_inst.numTypesOfLink(); r++)
         {
            for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
            {

               if (m_y[j][i][r][t - 1].getImpl() == nullptr)
                  continue;

               if (m_x[i][j][r][t].getImpl() == nullptr)
                  continue;

               IloConstraint c = m_y[j][i][r][t - 1] <= (int)m_inst.fixedCostLink(i, j, r, t - 1) * m_x[i][j][r][t];
               c.setName(buf);
               m_model.add(c);
               expr.clear();
            }
         }
      }
   }

   // Capacity served (18)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int h = 0; h < m_inst.numTypesOfHF(); h++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            if (m_q[i][h][t].getImpl() == nullptr)
               continue;

            if (m_q[i][h][t - 1].getImpl() == nullptr)
               continue;

            if (m_v[i][h][t - 1].getImpl() == nullptr)
               continue;

            IloConstraint c = m_q[i][h][t] >= m_q[i][h][t - 1] + m_v[i][h][t - 1];
            c.setName(buf);
            m_model.add(c);
            expr.clear();

            c = m_q[i][h][t] <= m_q[i][h][t - 1] + m_v[i][h][t - 1];
            c.setName(buf);
            m_model.add(c);
            expr.clear();

         }
      }
   }

   // Capacity served < max serving and > min serving (19)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int h = 0; h < m_inst.numTypesOfHF(); h++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            if (m_z[i][h][t].getImpl() == nullptr)
               continue;

            if (m_q[i][h][t].getImpl() == nullptr)
               continue;

            IloConstraint c = m_inst.minimumServingCapacity(i, h) * m_z[i][h][t] <= m_q[i][h][t];
            c.setName(buf);
            m_model.add(c);
            expr.clear();

            c = m_inst.maximumServingCapacityHF(i, h) * m_z[i][h][t] >= m_q[i][h][t];
            c.setName(buf);
            m_model.add(c);
            expr.clear();
         }
      }
   }

   // (20)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int h = 0; h < m_inst.numTypesOfHF(); h++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            if (m_z[i][h][t].getImpl() == nullptr)
               continue;

            if (m_z[i][h][t - 1].getImpl() == nullptr)
               continue;

            if (m_delta[i][h][t - 1].getImpl() == nullptr)
               continue;

            IloConstraint c = m_z[i][h][t] - m_z[i][h][t - 1] + m_delta[i][h][t - 1] <= 1;
            c.setName(buf);
            m_model.add(c);
            expr.clear();
         }
      }
   }

   // (21)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int h = 0; h < m_inst.numTypesOfHF(); h++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            if (m_z[i][h][t].getImpl() == nullptr)
               continue;

            if (m_delta[i][h][t - 1].getImpl() == nullptr)
               continue;

            IloConstraint c = m_delta[i][h][t - 1] <= m_z[i][h][t];
            c.setName(buf);
            m_model.add(c);
            expr.clear();
         }
      }
   }

   // facility once constructed remains functional (22)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int h = 0; h < m_inst.numTypesOfHF(); h++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            if (m_z[i][h][t].getImpl() == nullptr)
               continue;

            if (m_z[i][h][t - 1].getImpl() == nullptr)
               continue;

            IloConstraint c = m_z[i][h][t] >= m_z[i][h][t - 1];
            c.setName(buf);
            m_model.add(c);
            expr.clear();
         }
      }
   }

   // Link once constructed remain functional (23)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int j = i + 1; j < m_inst.numNodes(); j++)
      {
         for (int r = 0; r < m_inst.numTypesOfLink(); r++)
         {
            for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
            {

               if (m_x[j][i][r][t - 1].getImpl() == nullptr)
                  continue;

               if (m_x[i][j][r][t].getImpl() == nullptr)
                  continue;

               IloConstraint c = m_x[j][i][r][t] >= m_x[i][j][r][t - 1];
               c.setName(buf);
               m_model.add(c);
               expr.clear();
            }
         }
      }
   }

   // facility can't be constructed if population is less (28)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int h = 0; h < m_inst.numTypesOfHF(); h++)
      {
         for (int t = 1; t < m_inst.numTimePeriods() + 1; t++)
         {

            if (m_z[i][h][t].getImpl() == nullptr)
               continue;

            if (m_z[i][h][t - 1].getImpl() == nullptr)
               continue;

            if (m_inst.population(i, t) >= m_inst.minimumPopulationRequired(i, h))
               continue;

            IloConstraint c = m_z[i][h][t] >= m_z[i][h][t - 1];
            c.setName(buf);
            m_model.add(c);
            expr.clear();

            c = m_z[i][h][t] <= m_z[i][h][t - 1];
            c.setName(buf);
            m_model.add(c);
            expr.clear();
         }
      }
   }

   // (35) (36) (37) (38)

   for (int i = 0; i < m_inst.numNodes(); i++)
   {
      for (int j = 0; j < m_inst.numNodes(); j++)
      {
         for (int h = 0; h < m_inst.numTypesOfHF(); h++)
         {
            for (int r = 0; r < m_inst.numTypesOfLink(); r++)
            {
               for (int t = 0; t < m_inst.numTimePeriods() + 1; t++)
               {

                  if (m_w[i][j][h][r][t].getImpl() == nullptr)
                     continue;

                  IloConstraint c = m_w[i][j][h][r][t] >= 0;
                  c.setName(buf);
                  m_model.add(c);
                  expr.clear();

                  if (m_q[j][h][t].getImpl() != nullptr)
                  {

                     c = m_w[i][j][h][r][t] <= m_q[j][h][t];
                     c.setName(buf);
                     m_model.add(c);
                     expr.clear();

                     if (m_x[i][j][r][t].getImpl() != nullptr)
                     {

                        c = m_w[i][j][h][r][t] <= bigM * m_x[i][j][r][t];
                        c.setName(buf);
                        m_model.add(c);
                        expr.clear();

                        c = m_w[i][j][h][r][t] >= m_q[j][h][t] - (bigM * (1 - m_x[i][j][r][t]));
                        c.setName(buf);
                        m_model.add(c);
                        expr.clear();
                     }
                  }
               }
            }
         }
      }
   }

   expr.end();
}

MipModel::~MipModel() {
   m_env.end();
}

const Instance & MipModel::instance() const {
   return m_inst;
}