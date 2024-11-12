# [Projeto-TT304] Thread-Masters
![Em Desenvolvimento](https://img.shields.io/badge/status-em%20desenvolvimento-yellow)
Este projeto foi desenvolvido como parte da disciplina TT304 - Sistemas Operacionais, com o objetivo de consolidar os conhecimentos sobre programação com múltiplos threads em C utilizando a biblioteca POSIX Threads.

## Objetivo do Projeto

O programa desenvolvido tem como objetivo ler valores inteiros de múltiplos arquivos de entrada, ordená-los e gravar os valores em um único arquivo de saída. O programa utiliza T threads, onde T pode ser 1, 2, 4 ou 8, e implementa a maior quantidade de threads possível para otimizar o processamento.

## Estrutura do Projeto

- **Código Fonte:** Contém o código desenvolvido em C, seguindo os requisitos especificados.
- **Relatório:** Inclui os experimentos e resultados obtidos com tempos de processamento para diferentes números de threads.
- **Vídeo:** Demonstra a execução do programa, a compilação e a explicação dos resultados dos experimentos realizados.

## Especificações Técnicas

- **Linguagem:** C (pura), utilizando a biblioteca POSIX Threads
- **Sistema Operacional:** Linux
- **Algoritmo de Ordenação:** Qualquer algoritmo adequado, com a fonte citada se for externo

## Instruções de Execução

1. **Compilação:**
   - Utilize o `makefile` incluso para compilar o programa:
     ```bash
     make
     ```
   - Isso gerará o executável `mergesort`.

2. **Execução:**
   - Para rodar o programa, utilize a linha de comando especificando o número de threads e os arquivos de entrada e saída, como no exemplo:
     ```bash
     ./mergesort 4 arq1.dat arq2.dat arq3.dat -o saida.dat
     ```

## Entradas e Saídas

- **Entrada:** 
  - Número de threads
  - Lista de arquivos de entrada com valores inteiros não ordenados
- **Saída:** 
  - Arquivo com os valores ordenados
  - Tempos de execução de cada thread e o tempo total de execução exibidos na tela

## Experimentos

Foram realizados experimentos com as seguintes configurações de threads: T = 1, T = 2, T = 4, e T = 8, e analisados tempos de execução para avaliar o desempenho do programa em diferentes situações. Os tempos foram medidos utilizando a primitiva `clock_gettime`.

## Requisitos

1. **Alocação Dinâmica:** Todas as estruturas de dados devem ser alocadas dinamicamente.
2. **Compilação sem Erros:** A compilação deve ocorrer sem warnings ou erros.
3. **Formato de Arquivo:** Os dados devem estar em arquivos texto.
4. **Parâmetros pela Linha de Comando:** O programa deve ser executado conforme o exemplo especificado acima.

## Contribuições

Para contribuir, faça um fork do repositório e envie um pull request com suas sugestões ou correções.
---

*Desenvolvido para a disciplina TT304 - Sistemas Operacionais, Por Geovanne Silva e Heloisa Feitosa*

