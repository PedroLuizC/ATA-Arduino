/*
 * Caminho: C:\Work\Workspace\arduino\ATA\conexao.h
 */
#ifndef CONEXAO_H
#define CONEXAO_H

#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include "config.h"

bool conectarRede();
void desconectarRede();
bool enviarEmailReal(const char* assunto); 

#endif
