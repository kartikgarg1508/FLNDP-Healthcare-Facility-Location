#include "Instance.h"
#include "MipModel.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <filesystem>
#include <string>


using namespace std;

int main(int argc, char **argv) {

   if (argc != 2) {
      cout << "Usage: " << argv[0] << " <1:instance path>" << endl;
      return EXIT_FAILURE;
   }

   const char *instPath = argv[1];

   cout << "=== Fix-and-Optimize solver for HHCRSP ===\n";
   cout << "Instance: " << instPath << endl;

   unique_ptr<Instance> inst(new Instance(instPath));

   cout << *inst << endl;

   cout << "Creating MIP model... " << flush;
   unique_ptr <MipModel> model(new MipModel(*inst));
   cout << "Done!" << endl;

   model->setQuiet(true);
   model->maxThreads(1);

   cout << "Setting solution to MIP model..." << endl;
   model->solve(*model);
   cout << "Done! Initial solution cost: " << model->objValue() << "." << endl;

   cout << *model << endl;

   return EXIT_SUCCESS;
}