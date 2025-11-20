/*
 * Caminho: C:\Work\Workspace\arduino\ATA\ATA.ino
 * Descricao: Versao FINAL com LittleFS e CSV.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <LittleFS.h> // <--- IMPORTANTE
#include "config.h"
#include "tipos.h"
#include "banco_dados.h"
#include "conexao.h"

LiquidCrystal_I2C lcd(ENDERECO_LCD, COLUNAS_LCD, LINHAS_LCD);
Keypad teclado = Keypad(makeKeymap(TECLAS), (byte*)PINOS_LINHA, (byte*)PINOS_COLUNA, LINHAS_TECLADO, COLUNAS_TECLADO);

EstadoSistema estadoAtual = ESTADO_INICIAL;
EstadoSistema proximoEstado = ESTADO_INICIAL;
SessaoAta sessaoAtual;

char bufferEntrada[TAM_MATRICULA];
char bufferSenha[TAM_SENHA];
// bufferRelatorio nao e mais necessario globalmente pois usamos Arquivo, 
// mas mantemos caso queira debug no serial
char bufferRelatorio[TAM_RELATORIO]; 

byte indiceBuffer = 0;
bool modoSenha = false;

unsigned long tempoUltimaAcao = 0;
bool modoSuspenso = false;
const unsigned long TIMEOUT_MENSAGEM = 2000;

void executarDiagnosticoHardware() {
    Serial.println("\n--- HARDWARE CHECK ---");
    Wire.begin(PIN_SDA, PIN_SCL); 
    for(byte end = 1; end < 127; end++) {
        Wire.beginTransmission(end);
        if (Wire.endTransmission() == 0) Serial.printf("I2C OK: 0x%02X\n", end);
    }
}

// ... (Mantenha atualizarTela, acordarSistema, entrarModoSuspenso iguais) ...
void atualizarTela(const char* linha1, const char* linha2) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print(linha1); lcd.setCursor(0, 1); lcd.print(linha2);
}
void acordarSistema() {
    if (modoSuspenso) { modoSuspenso = false; lcd.backlight(); lcd.display(); transicaoEstado(estadoAtual); }
    tempoUltimaAcao = millis();
}
void entrarModoSuspenso() {
    if (!modoSuspenso) { modoSuspenso = true; lcd.noBacklight(); lcd.noDisplay(); desconectarRede(); }
}

void limparBuffers() {
    memset(bufferEntrada, 0, TAM_MATRICULA);
    memset(bufferSenha, 0, TAM_SENHA);
    indiceBuffer = 0;
    modoSenha = false;
}

void transicaoEstado(EstadoSistema novo) {
    estadoAtual = novo;
    if (!modoSuspenso) {
        if (novo != ESTADO_LOGIN && novo != ESTADO_VALIDACAO) limparBuffers();
        
        switch(estadoAtual) {
            case ESTADO_INICIAL: limparBuffers(); atualizarTela("ATA ELETRONICA", "Aperte A"); break;
            case ESTADO_LOGIN:
                if (!modoSenha) {
                    if (strlen(bufferEntrada) == 0) limparBuffers(); 
                    atualizarTela("Matricula:", bufferEntrada); lcd.blink();
                } else { atualizarTela("Senha:", ""); lcd.blink(); }
                break;
            case ESTADO_MENU_PRINCIPAL: lcd.noBlink(); atualizarTela("1.Enviar Email", "2.Ativar ATA"); break;
            case ESTADO_CONFIG_DATA: iniciarNovaSessao(); lcd.blink(); atualizarTela("Data (DDMMAAAA):", ""); break;
            case ESTADO_CONFIG_CURSO: atualizarTela("ID do Curso:", ""); break;
            case ESTADO_CONFIG_PERIODO: atualizarTela("Periodo (0-10):", ""); break;
            
            case ESTADO_ATA_ATIVA:
                lcd.blink(); 
                atualizarTela(obterNomeCursoPorID(sessaoAtual.idCurso), "Matr: ");
                break;
            case ESTADO_GERANDO_RELATORIO:
                lcd.noBlink();
                atualizarTela("Gerando CSV...", "Aguarde");
                
                if (gerarArquivoCSV(&sessaoAtual)) {
                    
                    atualizarTela("Conectando...", "WiFi");
                    if (conectarRede()) {
                        atualizarTela("Enviando Email", "Com Anexo...");
                        
                        char assunto[50];
                        sprintf(assunto, "ATA Presenca - %s", sessaoAtual.data);
                        
                        // 2. Envia com o arquivo anexado
                        if (enviarEmailReal(assunto)) {
                            atualizarTela("Sucesso!", "Email Enviado");
                        } else {
                            atualizarTela("Erro no Envio", "Ver Serial");
                        }
                        desconectarRede();
                    } else {
                        atualizarTela("Erro WiFi", "Sem Conexao");
                    }
                } else {
                    atualizarTela("Erro Disco", "Falha CSV");
                }
                
                delay(2500);
                transicaoEstado(ESTADO_MENU_PRINCIPAL);
                break;
            // --------------------------------

            case ESTADO_MENSAGEM_TEMPORARIA: lcd.noBlink(); tempoUltimaAcao = millis(); break;
            default: break;
        }
    }
}

void acaoBackspace(char* buffer, byte tamanhoMax, byte offsetVisual = 0) {
    if (indiceBuffer > 0) {
        indiceBuffer--;
        buffer[indiceBuffer] = '\0';
        lcd.setCursor(offsetVisual + indiceBuffer, 1); lcd.print(" "); lcd.setCursor(offsetVisual + indiceBuffer, 1);
    }
}

void processarInput(char tecla) {
    if (modoSuspenso) { acordarSistema(); return; }
    tempoUltimaAcao = millis();
    if (tecla == 'D' && estadoAtual != ESTADO_LOGIN) {
        if (estadoAtual == ESTADO_ATA_ATIVA) transicaoEstado(ESTADO_MENU_PRINCIPAL); 
        else transicaoEstado(ESTADO_INICIAL);
        return;
    }
    switch(estadoAtual) {
        case ESTADO_INICIAL: if (tecla == 'A') transicaoEstado(ESTADO_LOGIN); break;
        case ESTADO_LOGIN: {
            char* bufferAlvo = modoSenha ? bufferSenha : bufferEntrada;
            byte tamMax = modoSenha ? TAM_SENHA : TAM_MATRICULA;
            if (tecla == '*') { 
                if (!modoSenha) { if (strlen(bufferEntrada) > 0) { memset(bufferSenha, 0, TAM_SENHA); modoSenha = true; indiceBuffer = 0; transicaoEstado(ESTADO_LOGIN); } } 
                else { transicaoEstado(ESTADO_VALIDACAO); }
            } 
            else if (tecla == '#') { acaoBackspace(bufferAlvo, tamMax); }
            else if (tecla == 'D') { memset(bufferAlvo, 0, tamMax); indiceBuffer = 0; lcd.setCursor(0, 1); lcd.print("                "); lcd.setCursor(0, 1); }
            else if (indiceBuffer < tamMax - 1) { bufferAlvo[indiceBuffer++] = tecla; bufferAlvo[indiceBuffer] = '\0'; lcd.print(modoSenha ? '*' : tecla); }
            break;
        }
        case ESTADO_MENU_PRINCIPAL:
            if (tecla == '1') { if (strlen(sessaoAtual.data) > 0) transicaoEstado(ESTADO_GERANDO_RELATORIO); else { atualizarTela("Erro:", "Configure antes"); proximoEstado = ESTADO_MENU_PRINCIPAL; transicaoEstado(ESTADO_MENSAGEM_TEMPORARIA); } }
            if (tecla == '2') transicaoEstado(ESTADO_CONFIG_DATA);
            break;
        case ESTADO_CONFIG_DATA:
            if (tecla == '*') { if (strlen(bufferEntrada) == 8) { strcpy(sessaoAtual.data, bufferEntrada); limparBuffers(); transicaoEstado(ESTADO_CONFIG_CURSO); } } 
            else if (tecla == '#') { acaoBackspace(bufferEntrada, 8); } else if (indiceBuffer < 8) { bufferEntrada[indiceBuffer++] = tecla; bufferEntrada[indiceBuffer] = '\0'; lcd.print(tecla); }
            break;
        case ESTADO_CONFIG_CURSO:
            if (tecla == '*') { if (strlen(bufferEntrada) > 0) { sessaoAtual.idCurso = atoi(bufferEntrada); lcd.clear(); lcd.print(obterNomeCursoPorID(sessaoAtual.idCurso)); delay(1000); limparBuffers(); transicaoEstado(ESTADO_CONFIG_PERIODO); } } 
            else if (tecla == '#') { acaoBackspace(bufferEntrada, 3); } else if (indiceBuffer < 3) { bufferEntrada[indiceBuffer++] = tecla; bufferEntrada[indiceBuffer] = '\0'; lcd.print(tecla); }
            break;
        case ESTADO_CONFIG_PERIODO:
            if (tecla == '*') { if (strlen(bufferEntrada) > 0) { sessaoAtual.periodo = atoi(bufferEntrada); limparBuffers(); transicaoEstado(ESTADO_ATA_ATIVA); } } 
            else if (tecla == '#') { acaoBackspace(bufferEntrada, 2); } else if (indiceBuffer < 2) { bufferEntrada[indiceBuffer++] = tecla; bufferEntrada[indiceBuffer] = '\0'; lcd.print(tecla); }
            break;
        case ESTADO_ATA_ATIVA:
             if (tecla == '*') {
                Pessoa* aluno = buscarPessoaPorMatricula(bufferEntrada);
                if (aluno) { registrarPresenca(bufferEntrada); atualizarTela("Presenca OK:", aluno->nome); } else { atualizarTela("Erro", "Nao Encontrado"); }
                proximoEstado = ESTADO_ATA_ATIVA; transicaoEstado(ESTADO_MENSAGEM_TEMPORARIA);
             } 
             else if (tecla == '#') { acaoBackspace(bufferEntrada, TAM_MATRICULA, 6); } 
             else if (indiceBuffer < TAM_MATRICULA - 1) { bufferEntrada[indiceBuffer++] = tecla; bufferEntrada[indiceBuffer] = '\0'; lcd.setCursor(6 + (indiceBuffer - 1), 1); lcd.print(tecla); }
            break;
        default: break;
    }
}

void setup() {
    Serial.begin(115200);
    executarDiagnosticoHardware();
    
    // --- INICIALIZAR LITTLEFS ---
    if(!LittleFS.begin(true)){ // true = formata se falhar
        Serial.println("FALHA AO MONTAR LITTLEFS");
        return;
    }
    Serial.println("LittleFS montado com sucesso");
    // ---------------------------

    Wire.begin(PIN_SDA, PIN_SCL);
    lcd.init();
    lcd.backlight();
    WiFi.mode(WIFI_OFF);
    inicializarBancoDados();
    transicaoEstado(ESTADO_INICIAL);
    tempoUltimaAcao = millis();
}

void loop() {
    if (!modoSuspenso && millis() - tempoUltimaAcao > TEMPO_INATIVIDADE_SUSPENDER) {
        if (estadoAtual != ESTADO_ATA_ATIVA) entrarModoSuspenso(); else lcd.noBacklight();
    }
    char tecla = teclado.getKey();
    if (tecla) {
        if (estadoAtual == ESTADO_ATA_ATIVA) lcd.backlight();
        processarInput(tecla);
    }
    if (estadoAtual == ESTADO_VALIDACAO) {
        if (verificarCredenciaisAdmin(bufferEntrada, bufferSenha)) {
            Pessoa* p = buscarPessoaPorMatricula(bufferEntrada);
            if (p) strcpy(sessaoAtual.nomeProfessor, p->nome);
            transicaoEstado(ESTADO_MENU_PRINCIPAL);
        } else {
            atualizarTela("Acesso", "Negado");
            delay(1500);
            limparBuffers();
            transicaoEstado(ESTADO_INICIAL);
        }
    }
    if (estadoAtual == ESTADO_MENSAGEM_TEMPORARIA) {
        if (millis() - tempoUltimaAcao > TIMEOUT_MENSAGEM) transicaoEstado(proximoEstado);
    }
}
