//
// Created by inbal on 12/29/21.
//

#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
    vector<Command*> commands;
    DefaultIO* dio;

    // you can add data members
public:
    void printMenu();
    CLI(DefaultIO* dio);
    void start();
    virtual ~CLI();
};

#endif /* CLI_H_ */

