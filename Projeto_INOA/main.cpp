#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace std;

// Callback para receber resposta do curl
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* s = static_cast<string*>(userp);
    s->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

// Função que busca o preço via Alpha Vantage
// No terminal escreva (setx ALPHAVANTAGE_API_KEY "SUA_CHAVE_AQUI") para setar sua chave
double get_price(const string& symbol) {
    const char* api_key = getenv("ALPHAVANTAGE_API_KEY");
    if (!api_key) {
        throw runtime_error("Variável ALPHAVANTAGE_API_KEY não definida.");
    }

    string url = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE"
                      "&symbol=" + symbol + "&apikey=" + api_key;

    CURL* curl = curl_easy_init();
    if (!curl) throw runtime_error("Falha ao inicializar libcurl.");

    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        throw runtime_error("Erro curl: " + string(curl_easy_strerror(res)));
    }

    //cout << "Resposta da API: " << response << endl;
    auto j = json::parse(response);
    string price_str = j["Global Quote"]["05. price"];
    return stod(price_str);
}

double get_price_rand(int min, int max) {
    min = min * 0.8;
    max = max * 1.2;
    return min + rand() % (max - min + 1);
}

size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    const char **payload = (const char **)userp;
    size_t len = strlen(*payload);
    if (len) {
        memcpy(ptr, *payload, len);
        *payload += len;
        return len;
    }
    return 0;
}

double derivada(double vetor[], double preco){
    double deriv = 0;
    if(vetor[0] == 0.0){
        vetor[0] = preco;
        return 0.0;
    }
    else if(vetor[1] == 0){
        vetor[1] = preco;
    }
    else{
        vetor[0] = vetor[1];
        vetor[1] = preco;
    }
    deriv = (vetor[1] - vetor[0])/15.0;
    return deriv;
}

string horario_atual(){
    using namespace std::chrono;
    std::ostringstream hora;
    std::ostringstream minuto;
    std::ostringstream segundo;
    string str_hora;
    string str_minuto;
    string str_segundo;
    string retorno;
    // pega a contagem de segundos desde epoch
    auto now = system_clock::now().time_since_epoch();
    auto total_sec = duration_cast<seconds>(now).count();

    // aplica o offset de -3h (em segundos)
    constexpr int OFFSET = -3 * 3600;
    int secs_local = static_cast<int>((total_sec + OFFSET) % 86400);
    if (secs_local < 0) secs_local += 86400;

    int h = secs_local / 3600;
    int m = (secs_local % 3600) / 60;
    int s = secs_local % 60;
    
    hora << h;
    minuto << m;
    segundo << s;
    if(h < 10){ str_hora = "0" + hora.str();}
    else{str_hora = hora.str();}
    if(m < 10){ str_minuto = "0" + minuto.str();}
    else{str_minuto = minuto.str();}
    if(s < 10){ str_segundo = "0" + segundo.str();}
    else{str_segundo = segundo.str();}
    retorno = str_hora + ":" + str_minuto + ":" + str_segundo;
    hora.str("");
    hora.clear();
    minuto.str("");
    minuto.clear();
    segundo.str("");
    segundo.clear();
    return retorno;
}

int main(int argc, char* argv[]) {
    srand(time(0));
    ostringstream preco, dx;
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    string symbol;
    double max, min, aux;
    double vetor[2] = {0,0};
    double deriv = 0;
    string destinatario;
    string endereco;
    string remetente;
    string senha;

    ifstream arquivo("config.txt");
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo 'config.txt'\n";
        return 1;
    }

    getline(arquivo, destinatario);
    getline(arquivo, endereco);
    getline(arquivo, remetente);
    getline(arquivo, senha);

    arquivo.close();

    cout << endl << "remetente: " << remetente << endl;
    cout << "destinatario: " << destinatario << endl << endl;

    if (argc >= 2) {
        symbol = argv[1];
        max = atof(argv[2]);
        min = atof(argv[3]);
        if(min > max){
            aux = min;
            min = max;
            max = aux;
    }
    } else {
        cout << "Digite o codigo da acao (ex: AAPL, PETR4.SA): ";
        getline(cin, symbol);
    }
    cout << "Max: " << max << endl;
    cout << "Min: " << min << endl << endl;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_URL, endereco.c_str());
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_USERNAME, remetente.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, senha.c_str());

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + remetente + ">").c_str());
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        
        while (true) {
        try {
            double price = get_price(symbol);
            //double price = get_price_rand(min, max);
            deriv = derivada(vetor, price);
            cout << horario_atual() << endl;
            cout << "Tendencia: " << deriv << endl;
            if(price >= max){
                struct curl_slist *recipients = NULL;
                recipients = curl_slist_append(recipients, ("<" + destinatario + ">").c_str());
                curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

                cout << symbol << ": R$ " << price << " (ALTO)" << endl;

                preco << fixed << setprecision(2) << price;
                dx << fixed << setprecision(2) << deriv;

                string payload_venda =
                    "To: " + destinatario + "\r\n"
                    "From: Lucas <" + remetente + ">\r\n"
                    "Subject: (" + horario_atual() + ") Preco de " + symbol + " alto!\r\n"
                    "\r\n"
                    "O preco de " + symbol + " esta em R$" + preco.str() + " com tendencia de " + dx.str() + ", recomendamos a venda.\r\n";

                preco.str("");
                preco.clear();
                dx.str("");
                dx.clear();
                
                const char *payload_ptr = payload_venda.c_str();
                curl_easy_setopt(curl, CURLOPT_READDATA, &payload_ptr);

                res = curl_easy_perform(curl);
                if (res != CURLE_OK){
                    cerr << "Erro ao enviar: " << curl_easy_strerror(res) << endl << endl;
                }
                else {
                    cout << "E-mail enviado com sucesso!" << endl << endl;
                }
                curl_slist_free_all(recipients);
            }
            else if(price <= min){
                struct curl_slist *recipients = NULL;
                recipients = curl_slist_append(recipients, ("<" + destinatario + ">").c_str());
                curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

                cout << symbol << ": R$ " << price << " (BAIXO)" << endl;

                preco << fixed << setprecision(2) << price;
                dx << fixed << setprecision(2) << deriv;

                string payload_compra = 
                    "To: " + destinatario + "\r\n"
                    "From: Lucas <" + remetente + ">\r\n"
                    "Subject: (" + horario_atual() + ") Preco de "+symbol+" baixo!\r\n"
                    "\r\n"
                    "O preco de " + symbol + " esta em R$" + preco.str() + " com tendencia de " + dx.str() + ", recomendamos a compra.\r\n";

                preco.str("");
                preco.clear();
                dx.str("");
                dx.clear();

                const char *payload_ptr = payload_compra.c_str();
                curl_easy_setopt(curl, CURLOPT_READDATA, &payload_ptr);

                res = curl_easy_perform(curl);
                if (res != CURLE_OK){
                    cerr << "Erro ao enviar: " << curl_easy_strerror(res) << endl << endl;
                }
                else {
                    cout << "E-mail enviado com sucesso!" << endl << endl;
                }
                curl_slist_free_all(recipients);
            }
            else{
                cout << symbol << ": R$ " << price << endl << endl;
            }
        } catch (const exception& e) {
            cerr << "Erro: " << e.what() << endl;
        }
        this_thread::sleep_for(chrono::seconds(60));
        }
        curl_easy_cleanup(curl);
    }
    return (int)res;
}
