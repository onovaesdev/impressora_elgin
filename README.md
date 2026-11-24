# Controle da Impressora Elgin I9

## Sobre o projeto

Programa em C, para Windows, que controla a impressora térmica Elgin (modelos i8 e i9) por meio da DLL `E1_Impressora01.dll`, fornecida pela Elgin. O programa abre com um login e, depois, mostra um menu no terminal em que o usuário configura e abre a conexão com a impressora e escolhe o que imprimir: texto, QR Code, código de barras e XMLs de venda e de cancelamento do SAT. Também permite abrir a gaveta de dinheiro e emitir sinal sonoro.

## Ferramentas utilizadas

- Linguagem C
- VS Code
- GitHub
- GCC (MinGW)
- DLL E1_Impressora01.dll da Elgin

## Arquivos necessários

Estes arquivos precisam estar na raiz do projeto, na mesma pasta de onde o programa é executado:

- `E1_Impressora01.dll`
- `XMLSAT.xml` (usado na opção 7 do menu)
- `CANC_SAT.xml` (usado na opção 8 do menu)

## Qual DLL usar (32 ou 64 bits)

A DLL precisa ter a mesma arquitetura do compilador:

- Compilador de 32 bits (por exemplo, MinGW do Dev-C++ ou do Code::Blocks): usar a DLL da pasta `x86`.
- Compilador de 64 bits (por exemplo, MSYS2 mingw64): usar a DLL da pasta `x64`.

Se a arquitetura estiver errada, o programa não consegue carregar a DLL e se encerra com uma mensagem de erro. Para conferir qual é o seu compilador, rode `gcc -v` no terminal e veja a linha `Target`.

## Como compilar

No terminal, dentro da pasta do projeto:

```bash
gcc main.c -o impressora
```

## Como executar

1. Ligue a impressora Elgin e conecte ao computador.
2. Confirme que `E1_Impressora01.dll`, `XMLSAT.xml` e `CANC_SAT.xml` estão na mesma pasta do executável.
3. Execute:

```bash
impressora.exe
```

4. Faça o login (veja a seção abaixo).
5. No menu, use a opção 1 para configurar a conexão e a opção 2 para abri-la antes de imprimir. As demais opções só funcionam com a conexão aberta.

Os valores iniciais da conexão são USB, modelo i9 e parâmetro 0. Se for usar esses valores, pode pular a opção 1 e ir direto para a opção 2.

## Login de teste

- Usuário: `admin`
- Senha: `1234`

São 3 tentativas. Depois disso o programa é encerrado.

## Opções do menu

| Opção | Descrição |
|-------|-----------|
| 1 | Configurar Conexão (tipo, modelo, porta e parâmetro adicional) |
| 2 | Abrir Conexão |
| 3 | Fechar Conexão |
| 4 | Imprimir Texto |
| 5 | Imprimir QR Code |
| 6 | Imprimir Código de Barras |
| 7 | Imprimir XML SAT |
| 8 | Imprimir XML de Cancelamento SAT |
| 9 | Abrir Gaveta (Elgin) |
| 10 | Abrir Gaveta |
| 11 | Sinal Sonoro |
| 0 | Sair |

Ao sair pela opção 0, o programa fecha a conexão automaticamente se ela ainda estiver aberta.

## Autores

Em ordem alfabética:

- Antony Novaes Cruz
- Davi Santos dos Anjos
- Gustavo Henrique Almeida de Freitas
- Maria Vitória Barreto Queiroz
