/*
 * CLI.cpp
 *
 * Author: Noa Leshem - 314665415
 * Inbal Apt - 207031006
 * on 30/12/21.
 */
#include "CLI.h"

CLI::CLI(DefaultIO *dio) {
    this->dio = dio;
    commands.push_back(new UploadCSVFile(dio));
    commands.push_back(new AlgoSettings(dio));
    commands.push_back(new DetectAnom(dio));
    commands.push_back(new DisplayResults(dio));
    commands.push_back(new UploadAnom(dio));
}
/*
 * print the menu of the server to client.
 */
void CLI::printMenu() {
    //print the start
    dio->write("Welcome to the Anomaly Detection Server.\nPlease choose an option:\n");
    int commands_size = commands.size();
    int i = 0;
    // print all the options of the commands.
    for (; i < commands_size; i++) {
        dio->write(float(i + 1));
        dio->write("." + commands[i]->actDescription + "\n");
    }
    dio->write(float(i + 1));
    dio->write(".exit\n");

}

void CLI::start() {
    CLIData cliData;
    float option = -1;
    while (option != 6) {
        // print the menu of the options
        printMenu();
        // get the wanted option.
        dio->read(&option);
        // if the option is in the range.
        if (option > 0 && option < 6) {
            //move to the next line.
            //dio->write("\n");
            int i = int(option) - 1;
            commands[i]->execute(&cliData);
        }
    }
}


CLI::~CLI() {
    for (int i = 0; i < commands.size(); i++){
        delete commands[i];
    }
}
