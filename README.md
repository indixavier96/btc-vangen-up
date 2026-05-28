# Bitcoin Address Generator (C++ / OpenSSL 3)

A simple **C++ Bitcoin wallet generator** built using **OpenSSL 3.x**.  
This project generates **100 Bitcoin P2PKH wallets**, including:

- Private key (hex)
- WIF private key (compressed)
- P2PKH Bitcoin address (Base58Check)

All output is written to `output.txt`.

---

## ⚠️ Security Warning

**This project is for educational and testing purposes only.**

- ❌ Do **NOT** use generated keys for real Bitcoin
- ❌ Do **NOT** share or upload `output.txt`
- ❌ Keys are generated without entropy hardening or secure storage

Use only on offline / test environments.

---

## Features

- OpenSSL **3.x compatible**
- Uses **secp256k1**
- Generates **compressed public keys**
- Correct **HASH160** (SHA256 + RIPEMD160)
- Base58Check encoding
- Outputs **100 wallets**
- Writes results to `output.txt`

---

## Output Format

Each wallet is written in the following format:

===== Wallet 1 =====

Private Key (hex) : c90aa6139c704830ff2c358d6ba3e1161119e51c826d848d18a892d6a73b6982
WIF Private Key : L3xW8BDqZ3f9DBuBqk4CYQnELDL7ixPeCAUhTr6YYgfM1NdijKv
P2PKH Address : 14p5cGy5DZmtNMQwTQiytBvxMVuTmFMSyU


---

## Requirements

- **Windows 10 / 11**
- **Visual Studio 2022 or newer**
- **OpenSSL 3.x**
- **C++17 or newer**

---

## Build Instructions (Visual Studio)

1. Install **OpenSSL 3.x**
2. Open the project in **Visual Studio**
3. Configure include paths:

Project Properties → C/C++ → Additional Include Directories

4. Configure library paths:

Project Properties → Linker → Additional Include Directories


5. Link required libraries:

- libcrypto.lib
- libssl.lib


6. Build in **x64 Debug** or **x64 Release**
7. Run the executable

---

---

## Technical Notes

- Uses OpenSSL **EVP API** (no deprecated functions)
- Public key compression handled manually (required in OpenSSL 3)
- No global OpenSSL initialization needed
- Compatible with Windows builds

---

## Known Limitations

- No BIP32 / HD wallet support
- No Bech32 (`bc1`) addresses
- No testnet support
- No secure key storage or encryption

---

## License

This project is provided **as-is**, without warranty of any kind.  
Use at your own risk.

---

## Author

Created for educational purposes and cryptography learning.

---


## Disclaimer

This software is **not** intended for production or real-world Bitcoin usage.
