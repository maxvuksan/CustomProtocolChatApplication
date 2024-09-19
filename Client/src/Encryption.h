#include <cryptlib.h>
#include <osrng.h>
#include <aes.h>
#include <gcm.h>
#include <rsa.h>
#include <hex.h>
#include <filters.h>
#include <files.h>
#include <base64.h>
#include <iostream>
#include <string>
#include <vector>
#include <json.hpp>


class Encryption{

    public:

        static std::vector<CryptoPP::byte> GenerateRandomBytes(size_t length);

};