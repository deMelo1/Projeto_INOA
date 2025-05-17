## Pré-requisitos

* vcpkg com os pacotes curl e nlohmann-json
* CMake (>= 3.10) disponível no PATH
* Um servidor SMTP que aceite conexões via libcurl (ex.: Gmail, Outlook)
* Arquivo config.txt no mesmo diretório de main.cpp

## Arquivo config.txt

O arquivo deve conter exatamente 4 linhas, sem espaços em branco extras:

destinatario    # Ex.: destino@dominio.com  
smtp_url        # Ex.: smtp://smtp.gmail.com:587  
remetente       # Ex.: seuemail@gmail.com  
senha           # Ex.: sua senha  


## Build

1. Clone ou posicione o vcpkg em C:/vcpkg e instale as dependências:

Terminal  
   cd C:/vcpkg  
   .\vcpkg install curl nlohmann-json  
   .\vcpkg integrate install  


2. Gere o projeto com CMake, apontando para o toolchain do vcpkg:

Terminal  
   cd C:/Projeto_INOA  
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake (tudo na mesma linha)



3. Compile o executável:

Terminal  
   cmake --build build

## Uso

Terminal  
    projeto_acao.exe TICKER VALOR_MÁXIMO VALOR_MÍNIMO

Ex.:  
\build\projeto_acao.exe PETR4.SA 30.00 25.00


## Configuração da chave da API da Alpha Vantage

Configurar a variável de ambiente ALPHAVANTAGE_API_KEY:

Terminal  
   setx ALPHAVANTAGE_API_KEY "SUA_CHAVE_AQUI" 

Troque SUA_CHAVE_AQUI pela sua chave
