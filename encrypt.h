#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>

namespace Encryption
{
    std::string key, auxKey;
    int scrambleFactor;
    const std::string &B64  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefgijklmnopqrstuvwxyz0123456789<=>";
    const std::string &SALT = "~`!@#$%^&*()+/,.\'{}\\[]\";:|-_";

    struct CryptExtraction
    {
        std::string str;
        int scrambleFactor;
    };

    CryptExtraction extractDetails(std::string s);
    CryptExtraction extractDetails(std::string, const short int);
    void initialise(const std::string, const std::string);
    void initialise(const std::string, const int);
    int Hash(const std::string);
    void calculateScrambleFactor(const std::string, const std::string);
    void findPosInB64(unsigned int pos[5], char c1, char c2, char c3, char c4, char c5);
    std::string addSalt(std::string s);
    std::string removeSalt(std::string s);
    std::string B16B64_Encode(std::string);
    std::string B16B64_Decode(const std::string);

    std::string B16B64_Encode(const std::string s){
        std::string enc;
        unsigned int sz = key.size(), k = -1;

        for(unsigned int i=0; i<s.size(); i+=3){
            //|xxxx0000|00000000|
            enc.push_back( B64[((int)((s[i] & 0xF0) >> 4)                                        ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            //|0000xxxx|xx000000|
            if(i+1 < s.size())
                enc.push_back( B64[((int)(((s[i] & 0xF) << 2) | ((s[i + 1] & 0xC0) >> 6 ))       ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            else
                enc.push_back( B64[((int)(((s[i] & 0xF) << 2) | (('-' & 0xC0) >> 6 ))            ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            //|00xxxx00|00000000|
            if(i+1 < s.size())
                enc.push_back( B64[((int)((s[i + 1] & 0x3C) >> 2)                                ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            else
                enc.push_back( B64[((int)(('-' & 0x3C) >> 2)                                     ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            //|000000xx|xxxx0000|
            if(i+1 < s.size() && i+2 < s.size())
                enc.push_back( B64[((int)(((s[i + 1] & 0x3) << 4) | ((s[i + 2] & 0xF0) >> 4))    ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            else if(i+1 < s.size())
                enc.push_back( B64[((int)(((s[i + 1] & 0x3) << 4) | (('?' & 0xF0) >> 4))         ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            else
                enc.push_back( B64[((int)((('-' & 0x3) << 4) | (('?' & 0xF0) >> 4))              ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            //|0000xxxx|--------|
            if(i+2 < s.size())
                enc.push_back( B64[((int)(s[i + 2] & 0xF)                                        ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
            else
                enc.push_back( B64[((int)('?' & 0xF)                                             ^ ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F )) ]);
        }

        enc = addSalt(enc);
        return enc;
    }

    std::string B16B64_Decode(std::string s){
        std::string dec;
        unsigned int sz = key.size(), k = -1, pos[5];

        s = removeSalt(s);
        for(unsigned int i = 0; i<s.size(); i+=5){
            findPosInB64(pos, s[i], s[i+1], s[i+2], s[i+3], s[i+4]);
            for(unsigned int j = 0; j<5; ++j)
                pos[j] ^= ( ((int)(key[++k % sz]) ^ scrambleFactor) & 0x3F );

            dec.push_back( (((pos[0] & 0xF) << 4) | ((pos[1] & 0x3C) >> 2)) );
            dec.push_back( (((pos[1] & 0x3) << 6) | ((pos[2] & 0xF) << 2) | ((pos[3] & 0x30) >> 4)) );
            dec.push_back( (((pos[3]  & 0xF) << 4)|  (pos[4] & 0xF)) );
        }

        return dec;
    }

    std::string addSalt(std::string s){
        srand(time(nullptr));
        std::size_t rPos = 0, rChar = 0;
        unsigned int sz = s.size() + 2 * (rand() % (s.size()/4) + 1);

        do{
            rPos    = rand() % s.size();
            rChar   = rand() % SALT.size();
            s.insert(rPos, &SALT[rChar], 1);
        }while(s.size() < sz);

        return s;
    }

    std::string removeSalt(std::string s){
        std::size_t pos;
        for(std::size_t i = 0; i<SALT.size(); ++i){
            pos = s.find(SALT[i]);
            while( pos != std::string::npos ){
                s.erase(pos, 1);
                pos = s.find(SALT[i]);
            }
        }

        return s;
    }

    void findPosInB64(unsigned int pos[5], char c1, char c2, char c3, char c4, char c5){
        for(unsigned int i = 0; i<B64.size(); ++i){
            if(B64[i] == c1) pos[0] = i;
            if(B64[i] == c2) pos[1] = i;
            if(B64[i] == c3) pos[2] = i;
            if(B64[i] == c4) pos[3] = i;
            if(B64[i] == c5) pos[4] = i;
        }
    }

    void calculateScrambleFactor(const std::string s, const std::string data){
        int nr = s.size();
        int MOD = 1e9;
        int sum = 0, prod = 1;

        for(unsigned int i = 0; i<s.size(); ++i){
            sum +=  (int(s[i]) * i) % MOD;
            prod = (prod * (s[i] + i)) % MOD;

            if(prod == 0)
                prod = 1337;
            if(sum == 0)
                sum = 7331;
        }

        scrambleFactor = (((nr ^ ( (sum > prod) ? sum/prod: prod/sum)) ^ Hash(s)) ^ Hash(data));
    }

    int Hash(const std::string s){
        int hs = 5381;

        for(unsigned int i=0; i<s.size(); ++i)
            hs = ((hs << 5) + hs) + s[i];

        return hs;
    }

    void initialise(const std::string k, const std::string s){
        key = k;
        calculateScrambleFactor(key, s);
    }

    void initialise(const std::string k, const int scr){
        key = k;
        scrambleFactor = scr;
    }

    CryptExtraction extractDetails(std::string s){
        CryptExtraction data;
        char num[101];
        std::size_t p, u;

        p = s.find(";");
        if(p != std::string::npos)
            u = s.find(";", p + 1);

        if(p != std::string::npos && u != std::string::npos){
            //scrambleFactor is located somewhere in the middle
            s.copy(num, u - p - 1, p + 1);
            data.str = s.replace(p, u - p + 1, "");
            data.scrambleFactor = atoi(num);
        }
        else if(p != std::string::npos){
            //scrambleFactor is either left or right
            bool side = true;

            for(std::size_t i = 0; i<p; ++i)
                if(!(s[i]>='0' && s[i]<='9')){
                    side = false;
                    break;
                }

            if(side){   //left
                s.copy(num, p, 0);
                data.str = s.replace(0, p + 1, "");
                data.scrambleFactor = atoi(num);
            }
            else{       //right
                s.copy(num, s.size() - p, p + 1);
                data.str = s.replace(p, s.size() - p + 1, "");
                data.scrambleFactor = atoi(num);
            }

        }
        else{
            data.str = " ";
            data.scrambleFactor = 0;
        }

        return data;
    }

    CryptExtraction extractDetails(std::string s, const short int toDo){
        CryptExtraction data;
        char num[101];
        std::size_t p = -1, u = -1;
        bool ok = true;

        p = s.find(";");
        if(p != std::string::npos)
            u = s.find(";", p + 1);

        switch(toDo){
        case 0:         //LEFT
            if(p == std::string::npos){
                ok = false;
                break;
            }
            s.copy(num, p, 0);
            data.str = s.replace(0, p + 1, "");
            data.scrambleFactor = atoi(num);
            break;
        case 1:         //CENTER
            if(p != std::string::npos && u != std::string::npos){
                s.copy(num, u - p - 1, p + 1);
                data.str = s.replace(p, u - p + 1, "");
                data.scrambleFactor = atoi(num);
            }
            else ok = false;
            break;
        case 2:         //RIGHT
            if(p ==std::string::npos){
                ok = false;
                break;
            }
            s.copy(num, s.size() - p, p + 1);
            data.str = s.replace(p, s.size() - p + 1, "");
            data.scrambleFactor = atoi(num);
            break;
        }

        if(!ok){
            data.str = " ";
            data.scrambleFactor = 0;
        }

        return data;
    }
}


#endif // ENCRYPT_H
