#include "Instance.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <filesystem>
#include <string>


using namespace std;

int main(int argc, char **argv) {
   std::unique_ptr<Instance> inst(new Instance("../input_data/Instance_1.txt"));

   std::cout << *inst << std::endl;

   return EXIT_SUCCESS;
}