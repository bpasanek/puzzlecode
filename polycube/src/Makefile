PROG =	polycube

SRCS =	AngularOrderingHeuristic.cpp Config.cpp Constants.cpp FileConfig.cpp \
	FitOrderingHeuristic.cpp Image.cpp LinearOrderingHeuristic.cpp \
	main.cpp MonteCarloConfig.cpp OrderingHeuristicConfig.cpp \
	OrderingHeuristicFactory.cpp OutputFormatConfig.cpp \
	PerformanceRegistry.cpp PolyPerf.cpp Puzzle.cpp PuzzleParser.cpp \
	RadialOrderingHeuristic.cpp Rotation.cpp RotationImpl.cpp Shape.cpp \
	Solver.cpp dSFMT.c

OBJS =	AngularOrderingHeuristic.o Config.o Constants.o FileConfig.o \
	FitOrderingHeuristic.o Image.o LinearOrderingHeuristic.o main.o \
	MonteCarloConfig.o OrderingHeuristicConfig.o \
	OrderingHeuristicFactory.o OutputFormatConfig.o PerformanceRegistry.o \
	PolyPerf.o Puzzle.o PuzzleParser.o RadialOrderingHeuristic.o \
	Rotation.o RotationImpl.o Shape.o Solver.o dSFMT.o  

LIBS =	

CC = gcc
CXX = g++
CFLAGS = -O
CXXFLAGS = -O -I/share/apps/boost/1.55.0/include -DDSFMT_MEXP=19937 
LDFLAGS =  -L/share/apps/boost/1.55.0/lib
LIBS=-lboost_program_options -lboost_graph -lboost_regex
all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS) 

.SUFFIXES: $(SUFFIXES) .c .cpp .cxx

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o .cxx.o:
	$(CXX) $(CXXFLAGS) -c $<

AngularOrderingHeuristic.o: AngularOrderingHeuristic.cpp \
 AngularOrderingHeuristic.hpp OrderingHeuristic.hpp Printable.hpp \
 DlxHead.hpp DlxNode.hpp GridPoint.hpp Point.hpp Rotation.hpp \
 RotationImpl.hpp makeArray.hpp Constants.hpp
Config.o: Config.cpp Config.hpp FileConfig.hpp Constants.hpp \
 Printable.hpp MonteCarloConfig.hpp Scanable.hpp OutputFormatConfig.hpp \
 OrderingHeuristicConfig.hpp CmdLineConfig.hpp HiddenConfig.hpp
Constants.o: Constants.cpp Constants.hpp
FileConfig.o: FileConfig.cpp FileConfig.hpp Constants.hpp Printable.hpp \
 MonteCarloConfig.hpp Scanable.hpp OutputFormatConfig.hpp \
 OrderingHeuristicConfig.hpp NamedPiece.hpp Piece.hpp Point.hpp \
 Rotation.hpp RotationImpl.hpp
FitOrderingHeuristic.o: FitOrderingHeuristic.cpp FitOrderingHeuristic.hpp \
 OrderingHeuristic.hpp Printable.hpp DlxHead.hpp DlxNode.hpp \
 GridPoint.hpp Point.hpp Rotation.hpp RotationImpl.hpp makeArray.hpp
Image.o: Image.cpp Image.hpp Printable.hpp GridPoint.hpp DlxHead.hpp \
 DlxNode.hpp Point.hpp Rotation.hpp RotationImpl.hpp makeArray.hpp \
 Shape.hpp NamedPiece.hpp Piece.hpp
LinearOrderingHeuristic.o: LinearOrderingHeuristic.cpp \
 LinearOrderingHeuristic.hpp OrderingHeuristic.hpp Printable.hpp \
 DlxHead.hpp DlxNode.hpp GridPoint.hpp Point.hpp Rotation.hpp \
 RotationImpl.hpp makeArray.hpp
main.o: main.cpp Config.hpp FileConfig.hpp Constants.hpp Printable.hpp \
 MonteCarloConfig.hpp Scanable.hpp OutputFormatConfig.hpp \
 OrderingHeuristicConfig.hpp CmdLineConfig.hpp HiddenConfig.hpp \
 Solver.hpp OrderingHeuristic.hpp DlxHead.hpp DlxNode.hpp \
 PuzzleSolverInterface.hpp Puzzle.hpp PuzzleConfig.hpp NamedPiece.hpp \
 Piece.hpp Point.hpp Rotation.hpp RotationImpl.hpp PolyPerf.hpp \
 PerformanceRegistry.hpp PerformanceMeter.hpp PerformanceMeasurement.hpp \
 Random.hpp ParityMonitor.hpp makeArray.hpp VolumeMonitor.hpp \
 Image.hpp GridPoint.hpp Shape.hpp PuzzleParser.hpp PuzzleStream.hpp
