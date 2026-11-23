#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*
 * Controle da impressora Elgin (i8 / i9) em C, usando a DLL E1_Impressora01.dll.
 *
 * A DLL precisa estar na RAIZ do projeto (na mesma pasta de onde o programa e
 * executado), junto com os arquivos XMLSAT.xml e CANC_SAT.xml.
 *
 * IMPORTANTE: a DLL precisa ter a mesma arquitetura do compilador:
 *   - compilador de 32 bits (ex.: MinGW do Dev-C++ / Code::Blocks) -> use a DLL da pasta x86
 *   - compilador de 64 bits (ex.: MSYS2 mingw64)                   -> use a DLL da pasta x64
 */

/* ===================== Ligacao com a DLL =====================
 * Cada typedef descreve a "forma" de um comando da DLL:
 * o que devolve e quais parametros recebe.
 */
typedef int (*FnAbreConexaoImpressora)(int tipo, const char *modelo, const char *conexao, int param);
typedef int (*FnFechaConexaoImpressora)(void);
typedef int (*FnImpressaoTexto)(const char *dados, int posicao, int estilo, int tamanho);
typedef int (*FnCorte)(int avanco);
typedef int (*FnImpressaoQRCode)(const char *dados, int tamanho, int nivelCorrecao);
typedef int (*FnImpressaoCodigoBarras)(int tipo, const char *dados, int altura, int largura, int HRI);
typedef int (*FnAvancaPapel)(int linhas);
typedef int (*FnStatusImpressora)(int param);
typedef int (*FnAbreGavetaElgin)(int pino, int ti, int tf);
typedef int (*FnAbreGaveta)(int pino, int ti, int tf);
typedef int (*FnSinalSonoro)(int qtd, int tempoInicio, int tempoFim);
typedef int (*FnModoPagina)(void);
typedef int (*FnLimpaBufferModoPagina)(void);
typedef int (*FnImprimeModoPagina)(void);
typedef int (*FnModoPadrao)(void);
typedef int (*FnPosicaoImpressaoHorizontal)(int posicao);
typedef int (*FnPosicaoImpressaoVertical)(int posicao);
typedef int (*FnImprimeXMLSAT)(const char *dados, int param);
typedef int (*FnImprimeXMLCancelamentoSAT)(const char *dados, const char *assQRCode, int param);

/* Um ponteiro para cada comando.
 * Depois de carregarDLL(), e so chamar: AbreConexaoImpressora(...)
 */
static FnAbreConexaoImpressora       AbreConexaoImpressora;
static FnFechaConexaoImpressora      FechaConexaoImpressora;
static FnImpressaoTexto              ImpressaoTexto;
static FnCorte                       Corte;
static FnImpressaoQRCode             ImpressaoQRCode;
static FnImpressaoCodigoBarras       ImpressaoCodigoBarras;
static FnAvancaPapel                 AvancaPapel;
static FnStatusImpressora            StatusImpressora;
static FnAbreGavetaElgin             AbreGavetaElgin;
static FnAbreGaveta                  AbreGaveta;
static FnSinalSonoro                 SinalSonoro;
static FnModoPagina                  ModoPagina;
static FnLimpaBufferModoPagina       LimpaBufferModoPagina;
static FnImprimeModoPagina           ImprimeModoPagina;
static FnModoPadrao                  ModoPadrao;
static FnPosicaoImpressaoHorizontal  PosicaoImpressaoHorizontal;
static FnPosicaoImpressaoVertical    PosicaoImpressaoVertical;
static FnImprimeXMLSAT               ImprimeXMLSAT;
static FnImprimeXMLCancelamentoSAT   ImprimeXMLCancelamentoSAT;

static HMODULE dll = NULL;

/* Procura um comando dentro da DLL. Devolve 1 se achou, 0 se nao achou. */
static int carregaFuncao(void *destino, const char *nome)
{
    FARPROC f = GetProcAddress(dll, nome);

    if (f == NULL) {
        printf("Comando '%s' nao encontrado na DLL.\n", nome);
        return 0;
    }

    *(FARPROC *)destino = f;
    return 1;
}

