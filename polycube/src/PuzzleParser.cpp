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

#include "PuzzleParser.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"
#include <set>
#include "util.hpp"

using namespace std;
using boost::lexical_cast;

typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
typedef boost::char_separator<char> CharSeparator;


void trimAll(vector<string>& list)
{
    for(int i = 0; i < list.size(); ++i)
        trim(list[i]);
}


PuzzleParser::PuzzleParser()
    :
        puzzleStream(NULL),
        puzzleConfig(NULL),
        pieceId(0),
        parseMeter(NULL)
{
}


bool PuzzleParser::parse(PuzzleStream& puzzleStream, PuzzleConfig& puzzleConfig)
{
    if(parseMeter == NULL)
        parseMeter = PolyPerf::getInstance()->getMeter("parsing", "all");
    PerformanceMeasurement measure(parseMeter);

    try
    {
        this->puzzleStream = &puzzleStream;
        this->puzzleConfig = &puzzleConfig;

        this->pieceId = 0;
        pieceMap.clear();
        puzzleConfig.clear();

        string line;
        while(++puzzleStream.lineNumber, getline(puzzleStream.inputStream, line, '\n'))
        {
            int pound;
            if((pound = line.find('#')) != string::npos)
                line = line.substr(0, pound);

            // Trim leading and trailing whitespace.  Then if the line is
            // empty just skip it.
            //
            trim(line);
            if(line.length() == 0)
                continue;

            Tokenizer fieldsTok(line, CharSeparator(":", "", boost::keep_empty_tokens)); 
            vector<std::string> fields(fieldsTok.begin(), fieldsTok.end());
            trimAll(fields);
            string directiveType = fields[0];

            if(directiveType == "D")
            {
                if(puzzleConfig.xDim != 0)
                {
                    ostringstream errMsg;
                    errMsg << "Multiple definition (D) directives for puzzle definition.\n"
                        ">>> " << line;
                    throw runtime_error(errMsg.str());
                }

                parseD(fields, line);
                continue;
            }

            if(puzzleConfig.xDim == 0)
            {
                ostringstream errMsg;
                errMsg << "Unexpected directive type:  " << directiveType << ".\n"
                    "The first puzzle directive must be of type definition (D).\n"
                    ">>> " << line;
                throw runtime_error(errMsg.str());
            }

            if(directiveType == "C")
            {
                parseC(fields, line);
                continue;
            }

            if(directiveType == "L")
            {
                parseL(fields, line);
                continue;
            }

            if(directiveType != "~D")
            {
                ostringstream errMsg;
                errMsg << "Unknown directive type:  " << directiveType << ".\n"
                    ">>> " << line;
                throw runtime_error(errMsg.str());
            }

            if(pieceMap.size() > MAX_NUM_PIECES)
            {
                ostringstream errMsg;
                errMsg << "Too many pieces:  " << pieceMap.size() << ".\n"
                    "Maximum number of pieces limited to " << MAX_NUM_PIECES << ".\n"
                    "Recompile with preprocessor option -DPIECEID_SIZE=16 or\n"
                    "-DPIECEID_SIZE=32 to model larger puzzles.";
                throw runtime_error(errMsg.str());
            }
            return true;
        }
        if(puzzleConfig.xDim > 0)
        {
            ostringstream errMsg;
                errMsg << "Unexpected end of file.\n";
            throw runtime_error(errMsg.str());
        }
        return false;
    }
    catch(exception& x)
    {
        ostringstream errMsg;
        errMsg << "***Puzzle Parsing Error at line " << puzzleStream.lineNumber <<
                " of input stream " << puzzleStream.inputName << ".\n\n" <<
                x.what();
        throw runtime_error(errMsg.str());
    }
}


