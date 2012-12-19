
/* Implementado desde 11/06 - Desenho de modelos

O PROPÓSITO DESSE ARQUIVO É O DE APENAS INSERIR COMENTÁRIOS.

Tabela Modelos a serem desenvolvidos: ( em ordem de viabilidade )

    * Atrv                ok - 90%
    * Avatar Helicopter   ok - 100%
    * B21R                ok - 90%
    * Bandit              ok - 100%
    * Blimp               ok - 100%
    * Canon VCC4          ok - 100%
    * Car Chassis         ok - 100%
    * Clod Buster         ok - 95%
    * Factory             not necessary
    * Garmin GPS          ok - 100%
    * Ground Plane        ok - 100%
    * Light Source        DEVE SER DESENVOLVIDO !!
    * Mono Camera         ok - 100%
    * Observer Camera     DEVE SER DESENVOLVIDO !!
    * PeopleBot           ok - 100%
    * Pioneer 2 AT        ok - 100%
    * Pioneer 2 DX        ok - 90%
    * Pioneer 2 Gripper   ok - 100%
    * Road                no-picure
    * Segway RMP          ok - 100%
    * Shrimp              ok - 100%
    * Sick LMS 200        ok - 100%
    * Simple Solid        ok - 100%
    * Sky Dome            no-picture
    * Sony VID 30         ok - 100%
    * Stereo Head         ok - 100%
    * Terrain             no-picture
    * Truth Widget        not necessary ( completely not)
    * Wheelchair          no-picture
    * Global Attributes   not necessary

*/

/*  Leitura de arquivos XML ( World ) 

    Questoes a serem respondidas:

    - Utilizar a arquitetura MVC ou Doc-View ?
      MVC

    - Testar as classes WorldFile e WorldFileNode ....

    - Como implementar o carregamento e salvamente de cada modelo ?
      1) Criar classe sombra para cada modelo
       
    Visualizacao / Manipulacao do cenario 3D  

    - Como implementar multi-visualizacoa na aplicacao ? 
    Nao vai haver multivisualizacao apenas visualizacao selecionada por combobox

    - Como detectar qual objto foi selecionado ? 
    OpenGL tem funcoes para detectar objetos selecionados
    
    - Utilizar a visao de camera ou ortografica ?
    Apenas visao de perspectiva (camera) por motivo de simplicidade
    Perdeu-se um tempo implementando essas cameras como ortograficas

    - Como vao funcionar as cameras ??
      - como guardar o contexto ( posicionamento ) das cameras ??
         Num vector (STL) de contextos de cameras
      - Nas Cameras X/Y, X/Z, Y/Z só poderá ser feito translacao
      - Essas cameras sao as unicas que permitirao arrastar a posicao
        dos modelos (por motivo de simplicidade)
*/

/*  PAREI POR AQUI !!!! COMEÇAR DAQUI EM DIANTE !!

 - Duvida do prof: Existe a possibilidade de se escrever o controle em Java ?//

  - Como definir o conjunto de operacoes permitidas na aplicacao ????
    - Que tal definir Commands (ver pattern) ??? Definir as seguintes ferramentas:
      - Mão para mover o cenario
      - Seta para poder selecionar modelos (se arrastar pode mover a posicao)
        Ao selecionar um modelo, suas propriedades devem aparecer
      - Adicionar sphere || cylinder ( adicionar com propriedades padrao )
      - Adicionar GroundPlane, Modelos estaticos, etc...
      - Adicionar ObserverCam ( aparece a camera no combobox e configurar visao em perspectiva )
      - Adicionar Box ( desenho interativo - 1o clique ( 1o canto ) - 2o clique - ( 2o canto ) - estilo 3DHome
      - Ao adicionar qualquer Modelo que herda de Camera - adicionar no combobox e dar a opcao de visualizacao

   - Como arrastar os objetos ? 
          - Nessas cameras os objetos poderao ser movidos ou rotacionados se selecionados
            - Devera haver uma barra de ferramentas:
              - Mao ( translada (botao esquerdo) e zoom (botao direito) no cenario ) 
              - Seta ( seleciona models - mover (botao direito) e inserir/remover (botao esquerdo))
              - Os outros models disponiveis (GroundPlane, Pioneer, etc .... )

        - Na Camera ObserverCam pode-se mover o eye e o center position
        - Pode-se dar a opcao de ver atraves de uma MonoCam, CanonVCC4, SonyVID30  ( Opcional )

   Consultar/Alterar as propriedades de cada modelo 

    - Como listar dinamicamente as propriedades de cada modelo selecionado
    
   Biblioteca de obstáculos estáticos

   - Como fazer para criar um model que seja descrito apenas por um arquivo
     de geometria ( como um 3DS ) ?
     Verifique se ja existe um model para isso como o factory senao vc vai ter
     que criar um model para isso

   ATÉ AQUI JA TEMOS ALGO APRESENTÁVEL - CORRA PARA ESCREVER AGORA !!

   Biblioteca de obstáculos com movimento ( dinâmicos )

   - Como criar um model que sirva para varios obstaculos dinamicos com o
     mesmo comportamento com geometrias diferentes ?

*/

/*
- Revisar os objetivos acima

- Usar o evento gerado por wxGLCanvas para adquirir o objeto selecionado no Canvas
- Implementar adicao de camera ObserverCam ou outro tipo de camera e a sua movimentacao
- Implementar arrasto com o mouse da posicao dos models nas cameras ortograficas
- Testar e implementar carregamento e salvamento de arquivo XML de World
- Implementar carregamento/salvamento de arquivo de proejto que contem as infos integradas
- Implementar criacao, edicao, salvamento, compilacao e envio de controle para o Player/Gazebo
*/

/*
>>> Tarefas - 07/01/2007 <<<

- Atualizar a visualizacao 3D apos atualizar as propriedades de um modelo
- Atualizar as propriedades de um modelo após arrasta-lo pelo mouse ( left button up )
- Acertar o problema do posicionamento da camera e a visualizacao:
  - Acertar a visualizacao quando a orientacao e posicao sao informadas na propriedade
  - Acertar a mudanca de posicao da camera no arrasto do mouse
    ( Atualizar as propriedades da camera ) ( left button up )

*/

