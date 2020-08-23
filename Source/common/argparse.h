#pragma once

#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <fstream>


class ArgumentParser {
private:
    struct Argument;
    typedef std::string String;

    String AppName;
    std::vector<Argument> Arguments;

    std::unordered_map<String, String> ArgumentValues;
    MorphLanguageEnum  Language = morphUnknown;
    std::istream* InputStream;
    std::ifstream  InputStreamFile;
    std::ostream* OutputStream;
    std::ofstream  OutputStreamFile;

    static String strip(const String& name) {
        assert (name.size() > 2);
        assert (name.substr(0,2) == "--");
        return name.substr(2);
    }
    
    static String upper(const String& in) {
        String out(in);
        std::transform(out.begin(), out.end(), out.begin(), ::toupper);
        return out;
    }
    
    static String escape(const String& in) {
        String out(in);
        if (in.find(' ') != std::string::npos) out = String("\"").append(out).append("\"");
        return out;
    }

    struct Argument {
        String Name;
        bool Optional = true;
        bool MustHaveValue  = false;
        bool Final = false;
        String Help;
        

        Argument(const String& name, bool _optional, bool hasValue, String help)
                : Name(strip(name)), Optional(_optional),  MustHaveValue(hasValue), Help (help) {
            Final = name.substr(0,1) != "-";
        }
        
        String ToString() const {
            std::ostringstream s;
            String uname = upper(Name);
            if (Optional) s << "[";
            if (!Final) {
                s << "--" << Name;
                s << " ";
            }
            if (MustHaveValue) {
                s << " " << uname << " ";
            }
            if (Optional) s << "]";
            if (!Help.empty()) {
                s << " - " << Help;
            }
            return s.str();
        }
    };


    void ArgumentError(const std::string& msg, bool show_usage = true) const {
        std::cerr << "ArgumentParser error: " << msg << std::endl;
        if (show_usage) std::cerr << Usage() << std::endl;
        exit(-5);
    }

    const Argument* FindNotFreeArgument(String name) const {
        if ((name.length() > 2) && (name.substr(0,2) == "--")) {
            name = name.substr(2);
            for (auto &a: Arguments) {
                if (!a.Final) {
                    if (name == a.Name) {
                        return &a;
                    }
                }
            }
        }
        return nullptr;
    }

    void CheckValidAndInitCommon()  {
        for (auto& a : Arguments) {
            if (!a.Optional && !Exists(a.Name)) {
                ArgumentError(String("argument is missing: ") + a.Name);
            }
            if (Exists(a.Name) && a.MustHaveValue && Retrieve(a.Name).empty()) {
                ArgumentError(String("argument value is missing: ") + a.Name);
            }
            if (a.Name == "language") {
                if (!GetLanguageByString(Retrieve("language"), Language)) {
                    ArgumentError( "bad language " + Retrieve("language") );
                }

            }
            if (a.Name == "input") {
                if (Retrieve("input") != "-") {
                    std::cerr << "read " << Retrieve("input") << "\n";
                    InputStreamFile.open(Retrieve("input"));
                    if (!InputStreamFile.is_open()) {
                        ArgumentError("cannot open file " + Retrieve("input"));
                    }
                    InputStream = &InputStreamFile;
                } else {
                    InputStream = &std::cin;
                }
            }
            if (a.Name == "output") {
                if (Retrieve("output") != "-") {
					// to write unix eoln under windows open files in binary mode
                    OutputStreamFile.open(Retrieve("output"), std::ifstream::out| std::ifstream::binary);
                    if (!OutputStreamFile.is_open()) {
                        ArgumentError("cannot open file " + Retrieve("output"));
                    }
                    OutputStream = &OutputStreamFile;
                } else {
                    OutputStream = &std::cout;
                }
            }
        }
    }

    
public:
    ArgumentParser()  {}

    void AddArgument(const String& name, String help = "", bool _optional = false) {
        Argument arg(name, _optional, true, help);
        Arguments.push_back(arg);
    }

    void AddOption(const String& name, String help = "") {
        Argument arg(name, true, false, help);
        Arguments.push_back(arg);
    }

    void Parse(size_t argc, const char** argv) {
        const Argument* active = nullptr;
        size_t freeArgNo = 0;
        for (size_t argNo = 0; argNo < argc; ++argNo) {
            String argStr = argv[argNo];
            if (argNo == 0) {
                AppName = argStr;
            } else {
                const Argument *newActive = FindNotFreeArgument(argStr);
                if (newActive) {
                    active = newActive;
                    ArgumentValues[active->Name] = "";
                } else if (active && active->MustHaveValue) {
                    ArgumentValues[active->Name] = argStr;
                    active = nullptr;
                } else {
                    for (; freeArgNo < Arguments.size(); ++freeArgNo) {
                        auto &a = Arguments[freeArgNo];
                        if (a.Final) {
                            ArgumentValues[a.Name] = argStr;
                            break;
                        }
                    }
                    if (freeArgNo == Arguments.size()) {
                        ArgumentError(String("bad argument ") + argStr);
                    }
                }
            }
        }
        CheckValidAndInitCommon();
    }
    
    String Retrieve(const String& name) const {
        auto it = ArgumentValues.find(name);
        return (it == ArgumentValues.end()) ? "" : it->second;
    }

    bool Exists(const String& name) const {
        return ArgumentValues.find(name) != ArgumentValues.end();
    }

    MorphLanguageEnum GetLanguage() const {
        return Language;
    }

    std::ostream& GetOutputStream() const {
        return *OutputStream;
    }

    String CloseOutputStreamAndGetName() {
        if (OutputStreamFile.is_open()) {
            OutputStreamFile.close();
        };
        OutputStream  = 0;
        return Retrieve("output");
    }

    std::istream& GetInputStream() const {
        return *InputStream;
    }

    String Usage() const {
        std::stringstream help;
        help << "Usage: " << escape(AppName);
        String indent = "  ";

        // get the required arguments
        for (auto arg : Arguments) {
            if (!arg.Optional &&  !arg.Final)
                help << indent << arg.ToString() << "\n";
        }

        // get the Optional arguments
        for (auto arg : Arguments) {
            if (arg.Optional && !arg.Final )
                help << indent << arg.ToString() << "\n";
        }

        // get the Final arguments
        for (auto arg : Arguments) {
            if (arg.Final)
                help << indent << arg.ToString() << "\n";
        }
        return help.str();
    }

    StringVector GetInputFiles() const {
        std::vector<std::string> files;
        if (Exists("input-is-list-file")) {
            std::string s;
            while (getline(GetInputStream(), s)) {
                Trim(s);
                if (!FileExists(s.c_str())) {
                    std::cerr << "Cannot open " << s << "\n";
                    exit(1);
                };
                files.push_back(s);
            }
        } else
            files.push_back(Retrieve("input"));
        return files;
    }
};
