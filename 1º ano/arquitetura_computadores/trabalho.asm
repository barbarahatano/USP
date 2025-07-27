    .data
inicio: .asciz "Escolha: 0 - Pedra, 1 - Papel, 2 - Tesoura, 3 - Sair\n"
vitoria:       .asciz "Voc� venceu!\n"
derrota:      .asciz "Voc� perdeu!\n"
empate:      .asciz "Empate!\n"
invalido:   .asciz "Escolha inv�lida!\n"
historico:     .asciz "Hist�rico de jogadas (voc�-computador):\n"

lista:         .word 0 #ponteiro para o in�cio da lista, a princ�pio vazio

    .text
    .globl _start
_start:
    # Exibir op��es para o jogador
    la a0, inicio    # Carrega a mensagem para exibir as op��es
    li a7, 4	     # syscall para imprimir string
    ecall                   

    # Receber entrada do jogador
    li a7, 5                # Syscall para ler um inteiro do teclado
    ecall
    mv t0, a0               # Armazena a escolha do jogador em t0

    # Verificar se o jogador escolheu sair (3)
    li t1, 3                # Carrega o valor 3 para compara��o
    beq t0, t1, mostrar_historico    # Se o jogador escolheu 3, vai para o final do jogo
    j game_loop		    #Caso n�o, entra no loop do game

game_loop:
    # Gerar a escolha do computador
    li a0, 0		    #Carrega o valor do limite inferior do intervalo 
    li a1, 3		    #Carrega o valor do limite superior
    li a7, 42               # Syscall para n�mero aleat�rio entre dois valores
    ecall      		    # Gera um n�mero entre 0 e 2 (Pedra, Papel, Tesoura)
    mv t1, a0		    #move o valor do argumento de retorno para um registrador para fazer a compara��o

    # Comparar escolhas do jogador e computador
    
    beq t0, t1, _empate        # Se as escolhas s�o iguais, empate

    # Caso o jogador tenha escolhido Pedra (0)
    li t2, 0                # Verificar se jogador escolheu Pedra
    beq t0, t2, checar_pedra

    # Caso o jogador tenha escolhido Papel (1)
    li t2, 1                # Verificar se jogador escolheu Papel
    beq t0, t2, checar_papel

    # Caso o jogador tenha escolhido Tesoura (2)
    li t2, 2                # Verificar se jogador escolheu Tesoura
    beq t0, t2, checar_tesoura

    j _invalido         # Se n�o for nenhuma dessas, escolha inv�lida

checar_pedra:
    li t2, 2                # Se o computador escolheu Tesoura (2), jogador vence
    beq t1, t2, _vitoria
    j _derrota           # Caso contr�rio, o jogador perde

checar_papel:
    li t2, 0                # Se o computador escolheu Pedra (0), jogador vence
    beq t1, t2, _vitoria
    j _derrota           # Caso contr�rio, o jogador perde

checar_tesoura:
    li t2, 1                # Se o computador escolheu Papel (1), jogador vence
    beq t1, t2, _vitoria
    j _derrota           # Caso contr�rio, o jogador perde

_empate:
    la a0, empate          # Mensagem de empate
    li a7, 4
    ecall
    j add_historico           # Adiciona a jogada ao hist�rico

_vitoria:
    la a0, vitoria           # Mensagem de vit�ria
    li a7, 4
    ecall
    j add_historico          #Adiciona a jogada ao hist�rico

_derrota:
    la a0, derrota          # Mensagem de derrota
    li a7, 4
    ecall
    j add_historico          #Adiciona a jogada ao hist�rico

_invalido:
    la a0, invalido       # Mensagem de escolha inv�lida
    li a7, 4
    ecall
    j _start                # Reiniciar o jogo

add_historico:
			    #Lista ligada (12 bytes para o n�)
    li a0, 12		    #Carrega-se o valor 12 para o argumento da syscall
    li a7, 9		    #Syscall para aloca��o de mem�ria (sbrk)
    ecall
    
    sw t0, 0(a0)	    #Passa o conte�do de t0 (escolha do jogador) para o endere�o de a0
    sw t1, 4(a0)	    #Passa o conde�do de t1 (esecolha do computador) para o endere�o deslocado de a0
    
    li t2, 0		    #Coloca o ponteiro do pr�ximo n� como nulo
    sw t2, 8(a0)	     
    
    lw t2, lista	    #Carrega o valor atual do n�
    beqz t2, update	    #Se o valor do n� for 0, este � o primeiro n�
    
    achar_ultimo:
        lw t3, 8(t2)	    #Carrega o ponteiro para o pr�ximo n�
        beqz t3, colocar_final     #Se for nulo, encontrou-se o �ltimo n�
        mv t2, t3	    #Continua para o pr�ximo
        j achar_ultimo	    #Loop
        
    colocar_final:
        sw a0, 8(t2)	    #Atualiza o ponteiro do �ltimo n� para apontar para o novo n�
        j _start	    #Volta para o jogo
        
    update:
        la t2, lista	    #Carrega o endere�o da 'head' da lista ligada
        sw a0, 0(t2)	    #Atualiza a 'head' para apontar para o novo n�
        j _start	    #Volta para o jogo

mostrar_historico:
    la a0, historico	    #Carrega a mensagem de mostrar o hist�rico
    li a7, 4		    #Syscall para imprimir string
    ecall
    
    lw t0, lista
    beqz t0, fim	    #Se a head for 0 mostra que a lista est� vazia e encerra o programa
    j mostrar_resultados    #Caso n�o, mostra os resultados
    
mostrar_resultados:
    beqz t0, fim
    
    lw a0, 0(t0)	    #Carrega a escolha do jogador
    li a7, 1		    #Syscall para imprimir um inteiro
    ecall
    
    li a0, 45		    #Carrega o caractere '-' para separar os resultados
    li a7, 11		    #Syscall para imprimir inteiro
    ecall
    
    lw a0, 4(t0)	    #Carrega a escolha do computador
    li a7, 1		    #Syscall para imprimir inteiro
    ecall
    
    li a0, 10		    #Carrega o caractere de nova linha
    li a7, 11		    #Syscall para imprimir caractere
    ecall
    
    lw t0, 8(t0)	    #Carrega o ponteiro para o pr�ximo n�
    j mostrar_resultados    #Repete para o pr�ximo n�

fim:
    # Encerrar o programa
    li a7, 93               # Syscall para encerrar o programa
    ecall