void PuzzleParser::verifyNoDuplicateSettings(const vector<string>& fields)
{
    set<string> used;
    for(int f = 1; f < fields.size(); ++f)
    {
        Tokenizer settingTok(fields[f], CharSeparator("=", "", boost::keep_empty_tokens));
        vector<string> setting(settingTok.begin(), settingTok.end());
        trimAll(setting);
        string tag = setting[0];
        if(used.find(tag) != used.end())
        {
            ostringstream errMsg;
            errMsg << "Setting " << tag << " listed twice in " << fields[0] << " directive.\n";
            throw runtime_error(errMsg.str());
        }
        used.insert(tag);
    }
}


void PuzzleParser::verifySetting(const vector<string>& setting, bool required)
{
    if(setting.size() > 2)
    {
        ostringstream errMsg;
        errMsg << "Equals sign appears more than once for " << setting[0] << " setting.\n";
        throw runtime_error(errMsg.str());
    }
    else if(setting.size() == 1 && required)
    {
        ostringstream errMsg;
        errMsg << setting[0] << " setting requires a value.\n";
        throw runtime_error(errMsg.str());
    }
}


void PuzzleParser::parseD(const vector<string>& fields, const std::string& line)
{
    verifyNoDuplicateSettings(fields);
    for(int f = 1; f < fields.size(); ++f)
    {
        Tokenizer settingTok(fields[f], CharSeparator("=", "", boost::keep_empty_tokens));
        vector<string> setting(settingTok.begin(), settingTok.end());
        trimAll(setting);
        const string& tag = setting[0];
        if(tag == "xDim")
        {
            verifySetting(setting, true);
            puzzleConfig->xDim = lexical_cast<int>(setting[1]);
        }
        else if(tag == "yDim")
        {
            verifySetting(setting, true);
            puzzleConfig->yDim = lexical_cast<int>(setting[1]);
        }
        else if(tag == "zDim")
        {
            verifySetting(setting, true);
            puzzleConfig->zDim = lexical_cast<int>(setting[1]);
        }
        else if(tag == "oneSide")
        {
            verifySetting(setting, false);
            if(setting.size() > 1)
                puzzleConfig->oneSide = lexical_cast<bool>(setting[1]);
            else
                puzzleConfig->oneSide = true;
        }
        else
        {
            ostringstream errMsg;
            errMsg << "Invalid definition (D) directive.\n"
                "Unknown setting '" << tag << "'.\n"
                "Permissible settings are:  xDim, yDim, zDim, and oneSide.\n"
                ">>> " << line;
            throw runtime_error(errMsg.str());
        }
    }

    if(puzzleConfig->xDim < 1 || puzzleConfig->yDim < 1 || puzzleConfig->zDim < 1)
    {
        ostringstream errMsg;
        errMsg << "Invalid definition (D) directive.\n"
            "The provided puzzle dimensions " << puzzleConfig->xDim << "x" << puzzleConfig->yDim << "x" << puzzleConfig->zDim << " are invalid.\n"
            "All dimensions must have a value of at least 1.\n"
            ">>> " << line;
        throw runtime_error(errMsg.str());
    }

    if(puzzleConfig->oneSide && puzzleConfig->zDim != 1)
    {
        ostringstream errMsg;
        errMsg << "Invalid definition (D) directive.\n"
            "One-sided puzzles must have a Z dimension of 1.\n"
            ">>> " << line;
        throw runtime_error(errMsg.str());
    }
}


