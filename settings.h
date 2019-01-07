#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <string>

namespace SettingsReader
{
    inline unsigned int storeFactor, cycles;
    inline std::string key = "";

    inline void readData(){
        std::ifstream file("settings.cbdat");
        char line[101];

        int i=0;
        while(file.getline(line, 100)){
            switch(i){
            case 0:
                try{
                    storeFactor = atoi(line);
                }
                catch(...){
                    storeFactor = 0;
                    continue;
                }
                break;
            case 1:
                key.assign(line);
                break;
            case 2:
                try{
                    cycles = atoi(line);
                }
                catch(...){
                    cycles = 1;
                    continue;
                }
                break;
            }

            ++i;
        }

        file.close();
    }

    inline std::string setData(const unsigned int _storeFactor, const std::string _key, const unsigned int _cycles){
        std::ofstream file("settings.cbdat");

        if(_storeFactor <= 0 || _storeFactor > 3 || _key.compare("") == 0 || _cycles <= 0){
            readData();
            return "Invalid input\nCheck the private key and cycle number integrity\n";
        }

        key         = _key;
        cycles      = _cycles;
        storeFactor = _storeFactor;

        file << storeFactor << '\n' << key << '\n' << cycles << '\n';

        file.close();

        return "Data saved successfully in settings.cbdat";
    }
}

#endif // SETTINGS_H