/* Carrega a DLL da raiz do projeto e todos os comandos. Devolve 1 se deu certo. */
static int carregarDLL(void)
{
    int ok = 1;

    dll = LoadLibraryA("E1_Impressora01.dll");
    if (dll == NULL) {
        printf("Nao foi possivel carregar E1_Impressora01.dll (erro %lu).\n", GetLastError());
        printf("Confira: a DLL esta na raiz do projeto? Ela tem a mesma arquitetura (32/64 bits) do compilador?\n");
        return 0;
    }

    ok &= carregaFuncao(&AbreConexaoImpressora,      "AbreConexaoImpressora");
    ok &= carregaFuncao(&FechaConexaoImpressora,     "FechaConexaoImpressora");
    ok &= carregaFuncao(&ImpressaoTexto,             "ImpressaoTexto");
    ok &= carregaFuncao(&Corte,                      "Corte");
    ok &= carregaFuncao(&ImpressaoQRCode,            "ImpressaoQRCode");
    ok &= carregaFuncao(&ImpressaoCodigoBarras,      "ImpressaoCodigoBarras");
    ok &= carregaFuncao(&AvancaPapel,                "AvancaPapel");
    ok &= carregaFuncao(&StatusImpressora,           "StatusImpressora");
    ok &= carregaFuncao(&AbreGavetaElgin,            "AbreGavetaElgin");
    ok &= carregaFuncao(&AbreGaveta,                 "AbreGaveta");
    ok &= carregaFuncao(&SinalSonoro,                "SinalSonoro");
    ok &= carregaFuncao(&ModoPagina,                 "ModoPagina");
    ok &= carregaFuncao(&LimpaBufferModoPagina,      "LimpaBufferModoPagina");
    ok &= carregaFuncao(&ImprimeModoPagina,          "ImprimeModoPagina");
    ok &= carregaFuncao(&ModoPadrao,                 "ModoPadrao");
    ok &= carregaFuncao(&PosicaoImpressaoHorizontal, "PosicaoImpressaoHorizontal");
    ok &= carregaFuncao(&PosicaoImpressaoVertical,   "PosicaoImpressaoVertical");
    ok &= carregaFuncao(&ImprimeXMLSAT,              "ImprimeXMLSAT");
    ok &= carregaFuncao(&ImprimeXMLCancelamentoSAT,  "ImprimeXMLCancelamentoSAT");

    return ok;
}

/* ===================== Variaveis do programa ===================== */

static int  conexaoAberta = 0;      /* 0 = fechada, 1 = aberta */
static int  tipo          = 1;      /* 1 = USB, 2 = Serial, 3 = Rede... */
static char modelo[50]    = "i9";   /* "i8" ou "i9" */
static char conexao[50]   = "USB";
static int  parametro     = 0;

/* Chave do QR Code usada no cancelamento SAT */
static const char *ASS_QRCODE =
    "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/"
    "q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+"
    "TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+"
    "ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";

/* ===================== Funcoes auxiliares ===================== */

/* Mostra a mensagem e le uma linha de texto digitada (sem o ENTER no final) */
static void capturarEntrada(const char *mensagem, char *destino, int tamanho)
{
    printf("%s", mensagem);

    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    destino[strcspn(destino, "\r\n")] = '\0';
}

/* Mostra a mensagem e le um numero inteiro. Se digitar letras, pergunta de novo. */
static int lerInteiro(const char *mensagem)
{
    char linha[50];
    char *fim;
    long valor;

    while (1) {
        capturarEntrada(mensagem, linha, sizeof(linha));
        valor = strtol(linha, &fim, 10);

        if (linha[0] != '\0' && *fim == '\0') {
            return (int)valor;
        }

        printf("Digite apenas numeros!\n");
    }
}

/* Le um arquivo inteiro para a memoria. Devolve NULL se nao achar.
 * Quem chama deve dar free().
 */
static char *lerArquivo(const char *caminho)
{
    FILE *arq;
    long tamanho;
    char *conteudo;
    size_t lidos;

    arq = fopen(caminho, "rb");
    if (arq == NULL) {
        return NULL;
    }

    fseek(arq, 0, SEEK_END);
    tamanho = ftell(arq);
    rewind(arq);

    conteudo = (char *)malloc(tamanho + 1);
    if (conteudo == NULL) {
        fclose(arq);
        return NULL;
    }

    lidos = fread(conteudo, 1, tamanho, arq);
    conteudo[lidos] = '\0';
    fclose(arq);

    return conteudo;
}

/* Aviso padrao usado por todas as funcoes que precisam da impressora.
 * Devolve 1 se a conexao esta aberta e 0 se nao esta (ja mostrando o aviso).
 */
static int conexaoOk(void)
{
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return 0;
    }
    return 1;
}

/* ===================== Login ===================== */

/* Pede usuario e senha (3 tentativas).
 * Devolve 1 se acertou e 0 se errou as 3 vezes.
 */
int fazerLogin(void)
{
    char usuario[50];
    char senha[50];
    int tentativa;

    for (tentativa = 1; tentativa <= 3; tentativa++) {
        capturarEntrada("Usuario: ", usuario, sizeof(usuario));
        capturarEntrada("Senha: ", senha, sizeof(senha));

        if (strcmp(usuario, "admin") == 0 && strcmp(senha, "1234") == 0) {
            printf("Login realizado com sucesso.\n");
            return 1;
        }

        if (tentativa < 3) {
            printf("Usuario ou senha incorretos. Tentativas restantes: %d\n\n", 3 - tentativa);
        }
    }

    printf("Numero maximo de tentativas atingido.\n");
    return 0;
}