void PuzzleParser::parseC(const vector<string>& fields, const std::string& line)
{
    verifyNoDuplicateSettings(fields);
    string name;
    Mobility mobility = MOBILE;
    vector<Point> points;
    
    for(int f = 1; f < fields.size(); ++f)
    {
        Tokenizer settingTok(fields[f], CharSeparator("=", "", boost::keep_empty_tokens));
        vector<string> setting(settingTok.begin(), settingTok.end());
        trimAll(setting);
        const string& tag = setting[0];
        if(tag == "name")
        {
            verifySetting(setting, true);
            name = setting[1];

            if(pieceMap.find(name) != pieceMap.end())
            {
                ostringstream errMsg;
                errMsg << "Invalid coordinate (C) directive.\n"
                    "Piece with name '" << name << "' was previously defined."
                    ">>> " << line;
                throw runtime_error(errMsg.str());
            }
        }
        else if(tag == "type")
        {
            verifySetting(setting, true);
            string value = setting[1];
            if(value == "M")
                mobility = MOBILE;
            else if(value == "S")
                mobility = STATIONARY;
            else
            {
                ostringstream errMsg;
                errMsg << "Invalid coordinate (C) directive.\n"
                    "Type field must be one of 'M' (mobile) or 'S' (stationary).\n"
                    ">>> " << line;
                throw runtime_error(errMsg.str());
            }
        }
        else if(tag == "layout")
        {
            verifySetting(setting, true);
            Tokenizer coordinateTok(setting[1], CharSeparator(",", "", boost::keep_empty_tokens));
            vector<string> coordinate(coordinateTok.begin(), coordinateTok.end());
            for(int c = 0; c < coordinate.size(); ++c)
            {
                Tokenizer scalarTok(coordinate[c], CharSeparator(" \t"));
                vector<string> scalar(scalarTok.begin(), scalarTok.end());
                if(scalar.size() != 3)
                {
                    ostringstream errMsg;
                    errMsg << "Invalid coordinate (C) directive.\n"
                        "Layout value must be a list of x y z triplets. The triplets must be separated\n"
                        "by a commas, and the x y and z values separated by white space.  For example:\n"
                        "layout=0 0 0, 1 0 0, 1 1 0\n"
                        ">>> " << line;
                    throw runtime_error(errMsg.str());
                }
                int x = lexical_cast<int>(scalar[0]);
                int y = lexical_cast<int>(scalar[1]);
                int z = lexical_cast<int>(scalar[2]);
                if(puzzleConfig->oneSide && z != 0)
                {
                    ostringstream errMsg;
                    errMsg << "Invalid coordinate (C) directive.\n"
                        "Pieces in one-sided puzzles must have all z coordinates fixed at 0.\n"
                        ">>> " << line;
                    throw runtime_error(errMsg.str());
                }
                points.push_back(Point(x, y, z));
            }
        }
        else
        {
            ostringstream errMsg;
            errMsg << "Invalid coordinate (C) directive.\n"
                "Unknown setting '" << tag << "'.\n"
                "Permissible settings are:  name, type, and layout.\n"
                ">>> " << line;
            throw runtime_error(errMsg.str());
        }
    }

    if(name == "")
    {
        ostringstream errMsg;
        errMsg << "Invalid coordinate (C) directive.\n"
            "name setting is required.\n"
            ">>> " << line;
        throw runtime_error(errMsg.str());
    }

    if(points.size() == 0)
    {
        ostringstream errMsg;
        errMsg << "Invalid coordinate (C) directive.\n"
            "A non-empty layout setting is required.\n"
            ">>> " << line;
        throw runtime_error(errMsg.str());
    }

    NamedPiece& piece = genPiece(pieceMap, name, mobility);
    for(int p = 0; p < points.size(); ++p)
        piece.addPoint(points[p]);

    if(mobility == MOBILE)
        puzzleConfig->mobile.push_back(piece);
    else
        puzzleConfig->stationary.push_back(piece);
}


