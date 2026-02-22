// btc-address-gen.cpp
// OpenSSL 3.x compatible Bitcoin address generator
// Generates 100 wallets and writes output to output.txt

#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/sha.h>
#include <openssl/obj_mac.h>
#include <openssl/param_build.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// ---------- Base58 ----------
static const char* BASE58_ALPHABET =
"123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string base58Encode(const std::vector<unsigned char>& input) {
    std::vector<unsigned char> digits((input.size() * 138 / 100) + 1);
    size_t digitlen = 1;

    for (unsigned char c : input) {
        int carry = c;
        for (size_t j = 0; j < digitlen; ++j) {
            carry += 256 * digits[j];
            digits[j] = carry % 58;
            carry /= 58;
        }
        while (carry) {
            digits[digitlen++] = carry % 58;
            carry /= 58;
        }
    }

    std::string result;
    for (unsigned char c : input) {
        if (c == 0x00)
            result += BASE58_ALPHABET[0];
        else
            break;
    }

    for (size_t i = 0; i < digitlen; ++i)
        result += BASE58_ALPHABET[digits[digitlen - 1 - i]];

    return result;
}

std::string base58Check(const std::vector<unsigned char>& data) {
    unsigned char hash1[SHA256_DIGEST_LENGTH];
    unsigned char hash2[SHA256_DIGEST_LENGTH];

    SHA256(data.data(), data.size(), hash1);
    SHA256(hash1, SHA256_DIGEST_LENGTH, hash2);

    std::vector<unsigned char> extended = data;
    extended.insert(extended.end(), hash2, hash2 + 4);

    return base58Encode(extended);
}

// ---------- Hex ----------
std::string toHex(const std::vector<unsigned char>& data) {
    static const char hexmap[] = "0123456789abcdef";
    std::string out;
    out.reserve(data.size() * 2);
    for (unsigned char b : data) {
        out.push_back(hexmap[b >> 4]);
        out.push_back(hexmap[b & 0x0F]);
    }
    return out;
}

// ---------- HASH160 ----------
std::vector<unsigned char> hash160(const std::vector<unsigned char>& data) {
    unsigned char sha[SHA256_DIGEST_LENGTH];
    SHA256(data.data(), data.size(), sha);

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    std::vector<unsigned char> ripe(20);
    unsigned int len = 0;

    EVP_DigestInit_ex(ctx, EVP_ripemd160(), nullptr);
    EVP_DigestUpdate(ctx, sha, SHA256_DIGEST_LENGTH);
    EVP_DigestFinal_ex(ctx, ripe.data(), &len);
    EVP_MD_CTX_free(ctx);

    return ripe;
}

// ---------- Compress uncompressed EC pubkey ----------
std::vector<unsigned char> compressPubKey(const std::vector<unsigned char>& uncompressed) {
    // Format: 04 || X(32) || Y(32)
    std::vector<unsigned char> compressed(33);

    unsigned char y_last = uncompressed[64];
    compressed[0] = (y_last & 1) ? 0x03 : 0x02;

    std::copy(
        uncompressed.begin() + 1,
        uncompressed.begin() + 33,
        compressed.begin() + 1
    );

    return compressed;
}

// ---------- MAIN ----------
int main() {
    std::ofstream out("output.txt", std::ios::out | std::ios::trunc);
    if (!out) {
        std::cerr << "Failed to open output.txt\n";
        return 1;
    }

    for (int i = 1; i <= 1000; ++i) {
        // ---- Generate EC key (secp256k1) ----
        EVP_PKEY_CTX* kctx = EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr);
        EVP_PKEY* pkey = nullptr;

        OSSL_PARAM_BLD* bld = OSSL_PARAM_BLD_new();
        OSSL_PARAM_BLD_push_utf8_string(
            bld,
            OSSL_PKEY_PARAM_GROUP_NAME,
            "secp256k1",
            0
        );

        OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(bld);

        EVP_PKEY_keygen_init(kctx);
        EVP_PKEY_CTX_set_params(kctx, params);
        EVP_PKEY_keygen(kctx, &pkey);

        OSSL_PARAM_free(params);
        OSSL_PARAM_BLD_free(bld);
        EVP_PKEY_CTX_free(kctx);

        // ---- Private key ----
        BIGNUM* priv_bn = nullptr;
        EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_PRIV_KEY, &priv_bn);

        std::vector<unsigned char> privKey(32);
        BN_bn2binpad(priv_bn, privKey.data(), 32);
        BN_free(priv_bn);

        std::string privHex = toHex(privKey);

        // ---- WIF ----
        std::vector<unsigned char> wifData = { 0x80 };
        wifData.insert(wifData.end(), privKey.begin(), privKey.end());
        wifData.push_back(0x01);
        std::string wif = base58Check(wifData);

        // ---- Uncompressed public key ----
        size_t pub_len = 0;
        EVP_PKEY_get_octet_string_param(
            pkey,
            OSSL_PKEY_PARAM_PUB_KEY,
            nullptr,
            0,
            &pub_len
        );

        std::vector<unsigned char> pubUncompressed(pub_len);
        EVP_PKEY_get_octet_string_param(
            pkey,
            OSSL_PKEY_PARAM_PUB_KEY,
            pubUncompressed.data(),
            pubUncompressed.size(),
            &pub_len
        );

        // ---- Compress public key ----
        std::vector<unsigned char> pubKey = compressPubKey(pubUncompressed);

        // ---- Address ----
        std::vector<unsigned char> pubKeyHash = hash160(pubKey);
        std::vector<unsigned char> addrData = { 0x00 };
        addrData.insert(addrData.end(), pubKeyHash.begin(), pubKeyHash.end());
        std::string address = base58Check(addrData);

        // ---- Write output ----
        out << "===== Wallet " << i << " =====\n";
        out << "Private Key (hex) : " << privHex << "\n";
        out << "WIF Private Key   : " << wif << "\n";
        out << "P2PKH Address     : " << address << "\n\n";

        std::cout << "Generated wallet " << i << "\n";

        EVP_PKEY_free(pkey);
    }

    out.close();
    std::cout << "\nDone. Output written to output.txt\n";
    return 0;
}