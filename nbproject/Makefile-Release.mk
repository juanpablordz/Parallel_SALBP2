#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Activity.o \
	${OBJECTDIR}/ActivityCandidateStations.o \
	${OBJECTDIR}/AllCandidateStations.o \
	${OBJECTDIR}/AssemblyLine.o \
	${OBJECTDIR}/Balas.o \
	${OBJECTDIR}/BinaryMixer.o \
	${OBJECTDIR}/BinaryRestriction.o \
	${OBJECTDIR}/Constant.o \
	${OBJECTDIR}/CoordinatesVariable.o \
	${OBJECTDIR}/DurationNode.o \
	${OBJECTDIR}/DurationTree.o \
	${OBJECTDIR}/Equation.o \
	${OBJECTDIR}/FileReader.o \
	${OBJECTDIR}/Graph.o \
	${OBJECTDIR}/GraphCreator.o \
	${OBJECTDIR}/GroupFamilies.o \
	${OBJECTDIR}/Heuristic.o \
	${OBJECTDIR}/InfoVariableAssignation.o \
	${OBJECTDIR}/Integrator.o \
	${OBJECTDIR}/Mixer.o \
	${OBJECTDIR}/Model.o \
	${OBJECTDIR}/NegRestriction.o \
	${OBJECTDIR}/PrecedenceRestriction.o \
	${OBJECTDIR}/RestrictionGenerator.o \
	${OBJECTDIR}/Restrictions.o \
	${OBJECTDIR}/Simplifier.o \
	${OBJECTDIR}/Station.o \
	${OBJECTDIR}/Utilities.o \
	${OBJECTDIR}/Variable.o \
	${OBJECTDIR}/VariableFamily.o \
	${OBJECTDIR}/VectorBR.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/stdafx.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/balanceodelineas

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/balanceodelineas: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/balanceodelineas ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/Activity.o: Activity.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Activity.o Activity.cpp

${OBJECTDIR}/ActivityCandidateStations.o: ActivityCandidateStations.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/ActivityCandidateStations.o ActivityCandidateStations.cpp

${OBJECTDIR}/AllCandidateStations.o: AllCandidateStations.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AllCandidateStations.o AllCandidateStations.cpp

${OBJECTDIR}/AssemblyLine.o: AssemblyLine.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/AssemblyLine.o AssemblyLine.cpp

${OBJECTDIR}/Balas.o: Balas.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Balas.o Balas.cpp

${OBJECTDIR}/BinaryMixer.o: BinaryMixer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/BinaryMixer.o BinaryMixer.cpp

${OBJECTDIR}/BinaryRestriction.o: BinaryRestriction.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/BinaryRestriction.o BinaryRestriction.cpp

${OBJECTDIR}/Constant.o: Constant.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Constant.o Constant.cpp

${OBJECTDIR}/CoordinatesVariable.o: CoordinatesVariable.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/CoordinatesVariable.o CoordinatesVariable.cpp

${OBJECTDIR}/DurationNode.o: DurationNode.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DurationNode.o DurationNode.cpp

${OBJECTDIR}/DurationTree.o: DurationTree.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DurationTree.o DurationTree.cpp

${OBJECTDIR}/Equation.o: Equation.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Equation.o Equation.cpp

${OBJECTDIR}/FileReader.o: FileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/FileReader.o FileReader.cpp

${OBJECTDIR}/Graph.o: Graph.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Graph.o Graph.cpp

${OBJECTDIR}/GraphCreator.o: GraphCreator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/GraphCreator.o GraphCreator.cpp

${OBJECTDIR}/GroupFamilies.o: GroupFamilies.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/GroupFamilies.o GroupFamilies.cpp

${OBJECTDIR}/Heuristic.o: Heuristic.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Heuristic.o Heuristic.cpp

${OBJECTDIR}/InfoVariableAssignation.o: InfoVariableAssignation.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/InfoVariableAssignation.o InfoVariableAssignation.cpp

${OBJECTDIR}/Integrator.o: Integrator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Integrator.o Integrator.cpp

${OBJECTDIR}/Mixer.o: Mixer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Mixer.o Mixer.cpp

${OBJECTDIR}/Model.o: Model.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Model.o Model.cpp

${OBJECTDIR}/NegRestriction.o: NegRestriction.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/NegRestriction.o NegRestriction.cpp

${OBJECTDIR}/PrecedenceRestriction.o: PrecedenceRestriction.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/PrecedenceRestriction.o PrecedenceRestriction.cpp

${OBJECTDIR}/RestrictionGenerator.o: RestrictionGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/RestrictionGenerator.o RestrictionGenerator.cpp

${OBJECTDIR}/Restrictions.o: Restrictions.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Restrictions.o Restrictions.cpp

${OBJECTDIR}/Simplifier.o: Simplifier.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Simplifier.o Simplifier.cpp

${OBJECTDIR}/Station.o: Station.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Station.o Station.cpp

${OBJECTDIR}/Utilities.o: Utilities.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Utilities.o Utilities.cpp

${OBJECTDIR}/Variable.o: Variable.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Variable.o Variable.cpp

${OBJECTDIR}/VariableFamily.o: VariableFamily.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/VariableFamily.o VariableFamily.cpp

${OBJECTDIR}/VectorBR.o: VectorBR.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/VectorBR.o VectorBR.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/stdafx.o: stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/stdafx.o stdafx.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/balanceodelineas

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
