/*
 * Caminho: C:\Work\Workspace\arduino\ATA\config.h
 * Descricao: Configuracoes globais de Hardware, Rede, Memoria e Credenciais.
 * ATENCAO: Preencha os campos de WiFi e Email antes de compilar.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- Hardware LCD I2C ---
#define ENDERECO_LCD 0x27 // Se a tela nao ligar, tente 0x3F
#define COLUNAS_LCD 16
#define LINHAS_LCD 2

// --- Pinos I2C (ESP32 Padrao) ---
#define PIN_SDA 21
#define PIN_SCL 22

// --- Gerenciamento de Memoria (Tamanhos Maximos) ---
#define TAM_MATRICULA 12
#define TAM_SENHA 12
#define TAM_NOME 21       // Tamanho para caber no LCD e CSV
#define TAM_RELATORIO 2048 // Buffer para formatacao de texto

// --- Configuracoes de Rede WiFi ---
#define WIFI_SSID "INSIRA_NOME_WIFI_AQUI"
#define WIFI_SENHA "INSIRA_SENHA_WIFI_AQUI"

// --- Gestao de Energia ---
#define TEMPO_INATIVIDADE_SUSPENDER 30000 

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "seu_email@gmail.com"
#define AUTHOR_PASSWORD "sua_senha_de_aplicativo_aqui" 
#define RECIPIENT_EMAIL "email_coordenacao@exemplo.com"

// --- Configuracoes do Teclado Matricial ---
const byte LINHAS_TECLADO = 4;
const byte COLUNAS_TECLADO = 4;

// Mapeamento de Pinos (Da esquerda para direita no conector)
// Linhas: 1, 2, 3, 4 -> GPIOs do ESP32
const byte PINOS_LINHA[LINHAS_TECLADO] = {13, 12, 14, 27};
// Colunas: 1, 2, 3, 4 -> GPIOs do ESP32
const byte PINOS_COLUNA[COLUNAS_TECLADO] = {26, 25, 33, 32};

// Mapa de Caracteres
const char TECLAS[LINHAS_TECLADO][COLUNAS_TECLADO] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

#endif
