################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../FrameworkCommon.cpp \
../LoopDependencyResolver.cpp \
../LoopNestingAnalyzer.cpp \
../SACommon.cpp \
../SAInputAccessor.cpp \
../SAInputAccessor_test.cpp \
../SAOutputAccessor.cpp \
../VariableDependencyFinder.cpp \
/home/anirudhj/Downloads/tinyxml/tinystr.cpp \
/home/anirudhj/Downloads/tinyxml/tinyxml.cpp \
/home/anirudhj/Downloads/tinyxml/tinyxmlerror.cpp \
/home/anirudhj/Downloads/tinyxml/tinyxmlparser.cpp 

OBJS += \
./FrameworkCommon.o \
./LoopDependencyResolver.o \
./LoopNestingAnalyzer.o \
./SACommon.o \
./SAInputAccessor.o \
./SAInputAccessor_test.o \
./SAOutputAccessor.o \
./VariableDependencyFinder.o \
./tinystr.o \
./tinyxml.o \
./tinyxmlerror.o \
./tinyxmlparser.o 

CPP_DEPS += \
./FrameworkCommon.d \
./LoopDependencyResolver.d \
./LoopNestingAnalyzer.d \
./SACommon.d \
./SAInputAccessor.d \
./SAInputAccessor_test.d \
./SAOutputAccessor.d \
./VariableDependencyFinder.d \
./tinystr.d \
./tinyxml.d \
./tinyxmlerror.d \
./tinyxmlparser.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/roseInstall/include -I/home/anirudhj/Downloads/tinyxml -O3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tinystr.o: /home/anirudhj/Downloads/tinyxml/tinystr.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/roseInstall/include -I/home/anirudhj/Downloads/tinyxml -O3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tinyxml.o: /home/anirudhj/Downloads/tinyxml/tinyxml.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/roseInstall/include -I/home/anirudhj/Downloads/tinyxml -O3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tinyxmlerror.o: /home/anirudhj/Downloads/tinyxml/tinyxmlerror.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/roseInstall/include -I/home/anirudhj/Downloads/tinyxml -O3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tinyxmlparser.o: /home/anirudhj/Downloads/tinyxml/tinyxmlparser.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/anirudhj/Downloads/roseInstall/include -I/home/anirudhj/Downloads/tinyxml -O3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


