/*
 * Caminho: C:\Work\Workspace\arduino\ATA\banco_dados.h
 */
#ifndef BANCO_DADOS_H
#define BANCO_DADOS_H

#include <FS.h>
#include <LittleFS.h> 
#include "config.h"
#include "tipos.h"

void inicializarBancoDados();
Pessoa* buscarPessoaPorMatricula(const char* matricula);
bool verificarCredenciaisAdmin(const char* matricula, const char* senha);
const char* obterNomeCursoPorID(uint8_t id);

void iniciarNovaSessao(); 
bool registrarPresenca(const char* matricula);

bool gerarArquivoCSV(SessaoAta* sessao); 

#endif
