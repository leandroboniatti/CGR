# Lista 1 de FundamentosCG

Ian Rossetti Boniatti - Curso Jogos Digitais 

Entrega da Lista 1 de Fundamentos de Computação Gráfica

## Parte 1 - Teoria
1.
   a) O que é a GLSL?
   GLSL é uma Linguagem associada ao OpenGL, usada para criar shaders que manipulam vértices, pixels e calculam iluminação. Utilizada para programação gráfica em múltiplas plataformas, como Windows, Linux, MacOS,
etc.

   b) Quais os dois tipos de shaders são obrigatórios no pipeline programável da versão atual que trabalhamos em aula e o que eles processam?
   

   
2.
   a) O que são primitivas gráficas?


   b) Como fazemos o armazenamento dos vértices na OpenGL?
  

3. Explique o que é VBO e VAO, e como se relacionam (se achar mais fácil, pode fazer um gráfico representando a relação entre eles).


  
4. Analise o código fonte do projeto Hello Triangle. Localize e relacione os conceitos de shaders, VBOs e VAO apresentados até então. Não precisa entregar nada neste exercício.



## Parte 2 - Programação

5. Faça o desenho de 2 triângulos na tela. Desenhe eles:
   
   a. ![{D7D6BF2C-F9D3-40AA-BA86-F52FE9A7E235}](https://github.com/user-attachments/assets/3efa87d5-a18a-417d-8de5-61cf130c10ab)

   b. ![{56422F9E-227A-429C-A1C5-A18FB4FA25A7}](https://github.com/user-attachments/assets/177b0161-42f2-437d-bf34-1874f00d8c96)

   c. ![{E3414722-D7C8-44BA-ABD7-76F1CF366C1F}](https://github.com/user-attachments/assets/6f6605b4-3c32-4150-b3b4-90537794ab84)

   d. ![{06C8E713-3C3C-49A9-8D98-1019A1AF34D7}](https://github.com/user-attachments/assets/2501d3ce-18c5-425a-825d-1efe4376ad0c)




6. Faça o desenho de um círculo na tela, utilizando a equação paramétrica do círculo para gerar os vértices.

   ![{60B04DD6-E800-465A-BCEC-EC31B41ABB24}](https://github.com/user-attachments/assets/5a79a819-2e57-4390-a091-17fde12a8068)

   a. ![{A8125FB8-6AFC-482A-A14E-22E92B9869FD}](https://github.com/user-attachments/assets/e27a59a5-a010-413e-866c-71e2485d14de)

   b. ![{26D90F44-DC7D-479E-B3F6-172FE10BB2BD}](https://github.com/user-attachments/assets/d5869536-fb2c-484f-b88c-83bc0207cecf)

   c. ![{6D07F414-E7BD-4ED9-A0B4-CBDB589C8109}](https://github.com/user-attachments/assets/1cc81832-97b0-4844-ba6e-72908680449f)

   d. ![{59047D45-8092-4042-AD60-8FAB028EA85D}](https://github.com/user-attachments/assets/390c6219-ee4f-44e5-8395-3edcb1852c0f)

   e. ![{54FF709D-F41E-459B-A3D8-9345AADD1691}](https://github.com/user-attachments/assets/99bad192-20e5-47c9-b67e-905ba705374e)


7. Desenhe uma espiral.

   ![{4DD86853-7202-4C20-970D-1D8218E6E1FB}](https://github.com/user-attachments/assets/329e940a-da80-4060-b08a-8b3225fe3a4c)


8. Considerando o seguinte triângulo abaixo, formado pelos vértices P1, P2 e P3, respectivamente com as cores vermelho, verde e azul:

   a. Vertex Shader:    - definisse a varíavel de entrada "color" dos tipos "vec3" e "in" que vai receber os dados da "location=1" do VAO (cores dos vertices);
                        - definisse a variável de saída "vertexColor" dos tipos "vec3" e "out" que vai enviar as cores para os próximos estagios do pipeline;

      Fragment Shader:  - definisse a varíavel de entrada "vertexColor" dos tipos "vec3" e "in" que vai receber os dados da "location=1" do VAO oriundos do pipeline e substituindo a varíavel uniform usada antes ("inputColor");

      setupGeometry():   - temos um VBO com dois atributos


   definesse a "location 1" sendo que







Sintaxe básica de gravação e formatação no GitHub:
https://docs.github.com/pt/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax
