Com certeza. Como Programador Sênior, preparei um `README.md` técnico, profissional e bem estruturado, pronto para ser colocado no seu repositório (GitHub/GitLab).

Ele documenta a arquitetura, o hardware e o funcionamento do sistema.

***

# 📋 ATA Eletrônica IoT com ESP32

Este projeto consiste em um sistema embarcado de **Gestão de Presença Acadêmica** baseado no microcontrolador ESP32. O dispositivo substitui a lista de presença em papel, permitindo que o professor configure a aula e que os alunos registrem presença via matrícula. Ao final, o sistema gera um relatório em formato **CSV (Excel)** e o envia automaticamente por **E-mail** para a coordenação.

## 🚀 Funcionalidades

*   **Autenticação Segura:** Login administrativo para professores (Matrícula/Senha).
*   **Configuração de Sessão:** Definição de Data, Curso e Período letivo via interface.
*   **Persistência de Dados:** Uso do sistema de arquivos **LittleFS** (Memória Flash) para salvar a ATA, prevenindo perda de dados em caso de queda de energia.
*   **Conectividade IoT:** Envio de relatórios via **SMTP Seguro (SSL/TLS)** com anexo `.csv`.
*   **Gestão de Energia:** Modo *Deep/Light Sleep* automático após inatividade (desliga LCD e WiFi).
*   **Arquitetura Robusta:** Desenvolvido em **C++ Embarcado Estático**, utilizando Máquina de Estados Finitos (FSM) e gestão de memória sem fragmentação (evita travamentos).

---

## 🛠️ Componentes Necessários

Para replicar este projeto, você precisará dos seguintes itens de hardware:

| Qtd | Componente | Descrição |
| :-- | :--- | :--- |
| 1 | **ESP32 DevKit V1** | Microcontrolador principal (com WiFi e Bluetooth). |
| 1 | **Display LCD 16x2** | Com interface **I2C** soldada (endereço 0x27 ou 0x3F). |
| 1 | **Teclado Matricial** | Membrana ou Rígido, formato **4x4** (16 teclas). |
| 1 | **Fonte de Alimentação** | Cabo USB ou Fonte 5V externa. |
| - | **Jumpers** | Cabos Macho-Fêmea e Macho-Macho para conexões. |
| 1 | **Protoboard** | Para montagem do circuito. |

---

## ⚡ Pinagem e Conexões

Abaixo está o mapeamento físico dos pinos do ESP32 definidos no arquivo `config.h`.

### 1. Display LCD (Interface I2C)

| Pino do Módulo I2C | Pino do ESP32 | Observação |
| :--- | :--- | :--- |
| **GND** | GND | Terra Comum. |
| **VCC** | VIN (5V) | O LCD requer 5V para bom contraste. |
| **SDA** | GPIO 21 | Dados I2C. |
| **SCL** | GPIO 22 | Clock I2C. |

### 2. Teclado Matricial 4x4
*Considerando os pinos do teclado da esquerda (1) para a direita (8).*

| Pino do Teclado | Função | Pino do ESP32 |
| :--- | :--- | :--- |
| Pino 1 | Linha 1 | **GPIO 13** |
| Pino 2 | Linha 2 | **GPIO 12** |
| Pino 3 | Linha 3 | **GPIO 14** |
| Pino 4 | Linha 4 | **GPIO 27** |
| Pino 5 | Coluna 1 | **GPIO 26** |
| Pino 6 | Coluna 2 | **GPIO 25** |
| Pino 7 | Coluna 3 | **GPIO 33** |
| Pino 8 | Coluna 4 | **GPIO 32** |

---

## 💻 Configuração do Ambiente (IDE)

Para compilar este projeto, siga estas configurações no Arduino IDE:

1.  **Placa:** `DOIT ESP32 DEVKIT V1`
2.  **Partition Scheme:** `Huge APP (3MB No OTA/1MB SPIFFS)` *(Essencial para caber as bibliotecas de Email)*.
3.  **Bibliotecas Obrigatórias:**
    *   `LiquidCrystal_I2C` (Frank de Brabander)
    *   `Keypad` (Mark Stanley, Alexander Brevig)
    *   `ESP Mail Client` (Mobizt)

### ⚠️ Notas Importantes sobre Email

Para utilizar o envio via Gmail, você **não** deve usar sua senha normal.
1.  Ative a **Autenticação de 2 Fatores (2FA)** na sua conta Google.
2.  Vá em "Senhas de App" (App Passwords).
3.  Gere uma senha nova e coloque no arquivo `config.h` na constante `AUTHOR_PASSWORD`.

---

## 📖 Guia de Uso

1.  **Início:** Pressione `A` para iniciar o Login.
2.  **Login:**
    *   Digite a matrícula. Pressione `*` para confirmar.
    *   Digite a senha. Pressione `*` para entrar.
    *   *(Use `#` para corrigir e `D` para limpar)*.
3.  **Menu:**
    *   `1`: Enviar ATA por Email.
    *   `2`: Configurar/Iniciar nova ATA.
4.  **Configuração:** Insira Data, ID do Curso e Período. Confirme com `*`.
5.  **Ata Ativa:**
    *   O aluno digita a matrícula e pressiona `*`.
    *   O sistema confirma o nome e salva no arquivo interno.
    *   Pressione `D` para encerrar a aula e voltar ao menu.
6.  **Envio:** No menu, selecione `1` para processar o arquivo `.csv` e enviar o anexo para a coordenação.
