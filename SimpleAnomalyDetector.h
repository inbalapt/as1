/*
 * SimpleAnomalyDetector.h
 *
 * Author: Noa Leshem - 314665415
 * Inbal Apt - 207031006
 * on 10/20/21.
 */

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
    string feature1,feature2;  // names of the correlated features
    float corrlation;
    Line lin_reg;
    float threshold;
};


class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
    vector<correlatedFeatures> cf;
public:
    SimpleAnomalyDetector();
    virtual ~SimpleAnomalyDetector();

    virtual void learnNormal(const TimeSeries& ts);
    virtual vector<AnomalyReport> detect(const TimeSeries& ts);

    vector<correlatedFeatures> getNormalModel(){
        return cf;
    }
    virtual void associateCorrelatedFeatures(int i, int j, float cor, vector<std::pair<std::string, std::vector<float>>> table, int rows);

};



#endif /* SIMPLEANOMALYDETECTOR_H_ */