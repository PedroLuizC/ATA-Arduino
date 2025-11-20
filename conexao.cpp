/*
 * Caminho: C:\Work\Workspace\arduino\ATA\conexao.cpp
 * Descricao: Envio de email com ANEXO usando LittleFS.
 */

#include "conexao.h"
#include <LittleFS.h> // Necessario para ler o anexo

SMTPSession smtp;
Session_Config config;
SMTP_Message message;

void smtpCallback(SMTP_Status status);

bool conectarRede() {
    if (WiFi.status() == WL_CONNECTED) return true;
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_SENHA);
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) delay(500);
    return (WiFi.status() == WL_CONNECTED);
}

void desconectarRede() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

// Agora aceita apenas o assunto, pois o corpo sera padrao e o arquivo sera anexado
bool enviarEmailReal(const char* assunto) {
    if (WiFi.status() != WL_CONNECTED) return false;

    smtp.debug(1);
    smtp.callback(smtpCallback);

    config.server.host_name = SMTP_HOST;
    config.server.port = SMTP_PORT;
    config.login.email = AUTHOR_EMAIL;
    config.login.password = AUTHOR_PASSWORD;
    config.login.user_domain = "";

    message.sender.name = "ATA Eletronica ESP32";
    message.sender.email = AUTHOR_EMAIL;
    message.subject = assunto;
    message.addRecipient("Coordenacao", RECIPIENT_EMAIL);
    
    message.text.content = "Ola.\nSegue em anexo a ATA de presenca em formato CSV (Compativel com Excel).\n\nAtenciosamente,\nSistema ESP32";
    message.text.charSet = "us-ascii";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;

    
    SMTP_Attachment att;
    att.descr.filename = "ata_presenca.csv";
    att.descr.mime = "text/csv";
    att.file.path = "/ata.csv";
    att.file.storage_type = esp_mail_file_storage_type_flash; // Define que vem do LittleFS

    message.addAttachment(att);

    if (!smtp.connect(&config)) {
        Serial.printf("Erro SMTP Conexao: %s\n", smtp.errorReason().c_str());
        return false;
    }

    if (!MailClient.sendMail(&smtp, &message)) {
        Serial.printf("Erro SMTP Envio: %s\n", smtp.errorReason().c_str());
        return false;
    }
    
    message.resetAttachItem(att);
    
    return true;
}

void smtpCallback(SMTP_Status status) {
}
