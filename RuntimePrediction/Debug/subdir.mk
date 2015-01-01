################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/anirudhj/workspace/StaticCodeAnalyzer/FrameworkCommon.cpp \
../MPIBenchmarkAnalysis.cpp \
../PredictionEngine.cpp \
../PredictionEngine_test.cpp \
../RInstance.cpp \
/home/anirudhj/Downloads/tinyxml/tinystr.cpp \
/home/anirudhj/Downloads/tinyxml/tinyxml.cpp \
/home/anirudhj/Downloads/tinyxml/tinyxmlerror.cpp \
/home/anirudhj/Downloads/tinyxml/tinyxmlparser.cpp 

OBJS += \
./FrameworkCommon.o \
./MPIBenchmarkAnalysis.o \
./PredictionEngine.o \
./PredictionEngine_test.o \
./RInstance.o \
./tinystr.o \
./tinyxml.o \
./tinyxmlerror.o \
./tinyxmlparser.o 

CPP_DEPS += \
./FrameworkCommon.d \
./MPIBenchmarkAnalysis.d \
./PredictionEngine.d \
./PredictionEngine_test.d \
./RInstance.d \
./tinystr.d \
./tinyxml.d \
./tinyxmlerror.d \
./tinyxmlparser.d 


# Each subdirectory must supply rules for building sources it contributes
FrameworkCommon.o: /home/anirudhj/workspace/StaticCodeAnalyzer/FrameworkCommon.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/tinyxml -I/home/anirudhj/workspace/StaticCodeAnalyzer -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -I/usr/share/R/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/Rcpp/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/include -pipe -g -Wall    -Wl,--export-dynamic -fopenmp  -L/usr/lib/R/lib -lR -lrt -ldl -lm  -lblas -llapack  -L/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -lRInside -Wl,-rpath,/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/tinyxml -I/home/anirudhj/workspace/StaticCodeAnalyzer -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -I/usr/share/R/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/Rcpp/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/include -pipe -g -Wall    -Wl,--export-dynamic -fopenmp  -L/usr/lib/R/lib -lR -lrt -ldl -lm  -lblas -llapack  -L/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -lRInside -Wl,-rpath,/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

RInstance.o: ../RInstance.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/tinyxml -I/home/anirudhj/workspace/StaticCodeAnalyzer -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -I/usr/share/R/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/Rcpp/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/include -pipe -g -Wall    -Wl,--export-dynamic -fopenmp  -L/usr/lib/R/lib -lR -lrt -ldl -lm  -lblas -llapack  -L/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -lRInside -Wl,-rpath,/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -MMD -MP -MF"$(@:%.o=%.d)" -MT"RInstance.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tinystr.o: /home/anirudhj/Downloads/tinyxml/tinystr.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/tinyxml -I/home/anirudhj/workspace/StaticCodeAnalyzer -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -I/usr/share/R/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/Rcpp/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/include -pipe -g -Wall    -Wl,--export-dynamic -fopenmp  -L/usr/lib/R/lib -lR -lrt -ldl -lm  -lblas -llapack  -L/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -lRInside -Wl,-rpath,/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tinyxml.o: /home/anirudhj/Downloads/tinyxml/tinyxml.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/tinyxml -I/home/anirudhj/workspace/StaticCodeAnalyzer -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -I/usr/share/R/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/Rcpp/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/include -pipe -g -Wall    -Wl,--export-dynamic -fopenmp  -L/usr/lib/R/lib -lR -lrt -ldl -lm  -lblas -llapack  -L/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -lRInside -Wl,-rpath,/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tinyxmlerror.o: /home/anirudhj/Downloads/tinyxml/tinyxmlerror.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/tinyxml -I/home/anirudhj/workspace/StaticCodeAnalyzer -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -I/usr/share/R/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/Rcpp/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/include -pipe -g -Wall    -Wl,--export-dynamic -fopenmp  -L/usr/lib/R/lib -lR -lrt -ldl -lm  -lblas -llapack  -L/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -lRInside -Wl,-rpath,/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tinyxmlparser.o: /home/anirudhj/Downloads/tinyxml/tinyxmlparser.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/tinyxml -I/home/anirudhj/workspace/StaticCodeAnalyzer -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -I/usr/share/R/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/Rcpp/include -I/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/include -pipe -g -Wall    -Wl,--export-dynamic -fopenmp  -L/usr/lib/R/lib -lR -lrt -ldl -lm  -lblas -llapack  -L/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -lRInside -Wl,-rpath,/home/anirudhj/R/x86_64-pc-linux-gnu-library/3.1/RInside/lib -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


