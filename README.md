# CommunalLine - A KRL Backend Written in C++

## Use
```
cd CommunalLine
./build/bin/server
./build/bin/client
# then run localhost:3000 on your browser
```

## Build

### Linux/macOS
**Dependencies:**
- OpenSSL (see distro instructions on how to install OpenSSL)

**Clone the repo**
```
git clone https://github.com/kendy-lovely/CommunalLine
cd CommunalLine
mkdir build/bin
```

**GCC**
```
g++ -lssl -lcrypto src/server.cpp -o build/bin/server
g++ -lssl -lcrypto src/client.cpp -o build/bin/client
```

**Clang**
```
clang++ -lssl -lcrypto src/server.cpp -o build/bin/server
clang++ -lssl -lcrypto src/client.cpp -o build/bin/client
```

### Windows
**Dependencies:**
- OpenSSL

**Getting OpenSSL**
Use `vcpkg`
```
vcpkg install openssl
```
or get from a [binary](https://slproweb.com/products/Win32OpenSSL.html)

**Clone the repo**
```
git clone https://github.com/kendy-lovely/CommunalLine
cd CommunalLine
mkdir build\bin
```

If installed in a custom path, link OpenSSL using the `-I` and `-L` flags.

**GCC**
```
g++ src/server.cpp -I"C:\Program Files\OpenSSL-Win64\include" -L"C:\Program Files\OpenSSL-Win64\lib" -lssl -lcrypto -lws2_32 -o build/bin/server.exe
g++ src/client.cpp -I"C:\Program Files\OpenSSL-Win64\include" -L"C:\Program Files\OpenSSL-Win64\lib" -lssl -lcrypto -lws2_32 -o build/bin/client.exe
```

**Clang**
```
clang++ src/server.cpp -I"C:\Program Files\OpenSSL-Win64\include" -L"C:\Program Files\OpenSSL-Win64\lib" -lssl -lcrypto -lws2_32 -o build/bin/server.exe
clang++ src/client.cpp -I"C:\Program Files\OpenSSL-Win64\include" -L"C:\Program Files\OpenSSL-Win64\lib" -lssl -lcrypto -lws2_32 -o build/bin/client.exe
```

**MSVC**
```
cl /EHsc /I"C:\Program Files\OpenSSL-Win64\include" src/server.cpp /link /LIBPATH:"C:\Program Files\OpenSSL-Win64\lib" libssl.lib libcrypto.lib ws2_32.lib /OUT:build/bin/server.exe
cl /EHsc /I"C:\Program Files\OpenSSL-Win64\include" src/client.cpp /link /LIBPATH:"C:\Program Files\OpenSSL-Win64\lib" libssl.lib libcrypto.lib ws2_32.lib /OUT:build/bin/client.exe
```
