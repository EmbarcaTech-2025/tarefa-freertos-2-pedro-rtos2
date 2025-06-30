
# Tarefa: Projeto Livre com RTOS - EmbarcaTech 2025
# Sintetizador de Notas Musicais

Autor: **Pedro Henrique Dias Avelar**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, Junho de 2025

---

## Objetivo
O presente projeto visa utilizar a BitDogLab como um sintetizador de notas musicais.
Cada um dos buzzers é utilizado para tocar uma sequencia de 16 diferentes notas musicais. O usuário consegue, por meio do joystick, dos botões A e B e do display OLED, verificar e alterar as duas sequências de notas.
Para a execução foram criadas 4 tasks:
* Display Task - Controla a exibição de dados no display OLED
* Buzzer Task - Controla o acionamento dos dois buzzers da BitDogLab
* Button Task - Controla a entrada de comandos pelos botões A, B e botão do joystick
* Joystick Task - Controla a entrada de comandos pela movimentação do joystick

## Lista de periféricos utilizados

|Periférico    | Conexão na BitDogLab|
|--------------|---------------------|
| Botões A e B | GP5 e GP6           |
| Movimento do Joystick | GP27 (Eixo X) GP26 (Eixo Y)|
|Botão do Joystick | GP22 |
| Buzzers A e B| GP21 e GP10         |
|Display OLED  | GP14(SDA), GP15(SCL)|

## Lógica

Por meio do display OLED, são exibidas as duas sequências de notas a serem tocadas. Embaixo das sequências é exibido um cursor, o qual pode ser deslocado por meio do joystick. O acionamento dos botões A e B altera a nota indicada pelo cursor.
Ao acionar o botão do joystick, inicia-se a reprodução das sequências de notas configuradas. Uma seta indica quais as notas sendo executadas no momento. Durante esta reprodução ainda é possível alterar as notas. A reprodução é executada apenas uma vez - a task de execução do buzzer é controlada por um semáforo binário,
o qual é ativado com o acionamento do botão do joystick e desativado após o fim da reprodução. 

Na parte inferior do display são exibidas as funcionalidades dos botões A e B, e do botão do joystick, para facilitar o uso do usuário. Entendi não ser necessário apontar instruções para uso do joystick devido ao feedback visual do cursor.

![Imagem do WhatsApp de 2025-06-29 à(s) 21 52 12_6295c2fa](https://github.com/user-attachments/assets/ea508122-4b86-4aa6-9a5f-bf4410e86fc4)
Exibição do projeto em execução

Video: [https://www.youtube.com/shorts/j6pyCCiNvGw](https://www.youtube.com/shorts/OB_qh6hD-wE)


---

## 📜 Licença
GNU GPL-3.0.
