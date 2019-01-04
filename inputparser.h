#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#define COMM_MAX 100001

namespace InputParser
{
    char line[COMM_MAX];
    std::vector <std::string> storeStr;
    std::map <char, std::string> commands;
    std::string file = "", key = "", keyword = "", aux = "";
    bool show = false;
    unsigned int cycles = 0;
    unsigned short int baseOp;      //binary representation of base operations (1->true, 0->false)
                                    //S = 1, D = 2, E = 4, H = 8

    template <class T>
    std::string ToString(const T &);
    std::string cryptString(const unsigned int, const std::string, const std::string);
    std::string decryptString(const unsigned int, const std::string, const std::string);
    void cryptFile(const std::string);
    void decryptFile(const std::string);
    void initialise();
    void lineReader();
    void showHelp();

    void initialise(){
        commands.emplace('E', "encrypt");
        commands.emplace('D', "decrypt");
        commands.emplace('K', "keyword");
        commands.emplace('C', "cycles");
        commands.emplace('F', "file");
        commands.emplace('S', "store");
        commands.emplace('P', "private_key");
    }

    void lineReader(){
        initialise();
        char *p;
        std::cin.getline(line, COMM_MAX);

        p = strtok(line, "-");
        while(p){
            switch(p[0]){
                case 'H':
                    baseOp = 8;
                    break;
                case 'E':
                    baseOp = 4;
                    break;
                case 'D':
                    baseOp = 2;
                    break;
                case 'K':
                    keyword = (p + 2);
                    break;
                case 'C':
                    cycles = atoi(p + 2);
                    break;
                case 'F':
                    file = (p + 2);
                    break;
                case 'S':
                    baseOp = 1;
                    break;
                case 'P':
                    key = (p + 2);
                    break;
                default:
                    aux = p;
                    if(aux.compare(0, 6, "string") == 0)
                        storeStr.push_back(p + 7);
                    else if(aux.compare(0, 7, "encrypt") == 0)
                        baseOp = 4;
                    else if(aux.compare(0, 7, "decrypt") == 0)
                        baseOp = 2;
                    else if(aux.compare(0, 5, "store") == 0){
                        baseOp = 1;
                    }
                    else if(aux.compare(0, 11,"private_key") == 0)
                        key = (p + 12);
                    else if(aux.compare(0, 4, "file") == 0)
                        file = (p + 5);
                    else if(aux.compare(0, 6, "cycles") == 0)
                        cycles = atoi(p + 7);
                    else if(aux.compare(0, 7, "keyword") == 0)
                        keyword = (p + 8);
                    else if(aux.compare(0, 4, "help") == 0)
                        baseOp = 8;
                    else if(aux.compare(0, 4, "show") == 0)
                        show = true;
                    break;
            }

            p = strtok(NULL, "-");
        }

        cycles  = (cycles           == 0) ? SettingsReader::cycles  : cycles;
        key     = (key.compare("")  == 0) ? SettingsReader::key     : key;

    }

    void executeLine(){
        std::fstream file_S;

        switch(baseOp){
        case 1:     //S
            if(keyword.compare("") == 0){
                std::cout << "What are you doing? Give me something to do\n--show (data extraction)\n-K (store with this keyword)\n";
                break;
            }
            file = (file.compare("") == 0) ? "data.cbdat" : file;

            if(!show){
                file_S.open(file, std::ios_base::app);
                aux = "";

                for(unsigned int i = 0; i<storeStr.size(); ++i)
                    aux += storeStr[i] + '\n';

                file_S << keyword << "{:CBDATSEP:}" << cryptString(cycles, key, aux) << '\n';
            }
            else{
                file_S.open(file, std::ios::in);

                while(std::getline(file_S, aux)){
                    std::size_t pos = aux.find("{:CBDATSEP:}");

                    if(pos != std::string::npos){
                        if(keyword.compare(aux.substr(0, pos)) == 0)
                            std::cout << decryptString(cycles, key, aux.substr(pos + 12, aux.size() - (pos + 11))) << '\n';
                    }
                    else std::cout << "Ooops file seems to be corrupted...\nManually fix it by finding the missing separator {:CBDATSEP:}\nOr delete the file (data will be lost)\nFormat: KEYWORD + SEP + CRYPT\n";
                }
            }

            std::cout<<"Job done\n";
            break;
        case 2:     //D
            if(file.compare("") == 0 && storeStr.size() == 0){
                std::cout << "What are you doing?\nGive me something to do\n";
                break;
            }
            else if(keyword.compare("") != 0){
                std::cout << "I think you meant -P or --private_key instead of -K or --keyword.\n";
                break;
            }

            if(storeStr.size() > 0)
                for(unsigned int i = 0; i<storeStr.size(); ++i)
                    std::cout << decryptString(cycles, key, storeStr[i]) << '\n';

            if(file.compare("") != 0)
                decryptFile(file);
            break;
        case 4:     //E
            if(file.compare("") == 0 && storeStr.size() == 0){
                std::cout << "What are you doing?\nGive me something to do\n";
                break;
            }
            else if(keyword.compare("") != 0){
                std::cout << "I think you meant -P or --private_key instead of -K or --keyword.\n";
                break;
            }

            if(storeStr.size() > 0)
                for(unsigned int i = 0; i<storeStr.size(); ++i)
                    std::cout << cryptString(cycles, key, storeStr[i]) << '\n';

            if(file.compare("") != 0)
                cryptFile(file);
            break;
        case 8:     //H
            showHelp();
            break;

        }
    }

    void showHelp(){
        for (std::map<char, std::string>::iterator it = commands.begin();
                it != commands.end();
                ++it
            )
            std::cout << '-' << it->first << "; --" << it->second << '\n';

        std::cout << "--show \t\t(used with -S)\n";
        std::cout << "--string \t(used with -E -D)\n";
    }

    std::string cryptString(const unsigned int cycles, const std::string key, const std::string s){
        std::string crypt = s;
        Encryption::initialise(key, s);

        for(unsigned int i = 0; i<cycles; ++i)
            crypt = Encryption::B16B64_Encode(crypt);

        return std::string(crypt + ";" + ToString(Encryption::scrambleFactor));
    }

    void cryptFile(const std::string fl_name){
        try{
            std::ifstream f(fl_name);
            std::string fl_content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

            fl_content = cryptString(cycles, key, fl_content);

            std::ofstream newF(fl_name + ".cbcrypt");
            newF << fl_content;

            std::cout << "Job done\n";
        }
        catch(...){
            std::cout << "Weird... Something went wrong, the file must be in the program exe folder and write it`s correct name";
            return;
        }
    }

    std::string decryptString(const unsigned int cycles, const std::string key, const std::string s){
        std::string decrypt = s;
        Encryption::CryptExtraction data = Encryption::extractDetails(decrypt, 2);
        Encryption::initialise(key, data.scrambleFactor);
        decrypt = data.str;

        for(unsigned int i = 0; i<cycles; ++i)
            decrypt = Encryption::B16B64_Decode(decrypt);

        return decrypt;
    }

    void decryptFile(const std::string fl_name){
        try{
            std::ifstream f(fl_name);
            std::string fl_content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

            fl_content = decryptString(cycles, key, fl_content);

            std::ofstream newF(fl_name + ".cbdecrypt");
            newF << fl_content;

            std::cout << "Job done\n";
        }
        catch(...){
            std::cout << "Weird... Something went wrong, the file must be in the program exe folder and write it`s correct name";
            return;
        }
    }

    template <class T>
    std::string ToString(const T &e){
        std::ostringstream s;
        s << e;
        return s.str();
    }
}

#endif // INPUTPARSER_H
