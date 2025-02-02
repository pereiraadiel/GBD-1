// desenvolvido e testado em ambiente Linux

/* 
 * File:   main.cpp
 * Author: seunome
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class MeuArquivo {
public:
                                            // offset do primeiro disponivel
    struct cabecalho { int quantidade; int disponivel; } cabecalho;
    struct registro { int quantidade; int disponivel; } registro;

    // construtor: abre arquivo. Essa aplicacao deveria ler o arquivo se existente ou criar um novo.
    // Entretando recriaremos o arquivo a cada execucao ("w+").
    MeuArquivo() {
        fd = fopen("dados.dat","w+");

        cabecalho.disponivel = 0;
        cabecalho.quantidade = 0;
        fwrite(&cabecalho, sizeof(struct cabecalho), 1, fd);
    }

    // Destrutor: fecha arquivo
    ~MeuArquivo() {
        fclose(fd);
    }

    void atualizarCabecalho(int quantidade, int offset) {
        fseek(this->fd, 0, SEEK_SET);

        cabecalho.quantidade++;
        cabecalho.disponivel = offset;
        fwrite(&cabecalho, sizeof(struct cabecalho), 1, this->fd);
    }

    // Insere uma nova palavra, consulta se há espaco disponível ou se deve inserir no final
    void inserePalavra(char *palavra) {
        this->substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0

        // implementar aqui

        // ler o cabecalho
        fseek(this->fd, 0, SEEK_SET);
        fread(&cabecalho, sizeof(struct cabecalho), 1, this->fd);

        // ir para a posicao descrita no cabecalho
        fseek(this->fd, cabecalho.disponivel + sizeof(struct cabecalho), SEEK_SET);

        int tamanho = strlen(palavra);

        // registro novo da palavra
        registro.quantidade = tamanho;
        registro.disponivel = 0;
        fwrite(&registro, sizeof(struct registro), 1, this->fd);
        fwrite(palavra, sizeof(char), tamanho + 1, this->fd);
        int offsetAtual = ftell(this->fd) - tamanho - sizeof(struct registro);
        // printf("offsetAtual = %d\n", offsetAtual);

        // atualizar cabecalho
        int quantidadeNova = cabecalho.quantidade + 1;
        atualizarCabecalho(quantidadeNova, offsetAtual);
    }

    // Marca registro como removido, atualiza lista de disponíveis, incluindo o cabecalho
    void removePalavra(int offset) {
        // implementar aqui
    }

    // BuscaPalavra: retorno é o offset para o registro
    // Nao deve considerar registro removido
    int buscaPalavra(char *palavra) {
        this->substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0

        // implementar aqui

        // ler o cabecalho
        fseek(this->fd, 0, SEEK_SET);
        fread(&cabecalho, sizeof(struct cabecalho), 1, this->fd);

        // obter a quantidade de registros salvos
        int quantidade = cabecalho.quantidade;
        int tamanho = strlen(palavra);
        
        // posicionar offset do primeiro registro
        fseek(this->fd, cabecalho.disponivel , SEEK_SET);

        printf("quantidade: %d\n", quantidade);
        printf("disponivel: %d\n", cabecalho.disponivel);
        for( int i=0; i<quantidade; i++ ) {

            fread(&registro, sizeof(struct registro), 1 , this->fd);
            char * word = (char *) malloc(sizeof(char) * registro.quantidade);
            fread(&word, registro.quantidade, 1 , this->fd);

            substituiBarraNporBarraZero(word);
            // printf("word: \"%s\"\n", word);

            // verificar se o registro esta ocupado e tem o msm tamanho da palavra
            if(registro.disponivel == 0 && registro.quantidade == tamanho) {
                if(strcmp(word, palavra) == 0) return ftell(this->fd);
            }

        }

        // retornar -1 caso nao encontrar
        return -1;
    }

private:
    // descritor do arquivo é privado, apenas métodos da classe podem acessa-lo
    FILE *fd;

    // funcao auxiliar substitui terminador por \0
    void substituiBarraNporBarraZero(char *str) {
        int tam = strlen(str); for (int i = 0; i < tam; i++) if (str[i] == '\n') str[i] = '\0';
    }
};

int main(int argc, char** argv) {
    // abrindo arquivo dicionario.txt
    FILE *f = fopen("dicionario.txt","rt");

    // se não abriu
    if (f == NULL) {
        printf("Erro ao abrir arquivo.\n\n");
        return 0;
    }

    char *palavra = new char[50];

    // criando arquivo de dados
    MeuArquivo *arquivo = new MeuArquivo();
    while (!feof(f)) {
        fgets(palavra,50,f);
        arquivo->inserePalavra(palavra);
    }

    // fechar arquivo dicionario.txt
    fclose(f);

    printf("Arquivo criado.\n\n");

    char opcao;
    do {
        printf("\n\n1-Insere\n2-Remove\n3-Busca\n4-Sair\nOpcao:");
        opcao = getchar();
        if (opcao == '1') {
            printf("Palavra: ");
            scanf("%s",palavra);
            arquivo->inserePalavra(palavra);
        }
        else if (opcao == '2') {
            printf("Palavra: ");
            scanf("%s",palavra);
            int offset = arquivo->buscaPalavra(palavra);
            if (offset >= 0) {
                arquivo->removePalavra(offset);
                printf("Removido.\n\n");
            }
        }
        else if (opcao == '3') {
            printf("Palavra: ");
            scanf("%s",palavra);
            int offset = arquivo->buscaPalavra(palavra);
            if (offset >= 0)
                printf("Encontrou %s na posição %d\n\n",palavra,offset);
            else
                printf("Não encontrou %s\n\n",palavra);
        }
        if (opcao != '4') opcao = getchar();
    } while (opcao != '4');

    printf("\n\nAte mais!\n\n");

    return (EXIT_SUCCESS);
}