/* ===================== Funcoes da impressora ===================== */

void configurarConexao(void)
{
    if (!conexaoAberta) {
        tipo = lerInteiro("Digite o tipo de conexao (1 para USB, 2 para Serial, etc.): ");
        capturarEntrada("Digite o modelo da impressora: ", modelo, sizeof(modelo));
        capturarEntrada("Digite a porta de conexao (ex: USB): ", conexao, sizeof(conexao));
        parametro = lerInteiro("Digite o parametro adicional (ex: 0 para padrao): ");
        printf("Parametros de conexao configurados com sucesso.\n");
    } else {
        printf("Conexao ja configurada. Pronta para uso.\n");
    }
}

void abrirConexao(void)
{
    int retorno;

    if (!conexaoAberta) {
        retorno = AbreConexaoImpressora(tipo, modelo, conexao, parametro);

        if (retorno == 0) {
            conexaoAberta = 1;
            printf("Conexao aberta com sucesso.\n");
        } else {
            printf("Erro ao abrir conexao. Codigo de erro: %d\n", retorno);
        }
    } else {
        printf("Conexao ja esta aberta.\n");
    }
}

void fecharConexao(void)
{
    int retorno;

    if (!conexaoAberta) {
        printf("Conexao ja esta fechada.\n");
        return;
    }

    retorno = FechaConexaoImpressora();

    if (retorno == 0) {
        conexaoAberta = 0;
        printf("Conexao fechada com sucesso.\n");
    } else {
        printf("Erro ao fechar conexao. Codigo de erro: %d\n", retorno);
    }
    
}

void imprimirTexto(void)  {

    char texto[400];
    int retorno;

    if (!conexaoOk()) {
        return;
    }

    capturarEntrada("Texto a imprimir: ", texto, sizeof(texto));

    /* posicao 1 = centro, estilo 4, tamanho 0 = normal */
    retorno = ImpressaoTexto(texto, 1, 4, 0);

    if (retorno >= 0) {
        printf("Texto impresso!\n");
    } else {
        printf("Erro ao imprimir. Codigo: %d\n", retorno);
        return;
    }

    Corte(5);
}


void imprimirQRCode(void)
{
    char dados[200];
    int retorno;

    if (!conexaoOk()) {
        return;
    }

    capturarEntrada("Conteudo do QR Code: ", dados, sizeof(dados));
    retorno = ImpressaoQRCode(dados, 6, 4);

    if (retorno == 0) {
        printf("QR Code impresso!\n");
    } else {
        printf("Erro ao imprimir QR Code. Codigo: %d\n", retorno);
    }

    Corte(5);
}

void imprimirCodigoBarras(void)
{
   int retorno;

    if (!conexaoOk()) {
        return;
    }

    /* tipo 8 = CODE 128 (o texto comeca com {A), altura 100, largura 2,
     * HRI 3 = numero acima e abaixo
     */
    retorno = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);

    if (retorno == 0) {
        printf("Codigo de barras impresso!\n");
    } else {
        printf("Erro ao imprimir codigo de barras. Codigo: %d\n", retorno);
    }

    Corte(5);
}

void imprimirXMLSAT(void) {

    char *dadosXML;
    int retorno;

    if (!conexaoOk()) {
        return;
    }

    /* le o arquivo XML (na raiz do projeto) para a memoria */
    dadosXML = lerArquivo("XMLSAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler o XMLSAT.xml\n");
        return;
    }

    retorno = ImprimeXMLSAT(dadosXML, 0);
    Corte(5);
    free(dadosXML); /* devolve a memoria */

    if (retorno == 0) {
        printf("XML SAT impresso!\n");
    } else {
        printf("Erro ao imprimir XML SAT. Codigo: %d\n", retorno);
    }
    
}

void imprimirXMLCancelamentoSAT(void)
{
    /* le o arquivo XML (na raiz do projeto) para a memoria */
    
}

void abrirGavetaElgin(void)
{
    
}

void abrirGaveta(void)
{
    
}

void emitirSinalSonoro(void)
{
    
}

void exibirMenu(void)
{
    printf("\n*************************************************\n");
    printf("**************** MENU IMPRESSORA ****************\n");
    printf("*************************************************\n\n");

    printf("1  - Configurar Conexao\n");
    printf("--------------------------------------\n");
}

/* ===================== Programa principal ===================== */

int main(void) {
    int opcao = -1;

    if (!carregarDLL()) {
        printf("Programa encerrado.\n");
        return 1;
    }

    while (opcao != 0) {
        exibirMenu();
        opcao = lerInteiro("\nDigite a opcao desejada: ");


    FreeLibrary(dll);
    return 0;
}
}