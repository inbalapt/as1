
#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

struct ReportsRangers {
    string description;
    int begin;
    int end;
};

struct CLIData {
    float threshold;
    vector<AnomalyReport> report;
    int numOfRows;
};

class DefaultIO {
public:
    virtual string read() = 0;

    virtual void write(string text) = 0;

    virtual void write(float f) = 0;

    virtual void read(float *f) = 0;

    virtual ~DefaultIO() {}

    // you may add additional methods here
};

// you may add here helper classes


// you may edit this class
class Command {
protected:
    DefaultIO *dio;
    CLIData *cliData;

public:
    string actDescription;

    Command(DefaultIO *dio) : dio(dio) {}

    Command(DefaultIO *dio, const string actDescription) : dio(dio), actDescription(actDescription) {};

    virtual void execute() = 0;

    virtual ~Command() {}

};


/*
 * option 1 - uploading the csv files - train and test by the given paths.
 */
class UploadCSVFile : public Command {
public:
    UploadCSVFile(DefaultIO *dio) : Command(dio, "upload a time series csv file") {}

    virtual void execute() {
        std::string line;

        dio->write("Please upload your local train CSV file.\n");

        // get train file from client
        std::ofstream train("anomalyTrain.csv");
        line = dio->read();
        while (line != "done") {
            train << line << endl;
            line = dio->read();
            this->cliData->numOfRows += 1;
        }
        train.close();

        dio->write("Upload complete.\n");
        dio->write("Please upload your local test CSV file.\n");

        // get test file from client
        std::ofstream test("anomalyTrain.csv");
        line = dio->read();
        while (line != "done") {
            test << line << endl;
            line = dio->read();
            this->cliData->numOfRows += 1;
        }
        test.close();

        dio->write("Upload complete.\n");
    }
};

/*
 * option 2 - set the algorithm threshold for correlated features.
 */
class AlgoSettings : public Command {
public:
    AlgoSettings(DefaultIO *dio) : Command(dio, "algorithm settings") {}

    virtual void execute() {
        float thresh;
        dio->write("The current correlation threshold is ");
        dio->write(cliData->threshold);
        dio->write("\nType a new threshold\n");
        // convert from string to float.
        thresh = std::stof(dio->read());
        while (thresh < 0 || thresh > 1) {
            dio->write("please choose a value between 0 and 1.\n");
            thresh = std::stof(dio->read());
        }
        //set the new threshold.
        this->cliData->threshold = thresh;
    }
};

/*
 * option 3 - get the anomaly features.
 */
class DetectAnom : public Command {
public:
    DetectAnom(DefaultIO *dio) : Command(dio, "detect anomalies") {}

    virtual void execute() {
        //train and test of time series.
        TimeSeries train("anomalyTrain.csv");
        TimeSeries test("anomalyTest.csv");
        HybridAnomalyDetector anomalyDetector;
        //set the threshold.
        anomalyDetector.setThreshold(cliData->threshold);
        anomalyDetector.learnNormal(train);
        //save the normal features.
        cliData->report = anomalyDetector.detect(test);
        dio->write("anomaly detection complete.\n");
    }
};

/*
 * option 4 - display the results of the anomaly features.
 */
class DisplayResults : public Command {
public:
    DisplayResults(DefaultIO *dio) : Command(dio, "display results") {}

    virtual void execute() {
        // lopp all over the correlated features, and display them.
        for (int i = 0; i < cliData->report.size(); i++) {
            dio->write(cliData->report[i].timeStep);
            dio->write("\t");
            dio->write(cliData->report[i].description + "\n");
        }
        dio->write("Done.\n");
    }
};

class UploadAnom : public Command {
public:
    UploadAnom(DefaultIO *dio) : Command(dio, "upload anomalies and analyze results") {}

    virtual void execute() {
        std::string line;
        vector<ReportsRangers> reports_ranges;
        vector<ReportsRangers> user_ranges;

        // get the ranges of the reports that were discovered
        for (int i = 0; i < cliData->report.size(); i++) {
            int count = 1;  // count the sequence of deviations of this current report.
            ReportsRangers current_report;
            current_report.description = cliData->report[i].description;
            current_report.begin = cliData->report[i].timeStep;
            i++;

            /*
             * check if the description of this current report is as the one that comes after,
             * and if this is a part of the sequence of deviations.
             */
            while (cliData->report[i].description == current_report.description &&
                   cliData->report[i].timeStep == current_report.begin + count && i < cliData->report.size()) {
                count++;
                i++;
            }

            // update the end of the range to be the begin + count of the sequence deviations.
            current_report.end = current_report.begin + count;
            // pushing this report to the vector of reports ranges.
            reports_ranges.push_back(current_report);
        }

        int P;
        int N;
        int num_of_deviations = 0; // counting deviations for N

        line = dio->read();
        while (line != "done") {
            std::stringstream string_stream(line);
            int start = 0;
            int end = 0;
            string_stream >> start;
            if (string_stream.peek() == ',') string_stream.ignore();
            string_stream >> end;
            P++;
            num_of_deviations += end - start + 1;
            ReportsRangers user_range = {"", start, end};
            user_ranges.push_back(user_range);
            line = dio->read();
        }
        N = cliData->numOfRows - num_of_deviations;
        int TP = 0, FP = 0, FN = 0, TN = 0;
        for (ReportsRangers &user_range: user_ranges) {
            for (ReportsRangers &real_report: reports_ranges) {
                if (user_range.begin < real_report.begin) {
                    if (user_range.end > real_report.begin) {
                        TP++;
                        break;
                    } else if (user_range.end < real_report.begin) {
                        continue;
                    }
                } else if (user_range.begin > real_report.begin) {
                    if (user_range.begin > real_report.end) {
                        continue;
                    } else if (user_range.begin < real_report.end) {
                        TP++;
                        break;
                    }
                }
            }
        }


    }
};


#endif /* COMMANDS_H_ */