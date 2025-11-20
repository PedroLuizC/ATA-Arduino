/*
 * Caminho: C:\Work\Workspace\arduino\ATA\banco_dados.cpp
 * Descricao: Gera arquivo CSV real na memoria Flash do ESP32.
 */

#include "banco_dados.h"
#include <string.h>
#include <Arduino.h>

#define QTD_PESSOAS 6

const char CURSO_DESCONHECIDO[] = "Indefinido";
const char CURSO_COMPUTACAO[] = "Eng. Computacao";
const char CURSO_ADS[] = "Analise Desenv.";

Pessoa pessoas[QTD_PESSOAS] = {
    {"Peter Peterson", "01401401", "1111", true, false},
    {"Emanuela Santos", "14523777", "", false, false},
    {"Silvano Souza", "78994561", "", false, false},
    {"Gustavo Filho", "00000000", "", false, false},
    {"Sivester Jr", "11111111", "", false, false},
    {"Bacon Vida", "22222222", "", false, false}
};

void inicializarBancoDados() { iniciarNovaSessao(); }

void iniciarNovaSessao() {
    for(int i = 0; i < QTD_PESSOAS; i++) pessoas[i].presenteNaSessao = false;
}

Pessoa* buscarPessoaPorMatricula(const char* matricula) {
    for(int i = 0; i < QTD_PESSOAS; i++) {
        if (strcmp(pessoas[i].matricula, matricula) == 0) return &pessoas[i];
    }
    return nullptr;
}

bool registrarPresenca(const char* matricula) {
    Pessoa* p = buscarPessoaPorMatricula(matricula);
    if (p != nullptr) {
        p->presenteNaSessao = true;
        return true;
    }
    return false;
}

bool verificarCredenciaisAdmin(const char* matricula, const char* senha) {
    Pessoa* p = buscarPessoaPorMatricula(matricula);
    if (p != nullptr && p->ehAdmin) {
        if (strcmp(p->senha, senha) == 0) return true;
    }
    return false;
}

const char* obterNomeCursoPorID(uint8_t id) {
    switch(id) {
        case 12: return CURSO_COMPUTACAO;
        case 22: return CURSO_ADS;
        default: return CURSO_DESCONHECIDO;
    }
}

bool gerarArquivoCSV(SessaoAta* sessao) {
    // Abre o arquivo para escrita (w = write, sobrescreve se existir)
    File arquivo = LittleFS.open("/ata.csv", "w");
    
    if (!arquivo) {
        Serial.println("Erro ao abrir arquivo para escrita!");
        return false;
    }

    arquivo.println("RELATORIO DE PRESENCA;;;");
    arquivo.printf("Data;%s;;\n", sessao->data);
    arquivo.printf("Professor;%s;;\n", sessao->nomeProfessor);
    arquivo.printf("Curso;%s;;\n", obterNomeCursoPorID(sessao->idCurso));
    arquivo.printf("Periodo;%d;;\n", sessao->periodo);
    arquivo.println(";;;"); // Linha em branco

    arquivo.println("NOME;MATRICULA;STATUS");

    for(int i = 0; i < QTD_PESSOAS; i++) {
        if (!pessoas[i].ehAdmin) {
            arquivo.printf("%s;%s;%s\n", 
                pessoas[i].nome, 
                pessoas[i].matricula, 
                pessoas[i].presenteNaSessao ? "PRESENTE" : "AUSENTE"
            );
        }
    }

    arquivo.close();
    Serial.println("Arquivo /ata.csv gerado com sucesso!");
    return true;
}
