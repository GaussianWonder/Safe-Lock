#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <string>

namespace SettingsReader
{
    unsigned int storeFactor, cycles;
    std::string key = "";
    std::ifstream file("settings.cbdat");

    void readData(){
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
    }
}


#endif // SETTINGS_H
