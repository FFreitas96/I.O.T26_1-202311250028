# Instalação do Sistema Operacional do BeagleBone Black em Cartão microSD
 
## Relatório Acadêmico — Preparação, Gravação e Validação da Mídia de Armazenamento
 
---
 
**Disciplina:** Internet das Coisas (IoT)  

**Instituição:** Instituto Federal da Paraíba (IFPB), Campus Campina Grande
 
**Grupo:**
- Alberto Viturino Mamede
- Ana Beatriz Belo de Assis Menezes
- Felipe de Freitas Sales dos Santos
- Irene Isley Silva de Vasconcelos

**Data:** 11 de Maio de 2026
 
---
 
## Sumário
 
1. [Introdução](#1-introdução)
2. [Objetivos](#2-objetivos)
3. [Fundamentação Teórica](#3-fundamentação-teórica)
4. [Recursos Utilizados](#4-recursos-utilizados)
5. [Metodologia](#5-metodologia)
    - [5.1 Reconhecimento do Cartão pelo Sistema](#51-reconhecimento-do-cartão-pelo-sistema)
    - [5.2 Formatação via DiskPart](#52-formatação-via-diskpart)
    - [5.3 Instalação do BeagleBoard Imager](#53-instalação-do-beagleboard-imager)
    - [5.4 Seleção e Gravação da Imagem](#54-seleção-e-gravação-da-imagem)
    - [5.5 Customizações Aplicadas](#55-customizações-aplicadas)
    - [5.6 Inicialização e Acesso à BBB](#56-inicialização-e-acesso-à-bbb)
6. [Problemas Encontrados e Soluções](#6-problemas-encontrados-e-soluções)
7. [Resultados](#7-resultados)
8. [Conclusão](#8-conclusão)
9. [Referências](#9-referências)
---
 
## 1. Introdução
 
A **BeagleBone Black (BBB)** é uma plataforma de hardware open-source baseada no processador Texas Instruments AM3358 (ARM Cortex-A8, 1 GHz), amplamente utilizada em projetos de sistemas embarcados, automação e Internet das Coisas (IoT). Embora a placa possua memória interna eMMC de 4 GB, a inicialização a partir de um cartão microSD é o método recomendado para distribuições Linux mais recentes e para a execução de projetos que demandam maior espaço em disco.
 
Este relatório documenta o procedimento completo de **preparação de um cartão microSD para servir como mídia de boot** da BeagleBone Black, incluindo formatação, gravação da imagem oficial do sistema operacional Debian, aplicação de customizações e validação da inicialização do dispositivo.
 
---
 
## 2. Objetivos
 
### 2.1 Objetivo Geral
 
Preparar um cartão microSD para servir como mídia de inicialização do sistema operacional Debian Linux em uma placa BeagleBone Black, deixando-a operacional para uso em projetos de IoT.
 
### 2.2 Objetivos Específicos
 
- Selecionar uma mídia de armazenamento (microSD) adequada às exigências da imagem do sistema;
- Realizar a formatação completa do cartão utilizando a ferramenta DiskPart do Windows;
- Instalar e configurar o utilitário oficial BeagleBoard Imager;
- Selecionar a imagem oficial mais adequada ao perfil do projeto;
- Aplicar customizações de hostname, usuário, senha e rede durante a gravação;
- Validar a inicialização da BBB e o acesso remoto via SSH.
---
 
## 3. Fundamentação Teórica
 
### 3.1 Inicialização do BeagleBone Black
 
A BeagleBone Black possui dois modos de boot:
 
1. **Boot via eMMC interna:** modo padrão, utilizado quando nenhum microSD com sistema válido está presente;
2. **Boot via microSD:** ativado quando o botão **S2 (BOOT)** é pressionado durante a alimentação inicial da placa.
A inicialização via microSD permite atualizações de sistema mais simples, testes de múltiplas distribuições e desenvolvimento sem comprometer a memória interna.
 
### 3.2 Imagem do Sistema Operacional
 
A BeagleBoard.org disponibiliza imagens oficiais para diferentes finalidades:
 
- **Imagens com ambiente gráfico** (LXQt, Xfce): adequadas a uso geral;
- **Imagens IoT/Console:** sem ambiente gráfico, leves, ideais para projetos embarcados.
Para o presente projeto, optou-se pela versão **Debian 13 (Trixie) sem ambiente desktop**, otimizada para aplicações IoT.
 
### 3.3 BeagleBoard Imager
 
O **BeagleBoard Imager** é o utilitário oficial mantido pela BeagleBoard.org Foundation para gravação de imagens em mídias removíveis. Inspirado no Raspberry Pi Imager, oferece:
 
- Download automático das imagens oficiais;
- Verificação de integridade via SHA256;
- Aplicação de customizações no momento da gravação (hostname, usuário, Wi-Fi, SSH, etc.);
- Suporte multiplataforma (Windows, macOS, Linux).
### 3.4 DiskPart
 
O **DiskPart** é a ferramenta nativa do Windows para gerenciamento de discos, partições e volumes via linha de comando. Por operar em nível mais baixo do que o Explorador de Arquivos, é capaz de recuperar mídias com tabelas de partição corrompidas ou em estados inconsistentes.
 
---
 
## 4. Recursos Utilizados
 
### 4.1 Hardware
 
| Componente | Especificação | Função |
|------------|---------------|--------|
| BeagleBone Black | Rev C, AM3358, 512 MB RAM, 4 GB eMMC | Hardware-alvo |
| Cartão microSD | Kingston 8 GB Classe 10 | Mídia de armazenamento |
| Adaptador USB-C | Leitor de microSD | Conexão ao PC |
| Cabo Ethernet | Cat 5e | Conexão BBB-Roteador |
| Fonte de alimentação | 5 V / 2 A | Alimentação da BBB |
| Roteador Wi-Fi | Mercusys MW305R | Rede local |
| Notebook | Intel Core i7, Windows 11 | Estação de trabalho |
 
### 4.2 Software
 
| Software | Versão | Função |
|----------|--------|--------|
| Windows 11 | 24H2 | Sistema operacional do PC |
| BeagleBoard Imager | v1.0.5 | Utilitário de gravação |
| DiskPart | Built-in (Windows) | Formatação de baixo nível |
| Debian Linux | 13 (Trixie), v6.19.x | Sistema a ser instalado |
| OpenSSH | Built-in (Windows) | Acesso remoto à BBB |
| Visual Studio Code | Latest | Terminal e SSH |
 
---
 
## 5. Metodologia
 
O procedimento foi conduzido em seis etapas sequenciais, descritas a seguir.
 
### 5.1 Reconhecimento do Cartão pelo Sistema
 
O cartão microSD foi inserido no adaptador USB-C e conectado a uma das portas USB-C do notebook. Em alguns casos, o Windows pode não reconhecer automaticamente o cartão, especialmente quando este apresenta uma tabela de partições incompatível ou corrompida. Para verificar o estado do cartão, foi utilizado o **Gerenciamento de Disco** do Windows e, posteriormente, o **DiskPart**.
 
### 5.2 Formatação via DiskPart
 
Como o cartão utilizado havia sido previamente usado em uma câmera fotográfica (Sony), apresentando pastas de mídia (`DCIM`, `MISC`, `MP_ROOT`, `PRIVATE`), realizou-se uma formatação completa via **DiskPart**, garantindo a limpeza total da mídia.
 
O procedimento foi executado em Prompt de Comando aberto com privilégios administrativos, seguindo a sequência:
 
```bash
diskpart
list disk
```
 
A listagem dos discos retornou:
 
```
Nº Disco   Status      Tam.       Livre      Din.  GPT
--------   ---------   -------    -------    ----  ---
Disco 0    Online      953 GB     1024 KB          *
Disco 1    Online      7460 MB    3072 KB
```
 
O **Disco 1** (~7460 MB) corresponde ao cartão de 8 GB. A capacidade ligeiramente inferior ao valor nominal deve-se à diferença entre os sistemas de unidades binárias e decimais utilizados por fabricantes e sistemas operacionais.
 
Em seguida, foram executados:
 
```bash
select disk 1
clean
create partition primary
active
format fs=fat32 quick
assign
exit
```
 
A função de cada comando:
 
- `select disk 1` — Seleciona o cartão como disco-alvo (atenção redobrada para não selecionar o disco do sistema);
- `clean` — Remove toda a tabela de partições e estruturas de dados;
- `create partition primary` — Cria uma partição primária ocupando todo o espaço disponível;
- `active` — Marca a partição como ativa (necessário para boot);
- `format fs=fat32 quick` — Formata a partição em FAT32 (formato amplamente reconhecido);
- `assign` — Atribui uma letra de unidade (D:, E:, F:, etc.).
Após a execução, o cartão tornou-se visível no Explorador de Arquivos como uma unidade vazia, pronta para receber a imagem do sistema.
 
### 5.3 Instalação do BeagleBoard Imager
 
O utilitário foi obtido a partir do site oficial da BeagleBoard.org. O arquivo `bb-imager-gui_1_0_5_x64_en-US.msi` foi executado, prosseguindo-se com a instalação padrão no diretório `C:\Program Files\BeagleBoard Imaging Utility\`. Ao final, a opção "Launch BeagleBoard Imaging Utility" foi mantida selecionada para inicialização imediata do programa.
 
### 5.4 Seleção e Gravação da Imagem
 
A interface do BeagleBoard Imager apresenta três etapas principais:
 
1. **CHOOSE DEVICE** — Seleção do dispositivo-alvo;
2. **CHOOSE OS** — Seleção do sistema operacional;
3. **CHOOSE STORAGE** — Seleção da mídia de gravação.
**Dispositivo selecionado:** `BeagleBone Black`
 
**Imagem selecionada:** `BeagleBone Black Debian 13 v6.19.x (Stable)`
 
Detalhes técnicos da imagem:
 
| Atributo | Valor |
|----------|-------|
| Versão | Debian 13 (Trixie) sem ambiente desktop |
| Data de release | 2026-04-23 |
| Tamanho descompactado | 3,52 GiB |
| Tamanho do download | 369,06 MiB |
| Arquivo | `am335x-debian-13.4-base-v6.19-armhf-2026-04-23-4gb.img.xz` |
| Processador-alvo | TI AM335x |
 
**Mídia de gravação:** Cartão Kingston 8 GB (`Mass Storage Device USB Device`)
 
### 5.5 Customizações Aplicadas
 
O BeagleBoard Imager permite aplicar customizações de configuração diretamente na imagem antes da gravação, dispensando configurações manuais após o primeiro boot. As seguintes opções foram aplicadas:
 
| Parâmetro | Valor |
|-----------|-------|
| Hostname | `projeto1` |
| Usuário | `felip` |
| Senha | (definida pelo grupo) |
| DHCP via USB | Habilitado |
| Habilitar SSH | Sim (padrão) |
 
Essas customizações foram gravadas no arquivo `sysconf` localizado na partição BOOT do cartão.
 
### 5.6 Inicialização e Acesso à BBB
 
Após a conclusão da gravação (download + write + verify), o cartão foi removido com segurança do PC e inserido no slot microSD da BeagleBone Black.
 
#### 5.6.1 Procedimento de Boot
 
Para forçar o boot pelo microSD (em vez da eMMC interna), foi adotado o seguinte procedimento:
 
1. Manter o botão **S2 (BOOT)** pressionado;
2. Conectar a alimentação (5 V via jack DC);
3. Aguardar até que os 4 LEDs azuis (USR0-USR3) começassem a piscar;
4. Soltar o botão S2.
#### 5.6.2 Primeiro Boot
 
O primeiro boot consumiu aproximadamente 5 minutos, durante os quais o sistema:
 
- Expandiu a partição raiz para ocupar todo o espaço disponível;
- Aplicou as customizações definidas via `sysconf`;
- Inicializou os serviços do sistema (SSH, networking, etc.);
- Realizou um ou dois reboots automáticos.
#### 5.6.3 Conexão Ethernet e Acesso SSH
 
Após a estabilização do sistema, a BBB foi conectada a um roteador Wi-Fi via cabo Ethernet. A resolução do hostname foi validada com o comando:
 
```powershell
ping projeto1.local
```
 
Confirmada a comunicação, o acesso SSH foi estabelecido a partir do terminal do Visual Studio Code:
 
```bash
ssh felip@projeto1.local
```
 
Após autenticação com a senha definida, o sistema apresentou o prompt:
 
```
felip@projeto1:~$
```
 
Sinalizando o sucesso completo da instalação.
 
O endereço IP atribuído à BBB foi verificado com:
 
```bash
hostname -I
# Saída: 192.168.1.109
```
 
---
 
## 6. Problemas Encontrados e Soluções
 
### 6.1 Cartão microSD Inicial com Defeito Físico
 
**Sintoma:** O primeiro cartão utilizado (2 GB, sem marca identificada) apresentava capacidade insuficiente para a imagem do Debian (3,52 GB). Durante a tentativa de gravação, o BeagleBoard Imager retornou o erro:
 
```
IoError: Erro nos dados (verificação cíclica de redundância)
```
 
**Análise:** O erro CRC (Cyclic Redundancy Check) indica falha de leitura/escrita em setores específicos da memória flash, característica de mídias com desgaste físico ou defeito de fabricação.
 
**Solução:** Substituição por um cartão **Kingston 8 GB Classe 10**, adequado em capacidade e desempenho, e proveniente de fabricante confiável.
 
---
 
### 6.2 Cartão Antigo com Pastas de Câmera
 
**Sintoma:** O cartão de substituição apresentava as pastas `DCIM`, `MISC`, `MP_ROOT` e `PRIVATE`, característica típica de cartões previamente utilizados em câmeras fotográficas (Sony, no caso analisado).
 
**Análise:** Embora essas pastas não impeçam tecnicamente a gravação, ficou evidente que o cartão era usado, e a presença de uma tabela de partições incompatível poderia gerar inconsistências.
 
**Solução:** Formatação completa via **DiskPart** com o comando `clean`, que remove toda a estrutura de partições antes da criação de uma nova tabela limpa.
 
---
 
### 6.3 Falha em Formatação Anterior pelo Explorador de Arquivos
 
**Sintoma:** Após uma tentativa anterior de formatação pelo Explorador de Arquivos do Windows, o cartão ficou em estado inconsistente, exibindo a mensagem:
 
```
O Windows não conseguiu completar a formatação.
```
 
**Análise:** Formatações pelo Explorador podem falhar quando a tabela de partições está corrompida ou quando o cartão se desconecta brevemente durante o processo, deixando-o em um estado intermediário não-utilizável.
 
**Solução:** Uso do DiskPart com o comando `clean`, que opera em nível mais baixo e consegue recuperar a mídia.
 
---
 
### 6.4 Capacidade Real Inferior à Nominal
 
**Sintoma:** O cartão anunciado como 8 GB apresentou 7460 MB no DiskPart e 7,26 GB no Explorador de Arquivos.
 
**Análise:** Trata-se de comportamento normal, decorrente da diferença entre os sistemas de unidades:
 
- Fabricantes usam o sistema decimal: 1 GB = 1.000.000.000 bytes;
- Sistemas operacionais usam o sistema binário: 1 GiB = 1.073.741.824 bytes.
**Conclusão:** A capacidade é suficiente para a imagem de 3,52 GiB, com aproximadamente 3,7 GiB livres para o sistema e aplicações.
 
---
 
## 7. Resultados
 
### 7.1 Validação da Gravação
 
Após a conclusão da gravação, a inspeção da partição BOOT do cartão revelou a estrutura esperada:
 
```
BOOT (D:)
├── services/              (serviços do sistema)
├── ID                     (identificação da imagem)
├── START                  (arquivo de inicialização)
└── sysconf                (customizações aplicadas)
```
 
Os arquivos `services/`, `ID` e `START` apresentavam timestamp de **23/04/2026** (data de release da imagem), enquanto o arquivo `sysconf` apresentava timestamp da data de gravação, confirmando que as customizações foram aplicadas corretamente.
 
### 7.2 Inicialização da BeagleBone Black
 
A inicialização da BBB ocorreu sem incidentes:
 
- Boot iniciado em ~5 segundos após energização;
- LEDs piscando em padrões característicos durante o boot;
- Primeiro boot completo em aproximadamente 5 minutos;
- Boots subsequentes em menos de 60 segundos.
### 7.3 Validação do Acesso Remoto
 
A validação final foi realizada via SSH, confirmando:
 
```bash
PS C:\> ping projeto1.local
 
Resposta de fe80::7aa5:4ff:fedc:5943: tempo=4ms
Resposta de fe80::7aa5:4ff:fedc:5943: tempo=9ms
Resposta de fe80::7aa5:4ff:fedc:5943: tempo=14ms
 
Pacotes: Enviados = 4, Recebidos = 4, Perdidos = 0 (0% de perda)
```
 
```bash
PS C:\> ssh felip@projeto1.local
 
Linux projeto1 6.19.x ...
felip@projeto1:~$
```
 
A BBB tornou-se totalmente funcional, com o sistema Debian operando corretamente e pronta para receber a instalação dos serviços necessários ao restante do projeto.
 
---
 
## 8. Conclusão
 
A instalação do sistema operacional Debian no cartão microSD da BeagleBone Black foi concluída com sucesso, cumprindo todos os objetivos propostos. O procedimento, embora aparentemente simples, exigiu atenção a detalhes técnicos importantes:
 
- A **escolha adequada da mídia** (capacidade mínima, classe de velocidade e confiabilidade do fabricante);
- O **uso correto de ferramentas de baixo nível** (DiskPart) para garantir o estado inicial limpo;
- A **seleção criteriosa da imagem** considerando o perfil do projeto (IoT sem ambiente gráfico);
- A **aplicação prévia de customizações**, evitando configurações manuais após o boot;
- A **execução correta do procedimento de boot via microSD** (botão S2 pressionado).
Os problemas encontrados — desde o defeito físico do primeiro cartão até as inconsistências de formatação — reforçaram a importância de um diagnóstico cuidadoso e do uso das ferramentas apropriadas. A documentação dos problemas e soluções constitui um recurso valioso para futuras implementações.
 
Com a BBB plenamente operacional e acessível remotamente via SSH, a infraestrutura básica está estabelecida para a continuação do projeto, incluindo a instalação do broker MQTT Mosquitto e a integração com os módulos ESP32.
 
### 8.1 Aprendizados Técnicos
 
- Domínio do utilitário DiskPart para gerenciamento de mídias removíveis;
- Compreensão da diferença entre formatação rápida e formatação completa;
- Familiaridade com o BeagleBoard Imager e suas opções de customização;
- Conhecimento do procedimento de boot da BeagleBone Black;
- Resolução de nomes via mDNS (`.local`) sem dependência de servidor DNS;
- Acesso remoto via SSH em rede local.
---
 
## 9. Referências
 
1. BEAGLEBOARD.ORG FOUNDATION. **BeagleBone Black System Reference Manual.** Disponível em: <https://docs.beagleboard.org/boards/beaglebone/black/>. Acesso em: mai. 2026.
2. BEAGLEBOARD.ORG FOUNDATION. **Latest Software Images for BeagleBone Black.** Disponível em: <https://www.beagleboard.org/distros>. Acesso em: mai. 2026.
3. BEAGLEBOARD.ORG FOUNDATION. **BeagleBoard Imager.** Disponível em: <https://github.com/beagleboard/bb-imager-rs>. Acesso em: mai. 2026.
4. MICROSOFT CORPORATION. **DiskPart Command-Line Options.** Disponível em: <https://learn.microsoft.com/windows-server/administration/windows-commands/diskpart>. Acesso em: mai. 2026.
5. THE DEBIAN PROJECT. **Debian 13 (Trixie) Release Notes.** Disponível em: <https://www.debian.org/releases/trixie/>. Acesso em: mai. 2026.
6. INSTITUTO FEDERAL DA PARAÍBA. **Projeto 2: Controle de Iluminação (Gateway) — Roteiro.** Disciplina de Internet das Coisas, IFPB, 2026.
---
 
<div align="center">
**FIM DO RELATÓRIO**
 
*Documento gerado em maio de 2026*
 
</div> 