MonteCarloConfig.o: MonteCarloConfig.cpp MonteCarloConfig.hpp \
 Printable.hpp Scanable.hpp
OrderingHeuristicConfig.o: OrderingHeuristicConfig.cpp \
 OrderingHeuristicConfig.hpp Printable.hpp Scanable.hpp util.hpp
OrderingHeuristicFactory.o: OrderingHeuristicFactory.cpp \
 OrderingHeuristicFactory.hpp OrderingHeuristic.hpp Printable.hpp \
 DlxHead.hpp DlxNode.hpp AngularOrderingHeuristic.hpp \
 RadialOrderingHeuristic.hpp LinearOrderingHeuristic.hpp \
 FitOrderingHeuristic.hpp Constants.hpp util.hpp
OutputFormatConfig.o: OutputFormatConfig.cpp OutputFormatConfig.hpp \
 Printable.hpp Scanable.hpp
PerformanceRegistry.o: PerformanceRegistry.cpp PerformanceRegistry.hpp \
 Printable.hpp PerformanceMeter.hpp
PolyPerf.o: PolyPerf.cpp PolyPerf.hpp PerformanceRegistry.hpp \
 Printable.hpp PerformanceMeter.hpp PerformanceMeasurement.hpp
Puzzle.o: Puzzle.cpp Puzzle.hpp Constants.hpp OutputFormatConfig.hpp \
 Printable.hpp Scanable.hpp PuzzleConfig.hpp NamedPiece.hpp Piece.hpp \
 Point.hpp Rotation.hpp RotationImpl.hpp PolyPerf.hpp \
 PerformanceRegistry.hpp PerformanceMeter.hpp PerformanceMeasurement.hpp \
 Random.hpp ParityMonitor.hpp makeArray.hpp VolumeMonitor.hpp \
 Image.hpp GridPoint.hpp DlxHead.hpp DlxNode.hpp Shape.hpp
PuzzleParser.o: PuzzleParser.cpp PuzzleParser.hpp NamedPiece.hpp \
 Piece.hpp Point.hpp Rotation.hpp RotationImpl.hpp Printable.hpp \
 PolyPerf.hpp PerformanceRegistry.hpp PerformanceMeter.hpp \
 PerformanceMeasurement.hpp PuzzleConfig.hpp PuzzleStream.hpp util.hpp
RadialOrderingHeuristic.o: RadialOrderingHeuristic.cpp \
 RadialOrderingHeuristic.hpp OrderingHeuristic.hpp Printable.hpp \
 DlxHead.hpp DlxNode.hpp GridPoint.hpp Point.hpp Rotation.hpp \
 RotationImpl.hpp makeArray.hpp
Rotation.o: Rotation.cpp Rotation.hpp RotationImpl.hpp Printable.hpp \
 makeArray.hpp
RotationImpl.o: RotationImpl.cpp RotationImpl.hpp Printable.hpp
Shape.o: Shape.cpp Shape.hpp DlxHead.hpp DlxNode.hpp Printable.hpp \
 NamedPiece.hpp Piece.hpp Point.hpp Rotation.hpp RotationImpl.hpp \
 Image.hpp GridPoint.hpp makeArray.hpp
Solver.o: Solver.cpp Solver.hpp MonteCarloConfig.hpp Printable.hpp \
 Scanable.hpp OrderingHeuristicConfig.hpp OrderingHeuristic.hpp \
 DlxHead.hpp DlxNode.hpp PuzzleSolverInterface.hpp Puzzle.hpp \
 Constants.hpp OutputFormatConfig.hpp PuzzleConfig.hpp NamedPiece.hpp \
 Piece.hpp Point.hpp Rotation.hpp RotationImpl.hpp PolyPerf.hpp \
 PerformanceRegistry.hpp PerformanceMeter.hpp PerformanceMeasurement.hpp \
 Random.hpp ParityMonitor.hpp makeArray.hpp VolumeMonitor.hpp \
 Image.hpp GridPoint.hpp Shape.hpp OrderingHeuristicFactory.hpp
