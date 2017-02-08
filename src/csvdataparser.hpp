#ifndef CSVDATAPARSER_HPP
#define CSVDATAPARSER_HPP

/*
 *  Credit for parser goes to "sastanin" in http://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
*/

#include <map>
#include <vector>
#include <string>
#include <sstream>      // std::stringstream
#include <iostream>     // std::ios, std::istream, std::cout
#include <fstream>      // std::filebuf

namespace Utils {

class CsvData {
public:
    typedef std::map<int, std::vector<std::string> > DataMapType;

    CsvData() {}
    virtual ~CsvData() {}

    std::vector<std::string> getHeaders() { return mHeaders; }
    DataMapType getData() { return mRowDataMap; }

public:
    bool parseCsvDataFile(const std::string& inFileName)
    {
        std::ifstream fb(inFileName);
        if (fb)
        {
            std::vector<std::vector<std::string> > data = readCSV(fb);
            fb.close();

            mHeaders = data[0];
            for (size_t n=1; n<data.size(); n++)
            {
                mRowDataMap[n-1] = data[n];
            }
            return true;
        }
        return false;
    }

    enum class CSVState {
        UnquotedField,
        QuotedField,
        QuotedQuote
    };

    std::vector<std::string> readCSVRow(const std::string &row) {
        CSVState state = CSVState::UnquotedField;
        std::vector<std::string> fields {""};
        size_t i = 0; // index of the current field
        for (char c : row) {
            switch (state) {
                case CSVState::UnquotedField:
                    switch (c) {
                        case ',': // end of field
                                  fields.push_back(""); i++;
                                  break;
                        case '"': state = CSVState::QuotedField;
                                  break;
                        default:  fields[i].push_back(c);
                                  break; }
                    break;
                case CSVState::QuotedField:
                    switch (c) {
                        case '"': state = CSVState::QuotedQuote;
                                  break;
                        default:  fields[i].push_back(c);
                                  break; }
                    break;
                case CSVState::QuotedQuote:
                    switch (c) {
                        case ',': // , after closing quote
                                  fields.push_back(""); i++;
                                  state = CSVState::UnquotedField;
                                  break;
                        case '"': // "" -> "
                                  fields[i].push_back('"');
                                  state = CSVState::QuotedField;
                                  break;
                        default:  // end of quote
                                  state = CSVState::UnquotedField;
                                  break; }
                    break;
            }
        }
        return fields;
    }

    /// Read CSV file, Excel dialect. Accept "quoted fields ""with quotes"""
    std::vector<std::vector<std::string> > readCSV(std::istream &in) {
        std::vector<std::vector<std::string> > table;
        std::string row;
        while (true) {
            std::getline(in, row);
            if (in.bad() || in.eof()) {
                break;
            }
            auto fields = readCSVRow(row);
            table.push_back(fields);
        }
        return table;
    }

    bool isEmpty()
    {
        return (mRowDataMap.size() == 0);
    }

    void clear()
    {
        mRowDataMap.clear();
        mHeaders.clear();
    }

public:
    std::vector<std::string> mHeaders; // <heading index, header string>
    DataMapType mRowDataMap; // <row number, row data in a string vector>
};

} /* namespace Utils */

#endif // CSVDATAPARSER_HPP
