- Melhorar o fato de que algumas funções dependem de comportamentos ou medidas externas para funcionar corretamente. 
    - Por exemplo: WriteSector() necessita que o programador deixe o Buffer do jeito que ele deseja antes de chamar a WriteSector(). Isso para funções que usam a WriteSector() começa a ficar difícil de rastrear algum erro. Ainda que pareça uma abordagem plausível, pois a função funcionaria como um flush, é o que eu penso, pelo menos.

- ROADMAP WANNABE:
[1] Criar várias pastas (SD0, SD1, SD1.1, SD1.2, SD1.3, SD2, SD3, SD4)
[2] Criar arquivos pequenos (5000) (1KB, 4KB, 8KB)
[3] Encher o pen drive com vídeos (pasta SD1.3)
[4] Apagar alguns aleatoriamente 150 (sem ser vídeo)
[5] Criar 10 arquivos de 40KB
[6] Apagar o 3, o 5, e o 7
[7] Restaurar o 5 (Procurar nome dele no diretório e encontrar o cluster inicial que também é o índice na FAT)


1- Encontrar o nome do arquivo em algum diretório
	1.1- Em qual diretório está o arquivo? Eu posso escolher?

2- (Encontrei) Alterar primeiro byte do header do arquivo e identificar o initial cluster [índice na FAT (Qual o tamanho máximo da FAT?)]

2*- Ir até o inital cluster dentro do espaço dos arquivos (offset_files + cluster_size*initial cluster) e encontrar a primeira parte do arquivo
	2*.1 - Encontrar as partes subsequentes
	2*.2 - encontrar N em (((N - 2) * F) * A) + E = Endereço do Arquivo para identificar qual cluster dessa parte
	2*.3 criar um vector de vector contendo parte do arquivo, endereço e cluster para cada parte do arquivo.

3- Ir até a FAT, fazer 4 * inital cluster (ou parte do arquivo no índice 0 do ) e escrever FF FF FF NEXTCLUSTER [NEXTCLUSTER estará na próxima posição do vetor]
	3.1- andar pelo vector preenchendo a FAT até o último elemento do vector
	3.2- no último elemento, escrever 0F;