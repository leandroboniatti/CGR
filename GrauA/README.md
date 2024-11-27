# Simulando a cobrinha do Slither.io de FundamentosCG

Ian Rossetti Boniatti - Curso Jogos Digitais 

Entrega da cobrinha do Slither.io de Fundamentos de Computação Gráfica

1. Nome completo dos integrantes.
   
   Ian Rossetti Boniatti e Eduardo Silva Tropea

2. Instruções para executar o programa.
   
   Rodar o arquivo "SnakeFinal.cpp".

3. Qualquer explicação adicional necessária sobre o funcionamento do código.

   Se necessário, ajustar a velocidade em: "segmentFactor = ...f, headFactor = ...f;".
   Em alguns computadores a velocidade varia, portanto pode ser necessário encontrar a velocidade ideal para sua máquina.

   no final do vídeo eu cometi um engano, achei que já tivesse falado sobre as funções "aplicaTrasformaçoes" quando falei dos protótipos e acabei não "abrindo" e explicando as funções. Mas basicamente, nós optamos por modificar a função que a senhora disponibilizou, "drawGeometry", para separar as transformações da chamada de desenho. No código, por exemplo, ficamos com:
   aplicaTransformacoes(shaderID, snake[i]);
   glDrawArrays(GL_TRIANGLE_FAN, 0, 34);
