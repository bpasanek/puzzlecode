// polycube:  A polyomino and polycube puzzle solver.
// Copyright 2011 Matthew T. Busche.
//
// This program is free software:  you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <map>
#include "Config.hpp"
#include "Solver.hpp"
#include "Puzzle.hpp"
#include "NamedPiece.hpp"
#include "boost/program_options.hpp"
#include "PolyPerf.hpp"
#include "PuzzleParser.hpp"

using namespace std;

const string version = "1.2.1";

PerformanceMeter* allMeter  = PolyPerf::getInstance()->getMeter("all");

/** Although I'm impressed with the features of the program_options package,
 ** I find myself writing a lot of boiler-plate code over and over to set
 ** up this normal case of:
 **
 **    o having some options configurable either from file or the command line,
 **    o having some options configurable only from the command line, and
 **    o having final arguments processed as a list (hiddenOpts).
 **
 ** It really shouldn't be this hard to setup this normal case processing.
 ** Assuming I'm not missing some easier way to use the package, I think the
 ** design lacking, but I haven't really thought about it enough to suggest an
 ** alternative.
 **/
void setConfig(Config& config, int argc, char** argv)
{
    using namespace boost::program_options;

    int ll = 80;   // Line Length
    int mdl = 74;  // Minimum description length
    options_description configFileOpts(ll, mdl);
    config.FileConfig::add_options(configFileOpts);

    options_description cmdLineOpts("Command line only options", ll, mdl);
    config.CmdLineConfig::add_options(cmdLineOpts);

    options_description hiddenOpts("Hidden options", ll, mdl);
    config.HiddenConfig::add_options(hiddenOpts);

    options_description visibleOpts(
        "Takes a list of polycube puzzle definition files and outputs all solutions to\n"
        "them.  If no puzzle definition files are provided, puzzle definitions are read\n"
        "from standard input.  Common configuration options are");

    visibleOpts.add(configFileOpts);
    visibleOpts.add(cmdLineOpts);

    options_description allOpts;
    allOpts.add(configFileOpts);
    allOpts.add(cmdLineOpts);
    allOpts.add(hiddenOpts);

    options_description genericOpts;
    genericOpts.add(configFileOpts);
    genericOpts.add(hiddenOpts);

    positional_options_description pod;
    pod.add("inputFile", -1);

    variables_map vm;
    store(command_line_parser(argc, argv).options(allOpts).positional(pod).run(), vm);

    if(vm.count("conf"))
    {
        ifstream ifs(vm["conf"].as<string>().c_str());
        store(parse_config_file(ifs, genericOpts), vm);
    }

    notify(vm);

    if(config.getHelp())
    {
        cout << visibleOpts;
        exit(1);
    }

    if(config.getVersion())
    {
        cout << "Polycube, Version " << version << ".  Copyright 2011, Matthew T. Busche." << endl;
        exit(1);
    }

    config.validate();
}


void configSolver(Solver& solver, const Config& config)
{
    solver.setBruijn(config.getBruijn());
    solver.setOutputFormat(config.getOutputFormat());
    solver.setEmch(config.getEmch());
    solver.setFitFilter(config.getFitFilter());
    solver.setGoal(config.getGoal());
    solver.setInfo(config.getInfo());
    solver.setMch(config.getMch());
    solver.setMonteCarlo(config.getMonteCarlo());
    solver.setOrderingHeuristicConfig(config.getOrderingHeuristicConfig());
    solver.setParityBacktrack(config.getParityBacktrack());
    solver.setParityFilter(config.getParityFilter());
    solver.setQuiet(config.getQuiet());
    solver.setRedundancyFilterName(config.getRedundancyFilter());
    solver.setRedundancyFilterFirst(config.getRedundancyFilterFirst());
    solver.setTrace(config.getTrace());
    solver.setUnique(config.getUnique());
    solver.setVolumeBacktrack(config.getVolumeBacktrack());
    solver.setVolumeFilter(config.getVolumeFilter());
}


void solveStream(istream& is, const string& inputName, const Config& config)
{
    PuzzleParser parser;
    PuzzleStream puzzleStream(is, inputName);
    PuzzleConfig puzzleConfig;
    
    bool more = true;
    while(more)
    {
        {
            PerformanceMeasurement measure(allMeter);
            more = parser.parse(puzzleStream, puzzleConfig);
            if(more)
            {
                Solver solver;
                configSolver(solver, config);
                Puzzle puzzle(puzzleConfig);
                solver.solve(puzzle);
            }
        }
        if(more && config.getInfo())
            std::cout << std::endl << *PolyPerf::getInstance();
        allMeter->clear();
    }
}


int main(int argc, char** argv)
{
    try
    {
        Config config;
        {
            setConfig(config, argc, argv);

            if(config.getInfo())
            {
                cout << endl << "# Solver configuration settings\n";
                config.FileConfig::print(cout) << endl;
            }


            if(config.getInputFileList().size() > 0)
            {
                for(int fi = 0; fi < config.getInputFileList().size(); ++fi)
                {
                    string fileName = config.getInputFileList()[fi];

                    if(config.getInfo())
                        cout << endl << "# Puzzle file" << endl <<
                            "PUZZLE_FILE=" << fileName << endl;
                    ifstream ifs(fileName.c_str(), ifstream::in);
                    if(!ifs)
                    {
                        ostringstream errMsg;
                        errMsg << "***Puzzle Parsing Error:\n\n"
                            "Trouble opening file " << fileName << "." << endl;
                        throw runtime_error(errMsg.str());
                    }
                    solveStream(ifs, fileName, config);
                }
            }
            else
            {
                if(config.getInfo())
                    cout << endl << "# Puzzle file" << endl <<
                        "PUZZLE_FILE=STDIN" << endl;
                solveStream(cin, "stdin", config);
            }
        }
    }
    catch(exception& x)
    {
        cerr << "Exception: " << x.what() << endl;
    }

    return 0;
}
