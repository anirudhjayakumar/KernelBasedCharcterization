/*
 * PredictionEngine_test.cpp
 *
 *  Created on: 28-Jun-2013
 *      Author: anirudh
 */

#include "PredictionEngine.h"
#include "MPIBenchmarkAnalysis.h"
#include <iostream>
#include <numeric>
using namespace std;
using namespace Prediction;
int main()
{

	PredictionEngine oPEngine;
	SResultStruct sResult;
	string sXml;
	sXml = oPEngine.CleanXML("/home/anirudhj/smg2000/SMG32_256.xml");
	oPEngine.Initialize(sXml);

	oPEngine.ComputeRuntime(sResult);
	oPEngine.PrintBlock("/home/anirudhj/smg2000/SMG32_256_Output.xml");
	//oPEngine.ComputeRuntime(2147483648, 32, vecModelsToExclude, vecConst, sResult, e_WorkloadOnlyBased);
	sResult.Print();


	/*
	CMPIBenchmarkAnalysis oMPI;
	oMPI.Initialize("/home/anirudhj/workspace/RuntimePrediction/MPIBenchmark.xml");

	double dataSize, time;
	double inDataSize, inTime;
	//latency
	cout << endl;
	cout << "Latency" << endl;

	//104856 0.06839
	//0.3132 471856
	inDataSize = 104856;
	inTime     = 0.3132;
	dataSize = oMPI.GetDataSizeInBytes(inTime,4,e_Latency);
	time     = oMPI.GetTimeInSecs(inDataSize,4,e_Latency);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;

	//58980  0.605
	//2.07   183500
	inDataSize = 58980;
	inTime     = 2.07;
	dataSize = oMPI.GetDataSizeInBytes(inTime,8,e_Latency);
	time     = oMPI.GetTimeInSecs(inDataSize,8,e_Latency);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;

	//419304  35.877
	//64.44   7549744
	inDataSize = 419304;
	inTime     = 64.44;
	dataSize = oMPI.GetDataSizeInBytes(inTime,32,e_Latency);
	time     = oMPI.GetTimeInSecs(inDataSize,32,e_Latency);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;

	//838856  8.91
	//19.84   1887436
	inDataSize = 838856;
	inTime     = 19.84;
	dataSize = oMPI.GetDataSizeInBytes(inTime,48,e_Latency);
	time     = oMPI.GetTimeInSecs(inDataSize,48,e_Latency);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	cout << endl;



//alreduce
	cout << "AllReduce" << endl;
	// 60397976  0.125 -- CHECK
	//0.220    107374180
	inDataSize = 60397976;
	inTime     = 0.220;
	dataSize = oMPI.GetDataSizeInBytes(inTime,4,e_AllReduce);
	time     = oMPI.GetTimeInSecs(inDataSize,4,e_AllReduce);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	//40265316 0.8151
	//1.84  93952408
	inDataSize = 40265316;
	inTime     = 1.84;
	dataSize = oMPI.GetDataSizeInBytes(inTime,8,e_AllReduce);
	time     = oMPI.GetTimeInSecs(inDataSize,8,e_AllReduce);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	//235928  0.0863
	//1.161 2516580
	inDataSize = 235928;
	inTime     = 1.161;
	dataSize = oMPI.GetDataSizeInBytes(inTime,32,e_AllReduce);
	time     = oMPI.GetTimeInSecs(inDataSize,32,e_AllReduce);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	//10066328  3.57
	//8.27     23488100
	inDataSize = 10066328;
	inTime     = 8.27;
	dataSize = oMPI.GetDataSizeInBytes(inTime,48,e_AllReduce);
	time     = oMPI.GetTimeInSecs(inDataSize,48,e_AllReduce);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	cout << endl;

//reduce
	cout << "Reduce" << endl;
	//23488100 0.122
	//0.64 120795952
	inDataSize = 23488100;
	inTime     = 0.64;
	dataSize = oMPI.GetDataSizeInBytes(inTime,4,e_Reduce);
	time     = oMPI.GetTimeInSecs(inDataSize,4,e_Reduce);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
    //6710884  0.107
	//0.2396  15099492
	inDataSize = 6710884;
	inTime     = 0.2396;
	dataSize = oMPI.GetDataSizeInBytes(inTime,8,e_Reduce);
	time     = oMPI.GetTimeInSecs(inDataSize,8,e_Reduce);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;

	cout << endl;
	//1677720  0.2664
	//1.695  10066328
	inDataSize = 1677720;
	inTime     = 1.695;
	dataSize = oMPI.GetDataSizeInBytes(inTime,32,e_Reduce);
	time     = oMPI.GetTimeInSecs(inDataSize,32,e_Reduce);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;

	//1258288  0.480
	//0.9755  2516580
	inDataSize = 1258288;
	inTime     = 0.9755;
	dataSize = oMPI.GetDataSizeInBytes(inTime,48,e_Reduce);
	time     = oMPI.GetTimeInSecs(inDataSize,48,e_Reduce);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	cout << endl;

//attotall
	cout << "AlltoAll" << endl;
	//40265316  0.0845
	//0.1654  80530636
	inDataSize = 40265316;
	inTime     = 0.1654;
	dataSize = oMPI.GetDataSizeInBytes(inTime,4,e_AlltoAll);
	time     = oMPI.GetTimeInSecs(inDataSize,4,e_AlltoAll);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;

	cout << endl;
	//5872024  0.123
	//0.4  20132656
	inDataSize = 5872024;
	inTime     = 0.4;
	dataSize = oMPI.GetDataSizeInBytes(inTime,8,e_AlltoAll);
	time     = oMPI.GetTimeInSecs(inDataSize,8,e_AlltoAll);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;

	cout << endl;
	// 7549774  3.4484
	// 5.825    13421768
	inDataSize = 7549774;
	inTime     = 5.825;
	dataSize = oMPI.GetDataSizeInBytes(inTime,32,e_AlltoAll);
	time     = oMPI.GetTimeInSecs(inDataSize,32,e_AlltoAll);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;

	cout << endl;
	// 314572  0.1219
	// 0.454   838856
	inDataSize = 314572;
	inTime     = 0.454;
	dataSize = oMPI.GetDataSizeInBytes(inTime,48,e_AlltoAll);
	time     = oMPI.GetTimeInSecs(inDataSize,48,e_AlltoAll);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	cout << endl;

//broadcast
	cout << "Broadcast" << endl;
    //15099492  0.028
	// 0.100  53687088
	inDataSize = 15099492;
	inTime     = 0.100;
	dataSize = oMPI.GetDataSizeInBytes(inTime,4,e_Broadcast);
	time     = oMPI.GetTimeInSecs(inDataSize,4,e_Broadcast);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	//53687088  1.026
	//0.388  20132656
	inDataSize = 53687088;
	inTime     = 0.388;
	dataSize = oMPI.GetDataSizeInBytes(inTime,8,e_Broadcast);
	time     = oMPI.GetTimeInSecs(inDataSize,8,e_Broadcast);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;

	//2097152  0.321
	//1.458  8388608
	inDataSize = 2097152;
	inTime     = 1.458;
	dataSize = oMPI.GetDataSizeInBytes(inTime,32,e_Broadcast);
	time     = oMPI.GetTimeInSecs(inDataSize,32,e_Broadcast);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;

	//1048576 0.102
	//0.332  3355440
	inDataSize = 1048576;
	inTime     = 0.332;
	dataSize = oMPI.GetDataSizeInBytes(inTime,48,e_Broadcast);
	time     = oMPI.GetTimeInSecs(inDataSize,48,e_Broadcast);
	cout << "Time = " << inTime << " DataSize = " << dataSize << endl;
	cout << "DataSize = " << inDataSize << " Time = " << time << endl;
	cout << endl;
	cout << endl;


*/
	return 0;
}

// 2147483648
// 2.1990233e+12