void PuzzleParser::parseL(const vector<string>& fields, const std::string& line)
{
    verifyNoDuplicateSettings(fields);
    map<string,NamedPiece> layoutDirectivePieceMap;
    for(int f = 1; f < fields.size(); ++f)
    {
        Tokenizer settingTok(fields[f], CharSeparator("=", "", boost::keep_empty_tokens));
        vector<string> setting(settingTok.begin(), settingTok.end());
        trimAll(setting);
        const string& tag = setting[0];
        if(tag == "stationary")
        {
            verifySetting(setting, true);
            Tokenizer nameTok(setting[1], CharSeparator(" \t"));
            vector<string> nameList(nameTok.begin(), nameTok.end());
            for(int n = 0; n < nameList.size(); ++n)
            {
                string name = nameList[n];
                if(pieceMap.find(name) != pieceMap.end())
                {
                    ostringstream errMsg;
                    errMsg << "Invalid layout (L) directive.\n"
                        "Redefinition of piece " << name << ".\n"
                        ">>> " << line;
                    throw runtime_error(errMsg.str());
                }
                genPiece(layoutDirectivePieceMap, name, STATIONARY);
            }
        }
        else
        {
            ostringstream errMsg;
            errMsg << "Invalid layout (L) directive.\n"
                "Unknown setting '" << tag << "'.\n"
                "Permissible settings are:  stationary.\n"
                ">>> " << line;
            throw runtime_error(errMsg.str());
        }
    }

    int y = puzzleConfig->yDim - 1;
    string line2;
    while(true)
    {
        if(++puzzleStream->lineNumber, !getline(puzzleStream->inputStream, line2, '\n'))
        {
            ostringstream errMsg;
            errMsg << "Invalid layout (L) directive.\n"
                "Unexpectedly reached end of file.\n"
                ">>> " << line;
            throw runtime_error(errMsg.str());
        }

        int pound;
        if((pound = line2.find('#')) != string::npos)
            line2 = line2.substr(0, pound);


        istringstream iss(line2);
        string endCheck;
        iss >> endCheck;
        if(endCheck == "~L")
            break;

        Tokenizer layerTok(line2, CharSeparator(",", "", boost::keep_empty_tokens));
        std::vector<string> layer(layerTok.begin(), layerTok.end());
        for(int z = 0; z < layer.size(); ++z)
        {
            Tokenizer nameTok(layer[z], CharSeparator(" \t"));
            std::vector<string> nameList(nameTok.begin(), nameTok.end());
            for(int x = 0; x < nameList.size(); ++x)
            {
                string name = nameList[x];
                if(name != ".")
                {
                    if(pieceMap.find(name) != pieceMap.end())
                    {
                        ostringstream errMsg;
                        errMsg << "Invalid layout (L) directive.\n"
                            "Redefinition of piece '" << name << "'.\n"
                            ">>> " << line;
                        throw runtime_error(errMsg.str());
                    }

                    NamedPiece& piece = genPiece(layoutDirectivePieceMap, name, MOBILE);

                    if(puzzleConfig->oneSide && z != 0)
                    {
                        ostringstream errMsg;
                        errMsg << "Invalid layout (L) directive.\n"
                            "Pieces in one-sided puzzles must have all z coordinates fixed at 0.\n"
                            ">>> " << line;
                        throw runtime_error(errMsg.str());
                    }

                    piece.addPoint(Point(x, y, z));
                }
            }
        }
        --y;
    }

    // Copy the pieces defined in this layout directive to global pieceMap, and
    // to mobile or stationary as appropriate.
    //
    for(map<string,NamedPiece>::iterator i = layoutDirectivePieceMap.begin(); i != layoutDirectivePieceMap.end(); ++i)
    {
        NamedPiece& piece = (*i).second;

        if(piece.size() == 0)
        {
            ostringstream errMsg;
            errMsg << "Invalid layout (L) directive.\n"
                "Declared stationary piece named '" << piece.getName() << "' does not appear in the layout definition." << std::endl <<
                ">>> " << line;
            throw runtime_error(errMsg.str());
        }

        pieceMap.insert(pair<string,NamedPiece>(piece.getName(), piece));
        if(piece.getMobility() == STATIONARY)
            puzzleConfig->stationary.push_back(piece);
        else
            puzzleConfig->mobile.push_back(piece);
    }
}


NamedPiece& PuzzleParser::genPiece(map<string,NamedPiece>& pieceMap, const string& name, Mobility mobility)
{
    map<string,NamedPiece>::iterator i = pieceMap.find(name);
    if(i == pieceMap.end())
        pieceMap.insert(pair<string,NamedPiece>(name, NamedPiece(++pieceId, name, mobility)));
    return (*pieceMap.find(name)).second;
}
