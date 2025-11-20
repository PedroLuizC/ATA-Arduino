/*
 * Caminho: C:\Work\Workspace\arduino\ATA\tipos.h
 * Descricao: CORRIGIDO - Inclusao do config.h adicionada.
 */

#ifndef TIPOS_H
#define TIPOS_H

#include <Arduino.h>
#include "config.h" // <--- ESSA LINHA FALTAVA E CAUSOU O ERRO

enum EstadoSistema {
    ESTADO_INICIAL,
    ESTADO_LOGIN,
    ESTADO_VALIDACAO,
    ESTADO_MENU_PRINCIPAL,
    ESTADO_CONFIG_DATA,
    ESTADO_CONFIG_CURSO,
    ESTADO_CONFIG_PERIODO,
    ESTADO_ATA_ATIVA,
    ESTADO_GERANDO_RELATORIO,
    ESTADO_MENSAGEM_TEMPORARIA
};

struct Pessoa {
    char nome[TAM_NOME];
    char matricula[TAM_MATRICULA];
    char senha[TAM_SENHA]; 
    bool ehAdmin;
    bool presenteNaSessao;
};

struct SessaoAta {
    char data[9]; 
    uint8_t idCurso;
    uint8_t periodo;
    char nomeProfessor[TAM_NOME];
    bool ativa;
};

#endif
